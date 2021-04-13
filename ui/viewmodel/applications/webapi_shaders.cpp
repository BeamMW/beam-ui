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

WebAPI_Shaders::WebAPI_Shaders(IShadersManager::Ptr real)
    : _realShaders(std::move(real))
{
    assert(_realShaders != nullptr);
}

void WebAPI_Shaders::CompileAppShader(const std::vector<uint8_t>& shader)
{
    assert(_realShaders != nullptr);
    return _realShaders->CompileAppShader(shader);
}

void WebAPI_Shaders::Start(const std::string& args, unsigned method, DoneHandler doneHandler)
{
    assert(_realShaders != nullptr);
    return _realShaders->Start(args, method, doneHandler);
}

bool WebAPI_Shaders::IsDone() const
{
    assert(_realShaders != nullptr);
    return _realShaders->IsDone();
}
