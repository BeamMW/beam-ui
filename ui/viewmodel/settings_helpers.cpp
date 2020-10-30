// Copyright 2020 The Beam Team
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

#include "settings_helpers.h"

#include "model/helpers.h"

#include "utility/io/address.h"


QString AddressToQstring(const beam::io::Address& address)
{
    if (!address.empty())
    {
        return str2qstr(address.str());
    }
    return {};
}

QString formatAddress(const QString& address, const QString& port)
{
    return QString("%1:%2").arg(address).arg(port);
}

QString formatPort(uint16_t port)
{
    return port > 0 ? QString("%1").arg(port) : "";
}

UnpackedAddress parseAddress(const QString& address)
{
    UnpackedAddress res;
    auto separator = address.indexOf(':');
    if (separator > -1)
    {
        res.address = address.left(separator);
        res.port = address.mid(separator + 1);
    }
    else
    {
        res.address = address;
    }
    return res;
}