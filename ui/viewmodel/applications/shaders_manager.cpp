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
#include "shaders_manager.h"
/*
ShadersManager::ShadersManager()
{
}

ShadersManager::~ShadersManager()
{
}

bool ShadersManager::Compile(beam::ByteBuffer& res, const char* sz, Kind kind)
{
    std::FStream fs;
    if (!fs.Open(sz, true, true))
    {
        return false;
    }

    auto remaining = static_cast<size_t>(fs.get_Remaining());
    if(!remaining)
    {
        return false;
    }

    res.resize(remaining);
    if(remaining != fs.read(&res.front(), res.size()))
    {
        return false;
    }

    beam::bvm2::Processor::Compile(res, res, kind);
    // TODO:: error check

    return true;
}
*/