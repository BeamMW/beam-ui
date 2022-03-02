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
        Q_PROPERTY(QString nickname READ nickname WRITE nickname NOTIFY nicknameChanged)
        Q_PROPERTY(QString shortTitle READ shortTitle WRITE shortTitle NOTIFY shortTitleChanged)
        Q_PROPERTY(QString aboutMe READ aboutMe WRITE aboutMe NOTIFY aboutMeChanged)
        Q_PROPERTY(QString website READ website WRITE website NOTIFY websiteChanged)
        Q_PROPERTY(QString twitter READ twitter WRITE twitter NOTIFY twitterChanged)
        Q_PROPERTY(QString linkedin READ linkedin WRITE linkedin NOTIFY linkedinChanged)
        Q_PROPERTY(QString instagram READ instagram WRITE instagram NOTIFY instagramChanged)
        Q_PROPERTY(QString telegram READ telegram WRITE telegram NOTIFY telegramChanged)
        Q_PROPERTY(QString discord READ discord WRITE discord NOTIFY discordChanged)

    public:
        AppsViewModel();
        ~AppsViewModel() override;

        [[nodiscard]] QString getAppsUrl() const;
        [[nodiscard]] QString getUserAgent() const;
        [[nodiscard]] QList<QMap<QString, QVariant>> getApps();
        [[nodiscard]] QList<QMap<QString, QVariant>> getLocalApps();
        bool isPublisher() const;

        QString nickname() const;
        void nickname(const QString& name);
        QString shortTitle() const;
        void shortTitle(const QString& value);
        QString aboutMe() const;
        void aboutMe(const QString& value);
        QString website() const;
        void website(const QString& value);
        QString twitter() const;
        void twitter(const QString& value);
        QString linkedin() const;
        void linkedin(const QString& value);
        QString instagram() const;
        void instagram(const QString& value);
        QString telegram() const;
        void telegram(const QString& value);
        QString discord() const;
        void discord(const QString& value);

    public:
        Q_INVOKABLE void onCompleted(QObject *webView);
        Q_INVOKABLE [[nodiscard]] QString getAppCachePath(const QString& appid) const;
        Q_INVOKABLE [[nodiscard]] QString getAppStoragePath(const QString& appid) const;
        Q_INVOKABLE [[nodiscard]] QString chooseFile();
        Q_INVOKABLE [[nodiscard]] QString installFromFile(const QString& fname);
        Q_INVOKABLE void launchAppServer();
        Q_INVOKABLE [[nodiscard]] bool uninstallLocalApp(const QString& appid);
        Q_INVOKABLE [[nodiscard]] QString addPublisherByKey(const QString& publicKey);

    signals:
        void appsChanged();
        void isPublisherChanged();
        void nicknameChanged();
        void shortTitleChanged();
        void aboutMeChanged();
        void websiteChanged();
        void twitterChanged();
        void linkedinChanged();
        void instagramChanged();
        void telegramChanged();
        void discordChanged();

    private:
        [[nodiscard]] QString expandLocalUrl(const QString& folder, const std::string& url) const;
        [[nodiscard]] QString expandLocalFile(const QString& folder, const std::string& url) const;
        QMap<QString, QVariant> validateAppManifest(QTextStream& io, const QString& appFolder);
        void loadApps();

        QString _userAgent;
        QString _serverAddr;
        std::unique_ptr<AppsServer> _server;
        QList<QMap<QString, QVariant>> _lastLocalApps;
        QList<QMap<QString, QVariant>> _apps;
        bool _isPublisher = false;
        QString _nickname;
        QString _shortTitle;
        QString _aboutMe;
        QString _website;
        QString _twitter;
        QString _linkedin;
        QString _instagram;
        QString _telegram;
        QString _discord;
    };
}
