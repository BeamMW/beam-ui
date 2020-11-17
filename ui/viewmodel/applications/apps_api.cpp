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

AppsApi::AppsApi(IWalletData& walletData)
    : WalletApi(*this, boost::none)
    , WalletApiHandler(walletData, boost::none)
{
    using namespace beam;

    #define REG_FUNC(api, name, writeAccess) \
        _methods[name] = {BIND_THIS_MEMFN(on##api##Message), writeAccess};
    APPS_API_METHODS(REG_FUNC)
    #undef REG_FUNC
}

AppsApi::~AppsApi()
{
}

void AppsApi::onInvokeContractMessage(const beam::wallet::JsonRpcId& id, const nlohmann::json& params)
{
    using namespace beam::wallet;

    const char* CONTRACT = "contract";
    const char* ARGS = "args";
    InvokeContract message;

    if(existsJsonParam(params, CONTRACT))
    {
        if (!params[CONTRACT].is_array())
        {
            throw jsonrpc_exception{ApiError::InvalidJsonRpc, "contract must be a byte array", id};

        }
        message.contract = params[CONTRACT].get<std::vector<uint8_t>>();
    }

    if(existsJsonParam(params, ARGS))
    {
        if (!params[ARGS].is_string())
        {
            throw jsonrpc_exception{ApiError::InvalidJsonRpc, "args must be a string", id};
        }
        message.args = params[ARGS].get<std::string>();
    }

    onAppsApiMessage(id, message);
}

void AppsApi::getAppsApiResponse(const beam::wallet::JsonRpcId& id, const InvokeContract::Response& res, nlohmann::json& msg)
{
    msg = nlohmann::json
    {
        {JsonRpcHrd, JsonRpcVerHrd},
        {"id",       id},
        {"result",
            {
                {"output", res.output},
                {"txid",   res.txid}
            }
        }
    };
}
