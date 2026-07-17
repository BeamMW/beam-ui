// Copyright 2018-2024 The Beam Team
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

#include "receive_swap_view.h"
#include "ui_helpers.h"
#include "model/app_model.h"
#include "wallet/transactions/swaps/utils.h"
#include <QClipboard>
#include "qml_globals.h"
#include "fee_helpers.h"
#include "wallet/transactions/swaps/bridges/ethereum/ethereum_side.h"
#include "wallet/transactions/swaps/bridges/ethereum/common.h"
#include "atomic_swap/swap_utils.h"
#include "model/assets_manager.h"
#include "model/settings.h"
#include <algorithm>

namespace {
    enum
    {
        OfferExpires30min = 0,
        OfferExpires1h,
        OfferExpires2h,
        OfferExpires6h,
        OfferExpires12h
    };

    double GetHourCount(int offerExpires)
    {
        switch (offerExpires)
        {
        case OfferExpires30min:
            return 0.5;
        case OfferExpires1h:
            return 1.0;
        case OfferExpires2h:
            return 2.0;
        case OfferExpires6h:
            return 6.0;
        case OfferExpires12h:
            return 12.0;
        default:
        {
            assert(false && "Unexpected value!");
            return 0;
        }
        }
    }

    beam::Height GetBlockCount(int offerExpires)
    {
        return GetHourCount(offerExpires) * 60;
    }
}

ReceiveSwapViewModel::ReceiveSwapViewModel()
    : _amountToReceiveGrothes(0)
    , _amountSentGrothes(0)
    , _receiveFeeGrothes(0)
    , _sentFeeGrothes(0)
    , _receiveCurrency(OldWalletCurrency::OldCurrency::CurrBeam)
    , _sentCurrency(OldWalletCurrency::OldCurrency::CurrBitcoin)
    , _offerExpires(OfferExpires12h)
    , _saveParamsAllowed(false)
    , _walletModel(AppModel::getInstance().getWalletModel())
    , _rates(AppModel::getInstance().getRates())
    , _txParameters(beam::wallet::CreateSwapTransactionParameters())
    , _isBeamSide(false)
    , _minimalBeamFeeGrothes(minimalFee(OldWalletCurrency::OldCurrency::CurrBeam, false))
{
    //connect(_walletModel,  &WalletModel::generatedNewAddress, this, &ReceiveSwapViewModel::onGeneratedNewAddress);
    connect(_walletModel,  &WalletModel::swapParamsLoaded, this, &ReceiveSwapViewModel::onSwapParamsLoaded);
    connect(_walletModel,  &WalletModel::newAddressFailed, this, &ReceiveSwapViewModel::newAddressFailed);
    connect(_walletModel,  &WalletModel::walletStatusChanged, this, &ReceiveSwapViewModel::updateTransactionToken);
    connect(_walletModel,  &WalletModel::coinsSelected, this, &ReceiveSwapViewModel::onCoinsSelected);
    connect(_rates.get(), &ExchangeRatesManager::rateUnitChanged, this, &ReceiveSwapViewModel::secondCurrencyUnitNameChanged);
    connect(_rates.get(), &ExchangeRatesManager::activeRateChanged, this, &ReceiveSwapViewModel::secondCurrencyRateChanged);
    connect(AppModel::getInstance().getAssets().get(), &AssetsManager::assetsListChanged, this, [this]()
    {
        // assets feed currList (one entry per asset), so a change there
        // reshuffles both the combo model and index mapping
        emit currListChanged();
        emit sentCurrencyIndexChanged();
        emit receiveCurrencyIndexChanged();
        if (_selectedBeamAssetId != 0)
        {
            // drop the selection if its asset vanished from the wallet
            for (const auto& asset : AppModel::getInstance().getAssets()->getAssetsList())
            {
                if (asset.value("assetId").toUInt() == static_cast<uint>(_selectedBeamAssetId))
                {
                    return;
                }
            }
            setSelectedBeamAssetId(0);
        }
    });
    connect(&AppModel::getInstance().getSettings(), &WalletSettings::ethCustomTokensChanged, this, [this]()
    {
        // the stored token list feeds currList (one entry per token), so a
        // change there reshuffles both the combo model and index mapping
        emit currListChanged();
        emit sentCurrencyIndexChanged();
        emit receiveCurrencyIndexChanged();
        if (!_selectedTokenContract.isEmpty())
        {
            // drop the selection if its token was removed in the settings
            for (const auto& token : getCustomTokensList())
            {
                if (token.value("contract").toString() == _selectedTokenContract)
                {
                    return;
                }
            }
            setSelectedTokenContract(QString());
        }
    });
    // the combo index depends on both the settled currency and (for the
    // Ethereum side) which token is stamped; keep it in sync with either
    connect(this, &ReceiveSwapViewModel::sentCurrencyChanged, this, &ReceiveSwapViewModel::sentCurrencyIndexChanged);
    connect(this, &ReceiveSwapViewModel::receiveCurrencyChanged, this, &ReceiveSwapViewModel::receiveCurrencyIndexChanged);
    connect(this, &ReceiveSwapViewModel::selectedTokenChanged, this, &ReceiveSwapViewModel::sentCurrencyIndexChanged);
    connect(this, &ReceiveSwapViewModel::selectedTokenChanged, this, &ReceiveSwapViewModel::receiveCurrencyIndexChanged);
    connect(this, &ReceiveSwapViewModel::selectedBeamAssetChanged, this, &ReceiveSwapViewModel::sentCurrencyIndexChanged);
    connect(this, &ReceiveSwapViewModel::selectedBeamAssetChanged, this, &ReceiveSwapViewModel::receiveCurrencyIndexChanged);

    swapui::connectFeeRateClients(this, [this]()
    {
        ++_feeRatesRevision;
        emit feeRatesRevisionChanged();
    });

    generateNewAddress();
    updateTransactionToken();
}

