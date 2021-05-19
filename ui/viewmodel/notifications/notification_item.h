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
#include <QDateTime>
#include "model/wallet_model.h"
#include "viewmodel/ui_helpers.h"
#include "viewmodel/wallet/assets_manager.h"

class NotificationItem : public QObject
{
    Q_OBJECT

public:
    NotificationItem() = default;
    NotificationItem(const beam::wallet::Notification&);
    bool operator==(const NotificationItem& other) const;

    QDateTime timeCreated() const;
    beam::Timestamp getTimestamp() const;
    beam::wallet::Notification::State getState() const;
    QString title() const;
    QString message(AssetsManager::Ptr amgr) const;
    QString type() const;
    QString state() const;
    beam::Asset::ID assetId() const;

    ECC::uintBig getID() const;

    QString getTxID() const;
    beam::wallet::WalletAddress getWalletAddress() const;
 
signals:

private:
    beam::wallet::Notification m_notification;
};
