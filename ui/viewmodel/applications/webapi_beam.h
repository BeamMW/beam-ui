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
#include "apps_api_client.h"
#include "shaders_manager.h"

namespace beamui::applications {
    class WebAPI_Beam
            : public QObject
            , public AppsApiClient::IHandler
    {
    Q_OBJECT
    public:
        explicit WebAPI_Beam(QObject *parent = nullptr);

    //
    // Slots below are called by web in context of the UI thread
    //
    public slots:
       int test();
       void callWalletApi(const QString& request);

    //
    // Signals are received by web, should be fired in context of the UI thread
    //
    signals:
        void callWalletApiResult(const QString& result);

    private:
        //
        // AppsApiClient::IHandler
        // This callback would be called in context of reactor thread
        //
        void onInvokeContract(const beam::wallet::JsonRpcId& id, const InvokeContract& data) override;

        //
        // ApiClient should be called only in context of reactor thread
        //
        typedef std::shared_ptr<AppsApiClient> ApiClientPtr;
        typedef std::weak_ptr<AppsApiClient> WeakApiClientPtr;
        ApiClientPtr   _apiClient;
    };
}
