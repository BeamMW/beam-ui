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
#include "send_swap_view.h"
#include "model/app_model.h"
#include "qml_globals.h"
#include "wallet/transactions/swaps/common.h"
#include "wallet/transactions/swaps/swap_transaction.h"
#include "ui_helpers.h"
#include "fee_helpers.h"
#include "atomic_swap/swap_utils.h"
#include "wallet/transactions/swaps/bridges/ethereum/ethereum_side.h"
#include "wallet/transactions/swaps/bridges/ethereum/common.h"
#include <algorithm>
#include <regex>

SendSwapViewModel::SendSwapViewModel()
    : _sendAmountGrothes(0)
    , _sendFeeGrothes(0)
    , _sendCurrency(OldWalletCurrency::OldCurrency::CurrStart)
    , _receiveAmountGrothes(0)
    , _receiveFeeGrothes(0)
    , _receiveCurrency(OldWalletCurrency::OldCurrency::CurrStart)
    , _changeGrothes(0)
    , _walletModel(AppModel::getInstance().getWalletModel())
    , _rates(AppModel::getInstance().getRates())
    , _isBeamSide(true)
    , _minimalBeamFeeGrothes(minimalFee(OldWalletCurrency::OldCurrency::CurrBeam, false))
{
    connect(_walletModel, &WalletModel::changeCalculated,  this,  &SendSwapViewModel::onChangeCalculated);
    connect(_walletModel, &WalletModel::walletStatusChanged, this, &SendSwapViewModel::recalcAvailable);
    connect(_walletModel, &WalletModel::coinsSelected, this, &SendSwapViewModel::onCoinsSelected);
    connect(_rates.get(), SIGNAL(rateUnitChanged()), SIGNAL(secondCurrencyUnitNameChanged()));
    connect(_rates.get(), SIGNAL(activeRateChanged()), SIGNAL(secondCurrencyRateChanged()));
}

QString SendSwapViewModel::getToken() const
{
    return _token;
}

