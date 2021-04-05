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

#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable: 4127)
#endif
#include <QQmlEngine>
#if defined(_MSC_VER)
#pragma warning (pop)
#endif

#include "public.h"
#include "apps_view.h"

#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable: 4251)
#endif
#include "webapi_beam.h"
#if defined(_MSC_VER)
#pragma warning (pop)
#endif

namespace beamui::applications
{
    void RegisterQMLTypes()
    {
        qmlRegisterType<AppsViewModel>("Beam.Wallet", 1, 0, "ApplicationsViewModel");
        qmlRegisterType<WebAPICreator>("Beam.Wallet", 1, 0, "WebAPICreator");
    }
}
