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

#include <QObject>
#include "consent_handler.h"
#include "webapi_beam.h"
#include "webapi_shaders.h"
#include "viewmodel/wallet/assets_list.h"

namespace beamui::applications
{
    class WebAPICreator
        : public QObject
        , public IConsentHandler
    {
        Q_OBJECT
        Q_PROPERTY(QMap<QString, QVariant> assets READ getAssets NOTIFY assetsChanged)
        Q_PROPERTY(QObject* api READ getApi NOTIFY apiChanged)

    public:
        explicit WebAPICreator(QObject *parent = nullptr);
        ~WebAPICreator() override;

        Q_INVOKABLE void createApi(const QString& verWant, const QString& verMin, const QString& appName, const QString& appUrl);
        Q_INVOKABLE void sendApproved(const QString& request);
        Q_INVOKABLE void sendRejected(const QString& request);
        Q_INVOKABLE void contractInfoApproved(const QString& request);
        Q_INVOKABLE void contractInfoRejected(const QString& request);
        Q_INVOKABLE bool apiSupported(const QString& apiVersion) const;
        Q_INVOKABLE QString generateAppID(const QString& appName, const QString& appUrl);

        [[nodiscard]] QMap<QString, QVariant> getAssets();
        [[nodiscard]] QObject* getApi();

    signals:
        void apiCreated(const QString& appid);
        void approveSend(const QString& request, const QMap<QString, QVariant>& info);
        void approveContractInfo(const QString& request, const QMap<QString, QVariant>& info, QList<QMap<QString, QVariant>> amounts);
        void assetsChanged();
        void apiChanged();

    private:
        void AnyThread_getSendConsent(const std::string& request, const beam::wallet::IWalletApi::ParseResult&) override;
        void AnyThread_getContractInfoConsent(const std::string &request, const beam::wallet::IWalletApi::ParseResult &) override;

        void UIThread_getSendConsent(const std::string& request, const beam::wallet::IWalletApi::ParseResult&);
        void UIThread_getContractInfoConsent(const std::string& request, const beam::wallet::IWalletApi::ParseResult&);

        std::shared_ptr<WebAPI_Beam> _api;
        AssetsManager::Ptr _amgr;
        WalletModel::Ptr _wallet;
        beam::wallet::IWalletModelAsync::Ptr _asyncWallet;

        std::shared_ptr<bool> _sendConsentGuard = std::make_shared<bool>(true);
        std::shared_ptr<bool> _sendCSIGuard = std::make_shared<bool>(true);
        std::shared_ptr<bool> _contractConsentGuard = std::make_shared<bool>(true);

        std::set<beam::Asset::ID> _mappedAssets;
    };
}
