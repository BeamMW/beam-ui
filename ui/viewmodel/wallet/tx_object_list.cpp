// Copyright 2019 The Beam Team
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

#include "tx_object_list.h"

namespace
{
    using namespace beam::wallet;

    QString selectShilededStatus(TxAddressType addressType
        , const QString& maxPrivacy
        , const QString& offline
        , const QString& publicOffline
        , const QString& fallback)
    {
        switch (addressType)
        {
        case TxAddressType::MaxPrivacy:
            return maxPrivacy;
        case TxAddressType::Offline:
            return offline;
        case TxAddressType::PublicOffline:
            return publicOffline;
        default:
            return fallback;
        }
    }

QString getStatusTextTranslated(const QString& status, TxAddressType addressType)
{
    if (status == "pending")
    {
        //% "pending"
        return qtTrId("wallet-txs-status-pending");
    }
    else if (status == "waiting for sender")
    {
        //% "waiting for sender"
        return qtTrId("wallet-txs-status-waiting-sender");
    }
    else if (status == "waiting for receiver")
    {
        //% "waiting for receiver"
        return qtTrId("wallet-txs-status-waiting-receiver");
    }
    else if (status == "in progress")
    {
        //% "in progress"
        return qtTrId("wallet-txs-status-in-progress");
    }
    else if (status == "sent to own address")
    {
        //% "sent to own address"
        return qtTrId("wallet-txs-status-own-sent");
    }
    else if (status == "sending to own address")
    {
        //% "sending to own address"
        return qtTrId("wallet-txs-status-own-sending");
    }
    else if (status == "received")
    {
        //% "received"
        return qtTrId("wallet-txs-status-received");
    }
    else if (status == "sent")
    {
        //% "sent"
        return qtTrId("wallet-txs-status-sent");
    }
    else if (status == "cancelled")
    {
        //% "cancelled"
        return qtTrId("wallet-txs-status-cancelled");
    }
    else if (status == "expired")
    {
        //% "expired"
        return qtTrId("wallet-txs-status-expired");
    }
    else if (status == "failed")
    {
        //% "failed"
        return qtTrId("wallet-txs-status-failed");
    }
    else if (status == "in progress max privacy")
    {
        return selectShilededStatus(addressType,
        /*% "in progress
max privacy" */
            qtTrId("wallet-txs-status-in-progress-max"),
        /*% "in progress
offline" */
            qtTrId("wallet-txs-status-in-progress-max-offline"),
            /*% "in progress
public offline" */
            qtTrId("wallet-txs-status-in-progress-public-offline"),
            qtTrId("wallet-txs-status-in-progress")
        );
    }
    else if (status == "sent max privacy")
    {
        return selectShilededStatus(addressType,
        /*% "sent
max privacy"*/
            qtTrId("wallet-txs-status-sent-max"),
        /*% "sent
offline"*/
            qtTrId("wallet-txs-status-sent-max-offline"),
            /*% "sent
public offline" */
            qtTrId("wallet-txs-status-sent-public-offline"),
            qtTrId("wallet-txs-status-sent")
        );
    }
    else if (status == "received max privacy")
    {
        return selectShilededStatus(addressType,
        /*% "received
max privacy" */
            qtTrId("wallet-txs-status-received-max"),
        /*% "received
offline" */
            qtTrId("wallet-txs-status-received-max-offline"),
            /*% "received
public offline" */
            qtTrId("wallet-txs-status-received-public-offline"),
            qtTrId("wallet-txs-status-received")
        );
    }
    else if (status == "canceled max privacy")
    {
        return selectShilededStatus(addressType,
        /*% "canceled
max privacy" */
            qtTrId("wallet-txs-canceled-max"),
        /*% "canceled
offline" */
            qtTrId("wallet-txs-canceled-max-offline"),
        /*% "canceled
public offline" */
            qtTrId("wallet-txs-status-canceled-public-offline"),
            qtTrId("wallet-txs-status-cancelled")
        );
    }
    else if (status == "failed max privacy")
    {
        return selectShilededStatus(addressType,
            /*% "failed
max privacy" */
            qtTrId("wallet-txs-failed-max"),
        /*% "failed
offline" */
            qtTrId("wallet-txs-failed-max-offline"),
        /*% "failed
public offline" */
            qtTrId("wallet-txs-status-failed-public-offline"),
            qtTrId("wallet-txs-status-failed")
        );
    }
    else
    {
        //% "unknown"
        return qtTrId("wallet-txs-status-unknown");
    }
}

}  // namespace

