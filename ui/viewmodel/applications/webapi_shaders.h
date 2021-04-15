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

#include "wallet/core/contracts/i_shaders_manager.h"
#include "consent_handler.h"
#include "model/app_model.h"

namespace beamui::applications
{
    class WebAPI_Shaders: public beam::wallet::IShadersManager
    {
    public:
        typedef std::shared_ptr<WebAPI_Shaders> Ptr;

        WebAPI_Shaders(IConsentHandler& consentHandler, const std::string& appid);
        ~WebAPI_Shaders() override = default;

        //
        // AnyThread_ functions should be safe to call from any thread
        //
        void AnyThread_contractApproved();
        void AnyThread_contractRejected(bool byUser, const std::string& error);

    private:
        void CompileAppShader(const std::vector<uint8_t>& shader) override;
        void CallShaderAndStartTx(const std::string& args, unsigned method, DoneAllHandler doneHandler) override;
        void CallShader(const std::string& args, unsigned method, DoneCallHandler) override;
        void ProcessTxData(const beam::ByteBuffer& data, DoneTxHandler doneHandler) override;
        [[nodiscard]] bool IsDone() const override;

        void SetCurrentApp(const std::string& appid) override;
        void ReleaseCurrentApp(const std::string& appid) override;

    private:
        std::shared_ptr<bool> _guard = std::make_shared<bool>(true);
        IShadersManager::Ptr _realShaders;
        IConsentHandler& _consentHandler;

        struct ApproveData
        {
            ApproveData(beam::ByteBuffer d, boost::optional<std::string> o, DoneAllHandler h)
                : data(std::move(d)), output(std::move(o)), doneHandler(std::move(h))
            {}

            beam::ByteBuffer data;
            boost::optional<std::string> output;
            DoneAllHandler doneHandler;
        };

        std::shared_ptr<ApproveData> _approveData;
        beam::wallet::IWalletModelAsync::Ptr _asyncWallet;
    };
}
