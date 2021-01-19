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
    // in progress
    else if (status == "in progress max privacy")
    {
        /*% "in progress
max privacy" */
        return qtTrId("wallet-txs-status-in-progress-max");
    }
    else if (status == "in progress offline")
    {
        /*% "in progress
offline" */
        return qtTrId("wallet-txs-status-in-progress-max-offline");
    }
    else if (status == "in progress public offline")
    {
        /*% "in progress
public offline" */
        return qtTrId("wallet-txs-status-in-progress-public-offline");
    }
    // sent
    else if (status == "sent max privacy")
    {
        /*% "sent
max privacy"*/
        return qtTrId("wallet-txs-status-sent-max");
    }
    else if (status == "sent offline")
    {
        /*% "sent
offline" */
        return qtTrId("wallet-txs-status-sent-max-offline");
    }
    else if (status == "sent public offline")
    {
        /*% "sent
public offline" */
        return qtTrId("wallet-txs-status-sent-public-offline");
    }
    // received
    else if (status == "received max privacy")
    {
        /*% "received
max privacy" */
        return qtTrId("wallet-txs-status-received-max");
    }
    else if (status == "received offline")
    {
        /*% "received
offline" */
        return qtTrId("wallet-txs-status-received-max-offline");
    }
    else if (status == "received public offline")
    {
        /*% "received
public offline" */
        return qtTrId("wallet-txs-status-received-public-offline");
    }
    // canceled
    else if (status == "canceled max privacy")
    {
        /*% "canceled
max privacy" */
        return qtTrId("wallet-txs-status-canceled-max");
    }
    else if (status == "canceled offline")
    {
        /*% "canceled
offline" */
        return qtTrId("wallet-txs-status-canceled-max-offline");
    }
    else if (status == "canceled public offline")
    {
        /*% "canceled
public offline" */
        return qtTrId("wallet-txs-status-canceled-public-offline");
    }
    // failed
    else if (status == "failed max privacy")
    {
        /*% "failed
max privacy" */
        return qtTrId("wallet-txs-status-failed-max");
    }
    else if (status == "failed offline")
    {
        /*% "failed
offline" */
        return qtTrId("wallet-txs-status-failed-max-offline");
    }
    else if (status == "failed public offline")
    {
        /*% "failed
public offline" */
        return qtTrId("wallet-txs-status-failed-public-offline");
    }
    else if (status == "completed")
    {
        //% "completed"
        return qtTrId("wallet-txs-status-completed");
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
        { static_cast<int>(Roles::IsMaxPrivacy), "isMaxPrivacy"},
        { static_cast<int>(Roles::IsContractTx), "isContractTx"},
        { static_cast<int>(Roles::AssetFilter), "assetFilter"},
        { static_cast<int>(Roles::IsDexTx), "isDexTx"}
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
            return static_cast<qulonglong>(value->timeCreated());

        case Roles::AmountGeneralWithCurrency:
        {
            const auto& alist = value->getAssetsList();
            if (alist.size() == 1)
            {
                auto assetID = *alist.begin();
                return beamui::AmountToUIString(value->getAmountValue(), _amgr.getUnitName(assetID, true));
            }
            else
            {
                assert(value->isContractTx());
                return beamui::AmountToUIString(value->getAmountValue());
            }
        }

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
        case Roles::IsContractTx:
            return value->isContractTx();
        case Roles::IsDexTx:
            return value->isDexTx();
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
        {
            const auto& alist = value->getAssetsList();
            if (alist.size() == 1)
            {
                auto assetID = *alist.begin();
                return _amgr.getUnitName(assetID, false);
            }
            return "";
        }
        case Roles::Icon:
        {
            const auto& alist = value->getAssetsList();
            if (alist.size() == 1)
            {
                auto assetID = *alist.begin();
                return _amgr.getIcon(assetID);
            }
            return _amgr.getIcon(beam::Asset::s_BeamID);
        }
        default:
            return QVariant();
    }
}

void TxObjectList::onAssetInfo(beam::Asset::ID assetId)
{
    for (auto it = m_list.begin(); it != m_list.end(); ++it)
    {
        const auto& alist = (*it)->getAssetsList();
        if(std::find(alist.begin(), alist.end(), assetId) != alist.end())
        {
            const auto idx = it - m_list.begin();
            ListModel::touch(idx);
        }
    }
}