void SendSwapViewModel::fillParameters(const beam::wallet::TxParameters& parameters)
{
    // Set currency before fee, otherwise it would be reset to default fee
    using namespace beam::wallet;
    using namespace beam;

    auto isBeamSide = parameters.GetParameter<bool>(TxParameterID::AtomicSwapIsBeamSide);
    auto swapCoin = parameters.GetParameter<AtomicSwapCoin>(TxParameterID::AtomicSwapCoin);
    auto beamAmount = parameters.GetParameter<Amount>(TxParameterID::Amount);
    auto swapAmount = parameters.GetParameter<Amount>(TxParameterID::AtomicSwapAmount);
    auto peerAddr = parameters.GetParameter<WalletID>(TxParameterID::PeerAddr);
    auto peerResponseTime = parameters.GetParameter<Height>(TxParameterID::PeerResponseTime);
    auto offeredTime = parameters.GetParameter<Timestamp>(TxParameterID::CreateTime);
    auto minHeight = parameters.GetParameter<Height>(TxParameterID::MinHeight);

    if (peerAddr && swapAmount && beamAmount && swapCoin && isBeamSide
        && peerResponseTime && offeredTime && minHeight)
    {
        // extended-offer fields first: the amount/currency setters below key the
        // conversion decimals on them for ERC-20 offers
        auto tokenContract = parameters.GetParameter<std::string>(TxParameterID::AtomicSwapTokenContract);
        auto tokenSymbol = parameters.GetParameter<std::string>(TxParameterID::AtomicSwapTokenSymbol);
        auto tokenDecimals = parameters.GetParameter<uint8_t>(TxParameterID::AtomicSwapTokenDecimals);
        _tokenContract = tokenContract ? QString::fromStdString(*tokenContract) : QString();
        _tokenSymbol = tokenSymbol ? QString::fromStdString(*tokenSymbol) : QString();
        _tokenDecimals = tokenDecimals ? *tokenDecimals : 0;

        auto beamAssetId = parameters.GetParameter<Asset::ID>(TxParameterID::AtomicSwapBeamAssetID);
        auto beamAssetName = parameters.GetParameter<std::string>(TxParameterID::AtomicSwapBeamAssetName);
        _beamAssetId = beamAssetId ? *beamAssetId : 0;
        _beamAssetUnitName = beamAssetName ? QString::fromStdString(*beamAssetName) : QString();

        // Erc20Token has no OldCurrency of its own: it is handled as the
        // Ethereum currency slot plus the token params above
        const auto swapSideCurrency = (*swapCoin == AtomicSwapCoin::Erc20Token)
            ? OldWalletCurrency::OldCurrency::CurrEthereum
            : convertSwapCoinToCurrency(*swapCoin);

        if (*isBeamSide) // other participant is not a beam side
        {
            // Do not set fee, it is set automatically based on the currency param
            setSendCurrency(OldWalletCurrency::OldCurrency::CurrBeam);
            setSendAmount(beamui::AmountToUIString(*beamAmount));
            setReceiveCurrency(swapSideCurrency);
            setReceiveAmount(beamui::AmountToUIStringExactDecimals(*swapAmount, effectiveDecimals(swapSideCurrency)));
        }
        else
        {
            // Do not set fee, it is set automatically based on the currency param
            setSendCurrency(swapSideCurrency);
            setSendAmount(beamui::AmountToUIStringExactDecimals(*swapAmount, effectiveDecimals(swapSideCurrency)));
            setReceiveCurrency(OldWalletCurrency::OldCurrency::CurrBeam);
            setReceiveAmount(beamui::AmountToUIString(*beamAmount));
        }
        setOfferedTime(QDateTime::fromSecsSinceEpoch(*offeredTime));

        auto currentHeight = _walletModel->getCurrentHeight();
        assert(currentHeight);
        beam::Timestamp currentHeightTime = _walletModel->getCurrentHeightTimestamp();
        auto expiresHeight = *minHeight + *peerResponseTime;
        setExpiresTime(beamui::CalculateExpiresTime(currentHeightTime, currentHeight, expiresHeight));

        _txParameters = parameters;
        _isBeamSide = *isBeamSide;

        emit currListChanged(); // unit names/decimals follow the token/asset fields
    }

    _tokenGeneratebByNewAppVersionMessage.clear();
    std::string libraryVersionStr;
    ProcessLibraryVersion(parameters, [this, &libraryVersionStr](const auto& version, const auto& myVersion)
    {
        libraryVersionStr = version;
/*% "This address generated by newer Beam library version(%1)
Your version is: %2. Please, check for updates."
*/
        _tokenGeneratebByNewAppVersionMessage = qtTrId("swap-token-newer-lib")
            .arg(version.c_str())
            .arg(myVersion.c_str());
        emit tokenGeneratebByNewAppVersion();
    });

#ifdef BEAM_CLIENT_VERSION
    ProcessClientVersion(parameters, AppModel::getMyName(), BEAM_CLIENT_VERSION, libraryVersionStr, [this](const auto& version, const auto& myVersion)
    {
/*% "This address generated by newer Beam client version(%1)
Your version is: %2. Please, check for updates."
*/
        _tokenGeneratebByNewAppVersionMessage = qtTrId("swap-swap-token-newer-client")
            .arg(version.c_str())
            .arg(myVersion.c_str());
        emit tokenGeneratebByNewAppVersion();
    });
#endif // BEAM_CLIENT_VERSION
}

void SendSwapViewModel::setParameters(const QVariant& parameters)
{
    if (!parameters.isNull() && parameters.isValid())
    {
        auto p = parameters.value<beam::wallet::TxParameters>();
        fillParameters(p);
    }
}

void SendSwapViewModel::setToken(const QString& value)
{
    if (_token != value)
    {
        _token = value;
        auto parameters = beam::wallet::ParseParameters(_token.toStdString());
        if (getTokenValid() && parameters)
        {
            fillParameters(*parameters);
        }
        emit tokenChanged();
    }
}

bool SendSwapViewModel::getTokenValid() const
{
    return QMLGlobals::isSwapToken(_token);
}

bool SendSwapViewModel::getParametersValid() const
{
    auto type = _txParameters.GetParameter<beam::wallet::TxType>(beam::wallet::TxParameterID::TransactionType);
    return type && *type == beam::wallet::TxType::AtomicSwap;
}

QString SendSwapViewModel::getSendAmount() const
{
    return beamui::AmountToUIStringExactDecimals(_sendAmountGrothes, effectiveDecimals(_sendCurrency));
}

