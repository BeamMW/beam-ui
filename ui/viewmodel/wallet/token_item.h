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
#include "wallet/core/common.h"
#include "wallet/core/wallet_db.h"

class TokenInfoItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool    isPermanent           READ isPermanent                NOTIFY tokenChanged)
    Q_PROPERTY(bool    isMaxPrivacy          READ isMaxPrivacy               NOTIFY tokenChanged)
    Q_PROPERTY(bool    isOffline             READ isOffline                  NOTIFY tokenChanged)
    Q_PROPERTY(bool    isPublicOffline       READ isPublicOffline            NOTIFY tokenChanged)
    Q_PROPERTY(bool    ignoreStoredVouchers  READ getIgnoreStoredVouchers    WRITE  setIgnoreStoredVouchers   NOTIFY ignoreStoredVouchersChanged)
    Q_PROPERTY(QString transactionType       READ getTransactionType         NOTIFY tokenChanged)
    Q_PROPERTY(QString amount                READ getAmount                  NOTIFY tokenChanged)
    Q_PROPERTY(QString amountValue           READ getAmountValue             NOTIFY tokenChanged)
    Q_PROPERTY(QString address               READ getAddress                 NOTIFY tokenChanged)
    Q_PROPERTY(QString identity              READ getIdentity                NOTIFY tokenChanged)
    Q_PROPERTY(QString token                 READ getToken                   WRITE setToken   NOTIFY tokenChanged)
    Q_PROPERTY(int     offlinePayments       READ getOfflinePayments         WRITE setOfflinePayments         NOTIFY offlinePaymentsChanged)
    

public:
    TokenInfoItem(QObject* parent = nullptr);
    bool isPermanent() const;
    bool isMaxPrivacy() const;
    bool isOffline() const;
    bool isPublicOffline() const;
    QString getTransactionType() const;
    QString getAmount() const;
    QString getAmountValue() const;
    QString getAddress() const;
    QString getIdentity() const;
    QString getToken() const;
    void setToken(const QString& token);

    int getOfflinePayments() const;
    void setOfflinePayments(int value);
    
    bool getIgnoreStoredVouchers() const;
    void setIgnoreStoredVouchers(bool value);
private:
    void reset();
signals:
    void tokenChanged();
    void offlinePaymentsChanged();
    void ignoreStoredVouchersChanged();

private:
    QString m_token;
    bool m_isPermanent = false;
    bool m_isMaxPrivacy = false;
    bool m_isOffline = false;
    bool m_isPublicOffline = false;
    beam::Amount m_amountValue = 0;
    beam::wallet::WalletID m_addressSBBS = beam::Zero;
    beam::PeerID m_identity;
    int m_offlinePayments = 0;
    bool m_ignoreStoredVouchers = false;
};

