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

#include "receive_view.h"
#include "ui_helpers.h"
#include "model/qr.h"
#include "model/app_model.h"
#include <QClipboard>

namespace {
    enum {
        AddressExpires = 0,
        AddressNotExpires = 1
    };
}

ReceiveViewModel::ReceiveViewModel()
    : _amountToReceiveGrothes(0)
    , _addressExpires(AddressExpires)
    , _qr(std::make_unique<QR>())
    , _tokenQr(std::make_unique<QR>())
    , _walletModel(*AppModel::getInstance().getWallet())
    , _hasIdentity(true)
{
    connect(_qr.get(), &QR::qrDataChanged, this, &ReceiveViewModel::onReceiverQRChanged);
    connect(_tokenQr.get(), &QR::qrDataChanged, this, &ReceiveViewModel::onTokenQRChanged);
    connect(&_walletModel, &WalletModel::generatedNewAddress, this, &ReceiveViewModel::onGeneratedNewAddress);
    connect(&_walletModel, &WalletModel::newAddressFailed, this, &ReceiveViewModel::newAddressFailed);
    connect(&_exchangeRatesManager, SIGNAL(rateUnitChanged()), SIGNAL(secondCurrencyLabelChanged()));
    connect(&_exchangeRatesManager, SIGNAL(activeRateChanged()), SIGNAL(secondCurrencyRateChanged()));
    generateNewAddress();
    updateTransactionToken();
}

ReceiveViewModel::~ReceiveViewModel()
{
    disconnect(_tokenQr.get(), &QR::qrDataChanged, this, &ReceiveViewModel::onTokenQRChanged);
    disconnect(_qr.get(), &QR::qrDataChanged, this, &ReceiveViewModel::onReceiverQRChanged);
}

void ReceiveViewModel::onGeneratedNewAddress(const beam::wallet::WalletAddress& addr)
{
    _receiverAddress = addr;
    emit receiverAddressChanged();

    _qr->setAddr(beamui::toString(_receiverAddress.m_walletID));
    updateTransactionToken();
}

QString ReceiveViewModel::getAmountToReceive() const
{
    return beamui::AmountToUIString(_amountToReceiveGrothes);
}

void ReceiveViewModel::setAmountToReceive(QString value)
{
    auto amount = beamui::UIStringToAmount(value);
    if (amount != _amountToReceiveGrothes)
    {
        _amountToReceiveGrothes = amount;
        _qr->setAmount(_amountToReceiveGrothes);
        emit amountReceiveChanged();
        updateTransactionToken();
    }
}

void ReceiveViewModel::setAddressExpires(int value)
{
    if (value != _addressExpires)
    {
        _addressExpires = value;
        emit addressExpiresChanged();
    }
}

int ReceiveViewModel::getAddressExpires() const
{
    return _addressExpires;
}

QString ReceiveViewModel::getReceiverAddress() const
{
    return beamui::toString(_receiverAddress.m_walletID);
}

QString ReceiveViewModel::getReceiverAddressQR() const
{
    return _qr->getEncoded();
}

void ReceiveViewModel::onReceiverQRChanged()
{
    emit receiverAddressChanged();
}

void ReceiveViewModel::generateNewAddress()
{
    _receiverAddress = {};
    emit receiverAddressChanged();

    setAddressComment("");
    _walletModel.getAsync()->generateNewAddress();
}

QString ReceiveViewModel::getAddressComment() const
{
    return _addressComment;
}

void ReceiveViewModel::setTranasctionToken(const QString& value)
{
    if (_token != value)
    {
        _token = value;
        _tokenQr->setAddr(value);
        emit transactionTokenChanged();
    }
}

QString ReceiveViewModel::getTransactionToken() const
{
    return _token;
}

QString ReceiveViewModel::getTransactionTokenQR() const
{
    return _tokenQr->getEncoded();
}

void ReceiveViewModel::onTokenQRChanged()
{
    emit transactionTokenChanged();
}

bool ReceiveViewModel::getCommentValid() const
{
    return !_walletModel.isAddressWithCommentExist(_addressComment.toStdString());
}

void ReceiveViewModel::setAddressComment(const QString& value)
{
    auto trimmed = value.trimmed();
    if (_addressComment != trimmed)
    {
        _addressComment = trimmed;
        emit addressCommentChanged();
        emit commentValidChanged();
    }
}