unsigned int ReceiveSwapViewModel::getFeeRatesRevision() const
{
    return _feeRatesRevision;
}

//void ReceiveSwapViewModel::onGeneratedNewAddress(const beam::wallet::WalletAddress& addr)
//{
//    _receiverAddress = addr;
//    emit receiverAddressChanged();
//    updateTransactionToken();
//    loadSwapParams();
//}

void ReceiveSwapViewModel::loadSwapParams()
{
    _walletModel->getAsync()->loadSwapParams();
}

void ReceiveSwapViewModel::storeSwapParams()
{
    if(!_saveParamsAllowed) return;

    try {
        beam::Serializer ser;
        ser & _receiveCurrency
            & _sentCurrency
            & _receiveFeeGrothes
            & _sentFeeGrothes;

        beam::ByteBuffer buffer;
        ser.swap_buf(buffer);
        _walletModel->getAsync()->storeSwapParams(buffer);
    }
    catch(...)
    {
        BEAM_LOG_ERROR() << "failed to serialize swap params";
    }
}

bool ReceiveSwapViewModel::isSendBeam() const
{
    return _isBeamSide;
}

QString ReceiveSwapViewModel::getRate() const
{
    beam::Amount otherCoinAmount =
        isSendBeam() ? _amountToReceiveGrothes : _amountSentGrothes;
    beam::Amount beamAmount =
        isSendBeam() ? _amountSentGrothes : _amountToReceiveGrothes;

    if (!beamAmount) return QString();

    auto otherOldCurrency = isSendBeam() ? _receiveCurrency : _sentCurrency;
    uint8_t otherDecimals = effectiveDecimals(otherOldCurrency);

    return QMLGlobals::divideWithPrecision(
        beamui::AmountToUIStringExactDecimals(otherCoinAmount, otherDecimals),
        beamui::AmountToUIString(beamAmount),
        otherDecimals);
}

void ReceiveSwapViewModel::onSwapParamsLoaded(const beam::ByteBuffer& params)
{
    if(!params.empty())
    {
        try
        {
            beam::Deserializer der;
            der.reset(params);

            OldWalletCurrency::OldCurrency receiveCurrency, sentCurrency;
            beam::Amount receiveFee, sentFee;

            der & receiveCurrency
                & sentCurrency
                & receiveFee
                & sentFee;

            setReceiveCurrency(receiveCurrency);
            setSentCurrency(sentCurrency);

            if (receiveCurrency == OldWalletCurrency::OldCurrency::CurrBeam)
            {
                setReceiveFee(receiveFee);
            }
            else
            {
                setSentFee(sentFee);
            }
        }
        catch(...)
        {
            BEAM_LOG_ERROR() << "failed to deserialize swap params";
        }
    }

   _saveParamsAllowed = true;
}

void ReceiveSwapViewModel::onCoinsSelected(const beam::wallet::CoinsSelectionInfo& selectionRes)
{
    if (_sentCurrency == OldWalletCurrency::OldCurrency::CurrBeam)
    {
        _minimalBeamFeeGrothes = selectionRes.m_minimalExplicitFee;
        emit minimalBeamFeeGrothesChanged();

        if (_feeChangedByUI)
        {
            _feeChangedByUI = false;
            return;
        }

        _sentFeeGrothes = selectionRes.m_explicitFee;
        emit sentFeeChanged();
    }
}

