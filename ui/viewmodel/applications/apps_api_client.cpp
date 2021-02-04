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

namespace beamui {
    AppsApiClient::AppsApiClient(IHandler& handler)
        : WalletApi(
            AppModel::getInstance().getWalletDB(),
            AppModel::getInstance().getWalletModel()->getWallet(),
            nullptr,
            AppModel::getInstance().getWalletModel()->getAppsShaders()
        ),
        _handler(handler)
    {
    }

    void AppsApiClient::executeAPIRequest(const std::string &data)
    {
        WalletApi::parseJSON(data.c_str(), data.size());
    }

    void AppsApiClient::sendMessage(const nlohmann::json &msg)
    {
        auto result = msg.dump();
        _handler.onAPIResult(result);
    }
}
