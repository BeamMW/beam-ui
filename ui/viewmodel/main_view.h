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

#include <QObject>
#include <QTimer>

#include "model/settings.h"

class MainViewModel : public QObject
{
	Q_OBJECT
    Q_PROPERTY(int unsafeTxCount        READ getUnsafeTxCount       NOTIFY unsafeTxCountChanged)
    Q_PROPERTY(int unreadNotifications  READ getUnreadNotifications NOTIFY unreadNotificationsChanged)
    Q_PROPERTY(QString daoCoreAppID     READ getDaoCoreAppID        CONSTANT)
    Q_PROPERTY(QString faucetAppID      READ getFaucetAppID         CONSTANT)
    Q_PROPERTY(bool isDevMode           READ getDevMode             CONSTANT)
public:
    MainViewModel();

    Q_INVOKABLE void resetLockTimer();

signals:
    void gotoStartScreen();
    void showTrezorMessage();
    void hideTrezorMessage();
    void showTrezorError(const QString&);
    void unsafeTxCountChanged();
    void unreadNotificationsChanged();
    void clipboardChanged(const QString& message);

public slots:
	void update(int page);
    void lockWallet();
    void onLockTimeoutChanged();
private slots:
    void onClipboardDataChanged();
private:
    [[nodiscard]] int getUnsafeTxCount() const;
    [[nodiscard]] int getUnreadNotifications() const;
    [[nodiscard]] QString getDaoCoreAppID() const;
    [[nodiscard]] QString getFaucetAppID() const;
    [[nodiscard]] bool getDevMode() const;
private:
    WalletSettings& m_settings;
    QTimer m_timer;
};
