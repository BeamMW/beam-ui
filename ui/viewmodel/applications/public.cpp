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

#include <QQmlEngine>

#include "public.h"
#include "apps_view.h"
#include "apps_api_ui.h"
#include "webapi_creator.h"

namespace beamui::applications
{
    namespace
    {
        const std::string kAppIDPrefix = "appid:";
    }

    void RegisterQMLTypes()
    {
        qmlRegisterType<AppsViewModel>("Beam.Wallet", 1, 0, "ApplicationsViewModel");
        qmlRegisterType<WebAPICreator>("Beam.Wallet", 1, 0, "WebAPICreator");
    }

    std::string GenerateAppID(const std::string& appName, const std::string& appUrl)
    {
        QUrl url(QString::fromStdString(appUrl));
        const auto normalizedUrl = url.toString(QUrl::RemovePort).toStdString();

        ECC::Hash::Value hv;
        ECC::Hash::Processor() << appName << normalizedUrl >> hv;

        auto appid = kAppIDPrefix + hv.str();
        return appid;
    }

    std::string StripAppIDPrefix(const std::string& appId)
    {
        auto res = appId;

        size_t pos = appId.find(kAppIDPrefix);
        if (pos != std::string::npos)
        {
            res.erase(pos, kAppIDPrefix.length());
        }

        return res;
    }
}
