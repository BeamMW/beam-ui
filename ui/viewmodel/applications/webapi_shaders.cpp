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
#include "webapi_shaders.h"

namespace beamui::applications
{
    WebAPI_Shaders::WebAPI_Shaders(const std::string &appid, const std::string& appname)
    {
        _realShaders = AppModel::getInstance().getWalletModel()->getAppsShaders(appid, appname);
    }

    void WebAPI_Shaders::CallShaderAndStartTx(const std::vector<uint8_t>& shader, const std::string &args, unsigned method, DoneAllHandler doneHandler)
    {
        _realShaders->CallShaderAndStartTx(shader, args, method, doneHandler);
    }

    void WebAPI_Shaders::CallShader(const std::vector<uint8_t>& shader, const std::string &args, unsigned method, DoneCallHandler doneHandler)
    {
        _realShaders->CallShader(shader, args, method, doneHandler);
    }

    void WebAPI_Shaders::ProcessTxData(const beam::ByteBuffer& data, DoneTxHandler doneHandler)
    {
        _realShaders->ProcessTxData(data, doneHandler);
    }

    bool WebAPI_Shaders::IsDone() const
    {
        return _realShaders->IsDone();
    }

    void WebAPI_Shaders::SetCurrentApp(const std::string &appid, const std::string& appname)
    {
        assert(!"This should not be called");
        return _realShaders->SetCurrentApp(appid, appname);
    }

    void WebAPI_Shaders::ReleaseCurrentApp(const std::string &appid)
    {
        return _realShaders->ReleaseCurrentApp(appid);
    }
}
