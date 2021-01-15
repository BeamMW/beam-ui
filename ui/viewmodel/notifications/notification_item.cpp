// Copyright 2020 The Beam Team
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

#include "notification_item.h"
#include "utility/helpers.h"
#include "wallet/core/common.h"
#include "viewmodel/ui_helpers.h"
#include "viewmodel/qml_globals.h"

using namespace beam::wallet;

namespace {
    TxParameters getTxParameters(const Notification &notification)
    {
        TxToken token;
        Deserializer d;
        d.reset(notification.m_content);
        d & token;
        return token.UnpackParameters();
    }

    QString getAmount(const TxParameters &p)
    {
        return beamui::AmountToUIString(*p.GetParameter<Amount>(TxParameterID::Amount));
    }

    QString getSwapAmount(const TxParameters &p)
    {
        return beamui::AmountToUIString(*p.GetParameter<Amount>(TxParameterID::AtomicSwapAmount));
    }

    bool isBeamSide(const TxParameters &p)
    {
        return *p.GetParameter<bool>(TxParameterID::AtomicSwapCoin);
    }

    QString getSwapCoinName(const TxParameters &p)
    {
        auto swapCoin = p.GetParameter<AtomicSwapCoin>(TxParameterID::AtomicSwapCoin);
        return beamui::toString(beamui::convertSwapCoinToCurrency(*swapCoin));
    }


    bool getPeerID(const TxParameters &p, WalletID &result)
    {
        if (auto peerId = p.GetParameter<WalletID>(TxParameterID::PeerID))
        {
            result = *peerId;
            return true;
        } else
        {
            return false;
        }
    }

    Asset::ID getAssetId(const TxParameters &p)
    {
        Asset::ID assetId = Asset::s_InvalidID;
        if (p.GetParameter(TxParameterID::AssetID, assetId))
        {
            return assetId;
        }
        return Asset::s_BeamID;
    }

    bool isSender(const TxParameters& p)
    {
        return *p.GetParameter<bool>(TxParameterID::IsSender);
    }

    TxType getTxType(const TxParameters& p)
    {
        return *p.GetParameter<TxType>(TxParameterID::TransactionType);
    }

    QString getContractMessage(const TxParameters& p)
    {
        const auto rawMsg = p.GetParameter<beam::ByteBuffer>(TxParameterID::Message);
        if (rawMsg && !rawMsg->empty())
        {
            std::string str{rawMsg->cbegin(), rawMsg->cend()};
            return QString::fromStdString(str);
        }
        //% "Contract: no description provided"
        return qtTrId("notification-contract-no-message");
    }

    bool isExpired(const TxParameters& p)
    {
        auto status = p.GetParameter<TxStatus>(TxParameterID::FailureReason);
        auto reason =  p.GetParameter<TxFailureReason>(TxParameterID::FailureReason);
        return status && reason && *status == wallet::TxStatus::Failed && reason == TxFailureReason::TransactionExpired;
    }

    bool isSwapTxExpired(const TxParameters& p)
    {
        auto txStatus = p.GetParameter<wallet::TxStatus>(TxParameterID::Status);
        auto failureReason = p.GetParameter<TxFailureReason>(TxParameterID::InternalFailureReason);
        return txStatus
            && failureReason
            && *txStatus == wallet::TxStatus::Failed
            && *failureReason == TxFailureReason::TransactionExpired;
    }

    WalletAddress getWalletAddressRaw(const Notification& notification)
    {
        WalletAddress walletAddress;
        fromByteBuffer(notification.m_content, walletAddress);
        return walletAddress;
    }

    QString getAddress(const Notification& notification)
    {
        return beamui::toString(getWalletAddressRaw(notification).m_walletID);
    }

    QString getTxCompletedMessage(const QString& amount, const QString& unitName, const QString& peer, bool isSender)
    {
        return (isSender ? 
                //% "You sent <b>%1 %2</b> to <b>%3</b>."
                qtTrId("notification-transaction-sent-message")
                : 
                //% "You received <b>%1 %2</b> from <b>%3</b>."
                qtTrId("notification-transaction-received-message"))
                    .arg(amount)
                    .arg(unitName)
                    .arg(peer);
    }

    QString getTxFailedMessage(const QString& amount, const QString& unitName, const QString& peer, bool isSender)
    {
        return (isSender ?
            //% "Sending <b>%1 %2</b> to <b>%3</b> failed."
            qtTrId("notification-transaction-send-failed-message")
            :
            //% "Receiving <b>%1 %2</b> from <b>%3</b> failed."
            qtTrId("notification-transaction-receive-failed-message"))
                .arg(amount)
                .arg(unitName)
                .arg(peer);
    }

    QString getPushTxPeer(const TxParameters& p, bool isSender)
    {
        if (isSender)
        {
            WalletID wid;
            if (getPeerID(p, wid))
            {
                return std::to_string(wid).c_str();
            }
        }
        if (auto peerID = p.GetParameter<PeerID>(TxParameterID::PeerWalletIdentity); peerID)
        {
            return std::to_string(*peerID).c_str();
        }

        //% "shielded pool"
        return qtTrId("from-shielded-pool");
    }
}

