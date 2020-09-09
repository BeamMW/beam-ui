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
#include "send_view.h"
#include "model/app_model.h"
#include "wallet/core/common.h"
#include "wallet/core/simple_transaction.h"

#include "ui_helpers.h"
#include "qml_globals.h"

#include <algorithm>
#include <regex>
#include <QLocale>

namespace
{
    void CopyParameter(beam::wallet::TxParameterID paramID, const beam::wallet::TxParameters& input, beam::wallet::TxParameters& dest)
    {
        beam::wallet::ByteBuffer buf;
        if (input.GetParameter(paramID, buf))
        {
            dest.SetParameter(paramID, buf);
        }
    }
}

SendViewModel::SendViewModel()
    : _feeGrothes(QMLGlobals::getMinimalFee(Currency::CurrBeam, false))
    , _sendAmountGrothes(0)
    , _changeGrothes(0)
    , _walletModel(*AppModel::getInstance().getWallet())
    , _minimalFeeGrothes(QMLGlobals::getMinimalFee(Currency::CurrBeam, false))
    , _shieldedInputsFee(0)
{
    connect(&_walletModel, &WalletModel::changeCalculated, this, &SendViewModel::onChangeCalculated);
    connect(&_walletModel, SIGNAL(sendMoneyVerified()), this, SIGNAL(sendMoneyVerified()));
    connect(&_walletModel, SIGNAL(cantSendToExpired()), this, SIGNAL(cantSendToExpired()));
    connect(&_walletModel, SIGNAL(availableChanged()), this, SIGNAL(availableChanged()));
    connect(&_walletModel, &WalletModel::getAddressReturned, this, &SendViewModel::onGetAddressReturned);
    connect(&_exchangeRatesManager, SIGNAL(rateUnitChanged()), SIGNAL(secondCurrencyLabelChanged()));
    connect(&_exchangeRatesManager, SIGNAL(activeRateChanged()), SIGNAL(secondCurrencyRateChanged()));
    connect(&_walletModel, &WalletModel::shieldedCoinsSelectionCalculated, this, &SendViewModel::onShieldedCoinsSelectionCalculated);
    connect(&_walletModel, &WalletModel::needExtractShieldedCoins, this, &SendViewModel::onNeedExtractShieldedCoins);
}

unsigned int SendViewModel::getFeeGrothes() const
{
    return _feeGrothes;
}

unsigned int SendViewModel::getMinimalFeeGrothes() const
{
    return _minimalFeeGrothes;
}

void SendViewModel::setFeeGrothes(unsigned int value)
{
    if (value != _feeGrothes)
    {
        _feeGrothes = value;
        emit feeGrothesChanged();

        if (!_sendAmountGrothes || !_feeGrothes) return;

        _feeChangedByUi = true;

        if (_walletModel.hasShielded())
        {
            _walletModel.getAsync()->calcShieldedCoinSelectionInfo(_sendAmountGrothes, _feeGrothes, _isShieldedTx);
        }
        else
        {
            _walletModel.getAsync()->calcChange(_sendAmountGrothes + _feeGrothes);
            _feeChangedByUi = false;
            emit canSendChanged();
        }
    }
}

QString SendViewModel::getComment() const
{
    return _comment;
}

void SendViewModel::setComment(const QString& value)
{
    if (_comment != value)
    {
        _comment = value;
        emit commentChanged();
    }
}

QString SendViewModel::getSendAmount() const
{
    LOG_DEBUG() << "ret Send amount grothes: " << _sendAmountGrothes << " 2ui: " << beamui::AmountToUIString(_sendAmountGrothes).toStdString();
    return beamui::AmountToUIString(_sendAmountGrothes);
}

