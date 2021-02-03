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
#include "apps_api.h"

class AppsApiClient
    : public AppsApi // We provide wallet service API
{
public:
    struct IHandler
    {
        virtual void onInvokeContract(const beam::wallet::JsonRpcId& id, const InvokeContract& data) = 0;
    };

    explicit AppsApiClient(IHandler& handler);
    ~AppsApiClient() = default;

    //
    // Apps Api
    //
    void onAppsApiMessage(const beam::wallet::JsonRpcId& id, const InvokeContract& data) override
    {
        _handler.onInvokeContract(id, data);
    }

    //
    // Methods below must be called in the main (non-UI) reactor thread
    //
    std::string executeAPIRequest(const std::string&);

    //
    // WalletApi required overrides. Called in the main (non-UI) reactor thread
    //
    void sendMessage(const nlohmann::json& msg) override;

private:
    // this should be used ONLY in reactor thread
    std::string _lastResult;
    IHandler& _handler;
};
