// Copyright 2019 The Beam Team
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

class SwapOfferItem : public QObject
{
    Q_OBJECT

public:
    explicit SwapOfferItem(QObject* parent = nullptr);
    SwapOfferItem(const beam::wallet::SwapOffer& offer, const QDateTime& timeExpiration);
    bool operator==(const SwapOfferItem& other) const;

    QDateTime timeCreated() const;
    QDateTime timeExpiration() const;
    QString amountSend() const;
    QString amountReceive() const;
    QString rate() const;
    bool isOwnOffer() const;
    bool isSendBeam() const;

    beam::Amount rawAmountSend() const;
    beam::Amount rawAmountReceive() const;

    beam::wallet::TxParameters getTxParameters() const;
    beam::wallet::TxID getTxID() const;
    QString getSwapCoinName() const;
protected:
    void reset(const beam::wallet::SwapOffer& offer);

signals:

private:
    beamui::Currencies getSwapCoinType() const;

    beam::wallet::SwapOffer m_offer;          /// TxParameters subclass
    bool m_isBeamSide;                        /// pay beam to receive other coin
    QDateTime m_timeExpiration;
};
