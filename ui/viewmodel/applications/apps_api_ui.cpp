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
#include "apps_api_ui.h"
#include "utility/logger.h"
#include "model/app_model.h"

namespace beamui::applications {
    using namespace beam::wallet;

    namespace
    {
        namespace
        {
            typedef QList<QMap<QString, QVariant>> ApproveAmounts;
            typedef QMap<QString, QVariant> ApproveMap;
        }
    }

    AppsApiUI::AppsApiUI(const std::string& appid, const std::string& appname)
        : QObject(nullptr)
        , AppsApi<AppsApiUI>(appid, appname)
        , _amgr(AppModel::getInstance().getAssets())
    {
        // THIS SHOULD BE UI THREAD to let Qt register metatypes
        connect(_amgr.get(), &AssetsManager::assetsListChanged, this, &AppsApiUI::assetsChanged);
    }

    void AppsApiUI::AnyThread_sendApiResponse(const std::string& result)
    {
        //
        // Do not assume thread here
        // Should be safe to call from any thread
        //
        emit callWalletApiResult(QString::fromStdString(result));
    }

    QMap<QString, QVariant> AppsApiUI::getAssets()
    {
        return _amgr->getAssetsMap(_mappedAssets);
    }

    int AppsApiUI::test()
    {
        // only for test, always 42
        return 42;
    }

    void AppsApiUI::sendApproved(const QString& request)
    {
        //
        // This is UI thread
        //
        LOG_INFO() << "Contract tx rejected: " << getAppName() << ", " << getAppId() << ", " << request.toStdString();
        AnyThread_callWalletApiDirectly(request.toStdString());
    }

    void AppsApiUI::sendRejected(const QString& request)
    {
        //
        // This is UI thread
        //
        LOG_INFO() << "Contract tx rejected: " << getAppName() << ", " << getAppId() << ", " << request.toStdString();
        AnyThread_sendApiError(request.toStdString(), beam::wallet::ApiError::UserRejected, std::string());
    }

    void AppsApiUI::contractInfoApproved(const QString& request)
    {
        //
        // This is UI thread
        //
        LOG_INFO() << "Contract tx rejected: " << getAppName() << ", " << getAppId() << ", " << request.toStdString();
        AnyThread_callWalletApiDirectly(request.toStdString());
    }

    void AppsApiUI::contractInfoRejected(const QString& request)
    {
        //
        // This is UI thread
        //
        LOG_INFO() << "Contract tx rejected: " << getAppName() << ", " << getAppId() << ", " << request.toStdString();
        AnyThread_sendApiError(request.toStdString(), beam::wallet::ApiError::UserRejected, std::string());
    }

    void AppsApiUI::callWalletApi(const QString& request)
    {
        AnyThread_callWalletApiChecked(request.toStdString());
    }

    void AppsApiUI::ClientThread_getSendConsent(const std::string& request, const nlohmann::json& jinfo, const nlohmann::json& amounts)
    {
        ApproveMap info;

        const auto comment = QString::fromStdString(jinfo["comment"].get<std::string>());
        info.insert("comment", comment);

        const auto fee = AmountToUIString(jinfo["fee"].get<beam::Amount>());
        info.insert("fee", fee);

        const auto feeRate = AmountToUIString(_amgr->getRate(beam::Asset::s_BeamID));
        info.insert("feeRate", feeRate);

        const auto token = QString::fromStdString(jinfo["token"].get<std::string>());
        info.insert("token", token);

        info.insert("rateUnit", _amgr->getRateUnit());
        info.insert("isOnline",  jinfo["isOnline"].get<bool>());

        if (!amounts.is_array() || amounts.size() != 1)
        {
            assert(!"send must spend strictly 1 asset");
            return AnyThread_sendApiError(request, ApiError::NotAllowedError, "send must spend strictly 1 asset");
        }

        const auto amount = (*amounts.begin())["amount"].get<beam::Amount>();
        info.insert("amount", AmountToUIString(amount));

        const auto assetID = (*amounts.begin())["assetID"].get<beam::Asset::ID>();
        info.insert("assetID", assetID);

        decltype(_mappedAssets)().swap(_mappedAssets);
        _mappedAssets.insert(beam::Asset::s_BeamID);
        _mappedAssets.insert(assetID);

        emit approveSend(QString::fromStdString(request), info);
    }

    void AppsApiUI::ClientThread_getContractConsent(const std::string& request, const nlohmann::json& jinfo, const nlohmann::json& jamounts)
    {
        ApproveMap info;

        const auto comment = QString::fromStdString(jinfo["comment"].get<std::string>());
        info.insert("comment", comment);

        const auto fee = AmountToUIString(jinfo["fee"].get<beam::Amount>());
        info.insert("fee", fee);

        const auto feeRate = AmountToUIString(_amgr->getRate(beam::Asset::s_BeamID));
        info.insert("feeRate", feeRate);

        info.insert("rateUnit", _amgr->getRateUnit());
        info.insert("isSpend",  jinfo["isSpend"].get<bool>());
        info.insert("isEnough", jinfo["isEnough"].get<bool>());

        decltype(_mappedAssets)().swap(_mappedAssets);
        _mappedAssets.insert(beam::Asset::s_BeamID);

        ApproveAmounts amounts;
        for(const auto& jamount: jamounts)
        {
            const auto assetId = jamount["assetID"].get<beam::Asset::ID>();
            _mappedAssets.insert(assetId);

            QMap<QString, QVariant> entry;

            const auto amount = AmountToUIString(jamount["amount"].get<beam::Amount>());
            entry.insert("amount", amount);

            const auto spend = jamount["spend"].get<bool>();
            entry.insert("spend", spend);

            entry.insert("assetID", assetId);
            amounts.push_back(entry);
        }

        emit approveContractInfo(QString::fromStdString(request), info, amounts);
    }
}
