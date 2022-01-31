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

class DappsStoreViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QMap<QString, QVariant>> apps READ getApps NOTIFY appsChanged)
    Q_PROPERTY(QString publisherKey READ getPublisherKey NOTIFY publisherKeyChanged)
    Q_PROPERTY(QList<QMap<QString, QVariant>> publishers READ getPublishers NOTIFY publishersChanged)

public:
    DappsStoreViewModel();
    ~DappsStoreViewModel() override;

    [[nodiscard]] QList<QMap<QString, QVariant>> getApps();
    QString getPublisherKey();
    [[nodiscard]] QList<QMap<QString, QVariant>> getPublishers();

    void loadApps();
    void loadPublishers();

public:
    Q_INVOKABLE void onCompleted(QObject *webView);
    Q_INVOKABLE [[nodiscard]] QString chooseFile();
    Q_INVOKABLE [[nodiscard]] QString installFromFile(const QString& fname);
    Q_INVOKABLE void uploadApp();
    Q_INVOKABLE void registerPublisher();
    Q_INVOKABLE void installApp(const QString& guid);

signals:
    void appsChanged();
    void publisherKeyChanged();
    void appInstallOK(const QString& appName);
    void appInstallFail(const QString& appName);
    void publishersChanged();

private:
    [[nodiscard]] QString expandLocalUrl(const QString& folder, const std::string& url) const;
    [[nodiscard]] QString expandLocalFile(const QString& folder, const std::string& url) const;
    QMap<QString, QVariant> parseAppManifest(QTextStream& io, const QString& appFolder);
    void addAppToStore(QMap<QString, QVariant>&& app, const std::string& ipfsCID);
    QString installFromBuffer(QIODevice* ioDevice);

    QMap<QString, QVariant> loadLocalDapp(const QString& guid);

    QString _serverAddr;
    QList<QMap<QString, QVariant>> _apps;
    QString _publisherKey;
    QList<QMap<QString, QVariant>> _publishers;
};