void SendViewModel::setSendAmount(QString value)
{
    beam::Amount amount = beamui::UIStringToAmount(value);
    if (amount != _sendAmountGrothes || _maxAvailable)
    {
        if (!amount)
        {
            _sendAmountGrothes = amount;
            emit sendAmountChanged();
            resetMinimalFee();
            onChangeCalculated(0);
            return;
        }

        if (_walletModel.hasShielded())
        {
            if (amount < _sendAmountGrothes)
            {
                resetMinimalFee();
                onChangeCalculated(0);
                _feeGrothes = _minimalFeeGrothes;
                emit feeGrothesChanged();
            }
            _sendAmountGrothes = amount;
            emit sendAmountChanged();
            _walletModel.getAsync()->calcShieldedCoinSelectionInfo(_sendAmountGrothes, _feeGrothes, _isShieldedTx);
        }
        else
        {
            _sendAmountGrothes = amount - _feeGrothes;
            emit sendAmountChanged();
            _walletModel.getAsync()->calcChange(_sendAmountGrothes + _feeGrothes);
            emit canSendChanged();
        }
    }
}

QString SendViewModel::getReceiverTA() const
{
    return _receiverTA;
}

void SendViewModel::setReceiverTA(const QString& value)
{
    if (_receiverTA != value)
    {
        _tokenGeneratebByNewAppVersionMessage.clear();
        _receiverTA = value;
        if (_receiverTA.isEmpty())
        {
            _canChangeTxType = true;
            emit canChangeTxTypeChanged();
        }
        emit receiverTAChanged();

        if (QMLGlobals::isSwapToken(value))
        {
            // Just ignore, UI would handle this case
            // and automatically switch to another view
        }
        else
        {
            if(getRreceiverTAValid())
            {
                extractParameters();
            }
            else
            {
                setIsToken(false);
                setIsShieldedTx(false);
                setIsNonInteractive(false);
                setIsPermanentAddress(false);
                setComment("");
            }
            emit canSendChanged();
        }
    }
}

bool SendViewModel::getRreceiverTAValid() const
{
    return QMLGlobals::isTAValid(_receiverTA);
}

QString SendViewModel::getReceiverAddress() const
{
    return _receiverAddress;
}

QString SendViewModel::getReceiverIdentity() const
{
    return _receiverIdentityStr;
}

bool SendViewModel::isShieldedTx() const
{
    return _isShieldedTx;
}

void SendViewModel::setIsShieldedTx(bool value)
{
    if (_isShieldedTx != value)
    {
        _isShieldedTx = value;
        emit isShieldedTxChanged();
        resetMinimalFee();

        if (_walletModel.hasShielded())
        {
            if (_walletModel.getAvailable() - _sendAmountGrothes - _feeGrothes == 0) _maxAvailable = true;
            _walletModel.getAsync()->calcShieldedCoinSelectionInfo(_sendAmountGrothes, _minimalFeeGrothes, _isShieldedTx);
        }
        else
        {
            setFeeGrothes(_minimalFeeGrothes);
        }
    }
}

bool SendViewModel::isPermanentAddress() const
{
    return _isPermanentAddress;
}

void SendViewModel::setIsPermanentAddress(bool value)
{
    if (_isPermanentAddress != value)
    {
        _isPermanentAddress = value;
        emit isPermanentAddressChanged();
    }
}

bool SendViewModel::canChangeTxType() const
{
    return _canChangeTxType;
}

void SendViewModel::setCanChangeTxType(bool value)
{
    if (_canChangeTxType != value)
    {
        _canChangeTxType = value;
        emit canChangeTxTypeChanged();
    }
}

int SendViewModel::getOfflinePayments() const
{
    return _offlinePayments;
}

void SendViewModel::setOfflinePayments(int value)
{
    if (_offlinePayments != value)
    {
        _offlinePayments = value;
        emit offlinePaymentsChanged();
        emit canSendChanged();
    }
}

bool SendViewModel::isNonInteractive() const
{
    return _isNonInteractive;
}

void SendViewModel::setIsNonInteractive(bool value)
{
    if (_isNonInteractive != value)
    {
        _isNonInteractive = value;
        emit isNonInteractiveChanged();
    }
}

QString SendViewModel::getAvailable() const
{
    return  beamui::AmountToUIString(
        isEnough() ? _walletModel.getAvailable() - _sendAmountGrothes - _feeGrothes - _changeGrothes : 0);
}

