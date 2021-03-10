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
#include "fee_helpers.h"

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

    beam::AmountBig::Type getMaxInputAmount()
    {
        // not just const because can throw and this would cause compiler warning
        beam::AmountBig::Type kMaxInputAmount = 10000000000000000U;
        return kMaxInputAmount;
    }

}

SendViewModel::SendViewModel()
    : _walletModel(*AppModel::getInstance().getWalletModel())
    , _amgr(AppModel::getInstance().getAssets())
{
    connect(&_walletModel,           SIGNAL(sendMoneyVerified()),          this,  SIGNAL(sendMoneyVerified()));
    connect(&_walletModel,           SIGNAL(cantSendToExpired()),          this,  SIGNAL(cantSendToExpired()));
    connect(&_walletModel,           &WalletModel::walletStatusChanged,        this,  &SendViewModel::availableChanged);
    connect(&_exchangeRatesManager,  &ExchangeRatesManager::activeRateChanged, this,  &SendViewModel::assetsListChanged);
    connect(&_exchangeRatesManager,  &ExchangeRatesManager::rateUnitChanged,   this,  &SendViewModel::feeRateChanged);
    connect(&_exchangeRatesManager,  &ExchangeRatesManager::activeRateChanged, this,  &SendViewModel::feeRateChanged);
    connect(&_walletModel,           &WalletModel::coinsSelectionCalculated,   this,  &SendViewModel::onSelectionCalculated);
    connect(_amgr.get(),                    &AssetsManager::assetsListChanged,        this,  &SendViewModel::assetsListChanged);

    m_Csi.m_explicitFee = minFeeBeam(_isShielded);
    m_Csi.m_minimalExplicitFee = m_Csi.m_explicitFee;
}

unsigned int SendViewModel::getFeeGrothes() const
{
    return m_Csi.m_explicitFee;
}

unsigned int SendViewModel::getMinFee() const
{
    return m_Csi.m_minimalExplicitFee;
}

void SendViewModel::setFeeGrothesUI(unsigned int value)
{
    if (value != m_Csi.m_explicitFee)
    {
        _feeChangedByUi = true;
        setFeeGrothes(value);
    }
}

void SendViewModel::setFeeGrothes(unsigned int value)
{
    if (value != m_Csi.m_explicitFee)
    {
        m_Csi.m_explicitFee = value;
        emit feeGrothesChanged();
        RefreshCsiAsync();
    }
}

void SendViewModel::RefreshCsiAsync()
{
    _walletModel.getAsync()->calcShieldedCoinSelectionInfo(m_Csi.m_requestedSum, m_Csi.m_explicitFee, m_Csi.m_assetID, _isShielded);
}

beam::Amount SendViewModel::get_TotalSpendSelected() const
{
    auto val = m_Csi.m_requestedSum;
    if (!m_Csi.m_assetID)
        val += m_Csi.get_TotalFee();

    return val;
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
    return beamui::AmountToUIString(m_Csi.m_requestedSum);
}

