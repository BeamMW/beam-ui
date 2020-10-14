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
    , _walletModel(*AppModel::getInstance().getWallet())
{
    connect(&_walletModel, &WalletModel::generatedNewAddress, this, &ReceiveViewModel::onGeneratedNewAddress);
    connect(&_walletModel, &WalletModel::getAddressReturned, this, &ReceiveViewModel::onGetAddressReturned);
    connect(&_walletModel, &WalletModel::newAddressFailed, this, &ReceiveViewModel::newAddressFailed);
    connect(&_exchangeRatesManager, SIGNAL(rateUnitChanged()), SIGNAL(secondCurrencyLabelChanged()));
    connect(&_exchangeRatesManager, SIGNAL(activeRateChanged()), SIGNAL(secondCurrencyRateChanged()));
    updateTransactionToken();
}

ReceiveViewModel::~ReceiveViewModel()
{

}

void ReceiveViewModel::onGeneratedNewAddress(const beam::wallet::WalletAddress& addr)
{
    if (_receiverAddress == addr)
    {
        return;
    }
    _receiverAddress = addr;
    emit receiverAddressChanged();
    setIsPermanentAddress(addr.isPermanent());
    setAddressComment(QString::fromStdString(addr.m_label));
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

QString ReceiveViewModel::getReceiverAddressForExchange() const
{
    return beamui::toString(_receiverAddressForExchange.m_walletID);
}

void ReceiveViewModel::onReceiverQRChanged()
{
    emit receiverAddressChanged();
}

void ReceiveViewModel::initialize(const QString& address)
{
    beam::wallet::WalletID walletID;
    if (address.isEmpty() || !walletID.FromHex(address.toStdString()))
    {
        generateNewReceiverAddress();
    }
    else
    {
        _walletModel.getAsync()->getAddress(walletID);
    }
}

void ReceiveViewModel::generateNewReceiverAddress()
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
        emit transactionTokenChanged();
    }
}

QString ReceiveViewModel::getTransactionToken() const
{
    return _token;
}

QString ReceiveViewModel::getOfflineToken() const
{
    return _offlineToken;
}

void ReceiveViewModel::setOfflineToken(const QString& value)
{
    if (_offlineToken != value)
    {
        _offlineToken = value;
        emit offlineTokenChanged();
    }
}

void ReceiveViewModel::onTokenQRChanged()
{
    emit transactionTokenChanged();
}

void ReceiveViewModel::onGetAddressReturned(const beam::wallet::WalletID& id, const boost::optional<beam::wallet::WalletAddress>& address, int offlinePayments)
{
    if (address)
    {
        onGeneratedNewAddress(*address);
    }
}

bool ReceiveViewModel::getCommentValid() const
{
    return _walletModel.isOwnAddress(_receiverAddress.m_walletID) || !_walletModel.isAddressWithCommentExist(_addressComment.toStdString());
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

void ReceiveViewModel::saveAddress()
{
    using namespace beam::wallet;

    if (getCommentValid()) {
        _receiverAddress.m_label = _addressComment.toStdString();
        _receiverAddress.setExpiration(isPermanentAddress() ? WalletAddress::ExpirationStatus::Never : WalletAddress::ExpirationStatus::OneDay);
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
    else
    {
        _txParameters.DeleteParameter(TxParameterID::Amount);
    }
    _txParameters.SetParameter(TxParameterID::PeerID, _receiverAddress.m_walletID);
    _txParameters.SetParameter(TxParameterID::PeerWalletIdentity, _receiverAddress.m_Identity);
    _txParameters.SetParameter(TxParameterID::IsPermanentPeerID, isPermanentAddress());
    _txParameters.SetParameter(TxParameterID::TransactionType, TxType::Simple);
#ifdef BEAM_CLIENT_VERSION
    _txParameters.SetParameter(
        TxParameterID::ClientVersion,
        AppModel::getMyName() + " " + std::string(BEAM_CLIENT_VERSION));
#endif // BEAM_CLIENT_VERSION
    AppendLibraryVersion(_txParameters);

    if (isShieldedTx())
    {
        // change tx type
        _txParameters.SetParameter(TxParameterID::TransactionType, beam::wallet::TxType::PushTransaction);
        setOfflineToken("");
    }
    else
    {
        TxParameters offlineParameters = _txParameters;
        offlineParameters.SetParameter(TxParameterID::TransactionType, beam::wallet::TxType::PushTransaction);
        // add a vouchers
        auto vouchers = _walletModel.generateVouchers(_receiverAddress.m_OwnID, 10);
        if (!vouchers.empty())
        {
            // add voucher parameter
            offlineParameters.SetParameter(TxParameterID::ShieldedVoucherList, vouchers);
            setOfflineToken(QString::fromStdString(std::to_string(offlineParameters)));
        }
        else
        {
            setOfflineToken("");
        }
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
