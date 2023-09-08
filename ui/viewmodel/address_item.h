// Copyright 2023 The Beam Team
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
#include "wallet/core/wallet_db.h"

class AddressItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString   walletID         READ getWalletID       CONSTANT)
    Q_PROPERTY(QString   token            READ getToken          CONSTANT)
    Q_PROPERTY(QString   name             READ getName           CONSTANT)
    Q_PROPERTY(QString   category         READ getCategory       CONSTANT)
    Q_PROPERTY(QString   identity         READ getIdentity       CONSTANT)
    Q_PROPERTY(QDateTime expirationDate   READ getExpirationDate CONSTANT)
    Q_PROPERTY(QDateTime createDate       READ getCreateDate     CONSTANT)
    Q_PROPERTY(bool      neverExpired     READ isNeverExpired    CONSTANT)
    Q_PROPERTY(bool      isExpired        READ isExpired         CONSTANT)
    Q_PROPERTY(QString   strView          READ getStrView        CONSTANT)

public:

    AddressItem() = default;
    AddressItem(beam::wallet::WalletAddress);

    QString getWalletID() const;
    QString getToken() const;
    QString getName() const;
    QString getCategory() const;
    QString getIdentity() const;
    QDateTime getExpirationDate() const;
    QDateTime getCreateDate() const;
    bool isNeverExpired() const;
    QString getStrView() const;

    bool isExpired() const;
    beam::Timestamp getCreateTimestamp() const;
    beam::Timestamp getExpirationTimestamp() const;

private:
    beam::wallet::WalletAddress m_walletAddress;
};