QString ReceiveSwapViewModel::getAmountToReceive() const
{
    return beamui::AmountToUIStringExactDecimals(_amountToReceiveGrothes, effectiveDecimals(_receiveCurrency));
}

void ReceiveSwapViewModel::setAmountToReceive(QString value)
{
    auto amount = beamui::UIStringToAmountExactDecimals(value, effectiveDecimals(_receiveCurrency));
    if (amount != _amountToReceiveGrothes)
    {
        _amountToReceiveGrothes = amount;
        emit amountReceiveChanged();
        emit rateChanged();
        updateTransactionToken();
    }
}

QString ReceiveSwapViewModel::getAmountSent() const
{
    return beamui::AmountToUIStringExactDecimals(_amountSentGrothes, effectiveDecimals(_sentCurrency));
}

unsigned int ReceiveSwapViewModel::getReceiveFee() const
{
    return _receiveFeeGrothes;
}

void ReceiveSwapViewModel::setAmountSent(QString value)
{
    auto amount = beamui::UIStringToAmountExactDecimals(value, effectiveDecimals(_sentCurrency));
    if (amount != _amountSentGrothes)
    {
        bool isPreviouseSendWasZero = _amountSentGrothes == 0;
        bool isPreviouseGreaterThanNow = _amountSentGrothes > amount;
        _amountSentGrothes = amount;
        if (isPreviouseSendWasZero && _amountToReceiveGrothes) emit rateChanged();
        emit amountSentChanged();
        updateTransactionToken();
        if (_sentCurrency == OldWalletCurrency::OldCurrency::CurrBeam && _walletModel->hasShielded(beam::Asset::s_BeamID))
        {
            if (isPreviouseGreaterThanNow)
            {
                _minimalBeamFeeGrothes = minimalFee(OldWalletCurrency::OldCurrency::CurrBeam, false);
                _sentFeeGrothes = _minimalBeamFeeGrothes;
                emit minimalBeamFeeGrothesChanged();
                emit sentFeeChanged();
            }
            if (_amountSentGrothes)
            {
                _walletModel->getAsync()->selectCoins(_amountSentGrothes, _sentFeeGrothes, beam::Asset::s_BeamID);
            }
        }
    }
}

unsigned int ReceiveSwapViewModel::getSentFee() const
{
    return _sentFeeGrothes;
}

void ReceiveSwapViewModel::setSentFee(unsigned int value)
{
    if (value != _sentFeeGrothes)
    {
        _sentFeeGrothes = value;
        emit sentFeeChanged();
        updateTransactionToken();
        emit secondCurrencyRateChanged();
        storeSwapParams();

        if (_sentCurrency == OldWalletCurrency::OldCurrency::CurrBeam && _walletModel->hasShielded(beam::Asset::s_BeamID) && _amountSentGrothes)
        {
            _feeChangedByUI = true;
            _walletModel->getAsync()->selectCoins(_amountSentGrothes, _sentFeeGrothes, beam::Asset::s_BeamID);
        }
    }
}

OldWalletCurrency::OldCurrency ReceiveSwapViewModel::getReceiveCurrency() const
{
    return _receiveCurrency;
}

void ReceiveSwapViewModel::setReceiveCurrency(OldWalletCurrency::OldCurrency value)
{
    assert(value > OldWalletCurrency::OldCurrency::CurrStart && value < OldWalletCurrency::OldCurrency::CurrEnd);

    if (value != _receiveCurrency)
    {
        // different units for different currencies. example BTC and ETH
        QString amount = getAmountToReceive();
        _receiveCurrency = value;
        // deferred: the QML sides-flip updates sent/receive currency one binding at
        // a time, so the pair passes through transient states here
        QMetaObject::invokeMethod(this, &ReceiveSwapViewModel::syncExtendedSelections, Qt::QueuedConnection);
        setAmountToReceive(amount);
        emit receiveCurrencyChanged();
        updateTransactionToken();
        emit rateChanged();
        emit secondCurrencyRateChanged();
        storeSwapParams();
    }
}

OldWalletCurrency::OldCurrency ReceiveSwapViewModel::getSentCurrency() const
{
    return _sentCurrency;
}

