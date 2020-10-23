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

// TODO: receiving beam change is invalid

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
    : _fee(QMLGlobals::getMinimalFee(Currency::CurrBeam, false))
    , _walletModel(*AppModel::getInstance().getWallet())
    , _minFee(QMLGlobals::minFeeBeam(false))
{
    connect(&_walletModel,           &WalletModel::changeCalculated,                 this,  &SendViewModel::onChangeCalculated);
    connect(&_walletModel,           SIGNAL(sendMoneyVerified()),                 this,  SIGNAL(sendMoneyVerified()));
    connect(&_walletModel,           SIGNAL(cantSendToExpired()),                 this,  SIGNAL(cantSendToExpired()));
    connect(&_walletModel,           &WalletModel::walletStatusChanged,              this,  &SendViewModel::availableChanged);
    connect(&_exchangeRatesManager,  &ExchangeRatesManager::rateUnitChanged,         this,  &SendViewModel::rateChanged);
    connect(&_exchangeRatesManager,  &ExchangeRatesManager::activeRateChanged,       this,  &SendViewModel::rateChanged);
    connect(&_exchangeRatesManager,  &ExchangeRatesManager::rateUnitChanged,         this,  &SendViewModel::feeRateChanged);
    connect(&_exchangeRatesManager,  &ExchangeRatesManager::activeRateChanged,       this,  &SendViewModel::feeRateChanged);
    connect(&_walletModel,           &WalletModel::shieldedCoinsSelectionCalculated, this,  &SendViewModel::onShieldedCoinsSelectionCalculated);
    connect(&_walletModel,           &WalletModel::needExtractShieldedCoins,         this,  &SendViewModel::onNeedExtractShieldedCoins);
    connect(&_amgr,                  &AssetsManager::assetInfo,                      this,  &SendViewModel::onAssetInfo);
}

unsigned int SendViewModel::getFeeGrothes() const
{
    return _fee;
}

unsigned int SendViewModel::getMinFee() const
{
    return _minFee;
}

