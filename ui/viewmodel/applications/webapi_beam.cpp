// Copyright 2018 The Beam Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <QObject>
#include <QMessageBox>
#include <sstream>
#include "webapi_beam.h"
#include "utility/logger.h"
#include "model/app_model.h"

namespace beamui::applications {
    using namespace beam::wallet;

    namespace
    {
        WalletModel& getWallet() {
            return *AppModel::getInstance().getWalletModel();
        }

        IWalletModelAsync& getAsyncWallet() {
            return *getWallet().getAsync();
        }

        namespace
        {
            typedef QList<QMap<QString, QVariant>> ApproveAmounts;
            typedef QMap<QString, QVariant> ApproveMap;

            void printMap(const std::string& prefix, const ApproveMap& info)
            {
                QMapIterator<QString, QVariant> iter(info);

                while (iter.hasNext())
                {
                    iter.next();
                    if (iter.value().canConvert<QString>())
                    {
                        LOG_INFO () << prefix << iter.key().toStdString() << "=" << iter.value().toString().toStdString();
                    }
                    else
                    {
                        assert(false); // for now should not happen, add special case above to print correct logs
                        LOG_INFO () << prefix << iter.key().toStdString() << "=" << "unexpected no-str convertible";
                    }
                }
            }

            void printApproveLog(const std::string& preamble, const std::string& appid, const std::string& appname, const ApproveMap& info, const ApproveAmounts& amounts)
            {
                LOG_INFO() << preamble << " (" << appname << ", " << appid << "):";
                printMap("\t", info);

                if (!amounts.isEmpty())
                {
                    for (const auto &amountMap : amounts)
                    {
                        LOG_INFO() << "\tamount entry:";
                        printMap("\t\t", amountMap);
                    }
                }
            }
        }
    }

    WebAPI_Beam::WebAPI_Beam(const std::string& version, const std::string& appid, const std::string& appname)
        : QObject(nullptr)
        , _appId(appid)
        , _appName(appname)
        , _amgr(AppModel::getInstance().getAssets())
    {
        connect(_amgr.get(), &AssetsManager::assetsListChanged, this, &WebAPI_Beam::assetsChanged);

        //
        // THIS IS UI THREAD
        //
        ApiInitData data;

        data.contracts = std::make_shared<WebAPI_Shaders>(appid, appname);
        data.swaps     = nullptr;
        data.wallet    = AppModel::getInstance().getWalletModel()->getWallet();
        data.walletDB  = AppModel::getInstance().getWalletDB();
        data.appId     = _appId;
        data.appName   = _appName;

        _walletAPIProxy = std::make_shared<ApiHandlerProxy>();
        _walletAPI = IWalletApi::CreateInstance(version, *_walletAPIProxy, data);
        LOG_INFO () << "WebAPI_Beam created for " << data.appName << ", " << data.appId;
    }

    std::shared_ptr<WebAPI_Beam> WebAPI_Beam::Create(const std::string& version, const std::string& appid, const std::string& appname)
    {
        auto result = std::make_shared<WebAPI_Beam>(version, appid, appname);
        result->_walletAPIProxy->_handler = result;
        return result;
    }

    WebAPI_Beam::~WebAPI_Beam()
    {
        LOG_INFO () << "WebAPI_Beam Destroyed";

        //
        // THIS IS UI THREAD
        //
        AppModel::getInstance().getWalletModel()->releaseAppsShaders(_appId);
        getAsyncWallet().makeIWTCall(
            [proxy = std::move(_walletAPIProxy), api = std::move(_walletAPI)] () mutable -> boost::any {
                // api should be destroyed in context of the wallet thread
                // it is ASSUMED to be the last call in api calls chain
                api.reset();
                proxy.reset();
                return boost::none;
            },
        [] (const boost::any&){
        });
    }

    QMap<QString, QVariant> WebAPI_Beam::getAssets()
    {
        return _amgr->getAssetsMap(_mappedAssets);
    }