void ReceiveSwapViewModel::setSentCurrency(OldWalletCurrency::OldCurrency value)
{
    assert(value > OldWalletCurrency::OldCurrency::CurrStart && value < OldWalletCurrency::OldCurrency::CurrEnd);

    if (value != _sentCurrency)
    {
        // different units for different currencies. example BTC and ETH
        QString amount = getAmountSent();
        _sentCurrency = value;
        // deferred: the QML sides-flip updates sent/receive currency one binding at
        // a time, so the pair passes through transient states here
        QMetaObject::invokeMethod(this, &ReceiveSwapViewModel::syncExtendedSelections, Qt::QueuedConnection);
        setAmountSent(amount);
        emit sentCurrencyChanged();
        updateTransactionToken();
        emit rateChanged();
        emit secondCurrencyRateChanged();
        storeSwapParams();
    }
}

void ReceiveSwapViewModel::setReceiveFee(unsigned int value)
{
    if (value != _receiveFeeGrothes)
    {
        _receiveFeeGrothes = value;
        emit receiveFeeChanged();
        emit enoughToReceiveChanged();
        updateTransactionToken();
        emit secondCurrencyRateChanged();
        storeSwapParams();
    }
}

int ReceiveSwapViewModel::getOfferExpires() const
{
    return _offerExpires;
}

void ReceiveSwapViewModel::setOfferExpires(int value)
{
    if (value != _offerExpires)
    {
        _offerExpires = value;
        emit offerExpiresChanged();
        updateTransactionToken();
    }
}

QString ReceiveSwapViewModel::getReceiverAddress() const
{
    return QString();
    //return beamui::toString(_receiverAddress.m_BbsAddr);
}

void ReceiveSwapViewModel::generateNewAddress()
{
    //_receiverAddress = {};
    //emit receiverAddressChanged();

    //setAddressComment("");
    //_walletModel->getAsync()->generateNewAddress();
}

void ReceiveSwapViewModel::setTransactionToken(const QString& value)
{
    if (_token != value)
    {
        _token = value;
        emit transactionTokenChanged();
    }
}

QString ReceiveSwapViewModel::getTransactionToken() const
{
    return _token;
}

QString ReceiveSwapViewModel::getAddressComment() const
{
    return _addressComment;
}

void ReceiveSwapViewModel::setAddressComment(const QString& value)
{
    auto trimmed = value.trimmed();
    if (_addressComment != trimmed)
    {
        _addressComment = trimmed;
        emit addressCommentChanged();
        emit commentValidChanged();
    }
}

bool ReceiveSwapViewModel::getCommentValid() const
{
    return !_walletModel->isAddressWithCommentExist(_addressComment.toStdString());
}

bool ReceiveSwapViewModel::isEnough() const
{
    if (_amountSentGrothes == 0)
        return true;

    beam::AmountBig::Number total = _amountSentGrothes;
    total += beam::AmountBig::Number(_sentFeeGrothes);

    if (_sentCurrency == OldWalletCurrency::OldCurrency::CurrBeam)
    {
        if (_selectedBeamAssetId != 0)
        {
            // the offered value is a Confidential Asset; the fee is still paid in BEAM
            return _walletModel->getAvailable(_selectedBeamAssetId) >= beam::AmountBig::Number(_amountSentGrothes) &&
                   _walletModel->getAvailable(beam::Asset::s_BeamID) >= beam::AmountBig::Number(_sentFeeGrothes);
        }
        return _walletModel->getAvailable(beam::Asset::s_BeamID) >= total;
    }

    auto swapCoin = convertCurrencyToSwapCoin(_sentCurrency);
    if (isEthereumBased(_sentCurrency))
    {
        if (isTokenSide(_sentCurrency))
        {
            return swapui::enoughEthForTokenLock(_sentFeeGrothes);
        }

        if (_sentCurrency == OldWalletCurrency::OldCurrency::CurrEthereum)
        {
            total = _amountSentGrothes + beam::wallet::EthereumSide::CalcLockTxFee(_sentFeeGrothes, swapCoin);
            
            return beam::AmountBig::Number(AppModel::getInstance().getSwapEthClient()->getAvailable(swapCoin)) >= total;
        }
        
        return AppModel::getInstance().getSwapEthClient()->getAvailable(swapCoin) >= _amountSentGrothes &&
            AppModel::getInstance().getSwapEthClient()->getAvailable(beam::wallet::AtomicSwapCoin::Ethereum) >= 
            beam::wallet::EthereumSide::CalcLockTxFee(_sentFeeGrothes, swapCoin);
    }

    // TODO sentFee is fee rate. should be corrected
    auto swapTotal = beam::AmountBig::Number(AppModel::getInstance().getSwapCoinClient(swapCoin)->getAvailable());
    return swapTotal > total;
}