QString SendViewModel::getMissing() const
{
    return beamui::AmountToUIString(_sendAmountGrothes + _feeGrothes - _walletModel.getAvailable());
}

bool SendViewModel::isZeroBalance() const
{
    return _walletModel.getAvailable() == 0;
}

bool SendViewModel::isEnough() const
{
    return _walletModel.getAvailable() >= _sendAmountGrothes + _feeGrothes + _changeGrothes;
}

void SendViewModel::onChangeCalculated(beam::Amount change)
{
    _changeGrothes = change;
    emit availableChanged();
    emit canSendChanged();
    emit isEnoughChanged();
}

void SendViewModel::onShieldedCoinsSelectionCalculated(const beam::wallet::ShieldedCoinsSelectionInfo& selectionRes)
{
    _shieldedInputsFee = selectionRes.shieldedInputsFee;

    if (selectionRes.selectedSum < selectionRes.requestedSum + selectionRes.requestedFee && _maxAvailable)
    {
        _sendAmountGrothes = selectionRes.selectedSum - selectionRes.selectedFee;
        emit sendAmountChanged();
        _maxAvailable = false;
    }

    _minimalFeeGrothes = std::max(_minimalFeeGrothes, selectionRes.minimalFee);
    emit minimalFeeGrothesChanged();

    if (!_feeChangedByUi)
    {
        _feeGrothes = selectionRes.selectedFee;
        emit feeGrothesChanged();
    }
    _feeChangedByUi = false;

    onChangeCalculated(selectionRes.change);
}

void SendViewModel::onNeedExtractShieldedCoins(bool val)
{
    _isNeedExtractShieldedCoins = val;
    emit isNeedExtractShieldedCoinsChanged();
}

void SendViewModel::onGetAddressReturned(const beam::wallet::WalletID& id, const boost::optional<beam::wallet::WalletAddress>& address, int offlinePayments)
{
    if (id == _receiverWalletID)
    {
        if (address)
        {
            setWalletAddress(address);
            setComment(QString::fromStdString(address->m_label));
        }
        setOfflinePayments(offlinePayments);
    }
    else
    {
        setWalletAddress({});
        setOfflinePayments(0);
    }
}

QString SendViewModel::getChange() const
{
    return beamui::AmountToUIString(_changeGrothes);
}

QString SendViewModel::getFee() const
{
    return beamui::AmountToUIString(_feeGrothes);
}

QString SendViewModel::getTotalUTXO() const
{
    return beamui::AmountToUIString(_sendAmountGrothes + _feeGrothes + _changeGrothes);
}

bool SendViewModel::canSend() const
{
    return !QMLGlobals::isSwapToken(_receiverTA) && getRreceiverTAValid()
           && _sendAmountGrothes > 0 && isEnough()
           && QMLGlobals::isFeeOK(_feeGrothes, Currency::CurrBeam, isShieldedTx() || _isNeedExtractShieldedCoins) 
           && (!isShieldedTx() || !isNonInteractive() || getOfflinePayments() > 0)
           && !(isShieldedTx() && isOwnAddress());
}

bool SendViewModel::isToken() const
{
    return _isToken;
}

void SendViewModel::setIsToken(bool value)
{
    if (_isToken != value)
    {
        _isToken = value;
        emit isTokenChanged();
    }
}

bool SendViewModel::isOwnAddress() const
{
    return _walletModel.isOwnAddress(_receiverWalletID);
}

void SendViewModel::setMaxAvailableAmount()
{
    _maxAvailable = true;
    setSendAmount(beamui::AmountToUIString(_walletModel.getAvailable()));
}