void SendViewModel::setFeeGrothes(unsigned int value)
{
    if (value != _fee)
    {
        _feeChangedByUi = true;
        _fee = value;

        emit feeGrothesChanged();
        resetMinimalFee();

        if (_sendAmount == 0 || _fee == 0)
        {
            return;
        }

        if (_walletModel.hasShielded(_selectedAssetId))
        {
            // TODO:
            //_walletModel.getAsync()->calcShieldedCoinSelectionInfo(_sendAmount, _fee, _selectedAssetId, _isShielded);
        }
        else
        {
            _walletModel.getAsync()->calcChange(_sendAmount, _fee, _selectedAssetId);
            _feeChangedByUi = false; // TODO: WTF???? rename _feeChangedByUi or be consistent
        }

        emit canSendChanged();
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
    return beamui::AmountToUIString(_sendAmount);
}

void SendViewModel::setSendAmount(QString value)
{
    beam::Amount amount = beamui::UIStringToAmount(value);
    if (amount != _sendAmount || _maxAvailable)
    {
        if (!amount)
        {
            _sendAmount = amount;
            emit sendAmountChanged();
            resetMinimalFee();
            onChangeCalculated(0, 0, _selectedAssetId);
            _fee = _minFee;
            emit feeGrothesChanged();
            return;
        }

        if (_walletModel.hasShielded(_selectedAssetId))
        {
            if (amount < _sendAmount || _maxAvailable)
            {
                resetMinimalFee();
                onChangeCalculated(0, 0, _selectedAssetId);
                _fee = _minFee;
                emit feeGrothesChanged();
            }

            _sendAmount = amount;
            emit sendAmountChanged();
            // TODO:
            //_walletModel.getAsync()->calcShieldedCoinSelectionInfo(_sendAmountGrothes, _feeGrothes, _isShieldedTx);
        }
        else
        {
            if (_maxAvailable && _selectedAssetId == beam::Asset::s_BeamID)
            {
                amount = amount < _fee ? 0 : amount - _fee;
            }

            _sendAmount = amount;
            emit sendAmountChanged();

            _walletModel.getAsync()->calcChange(_sendAmount, _fee, _selectedAssetId);
            emit canSendChanged();
            _maxAvailable = false;
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
        _newTokenMsg.clear();
        _receiverTA = value;
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
                resetAddress();
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
    return QString::fromStdString(std::to_string(_receiverIdentity));
}

bool SendViewModel::isShieldedTx() const
{
    return _isShielded;
}

void SendViewModel::setIsShieldedTx(bool value)
{
    if (_isShielded != value)
    {
        _isShielded = value;
        emit isShieldedTxChanged();
        resetMinimalFee();

        if (_walletModel.hasShielded(_selectedAssetId) && _sendAmount)
        {
            // TODO: WTF? do we really need this? why not to wait for calc change?
            // Overflow??? when -?
            if (_walletModel.getAvailable(_selectedAssetId) - _sendAmount == 0 &&
                _walletModel.getAvailable(beam::Asset::s_BeamID) - _fee == 0)
            {
                setMaxAvailableAmount();
            }
            else
            {
                // TODO:
                // _walletModel.getAsync()->calcShieldedCoinSelectionInfo(_sendAmountGrothes, _minFee, _isShieldedTx);
            }
        }
        else
        {
            // TODO:
            /*if (_walletModel.getAvailable() - _sendAmountGrothes - _feeGrothes == 0)
            {
                if (_sendAmountGrothes >= _minimalFeeGrothes)
                {
                    _sendAmountGrothes -= _minimalFeeGrothes;
                }
                emit sendAmountChanged();
            }
            setFeeGrothes(_minimalFeeGrothes);
            */
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

bool SendViewModel::isOffline() const
{
    return _isOffline;
}

void SendViewModel::setIsOffline(bool value)
{
    if (_isOffline != value)
    {
        _isOffline = value;
        emit isOfflineChanged();
    }
}

bool SendViewModel::isMaxPrivacy() const
{
    return _isMaxPrivacy;
}

void SendViewModel::setIsMaxPrivacy(bool value)
{
    if (_isMaxPrivacy != value)
    {
        _isMaxPrivacy = value;
        emit isMaxPrivacyChanged();
    }
}

bool SendViewModel::isPublicOffline() const
{
    return _isPublicOffline;
}

void SendViewModel::setIsPublicOffline(bool value)
{
    if (_isPublicOffline != value)
    {
        _isPublicOffline = value;
        emit isPublicOfflineChanged();
    }
}

QString SendViewModel::getAssetAvailable() const
{
    auto amount = _sendAmount + (_selectedAssetId == beam::Asset::s_BeamID ? _fee : 0);
    auto available = _walletModel.getAvailable(_selectedAssetId);

    if (amount < available)
    {
        return beamui::AmountToUIString(available - amount);
    }

    return "0";
}

QString SendViewModel::getBeamAvailable() const
{
    if (_selectedAssetId == beam::Asset::s_BeamID)
    {
        return getAssetAvailable();
    }

    auto amount = _fee;
    auto available = _walletModel.getAvailable(beam::Asset::s_BeamID);

    if (amount < available)
    {
        return beamui::AmountToUIString(available - amount);
    }

    return "0";
}


QString SendViewModel::getAssetMissing() const
{
    auto amount = _sendAmount + (_selectedAssetId == beam::Asset::s_BeamID ? _fee : 0);
    auto available = _walletModel.getAvailable(_selectedAssetId);

    if (amount > available)
    {
        auto missing = amount - available;
        return beamui::AmountToUIString(missing);
    }

    return "0";
}

QString SendViewModel::getBeamMissing() const
{
    auto amount = _fee + (_selectedAssetId == beam::Asset::s_BeamID ? _sendAmount : 0);
    auto available = _walletModel.getAvailable(beam::Asset::s_BeamID);

    if (amount > available)
    {
        auto missing = amount - available;
        return beamui::AmountToUIString(missing);
    }

    return "0";
}

bool SendViewModel::isZeroBalance() const
{
    return _walletModel.getAvailable(beam::Asset::s_BeamID) == 0;
}

bool SendViewModel::isEnough() const
{
    LOG_INFO() << "Asset missing: " << getAssetMissing().toStdString();
    LOG_INFO() << "Beam missing: " << getBeamMissing().toStdString();
    return getAssetMissing() == "0" && getBeamMissing() == "0";
}

void SendViewModel::onChangeCalculated(beam::Amount changeAsset, beam::Amount changeBeam, beam::Asset::ID assetId)
{
    if (_selectedAssetId != assetId) {
        return;
    }

    _changeAsset = changeAsset;
    _changeBeam = changeBeam;

    emit availableChanged();
    emit canSendChanged();
    emit isEnoughChanged();
}

void SendViewModel::onShieldedCoinsSelectionCalculated(const beam::wallet::ShieldedCoinsSelectionInfo& selectionRes)
{
    /* TODO:
    _shieldedInputsFee = selectionRes.shieldedInputsFee;

    if (selectionRes.selectedSum < selectionRes.requestedSum + selectionRes.requestedFee && _maxAvailable)
    {
        _sendAmountGrothes = selectionRes.selectedSum - selectionRes.selectedFee;
        emit sendAmountChanged();
        _maxAvailable = false;
    }

    _minFee = std::max(_minFee, selectionRes.minimalFee);
    emit minFeeChanged();

    if (!_feeChangedByUi)
    {
        _feeGrothes = selectionRes.selectedFee;
        emit feeGrothesChanged();
    }
    _feeChangedByUi = false;

    onChangeCalculated(selectionRes.change);*/
}

void SendViewModel::onNeedExtractShieldedCoins(bool val)
{
    if (_isNeedExtractShieldedCoins != val)
    {
        _isNeedExtractShieldedCoins = val;
        emit isNeedExtractShieldedCoinsChanged();
    }
}

void SendViewModel::onGetAddressReturned(const boost::optional<beam::wallet::WalletAddress>& address, int offlinePayments)
{
    if (address)
    {
        setComment(QString::fromStdString(address->m_label));
    }
    else
    {
        setComment("");
    }

    setWalletAddress(address);
    setOfflinePayments(offlinePayments);
}

QString SendViewModel::getChangeBeam() const
{
    return beamui::AmountToUIString(_changeBeam);
}

QString SendViewModel::getChangeAsset() const
{
    return beamui::AmountToUIString(_changeAsset);
}

QString SendViewModel::getFee() const
{
    return beamui::AmountToUIString(_fee);
}

bool SendViewModel::canSend() const
{
    return !QMLGlobals::isSwapToken(_receiverTA) && getRreceiverTAValid()
           && _sendAmount > 0 && isEnough()
           && QMLGlobals::isFeeOK(_fee, Currency::CurrBeam, isShieldedTx() || _isNeedExtractShieldedCoins)
           && (!isShieldedTx() || !isOffline() || getOfflinePayments() > 0)
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
        emit tokenChanged();
    }
}

bool SendViewModel::isOwnAddress() const
{
    return _walletModel.isOwnAddress(_receiverWalletID);
}

void SendViewModel::setMaxAvailableAmount()
{
    _maxAvailable = true;
    auto amount = _walletModel.getAvailable(_selectedAssetId);
    setSendAmount(beamui::AmountToUIString(amount));
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

        auto params = CreateSimpleTransactionParameters();
        LoadReceiverParams(_txParameters, params);

        params.SetParameter(TxParameterID::Amount, _sendAmount)
              // fee for shielded inputs would be included automatically
              .SetParameter(TxParameterID::Fee, _shieldedFee != 0 ? _fee - _shieldedFee : _fee)
              .SetParameter(TxParameterID::AssetID, _selectedAssetId)
              .SetParameter(TxParameterID::Message, beam::ByteBuffer(messageString.begin(), messageString.end()));

        if (isShieldedTx())
        {
            params.SetParameter(TxParameterID::TransactionType, TxType::PushTransaction);
        }
        if (isMaxPrivacy())
        {
            CopyParameter(TxParameterID::Voucher, _txParameters, params);
            params.SetParameter(TxParameterID::MaxPrivacyMinAnonimitySet, uint8_t(64));
        }

        if (isToken())
        {
            params.SetParameter(TxParameterID::OriginalToken, _receiverTA.toStdString());
        }

        _walletModel.getAsync()->startTransaction(std::move(params));
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
    address.m_Address = _receiverTA.toStdString();
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

    resetAddress();
    _walletModel.getAsync()->getAddress(_receiverTA.toStdString(), [this](const boost::optional<WalletAddress>& addr, size_t c)
    {
        onGetAddressReturned(addr, (int)c);
    });

    if (auto peerID = _txParameters.GetParameter<WalletID>(TxParameterID::PeerID); peerID)
    {
        _receiverWalletID = *peerID;
        _receiverAddress = QString::fromStdString(std::to_string(*peerID));
        setIsToken(_receiverTA != _receiverAddress);
        emit receiverAddressChanged();
        
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
        emit receiverIdentityChanged();
    }

    if (auto isPermanent = _txParameters.GetParameter<bool>(TxParameterID::IsPermanentPeerID); isPermanent)
    {
        setIsPermanentAddress(*isPermanent);
    }

    if (auto txType = _txParameters.GetParameter<TxType>(TxParameterID::TransactionType); txType && *txType == TxType::PushTransaction)
    {
        setIsShieldedTx(true);
        auto vouchers = _txParameters.GetParameter<ShieldedVoucherList>(TxParameterID::ShieldedVoucherList);
        if (vouchers && !vouchers->empty())
        {
            if (_receiverWalletID != Zero)
            {
                _walletModel.getAsync()->saveVouchers(*vouchers, _receiverWalletID);
            }
            setIsOffline(true);
        }
        else
        {
            auto gen = _txParameters.GetParameter<ShieldedTxo::PublicGen>(TxParameterID::PublicAddreessGen);
            if (gen)
            {
                setIsPublicOffline(true);
            }
        }
        
        ShieldedTxo::Voucher voucher;
        setIsMaxPrivacy(_txParameters.GetParameter(TxParameterID::Voucher, voucher) && _receiverIdentity != Zero);
    }

    if (auto amount = _txParameters.GetParameter<beam::Amount>(TxParameterID::Amount); amount && *amount > 0)
    {
        setSendAmount(beamui::AmountToUIString(*amount));
    }
    if (auto fee = _txParameters.GetParameter<beam::Amount>(TxParameterID::Fee); fee)
    {
        setFeeGrothes(*fee);
    }
    if (auto comment = _txParameters.GetParameter<ByteBuffer>(TxParameterID::Message); comment)
    {
        std::string s(comment->begin(), comment->end());
        setComment(QString::fromStdString(s));
    }

    _newTokenMsg.clear();

    ProcessLibraryVersion(_txParameters, [this](const auto& version, const auto& myVersion)
    {
/*% "This address generated by newer Beam library version(%1)
Your version is: %2. Please, check for updates."
*/
        _newTokenMsg = qtTrId("address-newer-lib")
            .arg(version.c_str())
            .arg(myVersion.c_str());
        emit tokenChanged();
    });

#ifdef BEAM_CLIENT_VERSION
    ProcessClientVersion(_txParameters, AppModel::getMyName(), BEAM_CLIENT_VERSION, [this](const auto& version, const auto& myVersion)
    {
/*% "This address generated by newer Beam client version(%1)
Your version is: %2. Please, check for updates."
*/
        _newTokenMsg = qtTrId("address-newer-client")
            .arg(version.c_str())
            .arg(myVersion.c_str());
        emit tokenChanged();
    });
#endif // BEAM_CLIENT_VERSION
}

QString SendViewModel::getRateUnit() const
{
    return _selectedAssetId == beam::Asset::s_BeamID ? beamui::getCurrencyUnitName(_exchangeRatesManager.getRateUnitRaw()) : "";
}

QString SendViewModel::getRate() const
{
    auto rate = _selectedAssetId == beam::Asset::s_BeamID ? _exchangeRatesManager.getRate(beam::wallet::ExchangeRate::Currency::Beam) : 0;
    return beamui::AmountToUIString(rate);
}

QString SendViewModel::getFeeRateUnit() const
{
    return beamui::getCurrencyUnitName(_exchangeRatesManager.getRateUnitRaw());
}

QString SendViewModel::getFeeRate() const
{
    auto rate = _exchangeRatesManager.getRate(beam::wallet::ExchangeRate::Currency::Beam);
    return beamui::AmountToUIString(rate);
}

bool SendViewModel::isNeedExtractShieldedCoins() const
{
    return _isNeedExtractShieldedCoins;
}

bool SendViewModel::getIsNewToken() const
{
    return !_newTokenMsg.isEmpty();
}

QString SendViewModel::getNewTokenMsg() const
{
    return _newTokenMsg;
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
    _minFee = QMLGlobals::minFeeBeam(_isShielded);
    emit minFeeChanged();
    _shieldedFee = 0;
}

QString SendViewModel::getSendUnitName()
{
    return _amgr.getUnitName(_selectedAssetId);
}

void SendViewModel::onAssetInfo(beam::Asset::ID assetId)
{
    if (assetId != static_cast<beam::Asset::ID>(_selectedAssetId)) {
        return;
    }
    emit assetChanged();
}

int SendViewModel::getSelectedAsset() const
{
    return static_cast<int>(_selectedAssetId);
}

void SendViewModel::setSelectedAsset(int value)
{
    _selectedAssetId = value < 0 ? beam::Asset::s_BeamID : static_cast<beam::Asset::ID>(value);
    emit assetChanged();
    emit rateChanged();
}

void SendViewModel::resetAddress()
{
    setIsToken(false);
    setIsShieldedTx(false);
    setIsOffline(false);
    setIsMaxPrivacy(false);
    setIsPublicOffline(false);
    setIsPermanentAddress(false);
    onNeedExtractShieldedCoins(false);
    setWalletAddress({});
    setOfflinePayments(0);

    _receiverAddress.clear();
    _receiverWalletID = beam::Zero;
    _receiverIdentity = beam::Zero;
    _receiverIdentityStr.clear();

    emit receiverAddressChanged();
    emit receiverIdentityChanged();
}
