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

#include "push_notification_manager.h"

#include "viewmodel/wallet/tx_object.h"
#include "viewmodel/ui_helpers.h"

PushNotificationManager::PushNotificationManager()
    : m_walletModel(AppModel::getInstance().getWalletModel())
{
    connect(m_walletModel,
            SIGNAL(notificationsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Notification>&)),
            SLOT(onNotificationsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Notification>&)));

    connect(m_walletModel,
            SIGNAL(transactionsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>&)),
            SLOT(onTransactionsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>&)));

    m_walletModel->getAsync()->getNotifications();
}

void PushNotificationManager::onNewSoftwareUpdateAvailable(
        const beam::wallet::WalletImplVerInfo& info, const ECC::uintBig& notificationID, bool showPopup)
{
    auto currentLibVersion = beamui::getCurrentLibVersion();
    auto currentUIRevision = beamui::getCurrentUIRevision();
    if (currentLibVersion < info.m_version ||
        (currentLibVersion == info.m_version && currentUIRevision < info.m_UIrevision))
    {
        m_hasNewerVersion = true;
        if (showPopup)
        {
            QString newVersion = QString::fromStdString(
                std::to_string(info.m_version) + "." + std::to_string(info.m_UIrevision));
            QString currentVersion = QString::fromStdString(
                std::to_string(currentLibVersion) + "." + std::to_string(currentUIRevision));
            QVariant id = QVariant::fromValue(notificationID);
        
            emit showUpdateNotification(newVersion, currentVersion, id);
        }
    }
}

void PushNotificationManager::onNotificationsChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::Notification>& notifications)
{
    using namespace beam::wallet;

    if ((m_firstNotification && action == ChangeAction::Reset)
        || action == ChangeAction::Added)
    {
        for (const auto& n : notifications)
        {
            if (n.m_type == Notification::Type::WalletImplUpdateAvailable)
            {
                WalletImplVerInfo info;
                if (fromByteBuffer(n.m_content, info) &&
                    info.m_application == VersionInfo::Application::DesktopWallet)
                {
                    onNewSoftwareUpdateAvailable(info, n.m_ID, n.m_state == Notification::State::Unread);
                }
            }
        }
        m_firstNotification = false;
    }
}

void PushNotificationManager::onTransactionsChanged(
    beam::wallet::ChangeAction action, const std::vector<beam::wallet::TxDescription>& transactions)
{
    using namespace beam::wallet;

    for (const auto& modifiedTx : transactions)
    {
        if(const auto txType = modifiedTx.GetParameter<TxType>(TxParameterID::TransactionType); *txType != TxType::Contract)
        {
            continue;
        }

        auto modifiedTxObj = TxObject(modifiedTx);
        if (modifiedTxObj.canShowContractNotification())
        {
            const auto txId = modifiedTxObj.getTxID();
            if (m_contractNotifications.find(txId) == m_contractNotifications.end())
            {
                m_contractNotifications.insert(txId);
                const auto txIdStr = std::to_string(txId);
                // TODO add mechanism to get app icon
                emit showContractNotification(
                    txIdStr.c_str(), modifiedTxObj.getSource(), modifiedTxObj.getComment(), "");
            }
        }
        else
        {
            if (m_contractNotifications.empty()) continue;
            m_contractNotifications.erase(modifiedTxObj.getTxID());
        }
    }
}

void PushNotificationManager::onCancelPopup(const QVariant& variantID)
{
    auto id = variantID.value<ECC::uintBig>();
    m_walletModel->getAsync()->markNotificationAsRead(id);
}

void PushNotificationManager::closeContractNotification(const QString& txIdStr)
{
    if (m_contractNotifications.empty()) return;

    beam::wallet::TxID txId;
    auto txIdVec = beam::from_hex(txIdStr.toStdString());
    std::copy_n(txIdVec.begin(), 16, txId.begin());
    m_walletModel->getAsync()->markAppNotificationAsRead(txId);
    m_contractNotifications.erase(txId);
}

bool PushNotificationManager::hasNewerVersion() const
{
    return m_hasNewerVersion;
}