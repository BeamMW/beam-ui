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

namespace beamui::applications {
    class AppsViewModel : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString devAppUrl        READ getDevAppUrl        CONSTANT)
        Q_PROPERTY(QString devAppName       READ getDevAppName       CONSTANT)
        Q_PROPERTY(QString devAppApiVer     READ getDevAppApiVer     CONSTANT)
        Q_PROPERTY(QString devAppMinApiVer  READ getDevAppMinApiVer  CONSTANT)
        Q_PROPERTY(QString appsUrl          READ getAppsUrl          CONSTANT)
        Q_PROPERTY(QString userAgent        READ getUserAgent        CONSTANT)

    public:
        AppsViewModel();
        ~AppsViewModel() override;

        [[nodiscard]] QString getDevAppUrl() const;
        [[nodiscard]] QString getDevAppName() const;
        [[nodiscard]] QString getAppsUrl() const;
        [[nodiscard]] QString getDevAppApiVer() const;
        [[nodiscard]] QString getDevAppMinApiVer() const;
        [[nodiscard]] QString getUserAgent() const;

    public:
        Q_INVOKABLE void onCompleted(QObject *webView);
        Q_INVOKABLE QString getAppCachePath(const QString& appname) const;
        Q_INVOKABLE QString getAppStoragePath(const QString& appname) const;

    private:
        QString _userAgent;
    };
}
