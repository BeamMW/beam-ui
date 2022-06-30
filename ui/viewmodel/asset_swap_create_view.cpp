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

AssetSwapCreateViewModel::AssetSwapCreateViewModel()
    : _amgr(AppModel::getInstance().getAssets())
{
}

QList<QMap<QString, QVariant>> AssetSwapCreateViewModel::getCurrenciesList() const
{
    auto list = _amgr->getAssetsListFull();
    if (list.empty())
    {
        return getMyCurrenciesList();
    }
    return list;
}

QList<QMap<QString, QVariant>> AssetSwapCreateViewModel::getMyCurrenciesList() const
{
    return _amgr->getAssetsList();
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

QString AssetSwapCreateViewModel::getAmountSend() const
{
    return beamui::AmountToUIString(_amountSendGrothes);
}

void AssetSwapCreateViewModel::setAmountSend(QString value)
{
    auto amount = beamui::UIStringToAmount(value);
    if (amount != _amountSendGrothes)
    {
        _amountSendGrothes = amount;
        emit amountSendChanged();
    }
}

// unsigned int AssetSwapCreateViewModel::getFee() const
// {
//     return _feeGrothes;
// }

// void AssetSwapCreateViewModel::setFee(unsigned int value)
// {

// }

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