void SendViewModel::setSendAmount(QString value)
{
    beam::Amount amount = beamui::UIStringToAmount(value);
    if (amount != m_Csi.m_requestedSum || _maxPossible)
    {
        m_Csi.m_requestedSum = amount;
        emit sendAmountChanged();
        RefreshCsiAsync();
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
        RefreshCsiAsync();
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
    beam::AmountBig::Type amount = get_TotalSpendSelected();
    beam::AmountBig::Type walletAvailable = _walletModel.getAvailable(m_Csi.m_assetID);

    if (amount < walletAvailable)
    {
        auto available = walletAvailable;

        amount.Negate();
        available += amount;

        return beamui::AmountBigToUIString(available);
    }

    return "0";
}

QString SendViewModel::getBeamAvailable() const
{
    if (m_Csi.m_assetID == beam::Asset::s_BeamID)
    {
        return getAssetAvailable();
    }

    auto amount = m_Csi.m_explicitFee;
    auto available = beam::AmountBig::get_Lo(_walletModel.getAvailable(beam::Asset::s_BeamID));

    if (amount < available)
    {
        return beamui::AmountToUIString(available - amount);
    }

    return "0";
}


QString SendViewModel::getAssetMissing() const
{
    beam::AmountBig::Type amount = get_TotalSpendSelected();
    auto available = _walletModel.getAvailable(m_Csi.m_assetID);

    if (amount > available)
    {
        auto missing = amount;

        available.Negate();
        missing += available;

        return beamui::AmountBigToUIString(missing);
    }

    return "0";
}

QString SendViewModel::getBeamMissing() const
{
    auto amount = m_Csi.get_TotalFee();
    if (!m_Csi.m_assetID)
        amount += m_Csi.m_requestedSum;
    auto available = beam::AmountBig::get_Lo(_walletModel.getAvailable(beam::Asset::s_BeamID));

    if (amount > available)
    {
        auto missing = amount - available;
        return beamui::AmountToUIString(missing);
    }

    return "0";
}

bool SendViewModel::isZeroBalance() const
{
    using namespace beam;
    return _walletModel.getAvailable(beam::Asset::s_BeamID) == Zero;
}

bool SendViewModel::isEnough() const
{
    LOG_INFO() << "Asset missing: " << getAssetMissing().toStdString();
    LOG_INFO() << "Beam missing: " << getBeamMissing().toStdString();
    return getAssetMissing() == "0" && getBeamMissing() == "0";
}

void SendViewModel::onSelectionCalculated(const beam::wallet::CoinsSelectionInfo& selectionRes)
{
    if (selectionRes.m_requestedSum != m_Csi.m_requestedSum ||  selectionRes.m_assetID != m_Csi.m_assetID)
    {
        return;
    }

    m_Csi = selectionRes;

    if (!m_Csi.m_isEnought && _maxPossible)
    {
        m_Csi.m_requestedSum = m_Csi.get_NettoValue();
        m_Csi.m_isEnought = true;
        _maxPossible = false;
    }

    emit sendAmountChanged();
    emit minFeeChanged();
    emit availableChanged();
    emit canSendChanged();
    emit isEnoughChanged();

    if (!_feeChangedByUi)
    {
        emit feeGrothesChanged();
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
    return beamui::AmountBigToUIString(m_Csi.m_changeBeam);
}

QString SendViewModel::getChangeAsset() const
{
    return beamui::AmountBigToUIString(m_Csi.m_changeAsset);
}

QString SendViewModel::getFee() const
{
    return beamui::AmountToUIString(m_Csi.m_explicitFee);
}

bool SendViewModel::canSend() const
{
    return !QMLGlobals::isSwapToken(_receiverTA) && getRreceiverTAValid()
           && m_Csi.m_requestedSum > 0 && m_Csi.m_isEnought
           && (!isShieldedTx() || !isOffline() || getOfflinePayments() > 0)
           && canSendByOneTransaction();
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

void SendViewModel::setMaxPossibleAmount()
{
    _maxPossible = true;
    _feeChangedByUi = false;

    const auto amount = _walletModel.getAvailable(m_Csi.m_assetID);
    const auto maxAmount = std::min(amount, getMaxInputAmount());
    setSendAmount(beamui::AmountBigToUIString(maxAmount));
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

        params.SetParameter(TxParameterID::Amount, m_Csi.m_requestedSum)
              // fee for shielded inputs would be included automatically
              .SetParameter(TxParameterID::Fee, m_Csi.m_explicitFee)
              .SetParameter(TxParameterID::AssetID, m_Csi.m_assetID)
              .SetParameter(TxParameterID::Message, beam::ByteBuffer(messageString.begin(), messageString.end()));

        if (isShieldedTx())
        {
            params.SetParameter(TxParameterID::TransactionType, TxType::PushTransaction);
        }
        if (isMaxPrivacy())
        {
            CopyParameter(TxParameterID::Voucher, _txParameters, params);
            const auto& settings = AppModel::getInstance().getSettings();
            params.SetParameter(TxParameterID::MaxPrivacyMinAnonimitySet, settings.getMaxPrivacyAnonymitySet());
        }
        if (isShieldedTx())
        {
            CopyParameter(TxParameterID::PeerOwnID, _txParameters, params);
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
    using namespace beam::wallet;
    QString trimmed = name.trimmed();
    if (!_walletModel.isOwnAddress(_receiverWalletID))
    {
        WalletAddress address;
        address.m_walletID = _receiverWalletID;
        address.m_createTime = getTimestamp();
        address.m_Identity = _receiverIdentity;
        address.m_label = trimmed.toStdString();
        address.m_duration = WalletAddress::AddressExpirationNever;
        address.m_Address = _receiverTA.toStdString();
        _walletModel.getAsync()->saveAddress(address, false);
    }
    else
    {
        _walletModel.getAsync()->getAddress(_receiverWalletID, [this, trimmed](const boost::optional<WalletAddress>& addr, size_t c)
        {
            WalletAddress address = *addr;
            address.m_label = trimmed.toStdString();
            _walletModel.getAsync()->saveAddress(address, true);
        });
    }
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
    if (_receiverWalletID != Zero)
    {
        _walletModel.getAsync()->getAddress(_receiverWalletID, [this](const boost::optional<WalletAddress>& addr, size_t c)
        {
            onGetAddressReturned(addr, (int)c);
        });
    }
    else
    {
        _walletModel.getAsync()->getAddress(_receiverTA.toStdString(), [this](const boost::optional<WalletAddress>& addr, size_t c)
        {
            onGetAddressReturned(addr, (int)c);
        });
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
    return false; // TODO: remove
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

bool SendViewModel::canSendByOneTransaction() const
{
    return m_Csi.m_isEnought;
}

QString SendViewModel::getMaxSendAmount() const
{
    return beamui::AmountToUIString(m_Csi.get_NettoValue());
}

int SendViewModel::getSelectedAssetId() const
{
    return static_cast<int>(m_Csi.m_assetID);
}

void SendViewModel::setSelectedAssetId(int value)
{
    auto valueId = value < 0 ? beam::Asset::s_BeamID : static_cast<beam::Asset::ID>(value);
    if (m_Csi.m_assetID != valueId)
    {
        LOG_INFO () << "Selected asset id: " << value;
        m_Csi.m_assetID = valueId;

        emit selectedAssetChanged();
        emit availableChanged();
    }
}

void SendViewModel::resetAddress()
{
    setIsToken(false);
    setIsShieldedTx(false);
    setIsOffline(false);
    setIsMaxPrivacy(false);
    setIsPublicOffline(false);
    setIsPermanentAddress(false);
    setWalletAddress({});
    setOfflinePayments(0);

    _receiverAddress.clear();
    _receiverWalletID = beam::Zero;
    _receiverIdentity = beam::Zero;

    emit receiverAddressChanged();
    emit receiverIdentityChanged();
}

QList<QMap<QString, QVariant>> SendViewModel::getAssetsList() const
{
    return _amgr->getAssetsList();
}