TxObjectList::TxObjectList()
{
    connect(&_amgr, &AssetsManager::assetInfo, this, &TxObjectList::onAssetInfo);
}

QHash<int, QByteArray> TxObjectList::roleNames() const
{
    static const auto roles = QHash<int, QByteArray>
    {
        { static_cast<int>(Roles::TimeCreated), "timeCreated" },
        { static_cast<int>(Roles::TimeCreatedSort), "timeCreatedSort" },
        { static_cast<int>(Roles::AmountGeneralWithCurrency), "amountGeneralWithCurrency" },
        { static_cast<int>(Roles::AmountGeneralWithCurrencySort), "amountGeneralWithCurrencySort" },
        { static_cast<int>(Roles::AmountGeneral), "amountGeneral" },
        { static_cast<int>(Roles::AmountGeneralSort), "amountGeneralSort" },
        { static_cast<int>(Roles::Rate), "rate" },
        { static_cast<int>(Roles::AddressFrom), "addressFrom" },
        { static_cast<int>(Roles::AddressFromSort), "addressFromSort" },
        { static_cast<int>(Roles::AddressTo), "addressTo" },
        { static_cast<int>(Roles::AddressToSort), "addressToSort" },
        { static_cast<int>(Roles::Status), "status" },
        { static_cast<int>(Roles::StatusSort), "statusSort" },
        { static_cast<int>(Roles::Fee), "fee" },
        { static_cast<int>(Roles::Comment), "comment" },
        { static_cast<int>(Roles::TxID), "txID" },
        { static_cast<int>(Roles::KernelID), "kernelID" },
        { static_cast<int>(Roles::FailureReason), "failureReason" },
        { static_cast<int>(Roles::IsCancelAvailable), "isCancelAvailable" },
        { static_cast<int>(Roles::IsDeleteAvailable), "isDeleteAvailable" },
        { static_cast<int>(Roles::IsSelfTransaction), "isSelfTransaction" },
        { static_cast<int>(Roles::IsIncome), "isIncome" },
        { static_cast<int>(Roles::IsInProgress), "isInProgress" },
        { static_cast<int>(Roles::IsPending), "isPending" },
        { static_cast<int>(Roles::IsCompleted), "isCompleted" },
        { static_cast<int>(Roles::IsCanceled), "isCanceled" },
        { static_cast<int>(Roles::IsFailed), "isFailed" },
        { static_cast<int>(Roles::IsExpired), "isExpired" },
        { static_cast<int>(Roles::HasPaymentProof), "hasPaymentProof" },
        { static_cast<int>(Roles::RawTxID), "rawTxID" },
        { static_cast<int>(Roles::Search), "search" },
        { static_cast<int>(Roles::StateDetails), "stateDetails" },
        { static_cast<int>(Roles::Token), "token" },
        { static_cast<int>(Roles::SenderIdentity), "senderIdentity"},
        { static_cast<int>(Roles::ReceiverIdentity), "receiverIdentity"},
        { static_cast<int>(Roles::IsShieldedTx), "isShieldedTx"},
        { static_cast<int>(Roles::IsOfflineToken), "isOfflineToken"},
        { static_cast<int>(Roles::UnitName), "unitName"},
        { static_cast<int>(Roles::Icon), "icon"},
        { static_cast<int>(Roles::IsSent), "isSent"},
        { static_cast<int>(Roles::IsReceived), "isReceived"},
        { static_cast<int>(Roles::IsPublicOffline), "isPublicOffline"},
        { static_cast<int>(Roles::IsMaxPrivacy), "isMaxPrivacy"}
    };
    return roles;
}

