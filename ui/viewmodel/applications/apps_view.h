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
#include <boost/optional.hpp>
#include "utility/common.h"
#include "model/wallet_model.h"

namespace beamui::applications
{
    class AppsViewModel : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString appsUrl     READ getAppsUrl    CONSTANT)
        Q_PROPERTY(QString userAgent   READ getUserAgent  CONSTANT)
        Q_PROPERTY(QList<QVariantMap> localApps READ getLocalApps CONSTANT)
        Q_PROPERTY(QList<QVariantMap> apps READ getApps NOTIFY appsChanged)
        Q_PROPERTY(bool isPublisher READ isPublisher NOTIFY isPublisherChanged)
        Q_PROPERTY(QVariantMap publisherInfo READ getPublisherInfo NOTIFY publisherInfoChanged)
        Q_PROPERTY(QList<QVariantMap> publishers READ getPublishers NOTIFY publishersChanged)

    public:

        enum class Action
        {
            CreatePublisher,
            UpdatePublisher,
            UploadDApp,
            DeleteDApp
        };

        AppsViewModel();
        ~AppsViewModel() override;

        using Ptr = std::shared_ptr<AppsViewModel>;

        [[nodiscard]] QString getAppsUrl() const;
        [[nodiscard]] QString getUserAgent() const;
        [[nodiscard]] QList<QVariantMap> getApps();
        [[nodiscard]] QList<QVariantMap> getLocalApps();
        [[nodiscard]] QList<QVariantMap> getPublishers();
        bool isPublisher() const;

        QVariantMap getPublisherInfo() const;
        void setPublisherInfo(const QVariantMap& value);

    public:
        Q_INVOKABLE void onCompleted(QObject *webView);
        Q_INVOKABLE [[nodiscard]] QString getAppCachePath(const QString& appid) const;
        Q_INVOKABLE [[nodiscard]] QString getAppStoragePath(const QString& appid) const;
        Q_INVOKABLE [[nodiscard]] QString chooseFile(const QString& title);
        Q_INVOKABLE [[nodiscard]] QVariantMap getDAppFileProperties(const QString& fname);
        Q_INVOKABLE [[nodiscard]] QVariantMap parseDAppFile(const QString& fname);
        Q_INVOKABLE void publishDApp(bool isUpdating = false);
        Q_INVOKABLE void removeDApp(const QString& guid);
        Q_INVOKABLE bool checkDAppNewVersion(const QVariantMap& currentDApp, const QVariantMap& newDApp);
        Q_INVOKABLE void installApp(const QString& guid);
        Q_INVOKABLE [[nodiscard]] QString installFromFile(const QString& fname);
        Q_INVOKABLE void launchAppServer();
        Q_INVOKABLE [[nodiscard]] bool uninstallLocalApp(const QString& appid);
        Q_INVOKABLE [[nodiscard]] QString addPublisherByKey(const QString& publisherKey);
        Q_INVOKABLE void createPublisher(const QVariantMap& publisherInfo);
        Q_INVOKABLE void changePublisherInfo(const QVariantMap& publisherInfo);
        Q_INVOKABLE void contractInfoApproved(int action, const QString& data);
        Q_INVOKABLE void contractInfoRejected();

        Q_INVOKABLE [[nodiscard]] QList<QVariantMap> getPublisherDApps(const QString& publisherKey);

    public slots:
        void onTransactionsChanged(
            beam::wallet::ChangeAction action,
            const std::vector<beam::wallet::TxDescription>& transactions);

    signals:
        void appsChanged();
        void isPublisherChanged();
        void publisherInfoChanged();
        void publishersChanged();
        void shaderTxData(int action, const QString& data, const QString& comment, const QString& fee, const QString& feeRate, const QString& rateUnit);
        void showTxIsSent();
        void hideTxIsSent();
        void showYouArePublisher();
        void appInstallOK(const QString& appName);
        void appInstallFail(const QString& appName);

    private:
        [[nodiscard]] QString expandLocalUrl(const QString& folder, const std::string& url) const;
        [[nodiscard]] QString expandLocalFile(const QString& folder, const std::string& url) const;
        QVariantMap parseAppManifest(QTextStream& io, const QString& appFolder);
        void loadApps();
        void loadAppsFromStore();
        void loadPublishers();
        void loadMyPublisherInfo(bool hideTxIsSent = false, bool showYouArePublsher = false);
        void setPublishers(const QList<QVariantMap>& value);
        void handleShaderTxData(Action action, const beam::ByteBuffer& data);
        void uploadAppToStore(QVariantMap&& app, const std::string& ipfsID, bool isUpdating = false);
        void deleteAppFromStore(const QString& guid);
        void installFromBuffer(QIODevice* ioDevice, const QString& guid);
        QVariantMap getAppByGUID(const QString& guid);

        WalletModel::Ptr m_walletModel;

        QString _userAgent;
        QString _serverAddr;
        std::unique_ptr<AppsServer> _server;
        QList<QVariantMap> _lastLocalApps;
        QList<QVariantMap> _apps;
        QList<QVariantMap> _publishers;
        bool _isPublisher = false;
        QVariantMap _publisherInfo;
        boost::optional<beam::wallet::TxID> _txId;

		Action _action;
        boost::optional<beam::ByteBuffer> _loadedDAppBuffer;
        boost::optional<QVariantMap> _loadedDApp;
    };
}
