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
    beam::AmountBig::Type getMaxInputAmount()
    {
        // not just const because can throw and this would cause compiler warning
        beam::AmountBig::Type kMaxInputAmount = 10000000000000000U;
        return kMaxInputAmount;
    }

    void CopyParameter(beam::wallet::TxParameterID paramID, const beam::wallet::TxParameters& input, beam::wallet::TxParameters& dest)
    {
        beam::ByteBuffer buf;
        if (input.GetParameter(paramID, buf))
        {
            dest.SetParameter(paramID, buf);
        }
    }
}

SendViewModel::SendViewModel()
    : _walletModel(*AppModel::getInstance().getWalletModel())
    , _settings(AppModel::getInstance().getSettings())
    , _amgr(AppModel::getInstance().getAssets())
{
    connect(&_walletModel,           &WalletModel::walletStatusChanged,        this,  &SendViewModel::balanceChanged);
    connect(&_exchangeRatesManager,  &ExchangeRatesManager::rateUnitChanged,   this,  &SendViewModel::feeRateChanged);
    connect(&_exchangeRatesManager,  &ExchangeRatesManager::activeRateChanged, this,  &SendViewModel::feeRateChanged);
    connect(_amgr.get(),             &AssetsManager::assetsListChanged,        this,  &SendViewModel::assetsListChanged);
    connect(&_walletModel,           &WalletModel::coinsSelected,       this,  &SendViewModel::onCoinsSelected);
    connect(&_walletModel,           &WalletModel::sendMoneyVerified,          this,  &SendViewModel::sendMoneyVerified);
    connect(&_walletModel,           &WalletModel::cantSendToExpired,          this,  &SendViewModel::cantSendToExpired);
    connect(&_walletModel,           &WalletModel::publicAddressChanged,       this,  &SendViewModel::onPublicAddress);

    _walletModel.getAsync()->getPublicAddress();
}

beam::Amount SendViewModel::getTotalSpend() const
{
    auto val = m_Csi.m_requestedSum;
    if (!m_Csi.m_assetID)
    {
        val += m_Csi.get_TotalFee();
    }
    return val;
}

int SendViewModel::getAssetId() const
{
    return static_cast<int>(m_Csi.m_assetID);
}

void SendViewModel::setAssetId(int value)
{
    auto valueId = value < 0 ? beam::Asset::s_BeamID : static_cast<beam::Asset::ID>(value);
    if (m_Csi.m_assetID != valueId)
    {
        LOG_INFO () << "Selected asset id: " << value;
        m_Csi.m_assetID = valueId;
        emit assetIdChanged();
        RefreshCsiAsync();
    }
}

QString SendViewModel::getAssetAvailable() const
{
    beam::AmountBig::Type available = _walletModel.getAvailable(m_Csi.m_assetID);
    return beamui::AmountBigToUIString(available);
}

QString SendViewModel::getAssetRemaining() const
{
    beam::AmountBig::Type amount = getTotalSpend();
    beam::AmountBig::Type available = _walletModel.getAvailable(m_Csi.m_assetID);

    if (amount < available)
    {
        auto remaining = available;

        amount.Negate();
        remaining += amount;

        return beamui::AmountBigToUIString(remaining);
    }

    return "0";
}

QString SendViewModel::getBeamRemaining() const
{
    if (m_Csi.m_assetID == beam::Asset::s_BeamID)
    {
        return getAssetRemaining();
    }

    const auto amount = m_Csi.m_explicitFee;
    const auto available = beam::AmountBig::get_Lo(_walletModel.getAvailable(beam::Asset::s_BeamID));

    if (amount < available)
    {
        return beamui::AmountToUIString(available - amount);
    }

    return "0";
}

QString SendViewModel::getFee() const
{
    return beamui::AmountToUIString(m_Csi.get_TotalFee());
}

QString SendViewModel::getChangeBeam() const
{
    return beamui::AmountBigToUIString(m_Csi.m_changeBeam);
}