QVariant TxObjectList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size())
    {
       return QVariant();
    }
    
    auto& value = m_list[index.row()];
    switch (static_cast<Roles>(role))
    {
        case Roles::TimeCreated:
        {
            QDateTime datetime;
            datetime.setTime_t(value->timeCreated());
            return datetime.toString(m_locale.dateTimeFormat(QLocale::ShortFormat));
        }
            
        case Roles::TimeCreatedSort:
        {
            return static_cast<qulonglong>(value->timeCreated());
        }

        case Roles::AmountGeneralWithCurrency:
            return beamui::AmountToUIString(value->getAmountValue(), _amgr.getUnitName(value->getAssetId()));
        case Roles::AmountGeneralWithCurrencySort:
            return static_cast<qulonglong>(value->getAmountValue());
        case Roles::AmountGeneral:
            return value->getAmount();
        case Roles::AmountGeneralSort:
            return static_cast<qulonglong>(value->getAmountValue());
        case Roles::Rate:
            return value->getRate();
        case Roles::AddressFrom:
        case Roles::AddressFromSort:
            return value->getAddressFrom();
        case Roles::AddressTo:
        case Roles::AddressToSort:
            return value->getAddressTo();
        case Roles::Status:
        case Roles::StatusSort:
            return getStatusTextTranslated(value->getStatus(), value->getAddressType());
        case Roles::Fee:
            return value->getFee();
        case Roles::Comment:
            return value->getComment();
        case Roles::TxID:
            return value->getTransactionID();
        case Roles::KernelID:
            return value->getKernelID();
        case Roles::FailureReason:
            return value->getFailureReason();
        case Roles::IsCancelAvailable:
            return value->isCancelAvailable();
        case Roles::IsDeleteAvailable:
            return value->isDeleteAvailable();
        case Roles::IsSelfTransaction:
            return value->isSelfTx();
        case Roles::IsShieldedTx:
            return value->isShieldedTx();
        case Roles::IsOfflineToken:
            return value->getAddressType() == beam::wallet::TxAddressType::Offline;
        case Roles::IsPublicOffline:
            return value->getAddressType() == beam::wallet::TxAddressType::PublicOffline;
        case Roles::IsMaxPrivacy:
            return value->getAddressType() == beam::wallet::TxAddressType::MaxPrivacy;
        case Roles::IsIncome:
            return value->isIncome();
        case Roles::IsInProgress:
            return value->isInProgress();
        case Roles::IsPending:
            return value->isPending();
        case Roles::IsCompleted:
            return value->isCompleted();
        case Roles::IsSent:
            return value->isSent();
        case Roles::IsReceived:
            return value->isReceived();
        case Roles::IsCanceled:
            return value->isCanceled();
        case Roles::IsFailed:
            return value->isFailed();
        case Roles::IsExpired:
            return value->isExpired();
        case Roles::HasPaymentProof:
            return value->hasPaymentProof();
        case Roles::RawTxID:
            return QVariant::fromValue(value->getTxID());
        case Roles::Search: 
        {
            QString r = value->getTransactionID();
            r.append(" ");
            r.append(value->getKernelID());
            r.append(" ");
            r.append(value->getAddressFrom());
            r.append(" ");
            r.append(value->getAddressTo());
            r.append(" ");
            r.append(value->getComment());
            r.append(" ");
            r.append(value->getSenderIdentity());
            r.append(" ");
            r.append(value->getReceiverIdentity());
            r.append(" ");
            r.append(value->getToken());
            return r;
        }
        case Roles::StateDetails:
            return value->getStateDetails();
        case Roles::Token:
            return value->getToken();
        case Roles::SenderIdentity:
            return value->getSenderIdentity();
        case Roles::ReceiverIdentity:
            return value->getReceiverIdentity();
        case Roles::UnitName:
            return _amgr.getUnitName(value->getAssetId());
        case Roles::Icon:
            return _amgr.getIcon(value->getAssetId());
        default:
            return QVariant();
    }
}

void TxObjectList::onAssetInfo(beam::Asset::ID assetId)
{
    for (auto it = m_list.begin(); it != m_list.end(); ++it) {
        if ((*it)->getAssetId() == assetId) {
           const auto idx = it - m_list.begin();
           ListModel::touch(idx);
        }
    }
}
