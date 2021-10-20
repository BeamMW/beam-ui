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
#include "viewmodel/ui_helpers.h"

namespace beamui::applications {
    using namespace beam::wallet;

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
        LOG_INFO() << "Contract tx approved: " << getAppName() << ", " << getAppId() << ", " << request.toStdString();
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

    QString AppsApiUI::prepareInfo4QT(const nlohmann::json& info)
    {
        nlohmann::json result = json::object();
        for (const auto& kv: info.items())
        {
            if (kv.key() == "fee")
            {
                const auto fee = AmountToUIString(info["fee"].get<beam::Amount>());
                result["fee"] = fee.toStdString();

                const auto feeRate = AmountToUIString(_amgr->getRate(beam::Asset::s_BeamID));
                result["feeRate"]  = feeRate.toStdString();
                result["rateUnit"] = _amgr->getRateUnit().toStdString();
            }
            else
            {
                result.push_back({kv.key(), kv.value()});
            }
        }

        return QString::fromStdString(result.dump());
    }

    QString AppsApiUI::prepareAmounts4QT(const nlohmann::json& amounts)
    {
        decltype(_mappedAssets)().swap(_mappedAssets);
        _mappedAssets.insert(beam::Asset::s_BeamID);

        nlohmann::json result = json::array();
        for (const auto& val: amounts)
        {
            const auto assetId = val["assetID"].get<beam::Asset::ID>();
            const auto amount = val["amount"].get<beam::Amount>();
            const auto spend = val["spend"].get<bool>();

            _mappedAssets.insert(assetId);
            result.push_back({
                {"assetID", assetId},
                {"amount", AmountToUIString(amount).toStdString()},
                {"spend", spend}
            });
        }

        return QString::fromStdString(result.dump());
    }

    void AppsApiUI::ClientThread_getSendConsent(const std::string& request, const nlohmann::json& jinfo, const nlohmann::json& jamounts)
    {
        if (!jamounts.is_array() || jamounts.size() != 1)
        {
            assert(false);
            return AnyThread_sendApiError(request, ApiError::NotAllowedError, "send must spend strictly 1 asset");
        }

        const auto info = prepareInfo4QT(jinfo);
        const auto amounts = prepareAmounts4QT(jamounts);
        emit approveSend(QString::fromStdString(request), info, amounts);
    }

    void AppsApiUI::ClientThread_getContractConsent(const std::string& request, const nlohmann::json& jinfo, const nlohmann::json& jamounts)
    {
        const auto info = prepareInfo4QT(jinfo);
        const auto amounts = prepareAmounts4QT(jamounts);
        emit approveContractInfo(QString::fromStdString(request), info, amounts);
    }
}
