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
#pragma once

#include "model/app_model.h"
#include "wallet/api/i_wallet_api.h"
#include "wallet/core/contracts/i_shaders_manager.h"
#include "wallet/client/apps_api/apps_api.h"

namespace beamui::applications
{
    class WebAPI_Beam
        : public QObject
        , public beam::wallet::AppsApi<WebAPI_Beam>
        , public std::enable_shared_from_this<WebAPI_Beam>
    {
    private:
        Q_OBJECT
        Q_PROPERTY(QMap<QString, QVariant> assets READ getAssets NOTIFY assetsChanged)

        friend class beam::wallet::AppsApi<WebAPI_Beam>;
        WebAPI_Beam(const std::string& appid, const std::string& appname);
        ~WebAPI_Beam() override = default;

        void AnyThread_sendApiResponse(const std::string& result) override;
        void ClientThread_getSendConsent(const std::string& request, const nlohmann::json& info, const nlohmann::json& amounts) override;
        void ClientThread_getContractConsent(const std::string& request, const nlohmann::json& info, const nlohmann::json& amounts) override;

    public:
        [[nodiscard]] QMap<QString, QVariant> getAssets();

        Q_INVOKABLE int test();
        Q_INVOKABLE void callWalletApi(const QString& request);
        Q_INVOKABLE void sendApproved(const QString& request);
        Q_INVOKABLE void sendRejected(const QString& request);
        Q_INVOKABLE void contractInfoApproved(const QString& request);
        Q_INVOKABLE void contractInfoRejected(const QString& request);

    signals:
        void callWalletApiResult(const QString& result);
        void assetsChanged();
        void approveSend(const QString& request, const QMap<QString, QVariant>& info);
        void approveContractInfo(const QString& request, const QMap<QString, QVariant>& info, const QList<QMap<QString, QVariant>>& amounts);

    private:
        AssetsManager::Ptr _amgr;
        std::set<beam::Asset::ID> _mappedAssets;
    };
}