NotificationItem::NotificationItem(const Notification& notification)
    : m_notification{notification}
{}

bool NotificationItem::operator==(const NotificationItem& other) const
{
    return getID() == other.getID();
}

ECC::uintBig NotificationItem::getID() const
{
    return m_notification.m_ID;
}

QDateTime NotificationItem::timeCreated() const
{
    QDateTime datetime;
    datetime.setTime_t(m_notification.m_createTime);
    return datetime;
}

Timestamp NotificationItem::getTimestamp() const
{
    return m_notification.m_createTime;
}

Notification::State NotificationItem::getState() const
{
    return m_notification.m_state;
}

QString NotificationItem::title() const
{
    switch(m_notification.m_type)
    {
        case Notification::Type::WalletImplUpdateAvailable:
        {
            WalletImplVerInfo info;
            if (fromByteBuffer(m_notification.m_content, info))
            {
                QString ver = QString::fromStdString(
                    info.m_version.to_string() + "." + std::to_string(info.m_UIrevision));
                //% "New version v %1 is available"
                return qtTrId("notification-update-title").arg(ver);
            }
            else
            {
                LOG_ERROR() << "Software update notification deserialization error";
                return QString();
            }
        }
        case Notification::Type::AddressStatusChanged:
            //% "Address expired"
            return qtTrId("notification-address-expired");
        case Notification::Type::TransactionCompleted:
        {
            auto p = getTxParameters(m_notification);
            switch (getTxType(p))
            {
            case TxType::Simple:
            case TxType::PushTransaction:
                if (isSender(p))
                {
                    //% "Transaction was sent"
                    return qtTrId("notification-transaction-sent");
                }
                //% "Transaction was received"
                return qtTrId("notification-transaction-received");
            case TxType::AtomicSwap:
                //% "Atomic Swap offer completed"
                return qtTrId("notification-swap-completed");
            case TxType::Contract:
                //% "Transaction completed"
                return qtTrId("notification-contract-completed");
            default:
                return "error";
            }
        }            
        case Notification::Type::TransactionFailed:
        {
            auto p = getTxParameters(m_notification);
            switch (getTxType(p))
            {
            case TxType::Simple:
            case TxType::PushTransaction:
                //% "Transaction failed"
                return qtTrId("notification-transaction-failed");
            case TxType::AtomicSwap:
                return isSwapTxExpired(p) ?
                        //% "Atomic Swap offer expired"
                        qtTrId("notification-swap-expired")
                        :
                        //% "Atomic Swap offer failed"
                        qtTrId("notification-swap-failed");
            case TxType::Contract:
                return isExpired(p) ?
                    //% "Transaction expired"
                    qtTrId("notification-contract-expired") :
                    qtTrId("notification-transaction-failed");
            default:
                return "error";
            }
        }
        case Notification::Type::BeamNews:
            //% "BEAM in the press"
            return qtTrId("notification-news");
        default:
            return "error";
    }
}

