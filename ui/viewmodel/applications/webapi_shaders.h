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
#include "model/app_model.h"

namespace beamui::applications
{
    class WebAPI_Shaders: public beam::wallet::IShadersManager
    {
    public:
        typedef std::shared_ptr<WebAPI_Shaders> Ptr;

        explicit WebAPI_Shaders(const std::string& appid, const std::string& appname);
        ~WebAPI_Shaders() override = default;

    private:
        void CallShaderAndStartTx(const std::vector<uint8_t>& shader, const std::string& args, unsigned method, DoneAllHandler doneHandler) override;
        void CallShader(const std::vector<uint8_t>& shader, const std::string& args, unsigned method, DoneCallHandler) override;
        void ProcessTxData(const beam::ByteBuffer& data, DoneTxHandler doneHandler) override;
        [[nodiscard]] bool IsDone() const override;

        void SetCurrentApp(const std::string& appid, const std::string& appname) override;
        void ReleaseCurrentApp(const std::string& appid) override;

    private:
        IShadersManager::Ptr _realShaders;
    };
}
