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
#include "consent_handler.h"

namespace beamui::applications
{
    class WebAPI_Beam
        : public QObject
        , public beam::wallet::IWalletApiHandler
    {
        Q_OBJECT
    public:
        explicit WebAPI_Beam(IConsentHandler& handler,
                             beam::wallet::IShadersManager::Ptr shaders,
                             const std::string& version,
                             const std::string& appid,
                             const std::string& appname);

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
        void AnyThread_sendApproved(const std::string& request);
        void AnyThread_sendRejected(const std::string& request, beam::wallet::ApiError err, const std::string& message);
        void AnyThread_contractInfoApproved(const std::string& request);
        void AnyThread_contractInfoRejected(const std::string& request, beam::wallet::ApiError err, const std::string& message);

        [[nodiscard]] std::string getAppId() const
        {
            return _appId;
        }

        [[nodiscard]] std::string getAppName() const
        {
            return _appName;
        }

    private:
        // This can be called from any thread.
        void AnyThread_callWalletApiImp(const std::string& request);

        // This can be called from any thread
        void AnyThread_sendError(const std::string& request, beam::wallet::ApiError err, const std::string& message);

        // This can be called from any thread
        void AnyThread_sendAPIResponse(const beam::wallet::json& result);

        // This is called from API (REACTOR) thread
        void sendAPIResponse(const beam::wallet::json& result) override;

        // API should be accessed only in context of the reactor thread
        beam::wallet::IWalletApi::Ptr _walletAPI;
        IConsentHandler& _consentHandler;
        std::string _appId;
        std::string _appName;
    };
}