QString NotificationItem::message(AssetsManager& amgr) const
{
    switch(m_notification.m_type)
    {
        case Notification::Type::WalletImplUpdateAvailable:
        {
            WalletImplVerInfo info;
            if (fromByteBuffer(m_notification.m_content, info))
            {
                QString currentVer = QString::fromStdString(
                    beamui::getCurrentLibVersion().to_string() + "." + std::to_string(beamui::getCurrentUIRevision()));
                QString message("Your current version is v");
                message.append(currentVer);
                message.append(". Please update to get the most of your Beam wallet.");
                return message;
            }
            else
            {
                LOG_ERROR() << "Software update notification deserialization error";
                return QString();
            }
        }
        case Notification::Type::AddressStatusChanged:
        {
            QString address = getAddress(m_notification);
            //% "<b>%1</b> address expired."
            return qtTrId("notification-address-expired-message").arg(address);
        }
        case Notification::Type::TransactionCompleted:
        {
            auto p = getTxParameters(m_notification);

            switch (getTxType(p))
            {
            case TxType::Simple:
            {
                WalletID wid;
                getPeerID(p, wid);

                auto aid = getAssetId(p);
                auto unitName = amgr.getUnitName(aid);

                return getTxCompletedMessage(getAmount(p), unitName, std::to_string(wid).c_str(), isSender(p));
            }
            case TxType::PushTransaction:
            {
                bool sender = isSender(p);
                auto aid = getAssetId(p);
                auto unitName = amgr.getUnitName(aid);

                return getTxCompletedMessage(getAmount(p), unitName, getPushTxPeer(p, sender), sender);
            }
            case TxType::AtomicSwap:
            {
                QString message = (isBeamSide(p) ?
                    //% "Offer <b>%1 BEAM ➞ %2 %3</b> with transaction ID <b>%4</b> completed."
                    qtTrId("notification-swap-beam-completed-message")
                    :
                    //% "Offer <b>%1 %3 ➞ %2 BEAM</b> with transaction ID <b>%4</b> completed."
                    qtTrId("notification-swap-completed-message")
                    );
                
                return message.arg(getAmount(p))
                              .arg(getSwapAmount(p))
                              .arg(getSwapCoinName(p))
                              .arg(std::to_string(*p.GetTxID()).c_str());
            }
            case TxType::Contract:
                return getContractMessage(p);
            default:
                return "error";
            }
        }
        case Notification::Type::TransactionFailed:
        {
            auto p = getTxParameters(m_notification);
            switch (getTxType(p))
            {
            case TxType::Simple:
            {
                WalletID wid;
                getPeerID(p, wid);

                auto aid = getAssetId(p);
                auto unitName = amgr.getUnitName(aid);

                return getTxFailedMessage(getAmount(p), unitName, std::to_string(wid).c_str(), isSender(p));
            }
            case TxType::PushTransaction:
            {
                bool sender = isSender(p);
                auto aid = getAssetId(p);
                auto unitName = amgr.getUnitName(aid);

                return getTxFailedMessage(getAmount(p), unitName, getPushTxPeer(p, sender), sender);
            }
            case TxType::AtomicSwap:
            {
                QString message;
                if (isSwapTxExpired(p))
                {
                    message = isBeamSide(p) ?
                        //% "Offer <b>%1 BEAM ➞ %2 %3</b> with transaction ID <b>%4</b> expired."
                        qtTrId("notification-swap-beam-expired-message") :
                        //% "Offer <b>%1 %3 ➞ %2 BEAM</b> with transaction ID <b>%4</b> expired."
                        qtTrId("notification-swap-expired-message");
                }
                else
                {
                    message = isBeamSide(p) ?
                        //% "Offer <b>%1 BEAM ➞ %2 %3</b> with transaction ID <b>%4</b> failed."
                        qtTrId("notification-swap-beam-failed-message") :
                        //% "Offer <b>%1 %3 ➞ %2 BEAM</b> with transaction ID <b>%4</b> failed."
                        qtTrId("notification-swap-failed-message");
                }

                return message.arg(getAmount(p))
                    .arg(getSwapAmount(p))
                    .arg(getSwapCoinName(p))
                    .arg(std::to_string(*p.GetTxID()).c_str());
            }
            case TxType::Contract:
                return getContractMessage(p);
            default:
                return "error";
            }
        }
        case Notification::Type::BeamNews:
            return "BEAM in the press";
        default:
            return "error";
    }
}

QString NotificationItem::type() const
{
    // !TODO: full list of the supported item types is: update expired received sent failed inpress hotnews videos events newsletter community
    
    switch(m_notification.m_type)
    {
        case Notification::Type::SoftwareUpdateAvailable: // TODO(sergey.zavarza): deprecated 
        case Notification::Type::WalletImplUpdateAvailable:
            return "update";
        case Notification::Type::AddressStatusChanged:
        {
            const auto address = getWalletAddressRaw(m_notification);
            return address.isExpired() ? "expired" : "extended";
        }
        case Notification::Type::TransactionCompleted:
        {
            auto p = getTxParameters(m_notification);
            switch (getTxType(p))
            {
            case TxType::Simple:
            case TxType::PushTransaction:
                return (isSender(p) ? "sent" : "received");
            case TxType::AtomicSwap:
                return "swapCompleted";
            case TxType::Contract:
                return "contractCompleted";
            default:
                return "error";
            }
        }
        case Notification::Type::TransactionFailed:
        {
            auto p = getTxParameters(m_notification);
            switch (getTxType(p))
            {
            case TxType::Simple:
                return (isSender(p) ? "failedToSend" : "failedToReceive");
            case TxType::PushTransaction:
                return "failedToSend";
            case TxType::AtomicSwap:
                return isSwapTxExpired(p) ? "swapExpired" : "swapFailed";
            case TxType::Contract:
                return isExpired(p) ? "contractExpired" : "contractFailed";
            default:
                return "error";
            }
        }
        case Notification::Type::BeamNews:
            return "newsletter";
        default:
            return "error";
    }
}

QString NotificationItem::state() const
{
    switch(m_notification.m_state)
    {
        case Notification::State::Unread:
            return "unread";
        case Notification::State::Read:
            return "read";
        case Notification::State::Deleted:
            return "deleted";
        default:
            return "error";
    }
}

QString NotificationItem::getTxID() const
{
    try
    {
        auto p = getTxParameters(m_notification);
        return QString::fromStdString(std::to_string(*p.GetTxID()));
    }
    catch(...)
    { }
    return "";
}

WalletAddress NotificationItem::getWalletAddress() const
{
    return getWalletAddressRaw(m_notification);
}

beam::Asset::ID NotificationItem::assetId() const
{
    auto p = getTxParameters(m_notification);
    return getAssetId(p);
}