void SendSwapViewModel::setSendAmount(QString value)
{
    const auto amount = beamui::UIStringToAmountExactDecimals(value, effectiveDecimals(_sendCurrency));
    if (amount != _sendAmountGrothes)
    {
        _sendAmountGrothes = amount;
        emit sendAmountChanged();
        emit isSendFeeOKChanged();
        recalcAvailable();

        if (_sendCurrency == OldWalletCurrency::OldCurrency::CurrBeam && _walletModel->hasShielded(beam::Asset::s_BeamID))
        {
            _walletModel->getAsync()->selectCoins(_sendAmountGrothes, _sendFeeGrothes, _beamAssetId);
        }
    }
}

unsigned int SendSwapViewModel::getSendFee() const
{
    return _sendFeeGrothes;
}

void SendSwapViewModel::setSendFee(unsigned int value)
{
    if (value != _sendFeeGrothes)
    {
        _sendFeeGrothes = value;
        emit sendFeeChanged();
        emit isSendFeeOKChanged();
        recalcAvailable();

        if (_sendCurrency == OldWalletCurrency::OldCurrency::CurrBeam && _walletModel->hasShielded(beam::Asset::s_BeamID) && _sendAmountGrothes)
        {
            _feeChangedByUI = true;
            _walletModel->getAsync()->selectCoins(_sendAmountGrothes, _sendFeeGrothes, _beamAssetId);
        }
    }
}

OldWalletCurrency::OldCurrency SendSwapViewModel::getSendCurrency() const
{
    return _sendCurrency;
}

void SendSwapViewModel::setSendCurrency(OldWalletCurrency::OldCurrency value)
{
    assert(value > OldWalletCurrency::OldCurrency::CurrStart && value < OldWalletCurrency::OldCurrency::CurrEnd);

    if (value != _sendCurrency)
    {
        _sendCurrency = value;
        emit sendCurrencyChanged();
        emit isSendFeeOKChanged();
        recalcAvailable();
    }
}

QString SendSwapViewModel::getReceiveAmount() const
{
    return beamui::AmountToUIStringExactDecimals(_receiveAmountGrothes, effectiveDecimals(_receiveCurrency));
}

void SendSwapViewModel::setReceiveAmount(QString value)
{
    const auto amount = beamui::UIStringToAmountExactDecimals(value, effectiveDecimals(_receiveCurrency));
    if (amount != _receiveAmountGrothes)
    {
        _receiveAmountGrothes = amount;
        emit receiveAmountChanged();
        emit isReceiveFeeOKChanged();
    }
}

unsigned int SendSwapViewModel::getReceiveFee() const
{
    return _receiveFeeGrothes;
}

void SendSwapViewModel::setReceiveFee(unsigned int value)
{
    if (value != _receiveFeeGrothes)
    {
        _receiveFeeGrothes = value;
        emit receiveFeeChanged();
        emit canSendChanged();
        emit isReceiveFeeOKChanged();
        emit enoughToReceiveChanged();
    }
}

OldWalletCurrency::OldCurrency SendSwapViewModel::getReceiveCurrency() const
{
    return _receiveCurrency;
}

void SendSwapViewModel::setReceiveCurrency(OldWalletCurrency::OldCurrency value)
{
    assert(value > OldWalletCurrency::OldCurrency::CurrStart && value < OldWalletCurrency::OldCurrency::CurrEnd);

    if (value != _receiveCurrency)
    {
        _receiveCurrency = value;
        emit receiveCurrencyChanged();
        emit isReceiveFeeOKChanged();
    }
}

QString SendSwapViewModel::getComment() const
{
    return _comment;
}

void SendSwapViewModel::setComment(const QString& value)
{
    if (_comment != value)
    {
        _comment = value;
        emit commentChanged();
    }
}

QDateTime SendSwapViewModel::getOfferedTime() const
{
    return _offeredTime;
}

void SendSwapViewModel::setOfferedTime(const QDateTime& value)
{
    if (_offeredTime != value)
    {
        _offeredTime = value;
        emit offeredTimeChanged();
    }
}

QDateTime SendSwapViewModel::getExpiresTime() const
{
    return _expiresTime;
}

void SendSwapViewModel::setExpiresTime(const QDateTime& value)
{
    if (_expiresTime != value)
    {
        _expiresTime = value;
        emit expiresTimeChanged();
    }
}