QString SendViewModel::getChangeAsset() const
{
    return beamui::AmountBigToUIString(m_Csi.m_changeAsset);
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

QString SendViewModel::getFeeRateUnit() const
{
    return beamui::getCurrencyUnitName(_exchangeRatesManager.getRateCurrency());
}

QString SendViewModel::getFeeRate() const
{
    auto rate = _exchangeRatesManager.getRate(beam::wallet::Currency::BEAM());
    return beamui::AmountToUIString(rate);
}

bool SendViewModel::getIsEnough() const
{
    return m_Csi.m_isEnought;
}

bool SendViewModel::getIsEnoughAmount() const
{
    return m_Csi.m_requestedSum <= m_Csi.m_selectedSumAsset;
}

bool SendViewModel::getIsEnoughFee() const
{
    if (m_Csi.m_assetID)
    {
        return m_Csi.get_TotalFee() <= m_Csi.m_selectedSumBeam;
    }
    else
    {
        return m_Csi.get_TotalFee() + m_Csi.m_requestedSum <= m_Csi.m_selectedSumBeam;
    }
}

QString SendViewModel::getSendAmount() const
{
    return beamui::AmountToUIString(m_Csi.m_requestedSum);
}

void SendViewModel::setSendAmount(const QString& value)
{
    beam::Amount amount = beamui::UIStringToAmount(value);
    if (amount != m_Csi.m_requestedSum)
    {
        _maxPossible = false;
        m_Csi.m_requestedSum = amount;
        RefreshCsiAsync();
    }
}

bool SendViewModel::canSend() const
{
    if (QMLGlobals::isSwapToken(_token))
    {
        return false;
    }

    return getTokenValid() &&
           m_Csi.m_requestedSum > 0 &&
           m_Csi.m_isEnought;
}

QList<QMap<QString, QVariant>> SendViewModel::getAssetsList() const
{
    return _amgr->getAssetsList();
}

QString SendViewModel::getMaxSendAmount() const
{
    return beamui::AmountToUIString(m_Csi.get_NettoValue());
}

QString SendViewModel::getToken() const
{
    return _token;
}

void SendViewModel::setToken(const QString& value)
{
    if (_token != value)
    {
        _newTokenMsg.clear();
        _token = value;
        _choiceOffline = false;

        if (QMLGlobals::isSwapToken(value))
        {
            // Just ignore, UI would handle this case
            // and automatically switch to another view
        }
        else
        {
            extractParameters();
        }

        emit tokenChanged();
        emit tokenTipChanged();
        emit choiceChanged();
        emit canSendChanged();
    }
}

bool SendViewModel::getTokenValid() const
{
    return !_token.isEmpty() && QMLGlobals::isToken(_token);
}

QString SendViewModel::getNewTokenMsg() const
{
    return _newTokenMsg;
}

void SendViewModel::RefreshCsiAsync()
{
    if(m_Csi.m_requestedSum == 0UL)
    {
        // just reset everything to zero
        auto csi = decltype(m_Csi)();
        csi.m_assetID = m_Csi.m_assetID;
        return onCoinsSelected(csi);
    }

    using namespace beam::wallet;
    const auto type = getTokenValid() ? GetAddressType(_token.toStdString()) : TxAddressType::Unknown;
    bool isShielded = false;

    switch(type)
    {
        case TxAddressType::PublicOffline:
            isShielded = true;
            break;

        case TxAddressType::MaxPrivacy:
            isShielded = true;
            break;

        case TxAddressType::Offline:
            isShielded = _choiceOffline;
            break;

        case TxAddressType::Regular:
            isShielded = false;
            break;

        default:
            isShielded = false;
            break;
    }

    _walletModel.getAsync()->selectCoins(
            m_Csi.m_requestedSum,
            0,
            m_Csi.m_assetID,
            isShielded);
}

bool SendViewModel::getCanChoose() const
{
    using namespace beam::wallet;
    const auto type = getTokenValid() ? GetAddressType(_token.toStdString()) : TxAddressType::Unknown;
    return type == TxAddressType::Offline;
}

bool SendViewModel::getChoiceOffline() const
{
    return _choiceOffline;
}

void SendViewModel::setChoiceOffline(bool value)
{
    if (_choiceOffline != value)
    {
        _choiceOffline = value;
        emit choiceChanged();
        emit tokenTipChanged();
        RefreshCsiAsync();
    }
}

void SendViewModel::setMaxPossibleAmount()
{
    const auto amount = _walletModel.getAvailable(m_Csi.m_assetID);
    const auto maxAmount = std::min(amount, getMaxInputAmount());

    _maxPossible = true;
    m_Csi.m_requestedSum = beam::AmountBig::get_Lo(maxAmount);

    RefreshCsiAsync();
}

void SendViewModel::onPublicAddress(const QString& pubAddr)
{
    _publicOfflineAddr = pubAddr;
}

void SendViewModel::onCoinsSelected(const beam::wallet::CoinsSelectionInfo& selectionRes)
{
    if (selectionRes.m_requestedSum != m_Csi.m_requestedSum || selectionRes.m_assetID != m_Csi.m_assetID)
    {
        return;
    }

    m_Csi = selectionRes;
    if (!m_Csi.m_isEnought)
    {
        if(_maxPossible && m_Csi.m_requestedSum != m_Csi.get_NettoValue())
        {
            m_Csi.m_requestedSum = m_Csi.get_NettoValue();
            RefreshCsiAsync();
            return;
        }
    }

    emit balanceChanged();
    emit canSendChanged();
}

void SendViewModel::saveReceiverAddress(const QString& comment)
{
    using namespace beam::wallet;
    QString trimmed = comment.trimmed();

    if (_publicOfflineAddr == _token)
    {
        // just skip, never save own public address
    }
    else if (!_walletModel.isOwnAddress(_receiverWalletID))
    {
        WalletAddress address;
        address.m_walletID   = _receiverWalletID;
        address.m_createTime = beam::getTimestamp();
        address.m_Identity   = _receiverIdentity;
        address.m_label      = trimmed.toStdString();
        address.m_duration   = WalletAddress::AddressExpirationNever;
        address.m_Address    = _token.toStdString();
        _walletModel.getAsync()->saveAddress(address);
    }
    else
    {
        if (_receiverWalletID.IsValid())
        {
            _walletModel.getAsync()->getAddress(_receiverWalletID, [this, trimmed](const boost::optional<WalletAddress>& addr, size_t c)
            {
                WalletAddress address = *addr;
                address.m_label = trimmed.toStdString();
                _walletModel.getAsync()->saveAddress(address);
            });
        }
        else
        {
            // Max privacy & public offline tokens do not have valid PeerID (_receiverWalletID)
            _walletModel.getAsync()->getAddressByToken(_token.toStdString(), [this, trimmed](const boost::optional<WalletAddress>& addr, size_t c)
            {
                WalletAddress address = *addr;
                address.m_label = trimmed.toStdString();
                _walletModel.getAsync()->saveAddress(address);
            });
        }
    }
}

void SendViewModel::onGetAddressReturned(const boost::optional<beam::wallet::WalletAddress>& address, size_t offlinePayments)
{
    using namespace beam::wallet;

    if (address)
    {
        setComment(QString::fromStdString(address->m_label));

        [[maybe_unused]] const auto type = GetAddressType(address->m_Address);
        if (_receiverWalletID != beam::Zero)
        {
            if (_receiverWalletID != address->m_walletID)
            {
                assert(!"unexpected wallet id in send::onGetAddressReturned");
                throw std::runtime_error("unexpected walletID in send::onGetAddressReturned");
            }
        }
        else
        {
            assert(type == TxAddressType::MaxPrivacy || type == TxAddressType::PublicOffline);
            _receiverWalletID = address->m_walletID; // our maxprivacy will have id in db
        }

        if (_receiverIdentity != beam::Zero)
        {
            if (address->m_Identity != beam::Zero)
            {
                if (_receiverIdentity != address->m_Identity)
                {
                    assert(!"unexpected identity in send::onGetAddressReturned");
                    throw std::runtime_error("unexpected identity in send::onGetAddressReturned");
                }
            }
            else
            {
                // old SBBS address existed in address book, new token pasted for the same sbbs address
                assert(type == TxAddressType::Regular);
            }
        }
        else
        {
            if (address->m_Identity != beam::Zero)
            {
                _receiverIdentity = address->m_Identity;
            }
            else
            {
                assert(
                    type == TxAddressType::PublicOffline ||
                    (type == TxAddressType::Regular && _token.toStdString() == std::to_string(_receiverWalletID))
                );
            }
        }
    }
    else
    {
        setComment("");
    }

    _vouchersLeft = offlinePayments;
    emit tokenTipChanged();
}

void SendViewModel::extractParameters()
{
    using namespace beam::wallet;

    auto txParameters = ParseParameters(_token.toStdString());
    if (!txParameters)
    {
        return;
    }

    _txParameters     = *txParameters;
    _receiverWalletID = beam::Zero;
    _receiverIdentity = beam::Zero;
    _vouchersLeft     = 0;
    _newTokenMsg.clear();

    if (auto peerID = _txParameters.GetParameter<WalletID>(TxParameterID::PeerID); peerID)
    {
        _receiverWalletID = *peerID;
        if (_receiverWalletID != beam::Zero)
        {
            if(auto vouchers = _txParameters.GetParameter<ShieldedVoucherList>(TxParameterID::ShieldedVoucherList); vouchers)
            {
                if (!vouchers->empty())
                {
                    _walletModel.getAsync()->saveVouchers(*vouchers, _receiverWalletID);
                    _vouchersLeft = vouchers->size();
                }
            }
        }
    }

    if (auto peerIdentity = _txParameters.GetParameter<beam::PeerID>(TxParameterID::PeerWalletIdentity); peerIdentity)
    {
        _receiverIdentity = *peerIdentity;
    }

    if (auto amount = _txParameters.GetParameter<beam::Amount>(TxParameterID::Amount); amount && *amount > 0)
    {
        m_Csi.m_requestedSum = *amount;
    }

    if (auto assetId = _txParameters.GetParameter<beam::Asset::ID>(TxParameterID::AssetID); assetId)
    {
        if (_amgr->hasAsset(*assetId))
        {
            m_Csi.m_assetID = *assetId;
            emit assetIdChanged();
        }
    }

    if (auto comment = _txParameters.GetParameter<beam::ByteBuffer>(TxParameterID::Message); comment)
    {
        _comment = QString::fromStdString(std::string(comment->begin(), comment->end()));
        emit commentChanged();
    }

    if (_receiverWalletID.IsValid())
    {
        _walletModel.getAsync()->getAddress(_receiverWalletID, [this](const boost::optional<WalletAddress>& addr, size_t c)
        {
            onGetAddressReturned(addr, c);
        });
    }
    else
    {
        // Max privacy & public offline tokens do not have valid PeerID (_receiverWalletID)
        _walletModel.getAsync()->getAddressByToken(_token.toStdString(), [this](const boost::optional<WalletAddress>& addr, size_t c)
        {
            onGetAddressReturned(addr, c);
        });
    }

    std::string libVersion;
    ProcessLibraryVersion(_txParameters, [this, &libVersion](const auto& version, const auto& myVersion)
    {
        libVersion = version;
/*% "This address generated by newer Beam library version(%1)
Your version is: %2. Please, check for updates."
*/
        _newTokenMsg = qtTrId("address-newer-lib")
            .arg(version.c_str())
            .arg(myVersion.c_str());
    });

    #ifdef BEAM_CLIENT_VERSION
    ProcessClientVersion(_txParameters, AppModel::getMyName(), BEAM_CLIENT_VERSION, libVersion, [this](const auto& version, const auto& myVersion)
    {
/*% "This address generated by newer Beam client version(%1)
Your version is: %2. Please, check for updates."
*/
        _newTokenMsg = qtTrId("address-newer-client")
            .arg(version.c_str())
            .arg(myVersion.c_str());

    });
    #endif // BEAM_CLIENT_VERSION

    emit tokenChanged();
    emit tokenTipChanged();
    RefreshCsiAsync();
}

void SendViewModel::sendMoney()
{
    using namespace beam::wallet;

    if (!canSend())
    {
        assert(false);
        return;
    }

    auto messageString = _comment.toStdString();
    saveReceiverAddress(_comment);

    auto params = CreateSimpleTransactionParameters();
    const auto type = GetAddressType(_token.toStdString());

    if (type == TxAddressType::Unknown)
    {
        assert(false);
        return;
    }

    if (type == TxAddressType::MaxPrivacy || type == TxAddressType::PublicOffline || (type == TxAddressType::Offline && _choiceOffline))
    {
        if (!LoadReceiverParams(_txParameters, params, type))
        {
            assert(false);
            return;
        }
        CopyParameter(TxParameterID::PeerOwnID, _txParameters, params);
    }
    else
    {
        if(!LoadReceiverParams(_txParameters, params, TxAddressType::Regular))
        {
            assert(false);
            return;
        }
    }

    params.SetParameter(TxParameterID::Amount, m_Csi.m_requestedSum)
          // fee for shielded inputs would be included automatically
          .SetParameter(TxParameterID::Fee, m_Csi.m_explicitFee)
          .SetParameter(TxParameterID::AssetID, m_Csi.m_assetID)
          .SetParameter(TxParameterID::Message, beam::ByteBuffer(messageString.begin(), messageString.end()));

    if (type == TxAddressType::MaxPrivacy)
    {
        CopyParameter(TxParameterID::Voucher, _txParameters, params);
        const auto& settings = AppModel::getInstance().getSettings();
        params.SetParameter(TxParameterID::MaxPrivacyMinAnonimitySet, settings.getMaxPrivacyAnonymitySet());
    }

    params.SetParameter(TxParameterID::OriginalToken, _token.toStdString());
    _walletModel.getAsync()->startTransaction(std::move(params));
}

QString SendViewModel::getSendType() const
{
    return beamui::GetTokenTypeUIString(_token.toStdString(), _choiceOffline);
}

bool SendViewModel::getSendTypeOnline() const
{
    using namespace beam::wallet;
    const auto type = GetAddressType(_token.toStdString());

    if (type == TxAddressType::Offline && _choiceOffline)
    {
        return false;
    }

    if (type == TxAddressType::PublicOffline)
    {
        return false;
    }

    if (type == TxAddressType::MaxPrivacy)
    {
        return false;
    }

    return true;
}

QString SendViewModel::getTokenTip() const
{
    using namespace beam::wallet;
    const auto type = GetAddressType(_token.toStdString());

    if (type == TxAddressType::Regular || (type == TxAddressType::Offline && !_choiceOffline))
    {
        //% "Online address."
        return qtTrId("send-online-address");
    }

    if (type == TxAddressType::Offline && _choiceOffline)
    {
        QString left;

        if (_vouchersLeft == 1)
        {
            //% "Offline address: %1 transaction left."
            left = qtTrId("send-offline-tip-single").arg(_vouchersLeft);
        }
        else
        {
            //% "Offline address: %1 transactions left."
            left = qtTrId("send-offline-tip-many").arg(_vouchersLeft);
        }

        if (_vouchersLeft < 4)
        {
            //% "Ask receiver to come online to support more offline transactions."
            left += QString(" ") + qtTrId("send-receiver-online-tip");
        }

        return left;
    }

    if (type == TxAddressType::MaxPrivacy)
    {
        //% "Guarantees anonymity set of up to 64K."
        return qtTrId("send-anon-set");
    }

    if (type == TxAddressType::PublicOffline)
    {
        //% "Public offline address."
        return qtTrId("send-public-token");
    }

    //% "Unknown address."
    return qtTrId("send-unknown-token");
}

QString SendViewModel::getTokenTip2() const
{
    using namespace beam::wallet;
    const auto type = GetAddressType(_token.toStdString());

    if (type == TxAddressType::Regular || (type == TxAddressType::Offline && !_choiceOffline))
    {
        //% "The recipient must get online within the next 12 hours and you should get online within 2 hours afterwards."
        return qtTrId("send-online-tip");
    }

    if (type == TxAddressType::Offline && _choiceOffline)
    {
        //% "Make sure the address is correct as offline transactions\ncannot be canceled."
        return qtTrId("send-offline-refund");
    }

    if (type == TxAddressType::MaxPrivacy)
    {
        //% "Transaction can last up to 72 hours."
        return qtTrId("send-mp-tip");
    }

    return "";
}