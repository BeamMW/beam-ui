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

#include "apps_server.h"

namespace beamui::applications
{
    class AppsViewModel : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString appsUrl     READ getAppsUrl    CONSTANT)
        Q_PROPERTY(QString userAgent   READ getUserAgent  CONSTANT)
        Q_PROPERTY(QList<QMap<QString, QVariant>> localApps READ getLocalApps CONSTANT)

    public:
        AppsViewModel();
        ~AppsViewModel() override;

        [[nodiscard]] QString getAppsUrl() const;
        [[nodiscard]] QString getUserAgent() const;
        [[nodiscard]] QList<QMap<QString, QVariant>> getLocalApps();

    public:
        Q_INVOKABLE void onCompleted(QObject *webView);
        Q_INVOKABLE [[nodiscard]] QString getAppCachePath(const QString& appname) const;
        Q_INVOKABLE [[nodiscard]] QString getAppStoragePath(const QString& appname) const;
        Q_INVOKABLE [[nodiscard]] bool installFromFile();

    private:
        [[nodiscard]] QString expandLocalUrl(const QString& folder, const std::string& url) const;
        QMap<QString, QVariant> validateAppManifest(QTextStream& io, const QString& appFolder);
        void launchAppServer();

        QString _userAgent;
        QString _serverAddr;
        std::unique_ptr<AppsServer> _server;
    };
}
