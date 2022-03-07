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
        Q_PROPERTY(QList<QMap<QString, QVariant>> apps READ getApps NOTIFY appsChanged)
        Q_PROPERTY(bool isPublisher READ isPublisher NOTIFY isPublisherChanged)
        Q_PROPERTY(QVariantMap publisherInfo READ getPublisherInfo NOTIFY publisherInfoChanged)

    public:
        AppsViewModel();
        ~AppsViewModel() override;

        [[nodiscard]] QString getAppsUrl() const;
        [[nodiscard]] QString getUserAgent() const;
        [[nodiscard]] QList<QMap<QString, QVariant>> getApps();
        [[nodiscard]] QList<QMap<QString, QVariant>> getLocalApps();
        bool isPublisher() const;

        QVariantMap getPublisherInfo() const;
        void setPublisherInfo(const QVariantMap& value);

    public:
        Q_INVOKABLE void onCompleted(QObject *webView);
        Q_INVOKABLE [[nodiscard]] QString getAppCachePath(const QString& appid) const;
        Q_INVOKABLE [[nodiscard]] QString getAppStoragePath(const QString& appid) const;
        Q_INVOKABLE [[nodiscard]] QString chooseFile(const QString& title);
        Q_INVOKABLE [[nodiscard]] QMap<QString, QVariant> getDAppFileProperties(const QString& fname);
        Q_INVOKABLE [[nodiscard]] QMap<QString, QVariant> parseDAppFile(const QString& fname);
        Q_INVOKABLE [[nodiscard]] QString installFromFile(const QString& fname);
        Q_INVOKABLE void launchAppServer();
        Q_INVOKABLE [[nodiscard]] bool uninstallLocalApp(const QString& appid);
        Q_INVOKABLE [[nodiscard]] QString addPublisherByKey(const QString& publicKey);
        Q_INVOKABLE void createPublisher(const QVariantMap& publisherInfo);
        Q_INVOKABLE void changePublisherInfo(const QVariantMap& publisherInfo);

    signals:
        void appsChanged();
        void isPublisherChanged();
        void publisherInfoChanged();

    private:
        [[nodiscard]] QString expandLocalUrl(const QString& folder, const std::string& url) const;
        [[nodiscard]] QString expandLocalFile(const QString& folder, const std::string& url) const;
        QMap<QString, QVariant> parseAppManifest(QTextStream& io, const QString& appFolder);
        void loadApps();
        void loadMyPublisherInfo();

        QString _userAgent;
        QString _serverAddr;
        std::unique_ptr<AppsServer> _server;
        QList<QMap<QString, QVariant>> _lastLocalApps;
        QList<QMap<QString, QVariant>> _apps;
        bool _isPublisher = false;
        QString _publisherKey;
        QString _nickname;
        QString _shortTitle;
        QString _aboutMe;
        QString _website;
        QString _twitter;
        QString _linkedin;
        QString _instagram;
        QString _telegram;
        QString _discord;
        QVariantMap _publisherInfo;
    };
}
