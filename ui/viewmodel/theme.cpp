// Copyright 2019 The Beam Team
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

#include "theme.h"
#include "core/block_crypt.h"

using namespace beam;

// static
QString Theme::name()
{
    return Rules::get().get_NetworkName();
}

// static
QString Theme::iconPath() 
{
    switch (Rules::get().m_Network)
    {
#define THE_MACRO(name) case Rules::Network::name: return ":/assets/icon_" #name ".png";
        RulesNetworks(THE_MACRO)
#undef THE_MACRO
    default:
        return ":/assets/icon_mainnet.png";
    }
}
