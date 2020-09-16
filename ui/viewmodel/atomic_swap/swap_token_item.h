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
#include "swap_offer_item.h"

class SwapTokenInfoItem : public SwapOfferItem
{
    Q_OBJECT
    Q_PROPERTY(QString   sendAmount          READ amountSend                                     NOTIFY tokenChanged)
    Q_PROPERTY(QString   receiveAmount       READ amountReceive                                  NOTIFY tokenChanged)
    Q_PROPERTY(QString   exchangeRate        READ rate                                           NOTIFY tokenChanged)
    Q_PROPERTY(QString   expirationTime      READ getExpirationTime      WRITE setExpirationTime NOTIFY tokenChanged)

    Q_PROPERTY(QString   token               READ getToken               WRITE setToken          NOTIFY tokenChanged)

public:
    SwapTokenInfoItem(QObject* parent = nullptr);

    QString getExpirationTime() const;
    void setExpirationTime(const QString& value);

    QString getToken() const;
    void setToken(const QString& token);

signals:
    void tokenChanged();

private:
    QString m_token;
    beam::wallet::TxParameters m_parameters;
    QString m_expirationTime;
};