bool ReceiveViewModel::getHasIdentity() const
{
    return _hasIdentity;
}

void ReceiveViewModel::setHasIdentity(bool value)
{
    if (_hasIdentity != value)
    {
        _hasIdentity = value;
        emit hasIdentityChanged();
        updateTransactionToken();
    }
}

void ReceiveViewModel::saveAddress()
{
    using namespace beam::wallet;

    if (getCommentValid()) {
        _receiverAddress.m_label = _addressComment.toStdString();
        _receiverAddress.m_duration = isPermanentAddress() == false ? WalletAddress::AddressExpiration24h : WalletAddress::AddressExpirationNever;
        _walletModel.getAsync()->saveAddress(_receiverAddress, true);
    }
}

void ReceiveViewModel::updateTransactionToken()
{
    using namespace beam::wallet;
    if (_amountToReceiveGrothes > 0)
    {
        _txParameters.SetParameter(TxParameterID::Amount, _amountToReceiveGrothes);
    }
    _txParameters.SetParameter(TxParameterID::PeerID, _receiverAddress.m_walletID);
    _txParameters.SetParameter(TxParameterID::TransactionType, TxType::Simple);
#ifdef BEAM_CLIENT_VERSION
    _txParameters.SetParameter(
        TxParameterID::ClientVersion,
        AppModel::getMyName() + " " + std::string(BEAM_CLIENT_VERSION));
#endif // BEAM_CLIENT_VERSION
#ifdef BEAM_LIB_VERSION
    _txParameters.SetParameter(TxParameterID::LibraryVersion, std::string(BEAM_LIB_VERSION));
#endif // BEAM_LIB_VERSION
    if (_hasIdentity)
    {
        _txParameters.SetParameter(TxParameterID::PeerWalletIdentity, _receiverAddress.m_Identity);
    }
    if (isShieldedTx())
    {
        // change tx type
        _txParameters.SetParameter(TxParameterID::TransactionType, beam::wallet::TxType::PushTransaction);

        if (isNonInteractive())
        {
            // add a vouchers
            auto vouchers = _walletModel.generateVouchers(_receiverAddress.m_OwnID, 10);
            if (!vouchers.empty())
            {
                // add voucher parameter
                _txParameters.SetParameter(TxParameterID::ShieldedVoucherList, vouchers);
                _txParameters.DeleteParameter(TxParameterID::PeerID);
                _txParameters.DeleteParameter(TxParameterID::PeerWalletIdentity);
            }
        }
        else
        {
            _txParameters.DeleteParameter(beam::wallet::TxParameterID::ShieldedVoucherList);
        }
    }
    else
    {
        _txParameters.DeleteParameter(beam::wallet::TxParameterID::ShieldedVoucherList);
    }
    setTranasctionToken(QString::fromStdString(std::to_string(_txParameters)));
}

QString ReceiveViewModel::getSecondCurrencyLabel() const
{
    return beamui::getCurrencyLabel(_exchangeRatesManager.getRateUnitRaw());
}

QString ReceiveViewModel::getSecondCurrencyRateValue() const
{
    auto rate = _exchangeRatesManager.getRate(beam::wallet::ExchangeRate::Currency::Beam);
    return beamui::AmountToUIString(rate);
}

bool ReceiveViewModel::isShieldedTx() const
{
    return _isShieldedTx;
}

void ReceiveViewModel::setIsShieldedTx(bool value)
{
    if (_isShieldedTx != value)
    {
        _isShieldedTx = value;
        emit isShieldedTxChanged();
        updateTransactionToken();
    }
}

bool ReceiveViewModel::isPermanentAddress() const
{
    return _isPermanentAddress;
}

void ReceiveViewModel::setIsPermanentAddress(bool value)
{
    if (_isPermanentAddress != value)
    {
        _isPermanentAddress = value;
        emit isPermanentAddressChanged();
        updateTransactionToken();
    }
}

bool ReceiveViewModel::isNonInteractive() const
{
    return _isNonInteractive;
}

void ReceiveViewModel::setIsNonInteractive(bool value)
{
    if (_isNonInteractive != value)
    {
        _isNonInteractive = value;
        emit isNonInteractiveChanged();
        updateTransactionToken();
    }
}