void SendSwapViewModel::onChangeCalculated(beam::Amount changeAsset, beam::Amount changeBeam, beam::Asset::ID assetID)
{
    using namespace beam;

    assert(AmountBig::get_Hi(changeAsset) == 0);

    // the balance check needs the change of what is being spent: the asset's
    // change for an asset spend, BEAM change otherwise (the values coincide
    // when assetID is BEAM)
    _changeGrothes = AmountBig::get_Lo(changeAsset);
    emit enoughChanged();
    emit canSendChanged();
}

void SendSwapViewModel::onCoinsSelected(const beam::wallet::CoinsSelectionInfo& selectionRes)
{
    if (_sendCurrency == OldWalletCurrency::OldCurrency::CurrBeam)
    {
        _minimalBeamFeeGrothes = selectionRes.m_minimalExplicitFee;
        emit minimalBeamFeeGrothesChanged();

        if (_feeChangedByUI)
        {
            _feeChangedByUI = false;
            return;
        }
        _sendFeeGrothes = selectionRes.m_explicitFee;
        emit sendFeeChanged();
    }
}

bool SendSwapViewModel::isEnough() const
{
    auto total = _sendAmountGrothes + _sendFeeGrothes + _changeGrothes;

    // acceptor receives a Confidential Asset on the peer's BEAM side: the
    // redeem tx fee is a separate BEAM spend on top of whatever is checked below
    if (needsBeamForRedeemFee())
    {
        auto beamAvailable = beam::AmountBig::get_Lo(_walletModel->getAvailable(beam::Asset::s_BeamID));
        if (beamAvailable < _receiveFeeGrothes)
        {
            return false;
        }
    }

    if (OldWalletCurrency::OldCurrency::CurrBeam == _sendCurrency)
    {
        if (_beamAssetId != 0)
        {
            // sending a Confidential Asset; the tx fee is still paid in BEAM
            auto assetAvailable = beam::AmountBig::get_Lo(_walletModel->getAvailable(_beamAssetId));
            auto beamAvailable = beam::AmountBig::get_Lo(_walletModel->getAvailable(beam::Asset::s_BeamID));
            return assetAvailable >= _sendAmountGrothes + _changeGrothes &&
                   beamAvailable >= _sendFeeGrothes;
        }

        auto available = beam::AmountBig::get_Lo(_walletModel->getAvailable(beam::Asset::s_BeamID));
        return available >= total;
    }

    auto swapCoin = convertCurrencyToSwapCoin(_sendCurrency);
    if (isEthereumBased(_sendCurrency))
    {
        if (isTokenSide(_sendCurrency))
        {
            // no live balance for arbitrary custom tokens: only check the ETH
            // that pays the lock gas (incl. the approve calls, kApproveTxGasLimit)
            const beam::Amount lockFee = _sendFeeGrothes *
                (beam::ethereum::kLockTxGasLimit + 2 * beam::ethereum::kApproveTxGasLimit);
            return AppModel::getInstance().getSwapEthClient()->getAvailable(beam::wallet::AtomicSwapCoin::Ethereum) >= lockFee;
        }

        if (_sendCurrency == OldWalletCurrency::OldCurrency::CurrEthereum)
        {
            total = _sendAmountGrothes + beam::wallet::EthereumSide::CalcLockTxFee(_sendFeeGrothes, swapCoin);

            return AppModel::getInstance().getSwapEthClient()->getAvailable(swapCoin) >= total;
        }

        return AppModel::getInstance().getSwapEthClient()->getAvailable(swapCoin) >= _sendAmountGrothes &&
            AppModel::getInstance().getSwapEthClient()->getAvailable(beam::wallet::AtomicSwapCoin::Ethereum) >=
            beam::wallet::EthereumSide::CalcLockTxFee(_sendFeeGrothes, swapCoin);
    }

    // TODO sentFee is fee rate. should be corrected
    return AppModel::getInstance().getSwapCoinClient(swapCoin)->getAvailable() > total;
}

bool SendSwapViewModel::isEnoughToReceive() const
{
    if (isEthereumBased(_receiveCurrency))
    {
        auto swapCoin = convertCurrencyToSwapCoin(_receiveCurrency);
        auto fee = beam::wallet::EthereumSide::CalcWithdrawTxFee(_receiveFeeGrothes, swapCoin);

        return AppModel::getInstance().getSwapEthClient()->getAvailable(beam::wallet::AtomicSwapCoin::Ethereum) > fee;
    }
    return true;
}

