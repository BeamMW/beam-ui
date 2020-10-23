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
    connect(&_walletModel, &WalletModel::newAddressFailed, this, &ReceiveViewModel::newAddressFailed);
    connect(&_exchangeRatesManager, &ExchangeRatesManager::rateUnitChanged, this, &ReceiveViewModel::rateChanged);
    connect(&_exchangeRatesManager, &ExchangeRatesManager::activeRateChanged, this, &ReceiveViewModel::rateChanged);
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
        generateOfflineAddress();
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

void ReceiveViewModel::initialize(const QString& address)
{
    using namespace beam::wallet;
    beam::wallet::WalletID walletID;
    if (address.isEmpty() || !walletID.FromHex(address.toStdString()))
    {
        generateNewReceiverAddress();
    }
    else
    {
        _walletModel.getAsync()->getAddress(walletID, [this](const auto& addr, size_t count) { onGetAddressReturned(addr, count); });
    }
    _walletModel.getAsync()->generateNewAddress([this](const auto& addr) 
    {
        _receiverAddressForExchange = addr;
        _receiverAddressForExchange.setExpiration(beam::wallet::WalletAddress::ExpirationStatus::Never);
        emit receiverAddressForExchangeChanged();
    });

    generateOfflineAddress();
}

void ReceiveViewModel::generateNewReceiverAddress()
{
    _receiverAddress = {};
    emit receiverAddressChanged();

    setAddressComment("");
    _walletModel.getAsync()->generateNewAddress([this](const auto& addr){ onGeneratedNewAddress(addr); });
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

void ReceiveViewModel::onGetAddressReturned(const boost::optional<beam::wallet::WalletAddress>& address, size_t offlinePayments)
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

void ReceiveViewModel::saveReceiverAddress()
{
    using namespace beam::wallet;

    if (getCommentValid())
    {
        _receiverAddress.m_label = _addressComment.toStdString();
        _receiverAddress.setExpiration(isPermanentAddress() ? WalletAddress::ExpirationStatus::Never : WalletAddress::ExpirationStatus::OneDay);
        _walletModel.getAsync()->saveAddress(_receiverAddress, true);
    }
}

void ReceiveViewModel::saveExchangeAddress()
{
    _walletModel.getAsync()->saveAddress(_receiverAddressForExchange, true);
}

void ReceiveViewModel::saveOfflineAddress()
{
    _walletModel.getAsync()->saveAddress(_receiverOfflineAddress, true);
}

void ReceiveViewModel::updateTransactionToken()
{
    using namespace beam::wallet;
    TxParameters params;
    if (_amountToReceiveGrothes > 0)
    {
        params.SetParameter(TxParameterID::Amount, _amountToReceiveGrothes);
    }

#ifdef BEAM_CLIENT_VERSION
    params.SetParameter(
        TxParameterID::ClientVersion,
        AppModel::getMyName() + " " + std::string(BEAM_CLIENT_VERSION));
#endif // BEAM_CLIENT_VERSION
    AppendLibraryVersion(params);

    if (!isShieldedTx())
    {
        params.SetParameter(TxParameterID::PeerID, _receiverAddress.m_walletID);
        params.SetParameter(TxParameterID::PeerWalletIdentity, _receiverAddress.m_Identity);
        params.SetParameter(TxParameterID::IsPermanentPeerID, isPermanentAddress());
        params.SetParameter(TxParameterID::TransactionType, TxType::Simple);
        params.DeleteParameter(TxParameterID::Voucher);
        setTranasctionToken(QString::fromStdString(std::to_string(params)));
    }
    else
    {
        // change tx type
        params.SetParameter(TxParameterID::TransactionType, beam::wallet::TxType::PushTransaction);
        params.SetParameter(TxParameterID::PeerWalletIdentity, _receiverAddress.m_Identity);
        _walletModel.getAsync()->generateVouchers(_receiverAddress.m_OwnID, 1, [this, params=std::move(params)](ShieldedVoucherList v) mutable
        {
            if (!v.empty() && isShieldedTx())
            {
                params.SetParameter(TxParameterID::Voucher, v[0]);
                setTranasctionToken(QString::fromStdString(std::to_string(params)));
            }
        });
    }
}

QString ReceiveViewModel::getRateUnit() const
{
    return beamui::getCurrencyUnitName(_exchangeRatesManager.getRateUnitRaw());
}

QString ReceiveViewModel::getRate() const
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

void ReceiveViewModel::generateOfflineAddress()
{
    using namespace beam::wallet;
    _walletModel.getAsync()->generateNewAddress([this](const auto& addr)
    {
        _receiverOfflineAddress = addr;
        _receiverOfflineAddress.setExpiration(beam::wallet::WalletAddress::ExpirationStatus::Never);

        // add a vouchers
        _walletModel.getAsync()->generateVouchers(_receiverOfflineAddress.m_OwnID, 10, [this](ShieldedVoucherList vouchers)
        {
            if (!vouchers.empty())
            {
                TxParameters offlineParameters;
                offlineParameters.SetParameter(TxParameterID::TransactionType, beam::wallet::TxType::PushTransaction);
                // add voucher parameter
                offlineParameters.SetParameter(TxParameterID::ShieldedVoucherList, vouchers);
                offlineParameters.SetParameter(TxParameterID::PeerID, _receiverOfflineAddress.m_walletID);
                offlineParameters.SetParameter(TxParameterID::PeerWalletIdentity, _receiverOfflineAddress.m_Identity);
                offlineParameters.SetParameter(TxParameterID::IsPermanentPeerID, _receiverOfflineAddress.isPermanent());
                if (_amountToReceiveGrothes > 0)
                {
                    offlineParameters.SetParameter(TxParameterID::Amount, _amountToReceiveGrothes);
                }
                setOfflineToken(QString::fromStdString(std::to_string(offlineParameters)));
            }
            else
            {
                setOfflineToken("");
            }
        });
    });
}