    void WebAPI_Beam::callWalletApi(const QString& request)
    {
        //
        // THIS IS UI THREAD
        //
        LOG_INFO () << "WebAPP API call for " << _appName << ", " << _appId << "): " << request.toStdString();

        std::weak_ptr<WebAPI_Beam> wp = shared_from_this();
        getAsyncWallet().makeIWTCall(
            [wp, this, request]() -> boost::any {
                auto locked = wp.lock();
                if (!locked)
                {
                    // this means that api is disconnected and destroyed already, this is normal
                    return boost::none;
                }

                const auto stdreq = request.toStdString();
                if (auto pres = _walletAPI->parseAPIRequest(stdreq.c_str(), stdreq.size()); pres)
                {
                    const auto& acinfo = pres->acinfo;
                    if (acinfo.appsAllowed)
                    {
                        if (acinfo.method == "tx_send")
                        {
                            AnyThread_getSendConsent(stdreq, *pres);
                            return boost::none;
                        }

                        if (acinfo.method == "process_invoke_data")
                        {
                            AnyThread_getContractInfoConsent(stdreq, *pres);
                            return boost::none;
                        }

                        if (pres->minfo.fee > 0 || !pres->minfo.spend.empty())
                        {
                            LOG_INFO() << "Application called method " << acinfo.method << " that spends funds, but user consent is not handled";
                            assert(false);

                            const auto error = _walletAPI->fromError(stdreq, ApiError::NotAllowedError, std::string());
                            emit callWalletApiResult(QString::fromStdString(error));

                            return boost::none;
                        }

                        _walletAPI->executeAPIRequest(stdreq.c_str(), stdreq.size());
                        return boost::none;
                    }

                    LOG_INFO() << "Application requested call of the not allowed method: " << pres->acinfo.method;
                    AnyThread_sendError(stdreq, ApiError::NotAllowedError, std::string());
                    return boost::none;
                }
                else
                {
                    // parse failed, just log error and return. Error response is already sent back
                    LOG_ERROR() << "WebAPP API parse failed: " << request.toStdString();
                    return boost::none;
                }
            },
            [] (const boost::any&) {
            }
        );
    }

    void WebAPI_Beam::UIThread_callWalletApiImp(const std::string& request)
    {
        //
        // Do not assume thread here
        // Should be safe to call from any thread
        //
        std::weak_ptr<WebAPI_Beam> wp = shared_from_this();
        getAsyncWallet().makeIWTCall(
            [wp, this, request]() -> boost::any {
                auto locked = wp.lock();
                if (!locked)
                {
                    // this means that api is disconnected and destroyed already
                    // well, okay, nothing to do then
                    return boost::none;
                }

                _walletAPI->executeAPIRequest(request.c_str(), request.size());
                return boost::none;
            },
            [] (const boost::any&) {
            }
        );
    }

    void WebAPI_Beam::sendAPIResponse(const json& result)
    {
        //
        // This is reactor thread
        //
        AnyThread_sendAPIResponse(result);
        LOG_INFO() << "sendAPIResponse: " << result.dump();
    }

    void WebAPI_Beam::AnyThread_sendAPIResponse(const beam::wallet::json& result)
    {
        //
        // Do not assume thread here
        // Should be safe to call from any thread
        //
        auto str = result.dump();
        LOG_INFO() << "AnyThread_sendAPIResponse: " << str;
        if (!str.empty())
        {
            emit callWalletApiResult(QString::fromStdString(str));
        }
    }

    void WebAPI_Beam::AnyThread_sendError(const std::string& request, beam::wallet::ApiError err, const std::string& message)
    {
        //
        // Do not assume thread here
        // Should be safe to call from any thread
        //
        const auto error = _walletAPI->fromError(request, err, message);
        emit callWalletApiResult(QString::fromStdString(error));
    }

    int WebAPI_Beam::test()
    {
        // only for test, always 42
        return 42;
    }

    void WebAPI_Beam::AnyThread_getSendConsent(const std::string& request, const beam::wallet::IWalletApi::ParseResult& pinfo)
    {
        std::weak_ptr<WebAPI_Beam> wp = shared_from_this();
        getAsyncWallet().makeIWTCall([] () -> boost::any {return boost::none;},
            [this, wp, request, pinfo](const boost::any&)
            {
                auto locked = wp.lock();
                if (!locked)
                {
                    // Can happen if user leaves the application
                    LOG_WARNING() << "AT -> UIT send consent arrived but creator is already destroyed";
                }

                //
                // THIS IS THE UI THREAD
                //
                decltype(_mappedAssets)().swap(_mappedAssets);
                _mappedAssets.insert(beam::Asset::s_BeamID);

                //
                // Do not assume thread here
                // Should be safe to call from any thread
                //
                const auto &spend = pinfo.minfo.spend;
                const auto fee = pinfo.minfo.fee;

                if (spend.size() != 1)
                {
                    assert(!"tx_send must spend strictly 1 asset");
                    return AnyThread_sendError(request, ApiError::NotAllowedError, "tx_send must spend strictly 1 asset");
                }

                const auto assetId = spend.begin()->first;
                const auto amount = spend.begin()->second;
                _mappedAssets.insert(assetId);

                ApproveMap info;
                info.insert("amount",     AmountBigToUIString(amount));
                info.insert("fee",        AmountToUIString(fee));
                info.insert("feeRate",    AmountToUIString(_amgr->getRate(beam::Asset::s_BeamID)));
                info.insert("assetID",    assetId);
                info.insert("rateUnit",   _amgr->getRateUnit());
                info.insert("token",      QString::fromStdString(pinfo.minfo.token ? *pinfo.minfo.token : std::string()));
                info.insert("isOnline",   !pinfo.minfo.spendOffline);

                std::string comment = pinfo.minfo.confirm_comment ? *pinfo.minfo.confirm_comment : (pinfo.minfo.comment ? *pinfo.minfo.comment : std::string());
                info.insert("comment", QString::fromStdString(comment));

                std::weak_ptr<WebAPI_Beam> wpsel = shared_from_this();
                getAsyncWallet().selectCoins(beam::AmountBig::get_Lo(amount), fee, assetId, false, [this, wpsel, request, info](const CoinsSelectionInfo& csi) mutable {
                    auto locked = wpsel.lock();
                    if (!locked)
                    {
                        // Can happen if user leaves the application
                        LOG_WARNING() << "UIT send CSI arrived but creator is already destroyed";
                    }

                    info.insert("isEnough", csi.m_isEnought);
                    printApproveLog("Get user consent for send", getAppId(), getAppName(), info,ApproveAmounts());
                    emit approveSend(QString::fromStdString(request), info);
                });
            }
        );
    }