bool ReceiveSwapViewModel::isEnoughToReceive() const
{
    if (isEthereumBased(_receiveCurrency))
    {
        auto swapCoin = convertCurrencyToSwapCoin(_receiveCurrency);
        auto fee = beam::wallet::EthereumSide::CalcWithdrawTxFee(_receiveFeeGrothes, swapCoin);
    
        return AppModel::getInstance().getSwapEthClient()->getAvailable(beam::wallet::AtomicSwapCoin::Ethereum) > fee;
    }
    return true;
}

bool ReceiveSwapViewModel::isSendFeeOK() const
{
    return _amountSentGrothes == 0 || isSwapFeeOK(_amountSentGrothes, _sentFeeGrothes, _sentCurrency);
}

bool ReceiveSwapViewModel::isReceiveFeeOK() const
{
    return _amountToReceiveGrothes == 0 || isSwapFeeOK(_amountToReceiveGrothes, _receiveFeeGrothes, _receiveCurrency);
}

void ReceiveSwapViewModel::saveAddress()
{
    using namespace beam::wallet;

    //if (getCommentValid()) {
    //    _receiverAddress.m_label = _addressComment.toStdString();
    //    _receiverAddress.m_duration = WalletAddress::AddressExpirationAuto;
    //    _walletModel->getAsync()->saveAddress(_receiverAddress);
    //}
}

void ReceiveSwapViewModel::startListen()
{
    using namespace beam::wallet;

    auto txParameters = TxParameters(_txParameters);
    if (!_addressComment.isEmpty())
    {
        std::string localComment = _addressComment.toStdString();
        txParameters.SetParameter(
            TxParameterID::Message,
            beam::ByteBuffer(localComment.begin(), localComment.end()));
    }

    _walletModel->getAsync()->startTransaction(std::move(txParameters));
}

void ReceiveSwapViewModel::publishToken()
{
    const auto& mirroredTxParams = MirrorSwapTxParams(_txParameters);
    const auto& readyForTokenizeTxParams =
        PrepareSwapTxParamsForTokenization(mirroredTxParams);

    auto txId = readyForTokenizeTxParams.GetTxID();
    auto publisherAddr =
        readyForTokenizeTxParams.GetParameter<beam::wallet::WalletID>(
            beam::wallet::TxParameterID::PeerAddr);
    auto coin =
        readyForTokenizeTxParams.GetParameter<beam::wallet::AtomicSwapCoin>(
            beam::wallet::TxParameterID::AtomicSwapCoin);
    if (publisherAddr && txId && coin)
    {
        beam::wallet::SwapOffer offer(*txId);
        offer.m_txId = *txId;
        offer.m_publisherId = *publisherAddr;
        offer.m_status = beam::wallet::SwapOfferStatus::Pending;
        offer.m_coin = *coin;
        offer.SetTxParameters(readyForTokenizeTxParams.Pack());
        
        _walletModel->getAsync()->publishSwapOffer(offer);
    }
}

