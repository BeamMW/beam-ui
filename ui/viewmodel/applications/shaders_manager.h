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

#include "bvm/ManagerStd.h"
/*
class ShadersManager
    :  public beam::bvm2::ManagerStd
{
public:
    ShadersManager();
    ~ShadersManager();

    static bool Compile(beam::ByteBuffer& res, const char* sz, Kind kind);

    void OnDone(const std::exception* pExc) override
    {
        m_Done = true;
        m_Err = !!pExc;

        if (pExc)
            std::cout << "Shader exec error: " << pExc->what() << std::endl;
        else
            std::cout << "Shader output: " << m_Out.str() << std::endl;

        if (m_Async)
            beam::io::Reactor::get_Current().stop();
    }

    beam::Height get_Height() override
    {
        return m_sTip.m_Height;
    }

    // TODO: make private
    beam::Block::SystemState::Full m_sTip;
    bool m_Done = false;
    bool m_Err = false;
    bool m_Async = false;
};
*/