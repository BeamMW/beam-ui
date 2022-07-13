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
#include "viewmodel/ui_helpers.h"
#include "wallet/client/extensions/dex_board/dex_order.h"
#include "wallet/client/extensions/dex_board/asset_swap_order.h"
#include "viewmodel/dex/dex_orders_list.h"

#include <qdebug.h>

AssetSwapCreateViewModel::AssetSwapCreateViewModel()
    : _walletModel(AppModel::getInstance().getWalletModel())
    , _amgr(AppModel::getInstance().getAssets())
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

    auto expires = getTimestamp();
    expires += 60 * 60 * 24; // 24 hours for tests

    // DexOrder order(
    //     DexOrderID::generate(),
    //     _receiverAddress.m_walletID,
    //     _receiverAddress.m_OwnID,
    //     DexMarket(_sendAsset, _receiveAsset),
    //     DexMarketSide::Sell,
    //     _amountSendGrothes,
    //     _amountToReceiveGrothes,
    //     expires
    //     );

    AssetSwapOrder orderObj(
        DexOrderID::generate(),
        _receiverAddress.m_walletID,
        _receiverAddress.m_OwnID,
        _sendAsset,
        _amountSendGrothes,
        _sendAssetSname,
        _receiveAsset,
        _amountToReceiveGrothes,
        _receiveAssetSname,
        expires
    );

    // _walletModel->getAsync()->publishDexOrder(order);
    _walletModel->getAsync()->publishAssetSwapOrder(orderObj);
}

void AssetSwapCreateViewModel::onGeneratedNewAddress(const beam::wallet::WalletAddress& addr)
{
    _receiverAddress = addr;
    _walletModel->getAsync()->loadAssetSwapParams();
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
    }
}

QString AssetSwapCreateViewModel::getAmountToSend() const
{
    return beamui::AmountToUIString(_amountSendGrothes);
}

void AssetSwapCreateViewModel::setAmountToSend(QString value)
{
    auto amount = beamui::UIStringToAmount(value);
    if (amount != _amountSendGrothes)
    {
        _amountSendGrothes = amount;
        emit amountSendChanged();
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
        _receiveAsset = assetsInfoMap["assetId"].toUInt();
        _receiveAssetSname = assetsInfoMap["unitName"].toString().toStdString();

        // for (auto it = assetsInfoMap.cbegin(); it != assetsInfoMap.cend(); ++it)
        // {
        //     qDebug() << it.key();
        // }
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
        _sendAsset = assetsInfoMap["assetId"].toUInt();
        _sendAssetSname = assetsInfoMap["unitName"].toString().toStdString();
    }
}

void AssetSwapCreateViewModel::setOfferExpires(int value)
{

}

int AssetSwapCreateViewModel::getOfferExpires() const
{
    return _offerExpires;
}

void AssetSwapCreateViewModel::setComment(const QString& value)
{

}

QString AssetSwapCreateViewModel::getComment() const
{
    return _comment;
}

// void AssetSwapCreateViewModel::setTransactionToken(const QString& value)
// {

// }

// QString AssetSwapCreateViewModel::getTransactionToken() const
// {
//     return _token;
// }
