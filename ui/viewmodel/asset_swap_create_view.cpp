// Copyright 2022 The Beam Team
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

#include "asset_swap_create_view.h"
#include "model/app_model.h"
#include "viewmodel/qml_globals.h"
#include "viewmodel/ui_helpers.h"
#include "wallet/client/extensions/dex_board/dex_order.h"
#include "wallet/transactions/dex/dex_tx.h"

#include <qdebug.h>

namespace
{
    const char kBeamAssetSName[] = "BEAM";
    const char kAssedIdField[] = "assetId";
    const char kUnitNameField[] = "unitName";
    const uint64_t kExpireOptions[] = {30 * 60, 60 * 60, 2 * 60 * 60, 6 * 60 * 60, 12 * 60 * 60};
}  // namespace

AssetSwapCreateViewModel::AssetSwapCreateViewModel()
    : _walletModel(AppModel::getInstance().getWalletModel())
    , _amgr(AppModel::getInstance().getAssets())
    , _receiveAssetSname(kBeamAssetSName)
    , _sendAssetSname(kBeamAssetSName)
{
    connect(_walletModel.get(), &WalletModel::generatedNewAddress, this, &AssetSwapCreateViewModel::onGeneratedNewAddress);

    _myCurrenciesList = _amgr->getAssetsList();
    _currenciesList = _amgr->getAssetsListFull();
    if (_currenciesList.empty())
    {
        _currenciesList = _myCurrenciesList;
    }

    _walletModel->getAsync()->generateNewAddress();
}

void AssetSwapCreateViewModel::publishOffer()
{
    using namespace beam;
    using namespace beam::wallet;

    _walletModel->getAsync()->saveAddress(_receiverAddress);

    DexOrder orderObj(
        DexOrderID::generate(),
        _receiverAddress.m_walletID,
        _receiverAddress.m_OwnID,
        _sendAsset,
        _amountToSendGrothes,
        _sendAssetSname,
        _receiveAsset,
        _amountToReceiveGrothes,
        _receiveAssetSname,
        _offerExpires
    );

    _walletModel->getAsync()->publishDexOrder(orderObj);
}

void AssetSwapCreateViewModel::onGeneratedNewAddress(const beam::wallet::WalletAddress& addr)
{
    _receiverAddress = addr;
    _walletModel->getAsync()->loadDexOrderParams();
}

QList<QMap<QString, QVariant>> AssetSwapCreateViewModel::getCurrenciesList() const
{
    return _currenciesList;
}

QList<QMap<QString, QVariant>> AssetSwapCreateViewModel::getMyCurrenciesList() const
{
    return _myCurrenciesList;
}

QString AssetSwapCreateViewModel::getAmountToReceive() const
{
    return beamui::AmountToUIString(_amountToReceiveGrothes);
}

void AssetSwapCreateViewModel::setAmountToReceive(QString value)
{
    auto amount = beamui::UIStringToAmount(value);
    if (amount != _amountToReceiveGrothes)
    {
        _amountToReceiveGrothes = amount;
        emit amountReceiveChanged();
        emit rateChanged();
    }
}

QString AssetSwapCreateViewModel::getAmountToSend() const
{
    return beamui::AmountToUIString(_amountToSendGrothes);
}

void AssetSwapCreateViewModel::setAmountToSend(QString value)
{
    auto amount = beamui::UIStringToAmount(value);
    if (amount != _amountToSendGrothes)
    {
        _amountToSendGrothes = amount;
        emit amountSendChanged();
        emit rateChanged();
    }
}

uint AssetSwapCreateViewModel::getReceiveAssetIndex() const
{
    return _receiveAssetIndex;
}

void AssetSwapCreateViewModel::setReceiveAssetIndex(uint value)
{
    if (_receiveAssetIndex != value)
    {
        _receiveAssetIndex = value;
        emit receiveAssetIndexChanged();

        auto assetsInfoMap = _currenciesList[_receiveAssetIndex];
        _receiveAsset = assetsInfoMap[kAssedIdField].toUInt();
        _receiveAssetSname = _receiveAsset == beam::Asset::s_BeamID
            ? kBeamAssetSName
            : assetsInfoMap[kUnitNameField].toString().toStdString();
    }
}

uint AssetSwapCreateViewModel::getSendAssetIndex() const
{
    return _sendAssetIndex;
}

void AssetSwapCreateViewModel::setSendAssetIndex(uint value)
{
    if (_sendAssetIndex != value)
    {
        _sendAssetIndex = value;
        emit sendAssetIndexChanged();

        auto assetsInfoMap = _myCurrenciesList[_sendAssetIndex];
        _sendAsset = assetsInfoMap[kAssedIdField].toUInt();
        _sendAssetSname = _sendAsset == beam::Asset::s_BeamID
            ? kBeamAssetSName
            : assetsInfoMap[kUnitNameField].toString().toStdString();
    }
}

void AssetSwapCreateViewModel::setOfferExpires(int value)
{
    _offerExpires = kExpireOptions[value];
    emit offerExpiresChanged();
}

int AssetSwapCreateViewModel::getOfferExpires() const
{
    return _offerExpires;
}

void AssetSwapCreateViewModel::setComment(const QString& value)
{
    _comment = value;
    emit commentChanged();
}

QString AssetSwapCreateViewModel::getComment() const
{
    return _comment;
}

QString AssetSwapCreateViewModel::getRate() const
{
    if (!_amountToSendGrothes || !_amountToReceiveGrothes) return "-";
    return QMLGlobals::divideWithPrecision(
                beamui::AmountToUIString(_amountToReceiveGrothes),
                beamui::AmountToUIString(_amountToSendGrothes),
                beam::wallet::kDexOrderRatePrecission);
}

// void AssetSwapCreateViewModel::setTransactionToken(const QString& value)
// {

// }

// QString AssetSwapCreateViewModel::getTransactionToken() const
// {
//     return _token;
// }