void SendViewModel::sendMoney()
{
    using namespace beam::wallet;
    assert(canSend());
    if(canSend())
    {
        // TODO:SWAP show 'operation in process' animation here?
        auto messageString = _comment.toStdString();
        saveReceiverAddress(_comment);

        auto p = CreateSimpleTransactionParameters()
            .SetParameter(TxParameterID::Amount, _sendAmountGrothes)
            .SetParameter(TxParameterID::Fee, !!_shieldedInputsFee ? _feeGrothes - _shieldedInputsFee : _feeGrothes)
            .SetParameter(TxParameterID::Message, beam::ByteBuffer(messageString.begin(), messageString.end()));

        CopyParameter(TxParameterID::PeerID, _txParameters, p);
        CopyParameter(TxParameterID::PeerWalletIdentity, _txParameters, p);
        p.SetParameter(TxParameterID::TransactionType, isShieldedTx() ? TxType::PushTransaction : TxType::Simple);

        if (isShieldedTx())
        {
            CopyParameter(TxParameterID::ShieldedVoucherList, _txParameters, p);
        }
        if (isToken())
        {
            p.SetParameter(TxParameterID::OriginalToken, _receiverTA.toStdString());
        }

        _walletModel.getAsync()->startTransaction(std::move(p));
    }
}

void SendViewModel::saveReceiverAddress(const QString& name)
{
    if (_receiverWalletAddress.is_initialized())
        return;

    using namespace beam::wallet;
    QString trimmed = name.trimmed();
    WalletAddress address;
    address.m_walletID = _receiverWalletID;
    address.m_createTime = getTimestamp();
    address.m_Identity = _receiverIdentity;
    address.m_label = trimmed.toStdString();
    address.m_duration = WalletAddress::AddressExpirationNever;
    _walletModel.getAsync()->saveAddress(address, false);
}

void SendViewModel::extractParameters()
{
    using namespace beam::wallet;
    auto txParameters = ParseParameters(_receiverTA.toStdString());
    if (!txParameters)
    {
        return;
    }

    _txParameters = *txParameters;

    if (auto peerID = _txParameters.GetParameter<WalletID>(TxParameterID::PeerID); peerID)
    {
        _receiverWalletID = *peerID;
        _receiverAddress = QString::fromStdString(std::to_string(*peerID));
        setIsToken(_receiverTA != _receiverAddress);
        emit receiverAddressChanged();
        _walletModel.getAsync()->getAddress(_receiverWalletID);
    }
    else
    {
        _receiverWalletID = Zero;
        _receiverAddress = "";
        setIsToken(true);
        emit receiverAddressChanged();
    }

    if (auto peerIdentity = _txParameters.GetParameter<beam::PeerID>(TxParameterID::PeerWalletIdentity); peerIdentity)
    {
        _receiverIdentity = *peerIdentity;
        _receiverIdentityStr = QString::fromStdString(std::to_string(*peerIdentity));
        emit receiverIdentityChanged();
    }

    if (auto isPermanent = _txParameters.GetParameter<bool>(TxParameterID::IsPermanentPeerID); isPermanent)
    {
        setIsPermanentAddress(*isPermanent);
    }
    else
    {
        setIsPermanentAddress(false);
    }

    if (auto txType = _txParameters.GetParameter<TxType>(TxParameterID::TransactionType); txType && *txType == TxType::PushTransaction)
    {
        setCanChangeTxType(false);
        setIsShieldedTx(true);
        ShieldedVoucherList vouchers;
        auto hasVouchers = _txParameters.GetParameter(TxParameterID::ShieldedVoucherList, vouchers);
        if (hasVouchers)
        {
            if (_receiverWalletID != Zero)
            {
                _walletModel.getAsync()->saveVouchers(vouchers, _receiverWalletID);
            }
        }
        setIsNonInteractive(hasVouchers);
        // ignore other types
    }
    else
    {
        setIsShieldedTx(false);
        setIsNonInteractive(false);
        setCanChangeTxType(true);
    }

    if (auto amount = _txParameters.GetParameter<beam::Amount>(TxParameterID::Amount); amount && *amount > 0)
    {
        setSendAmount(beamui::AmountToUIString(*amount));
    }
    if (auto fee = _txParameters.GetParameter<beam::Amount>(TxParameterID::Fee); fee)
    {
        setFeeGrothes(*fee);
    }
    if (auto comment = _txParameters.GetParameter(TxParameterID::Message); comment)
    {
        std::string s(comment->begin(), comment->end());
        setComment(QString::fromStdString(s));
    }

    _tokenGeneratebByNewAppVersionMessage.clear();

#ifdef BEAM_LIB_VERSION
    if (auto libVersion = _txParameters.GetParameter(TxParameterID::LibraryVersion); libVersion)
    {
        std::string libVersionStr;
        beam::wallet::fromByteBuffer(*libVersion, libVersionStr);
        std::string myLibVersionStr = BEAM_LIB_VERSION;
        std::regex libVersionRegex("\\d{1,}\\.\\d{1,}\\.\\d{4,}");
        if (std::regex_match(libVersionStr, libVersionRegex) &&
            std::lexicographical_compare(
                myLibVersionStr.begin(),
                myLibVersionStr.end(),
                libVersionStr.begin(),
                libVersionStr.end(),
                std::less<char>{}))
        {
/*% "This address generated by newer Beam library version(%1) 
Your version is: %2. Please, check for updates."
*/
            _tokenGeneratebByNewAppVersionMessage = qtTrId("address-newer-lib")
                .arg(libVersionStr.c_str())
                .arg(myLibVersionStr.c_str());
            emit tokenGeneratebByNewAppVersion();
        }
    }
#endif // BEAM_LIB_VERSION

#ifdef BEAM_CLIENT_VERSION
    if (auto clientVersion = _txParameters.GetParameter(TxParameterID::ClientVersion); clientVersion)
    {
        std::string clientVersionStr;
        beam::wallet::fromByteBuffer(*clientVersion, clientVersionStr);
        std::string myClientVersionStr = BEAM_CLIENT_VERSION;

        auto appName = AppModel::getMyName();
        auto res = clientVersionStr.find(appName + " ");
        if (res != std::string::npos)
        {
            clientVersionStr.erase(0, appName.length() + 1);
            std::regex clientVersionRegex("\\d{1,}\\.\\d{1,}\\.\\d{4,}\\.\\d{4,}");
            if (std::regex_match(clientVersionStr, clientVersionRegex) &&
                std::lexicographical_compare(
                    myClientVersionStr.begin(),
                    myClientVersionStr.end(),
                    clientVersionStr.begin(),
                    clientVersionStr.end(),
                    std::less<char>{}))
            {
/*% "This address generated by newer Beam client version(%1) 
Your version is: %2. Please, check for updates."
*/
                _tokenGeneratebByNewAppVersionMessage = qtTrId("address-newer-client")
                    .arg(clientVersionStr.c_str())
                    .arg(myClientVersionStr.c_str());
                emit tokenGeneratebByNewAppVersion();
            }
        }
    }
#endif // BEAM_CLIENT_VERSION
}

