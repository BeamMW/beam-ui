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

public:
    DappsStoreViewModel();
    ~DappsStoreViewModel() override;

    [[nodiscard]] QList<QMap<QString, QVariant>> getApps();
    QString getPublisherKey();

    void loadApps();

public:
    Q_INVOKABLE void onCompleted(QObject *webView);
    Q_INVOKABLE [[nodiscard]] QString chooseFile();
    Q_INVOKABLE [[nodiscard]] QString installFromFile(const QString& fname);
    Q_INVOKABLE void uploadApp();

signals:
    void appsChanged();
    void publisherKeyChanged();

private:
    [[nodiscard]] QString expandLocalUrl(const QString& folder, const std::string& url) const;
    [[nodiscard]] QString expandLocalFile(const QString& folder, const std::string& url) const;
    QMap<QString, QVariant> parseAppManifest(QTextStream& io, const QString& appFolder);
    void addAppToStore(const QString& appName, const std::string& ipfsCID);


    QString _serverAddr;
    QList<QMap<QString, QVariant>> _apps;
    QString _publisherKey;
};
