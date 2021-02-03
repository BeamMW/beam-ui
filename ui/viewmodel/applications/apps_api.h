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

#include "wallet/core/common.h"
#include "wallet/api/wallet_api.h"

struct InvokeContract
{
    std::vector<uint8_t> contract;
    std::string args;

    struct Response
    {
        std::string output;
        std::string txid;
    };
};

#define APPS_API_METHODS(macro) \
    macro(InvokeContract, "invoke_contract", API_WRITE_ACCESS)

class AppsApi
    : public beam::wallet::WalletApi
{
public:
    AppsApi(beam::wallet::IWalletDB::Ptr wdb, beam::wallet::Wallet::Ptr wallet, beam::wallet::ISwapsProvider::Ptr swaps);
    AppsApi(const AppsApi&) = delete;
    ~AppsApi() = default;

    #define RESPONSE_FUNC(api, name, _) \
    void getAppsApiResponse(const beam::wallet::JsonRpcId& id, const api::Response& data, nlohmann::json& msg);
    APPS_API_METHODS(RESPONSE_FUNC)
    #undef RESPONSE_FUNC

protected:
    #define MESSAGE_FUNC(api, name, _) \
    virtual void onAppsApiMessage(const beam::wallet::JsonRpcId& id, const api& data) = 0;
    APPS_API_METHODS(MESSAGE_FUNC)
    #undef MESSAGE_FUNC

private:
    #define MESSAGE_FUNC(api, name, _) \
    void on##api##Message(const beam::wallet::JsonRpcId& id, const nlohmann::json& msg);
    APPS_API_METHODS(MESSAGE_FUNC)
    #undef MESSAGE_FUNC
};
