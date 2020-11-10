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
#include "apps_api_client.h"
#include "utility/logger.h"
#include "model/app_model.h"

AppsApiClient::AppsApiClient()
    : AppsApi(static_cast<WalletApiHandler::IWalletData&>(*this))
{
}

AppsApiClient::~AppsApiClient()
{
}

std::string AppsApiClient::pluginApiRequest(const std::string& data)
{
    AppsApi::parse(data.c_str(), data.size());

    assert(!_lastResult.empty());

    auto result = std::move(_lastResult);
    assert(_lastResult.empty());

    return result;
}

void AppsApiClient::serializeMsg(const nlohmann::json& msg)
{
    assert(_lastResult.empty());
    _lastResult = msg.dump();
}

beam::wallet::IWalletDB::Ptr AppsApiClient::getWalletDBPtr()
{
    return AppModel::getInstance().getWalletDB();
}

beam::wallet::Wallet::Ptr AppsApiClient::getWalletPtr()
{
    return AppModel::getInstance().getWalletModel()->getWallet();
}