void ReceiveSwapViewModel::updateTransactionToken()
{
    emit enoughChanged();
    emit isSendFeeOKChanged();
    emit isReceiveFeeOKChanged();

    _isBeamSide = (_sentCurrency == OldWalletCurrency::OldCurrency::CurrBeam);
    const auto otherCurrency = _isBeamSide ? _receiveCurrency : _sentCurrency;

    // a custom ERC-20 token replaces plain Ethereum as the swap coin: the core
    // (EthereumSide::IsERC20Token, offers board ExtendedOffer wire coin) keys on
    // AtomicSwapCoin::Erc20Token, the contract itself travels in param 43
    const bool tokenActive = isTokenSide(otherCurrency);
    auto swapCoin = tokenActive ? beam::wallet::AtomicSwapCoin::Erc20Token
                                : convertCurrencyToSwapCoin(otherCurrency);
    auto beamAmount =
        _isBeamSide ? _amountSentGrothes : _amountToReceiveGrothes;
    auto swapAmount =
        _isBeamSide ? _amountToReceiveGrothes : _amountSentGrothes;
    auto beamFee = _isBeamSide ? _sentFeeGrothes : _receiveFeeGrothes;
    auto swapFeeRate = _isBeamSide ? _receiveFeeGrothes : _sentFeeGrothes;


    QPointer<ReceiveSwapViewModel> guard = this;

    auto onSwapParams = [guard, this, tokenActive](beam::wallet::TxParameters&& params)
    {
        if (!guard)
            return;

        _txParameters = std::move(params);

        // both stampings re-checked against the current pair here: stale
        // selections must never ride on an unrelated offer
        if (_selectedBeamAssetId != 0 &&
            (_sentCurrency == OldWalletCurrency::OldCurrency::CurrBeam ||
             _receiveCurrency == OldWalletCurrency::OldCurrency::CurrBeam))
        {
            _txParameters.SetParameter(beam::wallet::TxParameterID::AtomicSwapBeamAssetID,
                                        beam::Asset::ID(_selectedBeamAssetId));
            _txParameters.SetParameter(beam::wallet::TxParameterID::AtomicSwapBeamAssetName,
                                        getSelectedBeamAssetUnitName().toStdString());
        }

        if (tokenActive && !_selectedTokenContract.isEmpty())
        {
            _txParameters.SetParameter(beam::wallet::TxParameterID::AtomicSwapTokenContract,
                                        _selectedTokenContract.toStdString());
            _txParameters.SetParameter(beam::wallet::TxParameterID::AtomicSwapTokenSymbol,
                                        _selectedTokenSymbol.toStdString());
            _txParameters.SetParameter(beam::wallet::TxParameterID::AtomicSwapTokenDecimals,
                                        static_cast<uint8_t>(_selectedTokenDecimals));
        }

#ifdef BEAM_CLIENT_VERSION
        _txParameters.SetParameter(
            beam::wallet::TxParameterID::ClientVersion,
            AppModel::getMyName() + " " + std::string(BEAM_CLIENT_VERSION));
#endif // BEAM_CLIENT_VERSION

        const auto& mirroredTxParams = MirrorSwapTxParams(_txParameters);
        const auto& readyForTokenizeTxParams =
            PrepareSwapTxParamsForTokenization(mirroredTxParams);

        setTransactionToken(
            QString::fromStdString(std::to_string(readyForTokenizeTxParams)));
    };

    _walletModel->getAsync()->CreateSwapTxParams(
        beamAmount,
        beamFee,
        swapCoin,
        swapAmount,
        swapFeeRate,
        _isBeamSide,
        GetBlockCount(_offerExpires), onSwapParams);

    //_txParameters = beam::wallet::CreateSwapTransactionParameters();

    //FillSwapTxParams(
    //    &_txParameters,
    //    _receiverAddress.m_BbsAddr,
    //    _walletModel->getCurrentHeight(),
    //    beamAmount,
    //    beamFee,
    //    swapCoin,
    //    swapAmount,
    //    swapFeeRate,
    //    _isBeamSide,
    //    GetBlockCount(_offerExpires)
    //);
}

QString ReceiveSwapViewModel::getSecondCurrencySendRateValue() const
{
    auto sendCurrency = convertCurrencyToExchangeCurrency(getSentCurrency());
    auto rate = _rates->getRate(sendCurrency);
    return beamui::AmountToUIString(rate);
}

QString ReceiveSwapViewModel::getSecondCurrencyReceiveRateValue() const
{
    auto receiveCurrency = convertCurrencyToExchangeCurrency(getReceiveCurrency());
    auto rate = _rates->getRate(receiveCurrency);
    return beamui::AmountToUIString(rate);
}

unsigned int ReceiveSwapViewModel::getMinimalBeamFeeGrothes() const
{
    return _minimalBeamFeeGrothes;
}

QString ReceiveSwapViewModel::getSecondCurrencyUnitName() const
{
    return beamui::getCurrencyUnitName(_rates->getRateCurrency());
}

QString ReceiveSwapViewModel::getSentFeeTitle() const
{
    return swapui::getSwapFeeTitle(_sentCurrency);
}

QString ReceiveSwapViewModel::getReceiveFeeTitle() const
{
    return swapui::getSwapFeeTitle(_receiveCurrency);
}

