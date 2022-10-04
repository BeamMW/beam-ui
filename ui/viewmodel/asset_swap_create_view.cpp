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
    , _offerExpires(kExpireOptions[0])
{
    connect(_walletModel.get(), &WalletModel::generatedNewAddress,    this, &AssetSwapCreateViewModel::onGeneratedNewAddress);
    connect(_walletModel.get(), &WalletModel::coinsSelected,          this, &AssetSwapCreateViewModel::onCoinsSelected);
    connect(_walletModel.get(), &WalletModel::assetsSwapParamsLoaded, this, &AssetSwapCreateViewModel::onAssetsSwapParamsLoaded);

    _myCurrenciesList = _amgr->getAssetsList();
    _currenciesList = _amgr->getAssetsListFull();
    if (_currenciesList.empty())
    {
        _currenciesList = _myCurrenciesList;
    }

    uint index = 0;
    for (const auto& currencyInfo: _currenciesList)
    {
        if (currencyInfo["verified"].toBool())
        {
            setReceiveAssetIndexImpl(index);
            break;
        }
        ++index;
    }

    _walletModel->getAsync()->loadDexOrderParams();
    _walletModel->getAsync()->generateNewAddress();
}

void AssetSwapCreateViewModel::publishOffer()
{
    _receiverAddress.m_label = _comment.toStdString();
    _receiverAddress.m_duration = beam::wallet::WalletAddress::AddressExpirationAuto;
    _walletModel->getAsync()->saveAddress(_receiverAddress);

    beam::wallet::DexOrder orderObj(
        beam::wallet::DexOrderID::generate(),
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
}

void AssetSwapCreateViewModel::onCoinsSelected(const beam::wallet::CoinsSelectionInfo& selectionRes)
{
    if (selectionRes.m_requestedSum != _amountToSendGrothes || selectionRes.m_assetID != _sendAsset)
    {
        return;
    }

    _isEnoughtToSend = selectionRes.m_selectedSumAsset >= selectionRes.m_requestedSum;
    _maxAmountToSendGrothes = selectionRes.get_NettoValue();
    emit canCreateChanged();
}

void AssetSwapCreateViewModel::onAssetsSwapParamsLoaded(const beam::ByteBuffer& params)
{
    beam::Deserializer der;
    der.reset(params);

    int sendAssetIndex = 0;
    der & sendAssetIndex;
    setSendAssetIndex(sendAssetIndex);

    int receiveAssetIndex = 0;
    der & receiveAssetIndex;
    setReceiveAssetIndex(receiveAssetIndex);

    int offerExpiresIndex = 0;
    der & offerExpiresIndex;
    setOfferExpires(offerExpiresIndex);
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
        emit canCreateChanged();
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
        emit canCreateChanged();

        _walletModel->getAsync()->selectCoins(
            _amountToSendGrothes,
            0,
            _sendAsset,
            false);
    }
}

uint AssetSwapCreateViewModel::getReceiveAssetIndex() const
{
    return _receiveAssetIndex;
}

void AssetSwapCreateViewModel::setReceiveAssetIndexImpl(int value)
{
    if (value >= _currenciesList.size())
    {
        value = _currenciesList.size() - 1;
    }

    if (_receiveAssetIndex != value)
    {
        _receiveAssetIndex = value;
        emit receiveAssetIndexChanged();

        auto assetsInfoMap = _currenciesList[_receiveAssetIndex];
        _receiveAsset = assetsInfoMap[kAssedIdField].toUInt();
        _receiveAssetSname = _receiveAsset == beam::Asset::s_BeamID
            ? kBeamAssetSName
            : assetsInfoMap[kUnitNameField].toString().toStdString();

        emit canCreateChanged();
    }
}

void AssetSwapCreateViewModel::setReceiveAssetIndex(int value)
{
    if (_receiveAssetIndex != value)
    {
        setReceiveAssetIndexImpl(value);
        saveLastOfferState();
    }
}

uint AssetSwapCreateViewModel::getSendAssetIndex() const
{
    return _sendAssetIndex;
}

void AssetSwapCreateViewModel::setSendAssetIndex(int value)
{
    if (value >= _myCurrenciesList.size())
    {
        value = _myCurrenciesList.size() - 1;
    }

    if (_sendAssetIndex != value)
    {
        _sendAssetIndex = value;
        emit sendAssetIndexChanged();

        auto assetsInfoMap = _myCurrenciesList[_sendAssetIndex];
        _sendAsset = assetsInfoMap[kAssedIdField].toUInt();
        _sendAssetSname = _sendAsset == beam::Asset::s_BeamID
            ? kBeamAssetSName
            : assetsInfoMap[kUnitNameField].toString().toStdString();

        saveLastOfferState();

        emit canCreateChanged();
    }
}

void AssetSwapCreateViewModel::setOfferExpires(int value)
{
    if (_offerExpiresIndex != value)
    {
        _offerExpiresIndex = value;
        _offerExpires = kExpireOptions[value];
        saveLastOfferState();
        emit offerExpiresChanged();
    }
}

int AssetSwapCreateViewModel::getOfferExpires() const
{
    return _offerExpiresIndex;
}

void AssetSwapCreateViewModel::setComment(const QString& value)
{
    auto trimmed = value.trimmed();
    if (_comment != trimmed)
    {
        _comment = trimmed;
        emit commentChanged();
        emit commentValidChanged();
        emit canCreateChanged();
    }
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

bool AssetSwapCreateViewModel::getCanCreate() const
{
    return _receiveAsset != _sendAsset && _amountToReceiveGrothes && _amountToSendGrothes && _isEnoughtToSend && getCommentValid();
}

bool AssetSwapCreateViewModel::getIsEnough() const
{
    return _amountToSendGrothes == 0 || _isEnoughtToSend;
}

QString AssetSwapCreateViewModel::getMaxSendAmount() const
{
    return beamui::AmountToUIString(_maxAmountToSendGrothes);
}

bool AssetSwapCreateViewModel::getIsAssetsSame() const
{
    return _receiveAsset == _sendAsset;
}

bool AssetSwapCreateViewModel::getCommentValid() const
{
    return !_walletModel->isAddressWithCommentExist(_comment.toStdString());
}

void AssetSwapCreateViewModel::saveLastOfferState()
{
    beam::ByteBuffer buffer;
    beam::Serializer ser;
    ser & _sendAssetIndex;
    ser & _receiveAssetIndex;
    ser & _offerExpiresIndex;
    ser.swap_buf(buffer);
    _walletModel->getAsync()->storeDexOrderParams(buffer);
}
