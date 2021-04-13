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
#include "webapi_shaders.h"
#include "wallet/api/i_wallet_api.h"

namespace beamui::applications
{
    struct IConsentHandler
    {
        virtual void getSendConsent(const std::string& request, const beam::wallet::IWalletApi::ParseResult&) = 0;
    };

    class WebAPI_Beam
        : public QObject
        , public beam::wallet::IWalletApiHandler
    {
        Q_OBJECT
    public:
        explicit WebAPI_Beam(IConsentHandler& handler, const std::string& version, const std::string& appid);
        ~WebAPI_Beam() override;

    //
    // Slots below are called by web in context of the UI thread
    //
    public slots:
       int test();
       void callWalletApi(const QString& request);

    //
    // Signals are received by web
    //
    signals:
        void callWalletApiResult(const QString& result);

    public:
        // This can be called from any thread.
        void callWalletApiImp(const std::string& request);

        // This can be called from any thread
        void sendError(const std::string& request, beam::wallet::ApiError err, const std::string& message);

    private:
        // This can be called from any thread
        void sendAPIResponse(const beam::wallet::json& result) override;

        // API should be accessed only in context of the reactor thread
        beam::wallet::IWalletApi::Ptr _walletAPI;
        IConsentHandler& _consentHandler;
        std::string _appid;
    };

    class WebAPICreator
            : public QObject
            , public IConsentHandler
    {
        Q_OBJECT
    public:
        explicit WebAPICreator(QObject *parent = nullptr);
        ~WebAPICreator() override;

        Q_INVOKABLE void createApi(const QString& version, const QString& appName, const QString& appUrl);
        Q_INVOKABLE void requestApproved(const QString& request);
        Q_INVOKABLE void requestRejected(const QString& request);

    signals:
        void apiCreated(QObject* api);
        void apiFailed(const QString& error);
        void approveSend(const QString& request, const QMap<QString, QVariant>& info);

    private:
        void getSendConsent(const std::string& request, const beam::wallet::IWalletApi::ParseResult&) override;
        std::unique_ptr<WebAPI_Beam> _api;
        AssetsManager::Ptr _amgr;
    };
}