QList<QMap<QString, QVariant>> ReceiveSwapViewModel::getCurrList() const
{
    auto list = swapui::getUICurrList();

    // Confidential Assets ride the BEAM swap side; each known asset gets its
    // own entry appended after the classic currencies so it is picked in the
    // same combo as BEAM/BTC/.../WBTC. "assetId" marks these entries for
    // selectCurrencyByListIndex/currencyToListIndex.
    for (const auto& asset : AppModel::getInstance().getAssets()->getAssetsList())
    {
        const auto assetId = asset.value("assetId").toUInt();
        if (assetId == static_cast<uint>(beam::Asset::s_BeamID))
        {
            continue;
        }

        QMap<QString, QVariant> entry;
        const auto name = asset.value("unitName").toString();

        entry.insert("isBEAM", true);
        entry.insert("unitName", name);
        entry.insert("unitNameWithId", QString("%1 (%2)").arg(name).arg(assetId));
        entry.insert("icon", asset.value("icon"));
        entry.insert("iconWidth", 22);
        entry.insert("iconHeight", 22);
        entry.insert("decimals", beamui::getCurrencyDecimals(beamui::Currencies::Beam));
        entry.insert("assetId", assetId);

        list.push_back(entry);
    }

    // stored custom ERC-20 tokens ride the Ethereum swap coin; each gets its
    // own entry appended after the classic currencies so it is picked in the
    // same combo as BEAM/BTC/.../WBTC (issue #1267). "tokenContract" marks
    // these entries for selectCurrencyByListIndex/currencyToListIndex; classic
    // entries simply don't carry the key (falsy/empty when read back in QML).
    for (const auto& token : getCustomTokensList())
    {
        QMap<QString, QVariant> entry;
        const auto symbol = token.value("symbol").toString();
        const auto decimals = static_cast<uint>(beamui::tokenWalletDecimals(token.value("decimals").toUInt()));

        entry.insert("isBEAM", false);
        entry.insert("unitName", symbol);
        entry.insert("unitNameWithId", symbol);
        // the combo delegate only renders a file icon (SvgImage source path,
        // no room for the generated TokenIcon glyph); reuse the ETH icon
        entry.insert("icon", "qrc:/assets/icon-eth.svg");
        entry.insert("iconWidth", 22);
        entry.insert("iconHeight", 22);
        entry.insert("decimals", decimals);
        entry.insert("tokenContract", token.value("contract"));

        list.push_back(entry);
    }

    return list;
}

unsigned int ReceiveSwapViewModel::getSelectedBeamAssetId() const
{
    return static_cast<unsigned int>(_selectedBeamAssetId);
}

void ReceiveSwapViewModel::setSelectedBeamAssetId(unsigned int value)
{
    auto assetId = static_cast<beam::Asset::ID>(value);
    if (assetId != _selectedBeamAssetId)
    {
        _selectedBeamAssetId = assetId;
        emit selectedBeamAssetChanged();
        emit currListChanged();
        emit enoughChanged();
        updateTransactionToken();
    }
}

QString ReceiveSwapViewModel::getSelectedBeamAssetUnitName() const
{
    if (_selectedBeamAssetId == 0)
    {
        return QMLGlobals::getCurrencyUnitName(OldWalletCurrency::OldCurrency::CurrBeam);
    }
    return AppModel::getInstance().getAssets()->getUnitName(_selectedBeamAssetId, AssetsManager::NoShorten);
}

QList<QMap<QString, QVariant>> ReceiveSwapViewModel::getCustomTokensList() const
{
    return AppModel::getInstance().getSettings().getEthCustomTokens();
}

QString ReceiveSwapViewModel::getSelectedTokenContract() const
{
    return _selectedTokenContract;
}

void ReceiveSwapViewModel::setSelectedTokenContract(const QString& value)
{
    if (value == _selectedTokenContract)
    {
        return;
    }

    // the eth-side amount is stored in wallet units whose scale follows the
    // selected token's decimals: keep the typed value, not the raw units
    const bool sentIsEth = _sentCurrency == OldWalletCurrency::OldCurrency::CurrEthereum;
    const bool receiveIsEth = _receiveCurrency == OldWalletCurrency::OldCurrency::CurrEthereum;
    const QString ethSideAmount = sentIsEth ? getAmountSent()
                                : receiveIsEth ? getAmountToReceive() : QString();

    _selectedTokenContract = value;
    _selectedTokenSymbol.clear();
    _selectedTokenDecimals = 0;

    if (!value.isEmpty())
    {
        for (const auto& token : getCustomTokensList())
        {
            if (token.value("contract").toString() == value)
            {
                _selectedTokenSymbol = token.value("symbol").toString();
                _selectedTokenDecimals = token.value("decimals").toUInt();
                break;
            }
        }
    }

    emit selectedTokenChanged();
    emit currListChanged();

    if (sentIsEth)
    {
        setAmountSent(ethSideAmount);
        emit amountSentChanged();
    }
    else if (receiveIsEth)
    {
        setAmountToReceive(ethSideAmount);
        emit amountReceiveChanged();
    }

    updateTransactionToken();
}

QString ReceiveSwapViewModel::getSelectedTokenSymbol() const
{
    return _selectedTokenSymbol;
}

unsigned int ReceiveSwapViewModel::getSelectedTokenDecimals() const
{
    return _selectedTokenDecimals;
}