    void WebAPI_Beam::AnyThread_getContractInfoConsent(const std::string &request, const beam::wallet::IWalletApi::ParseResult& pinfo)
    {
        std::weak_ptr<WebAPI_Beam> wp = shared_from_this();
        getAsyncWallet().makeIWTCall([] () -> boost::any {return boost::none;},
            [this, wp, request, pinfo](const boost::any&)
            {
                auto locked = wp.lock();
                if (!locked)
                {
                    // Can happen if user leaves the application
                    LOG_WARNING() << "AT -> UIT contract consent arrived but creator is already destroyed";
                }

                //
                // THIS IS THE UI THREAD
                //
                decltype(_mappedAssets)().swap(_mappedAssets);
                _mappedAssets.insert(beam::Asset::s_BeamID);

                ApproveMap info;
                info.insert("comment",   QString::fromStdString(pinfo.minfo.comment ? *pinfo.minfo.comment : std::string()));
                info.insert("fee",       AmountToUIString(pinfo.minfo.fee));
                info.insert("feeRate",         AmountToUIString(_amgr->getRate(beam::Asset::s_BeamID)));
                info.insert("rateUnit",        _amgr->getRateUnit());
                info.insert("isSpend",   !pinfo.minfo.spend.empty());

                std::string comment = pinfo.minfo.confirm_comment ? *pinfo.minfo.confirm_comment : (pinfo.minfo.comment ? *pinfo.minfo.comment : std::string());
                info.insert("comment", QString::fromStdString(comment));

                bool isEnough = true;
                ApproveAmounts amounts;
                for(const auto& sinfo: pinfo.minfo.spend)
                {
                    QMap<QString, QVariant> entry;
                    const auto assetId = sinfo.first;
                    const auto amount  = sinfo.second;

                    _mappedAssets.insert(assetId);
                    entry.insert("amount",   AmountBigToUIString(amount));
                    entry.insert("assetID",  assetId);
                    entry.insert("spend",    true);
                    amounts.push_back(entry);

                    auto totalAmount = amount;
                    if (assetId == beam::Asset::s_BeamID)
                    {
                        totalAmount += beam::AmountBig::Type(pinfo.minfo.fee);
                    }

                    isEnough = isEnough && (totalAmount <= getWallet().getAvailable(assetId));
                }

                for(const auto& sinfo: pinfo.minfo.receive)
                {
                    QMap<QString, QVariant> entry;
                    const auto assetId = sinfo.first;
                    const auto amount  = sinfo.second;

                    _mappedAssets.insert(assetId);
                    entry.insert("amount",   AmountBigToUIString(amount));
                    entry.insert("assetID",  assetId);
                    entry.insert("spend",    false);

                    amounts.push_back(entry);
                }

                info.insert("isEnough", isEnough);
                printApproveLog("Get user consent for contract tx", getAppId(), getAppName(), info, amounts);
                emit approveContractInfo(QString::fromStdString(request), info, amounts);
            }
        );
    }

    void WebAPI_Beam::sendApproved(const QString& request)
    {
        //
        // This is UI thread
        //
        LOG_INFO() << "Contract tx rejected: " << getAppName() << ", " << getAppId() << ", " << request.toStdString();
        UIThread_callWalletApiImp(request.toStdString());
    }

    void WebAPI_Beam::sendRejected(const QString& request)
    {
        //
        // This is UI thread
        //
        LOG_INFO() << "Contract tx rejected: " << getAppName() << ", " << getAppId() << ", " << request.toStdString();
        AnyThread_sendError(request.toStdString(), beam::wallet::ApiError::UserRejected, std::string());
    }

    void WebAPI_Beam::contractInfoApproved(const QString& request)
    {
        //
        // This is UI thread
        //
        LOG_INFO() << "Contract tx rejected: " << getAppName() << ", " << getAppId() << ", " << request.toStdString();
        UIThread_callWalletApiImp(request.toStdString());
    }

    void WebAPI_Beam::contractInfoRejected(const QString& request)
    {
        //
        // This is UI thread
        //
        LOG_INFO() << "Contract tx rejected: " << getAppName() << ", " << getAppId() << ", " << request.toStdString();
        AnyThread_sendError(request.toStdString(), beam::wallet::ApiError::UserRejected, std::string());
    }
}
