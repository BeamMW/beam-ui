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
#include "webapi_creator.h"
#include "wallet/api/i_wallet_api.h"
#include "wallet/core/common.h"
#include "bvm/invoke_data.h"
#include "public.h"

namespace beamui::applications
{
    namespace
    {
        WalletModel::Ptr getWalletModel() {
            return AppModel::getInstance().getWalletModel();
        }
    }

    WebAPICreator::WebAPICreator(QObject *parent)
        : QObject(parent)
    {
    }

    WebAPICreator::~WebAPICreator()
    {
        LOG_INFO () << "WebAPICreator destroyed";
    }

    void WebAPICreator::createApi(const QString& verWant, const QString& verMin, const QString &appName, const QString &appUrl)
    {
        using namespace beam::wallet;

        //
        // if can, create verWant API, otherwise verMin
        // if cannot to create any of these two just fail
        //
        std::string version;
        if (IWalletApi::ValidateAPIVersion(verWant.toStdString()))
        {
            version = verWant.toStdString();
        }
        else if (IWalletApi::ValidateAPIVersion(verMin.toStdString()))
        {
            version = verMin.toStdString();
        }

        if(version.empty())
        {
            //% "Unsupported API version requested: %1"
            auto error = qtTrId("apps-bad-api-version").arg(verWant);
            if (!verMin.isEmpty()) error += "-" + verMin;
            return qmlEngine(this)->throwError(error);
        }

        QPointer<WebAPICreator> guard = this;
        const auto appid = GenerateAppID(appName.toStdString(), appUrl.toStdString());

        AppsApiUI::ClientThread_Create(getWalletModel().get(), version, appid, appName.toStdString(),
            [this, guard, version, appName, appid] (AppsApiUI::Ptr api) {
                if (guard)
                {
                    _api = std::move(api);
                    emit apiCreated(_api.get());
                    LOG_INFO() << "API created: " << version << ", " << appName.toStdString() << ", " << appid;
                }
                else
                {
                    LOG_INFO() << "WebAPICreator destroyed before api created:" << version << ", " << appName.toStdString() << ", " << appid;
                }
            }
        );
    }

    bool WebAPICreator::apiSupported(const QString& apiVersion) const
    {
        return beam::wallet::IWalletApi::ValidateAPIVersion(apiVersion.toStdString());
    }

    QString WebAPICreator::generateAppID(const QString& appName, const QString& appUrl)
    {
        const auto appid = GenerateAppID(appName.toStdString(), appUrl.toStdString());
        return QString::fromStdString(appid);
    }
}
