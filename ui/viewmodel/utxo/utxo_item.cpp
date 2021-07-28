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

#include "utxo_item.h"
#include "model/app_model.h"
#include "viewmodel/ui_helpers.h"
#include "wallet/core/common.h"

using namespace beam;
using namespace std;
using namespace beamui;

bool BaseUtxoItem::operator==(const BaseUtxoItem& other) const
{
    return getHash() == other.getHash();
}

UtxoItem::UtxoItem(beam::wallet::Coin coin)
    : _coin(std::move(coin))
{
    _minConfirmations = _coin.m_offset;
}

uint64_t UtxoItem::getHash() const
{
    ECC::Hash::Value hv;
    _coin.m_ID.get_Hash(hv);

    return static_cast<uint64_t>(*reinterpret_cast<uint64_t*>(hv.m_pData));
}

QString UtxoItem::getAmount() const
{
    return AmountToUIString(rawAmount());
}

QString UtxoItem::maturity() const
{
    if (!_coin.IsMaturityValid())
        return QString{ "-" };
    return QString::number(rawMaturity());
}

QString UtxoItem::maturityPercentage() const
{
    return QString{ "100" };
}

QString UtxoItem::maturityTimeLeft() const
{
    return QString::number(rawMaturityTimeLeft());
}

UtxoViewStatus::EnStatus UtxoItem::status() const
{
    using namespace beam::wallet;

    switch (_coin.m_status)
    {
    case Coin::Available:
        return UtxoViewStatus::Available;
    case Coin::Maturing:
        return UtxoViewStatus::Maturing;
    case Coin::Unavailable:
        return UtxoViewStatus::Unavailable;
    case Coin::Outgoing:
        return UtxoViewStatus::Outgoing;
    case Coin::Incoming:
        return UtxoViewStatus::Incoming;
    case Coin::Spent:
        return UtxoViewStatus::Spent;
    default:
        assert(false && "Unknown key type");
    }

    return UtxoViewStatus::Undefined;
}

UtxoViewType::EnType UtxoItem::type() const
{
    switch (_coin.m_ID.m_Type)
    {
    case Key::Type::Comission: return UtxoViewType::Comission;
    case Key::Type::Coinbase: return UtxoViewType::Coinbase;
    case Key::Type::Regular: return UtxoViewType::Regular;
    case Key::Type::Change: return UtxoViewType::Change;
    case Key::Type::Treasury: return UtxoViewType::Treasury;
    }

    return UtxoViewType::Undefined;
}

beam::Amount UtxoItem::rawAmount() const
{
    return _coin.m_ID.m_Value;
}

const beam::wallet::Coin::ID& UtxoItem::get_ID() const
{
    return _coin.m_ID;
}

beam::Height UtxoItem::rawMaturity() const
{
    return _coin.get_Maturity(_minConfirmations);
}

uint16_t UtxoItem::rawMaturityTimeLeft() const
{
    auto walletModel = AppModel::getInstance().getWalletModel();
    if (walletModel->getCurrentHeight() < rawMaturity())
    {
        auto blocksLeft = rawMaturity() - walletModel->getCurrentHeight();
        return blocksLeft / 60;
    }

    return 0;
}

beam::Asset::ID UtxoItem::getAssetId() const
{
    return _coin.m_ID.m_AssetID;
}

// ShieldedCoinItem
ShieldedCoinItem::ShieldedCoinItem()
    : _walletModel{*AppModel::getInstance().getWalletModel()}
{
}

ShieldedCoinItem::ShieldedCoinItem(beam::wallet::ShieldedCoin coin)
    : _walletModel{*AppModel::getInstance().getWalletModel()}
    , _coin(std::move(coin))
{
}

uint64_t ShieldedCoinItem::getHash() const
{
    // maybe we don't need such a strong hashing here
    ECC::Hash::Processor hp;
    hp << _coin.m_TxoID
       << _coin.m_CoinID.m_Value
       << _coin.m_CoinID.m_AssetID;
    ECC::Hash::Value hv;
    hp >> hv;
    return static_cast<uint64_t>(*reinterpret_cast<uint64_t*>(hv.m_pData));
}

QString ShieldedCoinItem::getAmount() const
{
    return AmountToUIString(rawAmount());
}

QString ShieldedCoinItem::maturity() const
{
    if (!_coin.IsMaturityValid())
        return QString{ "-" };
    return QString::number(rawMaturity());
}

QString ShieldedCoinItem::maturityPercentage() const
{
    return QString::number(_walletModel.getMarurityProgress(_coin));
}

QString ShieldedCoinItem::maturityTimeLeft() const
{
    return QString::number(rawMaturityTimeLeft());
}

UtxoViewStatus::EnStatus ShieldedCoinItem::status() const
{
    using namespace beam::wallet;

    switch (_coin.m_Status)
    {
    case ShieldedCoin::Available:
        return UtxoViewStatus::Available;
    case ShieldedCoin::Maturing:
        return UtxoViewStatus::MaturingMP;
    case ShieldedCoin::Unavailable:
        return UtxoViewStatus::Unavailable;
    case ShieldedCoin::Outgoing:
        return UtxoViewStatus::Outgoing;
    case ShieldedCoin::Incoming:
        return UtxoViewStatus::Incoming;
    case ShieldedCoin::Spent:
        return UtxoViewStatus::Spent;
    default:
        assert(false && "Unknown key type");
    }

    return UtxoViewStatus::Undefined;
}

UtxoViewType::EnType ShieldedCoinItem::type() const
{
    return UtxoViewType::Shielded;
}

beam::Amount ShieldedCoinItem::rawAmount() const
{
    return _coin.m_CoinID.m_Value;
}

beam::Height ShieldedCoinItem::rawMaturity() const
{
    return _coin.m_confirmHeight;
}

uint16_t ShieldedCoinItem::rawMaturityTimeLeft() const
{
    return _walletModel.getMaturityHoursLeft(_coin);
}

beam::Asset::ID ShieldedCoinItem::getAssetId() const
{
    return _coin.m_CoinID.m_AssetID;
}