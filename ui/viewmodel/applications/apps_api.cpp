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
#include "apps_api.h"

AppsApi::AppsApi(beam::wallet::IWalletDB::Ptr wdb, beam::wallet::Wallet::Ptr wallet, beam::wallet::ISwapsProvider::Ptr swaps)
    : WalletApi(std::move(wdb), std::move(wallet), std::move(swaps), boost::none)
{
    using namespace beam;

    #define REG_FUNC(api, name, writeAccess) \
        _methods[name] = {BIND_THIS_MEMFN(on##api##Message), writeAccess};
    APPS_API_METHODS(REG_FUNC)
    #undef REG_FUNC
}

void AppsApi::onInvokeContractMessage(const beam::wallet::JsonRpcId& id, const nlohmann::json& params)
{
    using namespace beam::wallet;

    const char* CONTRACT = "contract";
    const char* ARGS = "args";
    InvokeContract message;

    if(const auto contract = getOptionalParam<JsonArray>(params, CONTRACT))
    {
        message.contract = params[CONTRACT].get<std::vector<uint8_t>>();
    }

    if (const auto args = getOptionalParam<NonEmptyString>(params, ARGS))
    {
        message.args = params[ARGS].get<std::string>();
    }

    onAppsApiMessage(id, message);
}

void AppsApi::getAppsApiResponse(const beam::wallet::JsonRpcId& id, const InvokeContract::Response& res, nlohmann::json& msg)
{
    msg = nlohmann::json
    {
        {JsonRpcHeader, JsonRpcVersion},
        {"id", id},
        {"result",
            {
                {"output", res.output},
                {"txid",   res.txid}
            }
        }
    };
}
