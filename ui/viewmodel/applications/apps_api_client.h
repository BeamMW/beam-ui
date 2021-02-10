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

#include "wallet/api/wallet_api.h"

namespace beamui {
    using namespace beam::wallet;

    class AppsApiClient
            : public beam::wallet::WalletApi
    {
    public:
        struct IHandler {
            // Called in the main (reactor, non-UI thread)
            virtual void onAPIResult(const std::string&) = 0;
        };

        explicit AppsApiClient(IHandler&);
        virtual ~AppsApiClient() = default;

        //
        // Methods below must be called in the main (non-UI) reactor thread
        //
        void executeAPIRequest(const std::string &);

        //
        // Called in the main (non-UI) reactor thread
        //
        void sendMessage(const nlohmann::json &msg) override;

   private:
        IHandler& _handler;
    };
}
