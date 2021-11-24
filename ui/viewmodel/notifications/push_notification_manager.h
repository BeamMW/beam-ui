// Copyright 2020 The Beam Team
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

#include <QObject>

#include "model/app_model.h"

/**
 *  Used to show user notification popup when a new wallet application was released.
 */
class PushNotificationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasNewerVersion            READ hasNewerVersion            NOTIFY showUpdateNotification)
public:
    PushNotificationManager();

    /// Will mark notification as read to ignore it next time.
    Q_INVOKABLE void onCancelPopup(const QVariant& variantID);
    Q_INVOKABLE void closeContractNotification(const QString& txIdStr);

    bool hasNewerVersion() const;

signals:
    void showUpdateNotification(const QString&, const QString&, const QVariant&);
    void showContractNotification(QString txId, QString appName, QString comment, QString icon);

public slots:
    void onNewSoftwareUpdateAvailable(
        const beam::wallet::WalletImplVerInfo&, const ECC::uintBig& notificationID, bool showPopup);
    void onNotificationsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Notification>&);
    void onTransactionsChanged(
        beam::wallet::ChangeAction action, const std::vector<beam::wallet::TxDescription>& items);

private:
    WalletModel::Ptr m_walletModel;
    bool m_firstNotification = true;
    bool m_hasNewerVersion = false;
    std::set<beam::wallet::TxID> m_contractNotifications;
};
