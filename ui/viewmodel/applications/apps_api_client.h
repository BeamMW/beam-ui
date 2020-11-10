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

#ifdef BEAM_ATOMIC_SWAP_SUPPORT
#include "wallet/api/i_atomic_swap_provider.h"
#endif

class AppsApiClient
    : private AppsApi // We provide wallet service API
    , private beam::wallet::WalletApiHandler::IWalletData  // We provide wallet data
{
public:
    AppsApiClient();
    ~AppsApiClient();

    //
    // WalletApiHandler::IWalletData methods
    //
    beam::wallet::IWalletDB::Ptr getWalletDBPtr() override;
    beam::wallet::Wallet::Ptr getWalletPtr() override;

    #ifdef BEAM_ATOMIC_SWAP_SUPPORT
    const beam::wallet::IAtomicSwapProvider& getAtomicSwapProvider() const override
    {
        // TODO: return real provider
        throw std::runtime_error("not supported");
    }
    #endif  // BEAM_ATOMIC_SWAP_SUPPORT

    //
    // Methods below are called in reactor thread
    //
    std::string pluginApiRequest(const std::string&);
    void serializeMsg(const nlohmann::json& msg) override;

private:
    // this should be used ONLY in reactor thread
    std::string _lastResult;
};