int ReceiveSwapViewModel::getSentCurrencyIndex() const
{
    return currencyToListIndex(_sentCurrency);
}

void ReceiveSwapViewModel::setSentCurrencyIndex(int index)
{
    selectCurrencyByListIndex(true, index);
}

int ReceiveSwapViewModel::getReceiveCurrencyIndex() const
{
    return currencyToListIndex(_receiveCurrency);
}

void ReceiveSwapViewModel::setReceiveCurrencyIndex(int index)
{
    selectCurrencyByListIndex(false, index);
}

int ReceiveSwapViewModel::currencyToListIndex(OldWalletCurrency::OldCurrency currency) const
{
    if (currency == OldWalletCurrency::OldCurrency::CurrBeam && _selectedBeamAssetId != 0)
    {
        const auto list = getCurrList();
        for (int i = 0; i < list.size(); ++i)
        {
            if (list[i].value("assetId").toUInt() == static_cast<uint>(_selectedBeamAssetId))
            {
                return i;
            }
        }
        // the selected asset vanished from the wallet's list; fall back to
        // the plain BEAM slot
        return static_cast<int>(OldWalletCurrency::OldCurrency::CurrBeam);
    }
    if (isTokenSide(currency))
    {
        const auto list = getCurrList();
        for (int i = 0; i < list.size(); ++i)
        {
            if (list[i].value("tokenContract").toString() == _selectedTokenContract)
            {
                return i;
            }
        }
        // the selected token vanished from settings mid-flight (race with
        // syncExtendedSelections); fall back to the plain Ethereum slot
        return static_cast<int>(OldWalletCurrency::OldCurrency::CurrEthereum);
    }
    return static_cast<int>(currency);
}

void ReceiveSwapViewModel::selectCurrencyByListIndex(bool isSent, int index)
{
    const auto list = getCurrList();
    if (index < 0 || index >= list.size())
    {
        // a stale/removed entry (e.g. combo model changed underneath a
        // pending selection): ignore rather than crash
        return;
    }

    const auto contract = list[index].value("tokenContract").toString();
    const auto assetId = list[index].value("assetId").toUInt();
    const auto currency = !contract.isEmpty() ? OldWalletCurrency::OldCurrency::CurrEthereum
        : assetId != 0                        ? OldWalletCurrency::OldCurrency::CurrBeam
                                              : static_cast<OldWalletCurrency::OldCurrency>(index);

    if (!contract.isEmpty())
    {
        // stamp the token identity before flipping the enum so
        // isTokenSide()/effectiveDecimals() already see it once
        // setSentCurrency/setReceiveCurrency runs its amount conversion
        setSelectedTokenContract(contract);
    }
    else if (currency == OldWalletCurrency::OldCurrency::CurrEthereum)
    {
        // explicit "plain ETH" pick on a side that already was Ethereum:
        // syncExtendedSelections() only clears once neither side is
        // Ethereum anymore, so drop the stale token stamp here
        setSelectedTokenContract(QString());
    }

    if (assetId != 0)
    {
        setSelectedBeamAssetId(assetId);
    }
    else if (currency == OldWalletCurrency::OldCurrency::CurrBeam)
    {
        // explicit plain-BEAM pick drops a stale asset stamp
        setSelectedBeamAssetId(0);
    }

    if (isSent)
    {
        setSentCurrency(currency);
    }
    else
    {
        setReceiveCurrency(currency);
    }
}

bool ReceiveSwapViewModel::isTokenSide(OldWalletCurrency::OldCurrency currency) const
{
    return swapui::isTokenSide(currency, _selectedTokenContract);
}

uint8_t ReceiveSwapViewModel::effectiveDecimals(OldWalletCurrency::OldCurrency currency) const
{
    return swapui::effectiveSwapDecimals(currency, _selectedTokenContract, _selectedTokenDecimals);
}

void ReceiveSwapViewModel::syncExtendedSelections()
{
    if (_selectedBeamAssetId != 0 &&
        _sentCurrency != OldWalletCurrency::OldCurrency::CurrBeam &&
        _receiveCurrency != OldWalletCurrency::OldCurrency::CurrBeam)
    {
        setSelectedBeamAssetId(0);
    }

    if (!_selectedTokenContract.isEmpty() &&
        _sentCurrency != OldWalletCurrency::OldCurrency::CurrEthereum &&
        _receiveCurrency != OldWalletCurrency::OldCurrency::CurrEthereum)
    {
        setSelectedTokenContract(QString());
    }
}
