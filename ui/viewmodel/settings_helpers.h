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

#pragma once

#include <QObject>

namespace beam::io
{
    struct Address;
}

struct UnpackedAddress
{
    QString address;
    QString port = 0;
};

QString AddressToQstring(const beam::io::Address& address);
QString formatAddress(const QString& address, const QString& port);
QString formatPort(uint16_t port);
UnpackedAddress parseAddress(const QString& address);