void SendSwapViewModel::recalcAvailable()
{
    switch(_sendCurrency)
    {
    case OldWalletCurrency::OldCurrency::CurrBeam:
        _changeGrothes = 0;
        _walletModel->getAsync()->calcChange(_sendAmountGrothes, _sendFeeGrothes, _beamAssetId);
        return;
    default:
        // TODO:SWAP implement for all currencies
        _changeGrothes = 0;
    }

    emit enoughChanged();
    emit canSendChanged();
}

QString SendSwapViewModel::getReceiverAddress() const
{
    auto peerAddr = _txParameters.GetParameter<beam::wallet::WalletID>(beam::wallet::TxParameterID::PeerAddr);
    if (peerAddr)
    {
        return beamui::toString(*peerAddr);
    }
    return _token;
}

bool SendSwapViewModel::canSend() const
{
    // TODO:SWAP check if correct
    return isFeeOK(_sendFeeGrothes, _sendCurrency, false) &&
           _sendCurrency != _receiveCurrency &&
           isEnough() &&
           QDateTime::currentDateTime() < _expiresTime;
}

void SendSwapViewModel::sendMoney()
{
    using beam::wallet::TxParameterID;
    
    auto txParameters = beam::wallet::TxParameters(_txParameters);
    auto beamFee = _isBeamSide ? getSendFee() : getReceiveFee();
    auto swapFee = _isBeamSide ? getReceiveFee() : getSendFee();

    beam::wallet::FillSwapFee(
        &txParameters,
        beam::Amount(beamFee),
        beam::Amount(swapFee),
        _isBeamSide);

    if (!_comment.isEmpty())
    {
        std::string localComment = _comment.toStdString();
        txParameters.SetParameter(TxParameterID::Message, beam::ByteBuffer(localComment.begin(), localComment.end()));
    }

    {
        auto txID = txParameters.GetTxID();
        auto swapCoin = txParameters.GetParameter<beam::wallet::AtomicSwapCoin>(TxParameterID::AtomicSwapCoin);
        auto amount = txParameters.GetParameter<beam::Amount>(TxParameterID::Amount);
        auto swapAmount = txParameters.GetParameter<beam::Amount>(TxParameterID::AtomicSwapAmount);
        auto responseHeight = txParameters.GetParameter<beam::Height>(TxParameterID::PeerResponseTime);
        auto minimalHeight = txParameters.GetParameter<beam::Height>(TxParameterID::MinHeight);

        BEAM_LOG_INFO() << *txID << " Accept offer.\n\t"
                    << "isBeamSide: " << (_isBeamSide ? "true" : "false") << "\n\t"
                    << "swapCoin: " << std::to_string(*swapCoin) << "\n\t"
                    << "amount: " << *amount << "\n\t"
                    << "swapAmount: " << *swapAmount << "\n\t"
                    << "responseHeight: " << *responseHeight << "\n\t"
                    << "minimalHeight: " << *minimalHeight;
    }

    _walletModel->getAsync()->startTransaction(std::move(txParameters));
}

bool SendSwapViewModel::isSendFeeOK() const
{
    return _sendAmountGrothes == 0 || isSwapFeeOK(_sendAmountGrothes, _sendFeeGrothes, _sendCurrency);
}

bool SendSwapViewModel::isReceiveFeeOK() const
{
    return _receiveAmountGrothes == 0 || isSwapFeeOK(_receiveAmountGrothes, _receiveFeeGrothes, _receiveCurrency);
}

bool SendSwapViewModel::isSendBeam() const
{
    return _isBeamSide;
}

QString SendSwapViewModel::getRate() const
{
    beam::Amount otherCoinAmount =
        isSendBeam() ? _receiveAmountGrothes : _sendAmountGrothes;
    beam::Amount beamAmount =
        isSendBeam() ? _sendAmountGrothes : _receiveAmountGrothes;

    if (!beamAmount) return QString();

    auto otherOldCurrency = isSendBeam() ? _receiveCurrency : _sendCurrency;
    uint8_t otherDecimals = effectiveDecimals(otherOldCurrency);

    return QMLGlobals::divideWithPrecision(
        beamui::AmountToUIStringExactDecimals(otherCoinAmount, otherDecimals),
        beamui::AmountToUIString(beamAmount),
        otherDecimals);
}

