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
#include <QQmlEngine>
#include "webapi_creator.h"
#include "wallet/api/i_wallet_api.h"
#include "wallet/core/common.h"
#include "bvm/invoke_data.h"

namespace beamui::applications
{
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
            LOG_INFO() << preamble << "(" << appname << ", " << appid << "):";
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

    WebAPICreator::WebAPICreator(QObject *parent)
        : QObject(parent)
    {
        _amgr = AppModel::getInstance().getAssets();
        _asyncWallet = AppModel::getInstance().getWalletModel()->getAsync();
    }

    void WebAPICreator::createApi(const QString &version, const QString &appName, const QString &appUrl)
    {
        using namespace beam::wallet;

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

        _webShaders = std::make_shared<WebAPI_Shaders>(appid, appName.toStdString());
        _api = std::make_unique<WebAPI_Beam>(*this, _webShaders, stdver, appid, appName.toStdString());

        QQmlEngine::setObjectOwnership(_api.get(), QQmlEngine::CppOwnership);
        emit apiCreated(_api.get());
        LOG_INFO() << "API created: " << stdver << ", " << appName.toStdString() << ", " << appid;
    }

    void WebAPICreator::AnyThread_getSendConsent(const std::string& request, const beam::wallet::IWalletApi::ParseResult& pinfo)
    {
        std::weak_ptr<bool> wp = _sendConsentGuard;
        _asyncWallet->makeIWTCall([] () -> boost::any {return boost::none;},
            [this, wp, request, pinfo](const boost::any&)
            {
                if (wp.lock())
                {
                    UIThread_getSendConsent(request, pinfo);
                }
                else
                {
                    // Can happen if user leaves the application
                    LOG_WARNING() << "AT -> UIT send consent arrived but creator is already destroyed";
                }
            }
        );
    }

    void WebAPICreator::AnyThread_getContractInfoConsent(const std::string &request, const beam::wallet::IWalletApi::ParseResult& pinfo)
    {
        std::weak_ptr<bool> wp = _sendConsentGuard;
        _asyncWallet->makeIWTCall([] () -> boost::any {return boost::none;},
            [this, wp, request, pinfo](const boost::any&)
            {
                if (wp.lock())
                {
                    UIThread_getContractInfoConsent(request, pinfo);
                }
                else
                {
                    // Can happen if user leaves the application
                    LOG_WARNING() << "AT -> UIT contract consent arrived but creator is already destroyed";
                }
            }
        );
    }

    void WebAPICreator::UIThread_getSendConsent(const std::string& request, const beam::wallet::IWalletApi::ParseResult& pinfo)
    {
        using namespace beam::wallet;

        //
        // Do not assume thread here
        // Should be safe to call from any thread
        //
        const auto& spend = pinfo.minfo.spend;
        const auto fee = pinfo.minfo.fee;

        if (spend.size() != 1)
        {
            assert(!"tx_send must spend strictly 1 asset");
            return _api->AnyThread_sendRejected(request, ApiError::NotAllowedError, "tx_send must spend strictly 1 asset");
        }

        const auto assetId = spend.begin()->first;
        const auto amount = spend.begin()->second;

        ApproveMap info;
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
        }
        else
        {
            assert(!"Failed to parse token");
        }

        printApproveLog("Get user consent for send", _api->getAppId(), _api->getAppName(), info, ApproveAmounts());
        emit approveSend(QString::fromStdString(request), info);
    }

    void WebAPICreator::UIThread_getContractInfoConsent(const std::string& request, const beam::wallet::IWalletApi::ParseResult& pinfo)
    {
        ApproveMap info;
        info.insert("comment",   QString::fromStdString(pinfo.minfo.comment));
        info.insert("fee",       AmountToUIString(pinfo.minfo.fee));
        info.insert("feeRate",   AmountToUIString(_amgr->getRate(beam::Asset::s_BeamID)));
        info.insert("rateUnit",  _amgr->getRateUnit());

        ApproveAmounts amounts;
        for(const auto& sinfo: pinfo.minfo.spend)
        {
            QMap<QString, QVariant> entry;
            const auto assetId = sinfo.first;
            const auto amount  = sinfo.second;

            entry.insert("amount",    AmountBigToUIString(amount));
            entry.insert("unitName", _amgr->getUnitName(assetId, AssetsManager::NoShorten));
            entry.insert("rate",     AmountToUIString(_amgr->getRate(assetId)));
            entry.insert("spend",    true);

            amounts.push_back(entry);
        }

        for(const auto& sinfo: pinfo.minfo.receive)
        {
            QMap<QString, QVariant> entry;
            const auto assetId = sinfo.first;
            const auto amount  = sinfo.second;

            entry.insert("amount",    AmountBigToUIString(amount));
            entry.insert("unitName", _amgr->getUnitName(assetId, AssetsManager::NoShorten));
            entry.insert("rate",     AmountToUIString(_amgr->getRate(assetId)));
            entry.insert("spend",    false);

            amounts.push_back(entry);
        }

        printApproveLog("Get user consent for contract tx", _api->getAppId(), _api->getAppName(), info, amounts);
        emit approveContractInfo(QString::fromStdString(request), info, amounts);
    }

    void WebAPICreator::sendApproved(const QString& request)
    {
        //
        // This is UI thread
        //
        LOG_INFO() << "Contract tx rejected: " << _api->getAppName() << ", " << _api->getAppId() << ", " << request.toStdString();
        _api->AnyThread_sendApproved(request.toStdString());
    }

    void WebAPICreator::sendRejected(const QString& request)
    {
        //
        // This is UI thread
        //
        LOG_INFO() << "Contract tx rejected: " << _api->getAppName() << ", " << _api->getAppId() << ", " << request.toStdString();
        _api->AnyThread_sendRejected(request.toStdString(), beam::wallet::ApiError::UserRejected, std::string());
    }

    void WebAPICreator::contractInfoApproved(const QString& request)
    {
        //
        // This is UI thread
        //
        LOG_INFO() << "Contract tx rejected: " << _api->getAppName() << ", " << _api->getAppId() << ", " << request.toStdString();
        _api->AnyThread_contractInfoApproved(request.toStdString());
    }

    void WebAPICreator::contractInfoRejected(const QString& request)
    {
        //
        // This is UI thread
        //
        LOG_INFO() << "Contract tx rejected: " << _api->getAppName() << ", " << _api->getAppId() << ", " << request.toStdString();
        _api->AnyThread_contractInfoRejected(request.toStdString(), beam::wallet::ApiError::UserRejected, std::string());
    }
}
