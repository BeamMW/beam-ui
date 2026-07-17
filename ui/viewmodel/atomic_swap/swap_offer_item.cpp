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
#include "wallet/transactions/swaps/common.h"
#include "viewmodel/ui_helpers.h"
#include "viewmodel/qml_globals.h"
#include "swap_utils.h"
#include <algorithm>

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
    datetime.setSecsSinceEpoch(m_offer.timeCreated());
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

    QString otherAmountStr;
    uint8_t otherDecimals = beamui::getCurrencyDecimals(getSwapCoinType());

    if (m_offer.ResolveCoin() == beam::wallet::AtomicSwapCoin::Erc20Token)
    {
        uint8_t onChainDecimals = 0;
        m_offer.GetParameter(beam::wallet::TxParameterID::AtomicSwapTokenDecimals, onChainDecimals);
        otherDecimals = beamui::tokenWalletDecimals(onChainDecimals);
        otherAmountStr = beamui::AmountToUIStringExactDecimals(otherCoinAmount, otherDecimals);
    }
    else
    {
        otherAmountStr = beamui::AmountToUIString(otherCoinAmount, getSwapCoinType(), false);
    }

    return QMLGlobals::divideWithPrecision(
        otherAmountStr,
        beamui::AmountToUIString(beamAmount),
        otherDecimals);
}

QString SwapOfferItem::amountSend() const
{
    return isSendBeam() ? amountBeamSide(rawAmountSend()) : amountSwapCoinSide(rawAmountSend());
}

QString SwapOfferItem::amountReceive() const
{
    return isSendBeam() ? amountSwapCoinSide(rawAmountReceive()) : amountBeamSide(rawAmountReceive());
}

QString SwapOfferItem::amountBeamSide(beam::Amount value) const
{
    beam::Asset::ID assetId = 0;
    m_offer.GetParameter(beam::wallet::TxParameterID::AtomicSwapBeamAssetID, assetId);
    if (assetId != 0)
    {
        std::string assetName;
        m_offer.GetParameter(beam::wallet::TxParameterID::AtomicSwapBeamAssetName, assetName);
        return beamui::AmountToUIString(value, QString::fromStdString(assetName));
    }
    return beamui::AmountToUIString(value, beamui::Currencies::Beam);
}

QString SwapOfferItem::amountSwapCoinSide(beam::Amount value) const
{
    if (m_offer.ResolveCoin() == beam::wallet::AtomicSwapCoin::Erc20Token)
    {
        return swapui::erc20AmountString(m_offer, value, true);
    }
    return beamui::AmountToUIString(value, getSwapCoinType());
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
    if (m_offer.ResolveCoin() == beam::wallet::AtomicSwapCoin::Erc20Token)
    {
        return swapui::erc20Symbol(m_offer);
    }
    return toString(getSwapCoinType());
}

beam::wallet::AtomicSwapCoin SwapOfferItem::resolvedSwapCoin() const
{
    return m_offer.ResolveCoin();
}

QString SwapOfferItem::getBeamSideName() const
{
    beam::Asset::ID assetId = 0;
    m_offer.GetParameter(beam::wallet::TxParameterID::AtomicSwapBeamAssetID, assetId);
    if (assetId != 0)
    {
        std::string assetName;
        m_offer.GetParameter(beam::wallet::TxParameterID::AtomicSwapBeamAssetName, assetName);
        return assetName.empty() ? QString("beam") : QString::fromStdString(assetName);
    }
    return "beam";
}

void SwapOfferItem::reset(const beam::wallet::SwapOffer& offer)
{
    m_offer = offer;
    m_isBeamSide = offer.isBeamSide();
}