QString SendViewModel::getSecondCurrencyLabel() const
{
    return beamui::getCurrencyLabel(_exchangeRatesManager.getRateUnitRaw());
}

QString SendViewModel::getSecondCurrencyRateValue() const
{
    auto rate = _exchangeRatesManager.getRate(beam::wallet::ExchangeRate::Currency::Beam);
    return beamui::AmountToUIString(rate);
}

bool SendViewModel::isNeedExtractShieldedCoins() const
{
    return _isNeedExtractShieldedCoins;
}

bool SendViewModel::isTokenGeneratebByNewAppVersion() const
{
    return !_tokenGeneratebByNewAppVersionMessage.isEmpty();
}

QString SendViewModel::tokenGeneratebByNewAppVersionMessage() const
{
    return _tokenGeneratebByNewAppVersionMessage;
}

bool SendViewModel::hasAddress() const
{
    return _receiverWalletAddress.is_initialized();
}

void SendViewModel::setWalletAddress(const boost::optional<beam::wallet::WalletAddress>& value)
{
    if (_receiverWalletAddress != value)
    {
        _receiverWalletAddress = value;
        emit hasAddressChanged();
    }
}

void SendViewModel::resetMinimalFee()
{
    _minimalFeeGrothes = QMLGlobals::getMinimalFee(Currency::CurrBeam, _isShieldedTx);
    emit minimalFeeGrothesChanged();
    _shieldedInputsFee = 0;
}
