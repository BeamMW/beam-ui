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
#include <QQmlEngine>
#include <sstream>
#include "webapi_beam.h"
#include "utility/logger.h"
#include "model/app_model.h"

namespace beamui::applications {
    using namespace beam::wallet;

    namespace {
        WalletModel& getWallet() {
            return *AppModel::getInstance().getWalletModel();
        }

        IWalletModelAsync& getAsyncWallet() {
            return *getWallet().getAsync();
        }
    }

    WebAPI_Beam::WebAPI_Beam(IConsentHandler& handler, const std::string& version, const std::string& appid)
        : QObject(nullptr)
        , _consentHandler(handler)
    {
        IWalletApi::InitData data;

        data.contracts = AppModel::getInstance().getWalletModel()->getAppsShaders();
        data.swaps     = nullptr;
        data.wallet    = AppModel::getInstance().getWalletModel()->getWallet();
        data.walletDB  = AppModel::getInstance().getWalletDB();
        data.appid     = appid;

        _walletAPI = IWalletApi::CreateInstance(version, *this, data);
    }

    WebAPI_Beam::~WebAPI_Beam()
    {
    }

    void WebAPI_Beam::callWalletApi(const QString& request)
    {
        //
        // THIS IS THE UI THREAD
        //
        LOG_INFO () << "WebAPP API call: " << request.toStdString();

        const auto stdreq = request.toStdString();
        if (auto pres = _walletAPI->parseAPIRequest(stdreq.c_str(), stdreq.size()); pres)
        {
            if (pres->acinfo.appsAllowed)
            {
                if (pres->acinfo.method == "tx_send")
                {
                    return _consentHandler.getSendConsent(stdreq, *pres);
                }

                if (pres->acinfo.method == "invoke_contract")
                {
                    // TODO: handle consent
                    return callWalletApiImp(stdreq);
                }

                if (pres->minfo.fee > 0 || !pres->minfo.spend.empty())
                {
                    LOG_INFO() << "Application called method " << pres->acinfo.method << " that spends funds, but user consent is not handled";
                    assert(false);

                    const auto error = _walletAPI->fromError(stdreq, ApiError::NotAllowedError, std::string());
                    emit callWalletApiResult(QString::fromStdString(error));

                    return;
                }

                return callWalletApiImp(stdreq);
            }

            LOG_INFO() << "Application requested call of the not allowed method: " << pres->acinfo.method;
            return sendError(stdreq, ApiError::NotAllowedError, std::string());
        }
        else
        {
            // parse failed, just log error and return. Error response is already sent back
            LOG_ERROR() << "WebAPP API parse failed: " << request.toStdString();
            return;
        }
    }

    void WebAPI_Beam::callWalletApiImp(const std::string& request)
    {
        //
        // this can be any thread, for now the UI thread only
        //
        IWalletApi::WeakPtr wp = _walletAPI;
        getAsyncWallet().makeIWTCall(
            [wp, request]() -> boost::any {
                if(auto sp = wp.lock())
                {
                    sp->executeAPIRequest(request.c_str(), request.size());
                    return boost::none;
                }
                // this means that api is disconnected and destroyed already
                // well, okay, nothing to do then
                return std::string();
            },
            [] (boost::any) {
            }
        );
    }

    void WebAPI_Beam::sendAPIResponse(const json& result)
    {
        auto str = result.dump();
        if (!str.empty())
        {
            emit callWalletApiResult(QString::fromStdString(str));
        }
    }

     void WebAPI_Beam::sendError(const std::string& request, beam::wallet::ApiError err, const std::string& message)
     {
        const auto error = _walletAPI->fromError(request, err, message);
        emit callWalletApiResult(QString::fromStdString(error));
     }

    int WebAPI_Beam::test()
    {
        // only for test, always 42
        return 42;
    }

    WebAPICreator::WebAPICreator(QObject *parent)
        : QObject(parent)
    {
        _amgr = AppModel::getInstance().getAssets();
    }

    WebAPICreator::~WebAPICreator()
    {
    }

    void WebAPICreator::createApi(const QString &version, const QString &appName, const QString &appUrl)
    {
        const auto stdver = version.toStdString();
        if (!IWalletApi::ValidateAPIVersion(stdver))
        {
            //% "Unsupported API version requested: %1"
            const auto error = qtTrId("apps-bad-api-version").arg(version);
            return qmlEngine(this)->throwError(error);
        }

        ECC::Hash::Value hv;
        ECC::Hash::Processor() << appName.toStdString() << appUrl.toStdString() >> hv;
        const auto appid = std::string("appid:") + hv.str();

        _api = std::make_unique<WebAPI_Beam>(*this, stdver, appid);
        QQmlEngine::setObjectOwnership(_api.get(), QQmlEngine::CppOwnership);
        emit apiCreated(_api.get());
    }

    void WebAPICreator::getSendConsent(const std::string& request, const beam::wallet::IWalletApi::ParseResult& pinfo)
    {
        //
        // THIS IS THE UI THREAD
        //
        const auto& spend = pinfo.minfo.spend;
        const auto fee = pinfo.minfo.fee;

        if (spend.size() != 1)
        {
            assert(!"tx_send must spend strictly 1 asset");
            return _api->sendError(request, ApiError::NotAllowedError, "tx_send must spend strictly 1 asset");
        }

        const auto assetId = spend.begin()->first;
        const auto amount = spend.begin()->second;

        QMap<QString, QVariant> info;
        info.insert("amount",     AmountBigToUIString(amount));
        info.insert("fee",        AmountToUIString(fee));
        info.insert("feeRate",    AmountToUIString(_amgr->getRate(beam::Asset::s_BeamID)));
        info.insert("unitName",   _amgr->getUnitName(assetId, AssetsManager::NoShorten));
        info.insert("rate",       AmountToUIString(_amgr->getRate(assetId)));
        info.insert("rateUnit",   _amgr->getRateUnit());
        info.insert("token",      QString::fromStdString(pinfo.minfo.token));
        info.insert("tokenType",  GetTokenTypeUIString(pinfo.minfo.token, pinfo.minfo.spendOffline));
        info.insert("isOnline",   !pinfo.minfo.spendOffline);
        info.insert("comment",    QString::fromStdString(pinfo.minfo.comment));

        if (const auto params = ParseParameters(pinfo.minfo.token))
        {
            if (const auto walletID = params->GetParameter<beam::wallet::WalletID>(TxParameterID::PeerID))
            {
                const auto widStr = std::to_string(*walletID);
                info.insert("walletID", QString::fromStdString(widStr));
            }
            else
            {
                assert(!"Wallet ID is missing");
            }
        }
        else
        {
            assert(!"Failed to parse token");
        }

        emit approveSend(QString::fromStdString(request), info);
    }

    void WebAPICreator::requestApproved(const QString& request)
    {
        _api->callWalletApiImp(request.toStdString());
    }

    void WebAPICreator::requestRejected(const QString& request)
    {
        _api->sendError(request.toStdString(), ApiError::UserRejected, std::string());
    }
}
