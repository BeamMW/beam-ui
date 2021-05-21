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

#include "swap_offer_item.h"
#include "utility/helpers.h"
#include "wallet/core/common.h"
#include "viewmodel/ui_helpers.h"
#include "viewmodel/qml_globals.h"


SwapOfferItem::SwapOfferItem(QObject* parent /* = nullptr*/)
    : QObject(parent)
    , m_isBeamSide(true)
{

}

SwapOfferItem::SwapOfferItem(const beam::wallet::SwapOffer& offer,
                             const QDateTime& timeExpiration)
    : m_offer{offer}
    , m_isBeamSide{offer.isBeamSide()}
    , m_timeExpiration{timeExpiration} 
{}

bool SwapOfferItem::operator==(const SwapOfferItem& other) const
{
    return getTxID() == other.getTxID();
}

QDateTime SwapOfferItem::timeCreated() const
{
    QDateTime datetime;
    datetime.setTime_t(m_offer.timeCreated());
    return datetime;
}

QDateTime SwapOfferItem::timeExpiration() const
{
    return m_timeExpiration;
}

beam::Amount SwapOfferItem::rawAmountSend() const
{
    return isSendBeam() ? m_offer.amountBeam() : m_offer.amountSwapCoin(); 
}

beam::Amount SwapOfferItem::rawAmountReceive() const
{
    return isSendBeam() ? m_offer.amountSwapCoin() : m_offer.amountBeam(); 
}

QString SwapOfferItem::rate() const
{
    beam::Amount otherCoinAmount =
        isSendBeam() ? rawAmountReceive() : rawAmountSend();
    beam::Amount beamAmount =
        isSendBeam() ? rawAmountSend() : rawAmountReceive();

    if (!beamAmount) return QString();

    return QMLGlobals::divideWithPrecision(
        beamui::AmountToUIString(otherCoinAmount, getSwapCoinType(), false),
        beamui::AmountToUIString(beamAmount), 
        beamui::getCurrencyDecimals(getSwapCoinType()));
}

QString SwapOfferItem::amountSend() const
{
    auto coinType = isSendBeam() ? beamui::Currencies::Beam : getSwapCoinType();
    return beamui::AmountToUIString(rawAmountSend(), coinType);
}

QString SwapOfferItem::amountReceive() const
{
    auto coinType = isSendBeam() ? getSwapCoinType() : beamui::Currencies::Beam;
    return beamui::AmountToUIString(rawAmountReceive(), coinType);
}

bool SwapOfferItem::isOwnOffer() const
{
    return m_offer.m_isOwn;
}

bool SwapOfferItem::isSendBeam() const
{
    return m_offer.m_isOwn ? !m_isBeamSide : m_isBeamSide;
}

beam::wallet::TxParameters SwapOfferItem::getTxParameters() const
{
    return m_offer;
}

beam::wallet::TxID SwapOfferItem::getTxID() const
{
    return m_offer.m_txId;    
}

beamui::Currencies SwapOfferItem::getSwapCoinType() const
{
    return beamui::convertSwapCoinToCurrency(m_offer.swapCoinType());
}

QString SwapOfferItem::getSwapCoinName() const
{
    return toString(getSwapCoinType());
}

void SwapOfferItem::reset(const beam::wallet::SwapOffer& offer)
{
    m_offer = offer;
    m_isBeamSide = offer.isBeamSide();
}
