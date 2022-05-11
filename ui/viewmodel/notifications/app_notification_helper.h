// Copyright 2021 The Beam Team
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
#include "model/wallet_model.h"

using namespace beam::wallet;

class AppNotificationHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString txId READ getTxId WRITE setTxId NOTIFY txIdChanged)
    Q_PROPERTY(qlonglong estimateBlockTime READ getEstimateBlockTime)

public:
    AppNotificationHelper();
    ~AppNotificationHelper() override = default;
    QString getTxId() const;
    void setTxId(QString txId);
    qlonglong getEstimateBlockTime() const;

public slots:
    void onTransactionsChanged(ChangeAction action, const std::vector<TxDescription>& items);

signals:
    void txIdChanged();
    void txFinished();

private:
    TxID m_txId = {};
    beam::Timestamp m_estimateBlockTime;

};