QString SendSwapViewModel::getSecondCurrencySendRateValue() const
{
    auto sendCurrency = convertCurrencyToExchangeCurrency(getSendCurrency());
    auto rate = _rates->getRate(sendCurrency);
    return beamui::AmountToUIString(rate);
}

QString SendSwapViewModel::getSecondCurrencyReceiveRateValue() const
{
    auto receiveCurrency = convertCurrencyToExchangeCurrency(getReceiveCurrency());
    auto rate = _rates->getRate(receiveCurrency);
    return beamui::AmountToUIString(rate);
}

QString SendSwapViewModel::getSecondCurrencyUnitName() const
{
    return beamui::getCurrencyUnitName(_rates->getRateCurrency());
}

bool SendSwapViewModel::isTokenGeneratedByNewVersion() const
{
    return !_tokenGeneratebByNewAppVersionMessage.isEmpty();
}

QString SendSwapViewModel::tokenGeneratedByNewVersionMessage() const
{
    return _tokenGeneratebByNewAppVersionMessage;
}

unsigned int SendSwapViewModel::getMinimalBeamFeeGrothes() const
{
    return _minimalBeamFeeGrothes;
}

QString SendSwapViewModel::getSentFeeTitle() const
{
    return swapui::getSwapFeeTitle(_sendCurrency);
}

QString SendSwapViewModel::getReceiveFeeTitle() const
{
    return swapui::getSwapFeeTitle(_receiveCurrency);
}

QList<QMap<QString, QVariant>> SendSwapViewModel::getCurrList() const
{
    auto list = swapui::getUICurrList();

    // show the offer's real unit instead of "BEAM"/"ETH" for asset/token offers;
    // the token entry also carries the token's wallet decimals
    if (_beamAssetId != 0 && !_beamAssetUnitName.isEmpty())
    {
        auto& beamEntry = list[static_cast<int>(OldWalletCurrency::OldCurrency::CurrBeam)];
        beamEntry["unitName"] = _beamAssetUnitName;
        beamEntry["unitNameWithId"] = _beamAssetUnitName;
        beamEntry["assetId"] = static_cast<uint>(_beamAssetId);
    }

    if (!_tokenContract.isEmpty())
    {
        auto& ethEntry = list[static_cast<int>(OldWalletCurrency::OldCurrency::CurrEthereum)];
        const QString symbol = _tokenSymbol.isEmpty() ? QString("ERC20") : _tokenSymbol;
        ethEntry["unitName"] = symbol;
        ethEntry["unitNameWithId"] = symbol;
        ethEntry["decimals"] = static_cast<uint>(std::min<uint8_t>(_tokenDecimals, 9));
    }

    return list;
}

bool SendSwapViewModel::isErc20Swap() const
{
    return !_tokenContract.isEmpty();
}

QString SendSwapViewModel::getTokenContract() const
{
    return _tokenContract;
}

QString SendSwapViewModel::getTokenSymbol() const
{
    return _tokenSymbol;
}

uint SendSwapViewModel::getTokenDecimals() const
{
    return _tokenDecimals;
}

bool SendSwapViewModel::isBeamAssetSwap() const
{
    return _beamAssetId != 0;
}

uint SendSwapViewModel::getBeamAssetId() const
{
    return static_cast<uint>(_beamAssetId);
}

QString SendSwapViewModel::getBeamAssetUnitName() const
{
    return _beamAssetUnitName;
}

bool SendSwapViewModel::needsBeamForRedeemFee() const
{
    return isBeamAssetSwap() && !isSendBeam();
}

bool SendSwapViewModel::isTokenSide(OldWalletCurrency::OldCurrency currency) const
{
    return currency == OldWalletCurrency::OldCurrency::CurrEthereum && !_tokenContract.isEmpty();
}

uint8_t SendSwapViewModel::effectiveDecimals(OldWalletCurrency::OldCurrency currency) const
{
    if (isTokenSide(currency))
    {
        // core stores AtomicSwapAmount in 10^min(decimals, 9) units per token
        // (WalletUnitsPerToken, swaps/bridges/ethereum/common.cpp)
        return std::min<uint8_t>(_tokenDecimals, 9);
    }
    return beamui::getCurrencyDecimals(convertCurrency(currency));
}
