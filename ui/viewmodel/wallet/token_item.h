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

class TokenInfoItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool    isPermanent         READ isPermanent            NOTIFY tokenChanged)
    Q_PROPERTY(QString transactionType     READ getTransactionType     NOTIFY tokenChanged)
    Q_PROPERTY(QString amount              READ getAmount              NOTIFY tokenChanged)
    Q_PROPERTY(QString amountValue         READ getAmountValue         NOTIFY tokenChanged)
    Q_PROPERTY(QString address             READ getAddress             NOTIFY tokenChanged)
    Q_PROPERTY(QString identity            READ getIdentity            NOTIFY tokenChanged)
    Q_PROPERTY(QString token               READ getToken               WRITE setToken   NOTIFY tokenChanged)

public:
    TokenInfoItem(QObject* parent = nullptr);
    bool isPermanent() const;
    QString getTransactionType() const;
    QString getAmount() const;
    QString getAmountValue() const;
    QString getAddress() const;
    QString getIdentity() const;
    QString getToken() const;
    void setToken(const QString& token);

signals:
    void tokenChanged();

private:
    QString m_token;
    beam::wallet::TxParameters m_parameters;
};

