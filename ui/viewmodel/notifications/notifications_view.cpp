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

#include "notifications_view.h"

#include "utility/logger.h"
#include "wallet/client/extensions/news_channels/interface.h"

NotificationsViewModel::NotificationsViewModel()
    : m_walletModel{*AppModel::getInstance().getWalletModel()}
{
    connect(&m_walletModel,
            SIGNAL(notificationsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Notification>&)),
            SLOT(onNotificationsDataModelChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Notification>&)));

    m_walletModel.getAsync()->getNotifications();
}

QAbstractItemModel* NotificationsViewModel::getNotifications()  
{
    return &m_notificationsList;
}

void NotificationsViewModel::clearAll()
{
    for(auto& n : m_notificationsList)
    {
        m_walletModel.getAsync()->deleteNotification(n->getID());
    }
}

void NotificationsViewModel::removeItem(const ECC::uintBig& id)
{
    m_walletModel.getAsync()->deleteNotification(id);
}

void NotificationsViewModel::markItemAsRead(const ECC::uintBig& id)
{
    m_walletModel.getAsync()->markNotificationAsRead(id);
}

QString NotificationsViewModel::getItemTxID(const ECC::uintBig& id)
{
    for (const auto& n : m_notificationsList)
    {
        if (n->getID() == id)
        {
            return n->getTxID();
        }
    }
    return "";
}

/// Activate wallet address. @id - notification ID.
void NotificationsViewModel::activateAddress(const ECC::uintBig& id)
{
    for (const auto& n : m_notificationsList)
    {
        if (n->getID() == id)
        {
            const auto walletAddress = n->getWalletAddress();
            m_walletModel.getAsync()->activateAddress(walletAddress.m_walletID);
        }
    }
}

void NotificationsViewModel::onNotificationsDataModelChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::Notification>& notifications)
{
    using namespace beam::wallet;

    std::vector<std::shared_ptr<NotificationItem>> modifiedNotifications;
    modifiedNotifications.reserve(notifications.size());

    for (const auto& n : notifications)
    {
        if (action == ChangeAction::Removed || n.m_state != Notification::State::Deleted)
        {
            if (n.m_type == Notification::Type::WalletImplUpdateAvailable)
            {
                WalletImplVerInfo walletVersionInfo;
                if (!fromByteBuffer(n.m_content, walletVersionInfo)) continue; 
                if (walletVersionInfo.m_application != VersionInfo::Application::DesktopWallet) continue;

                auto currentLibVersion = beamui::getCurrentLibVersion();
                if (walletVersionInfo.m_version < currentLibVersion ||
                    (walletVersionInfo.m_version == currentLibVersion &&
                     walletVersionInfo.m_UIrevision <= beamui::getCurrentUIRevision()))
                {
                    continue;
                }
            }

            modifiedNotifications.push_back(std::make_shared<NotificationItem>(n));
        }
    }

    switch (action)
    {
        case ChangeAction::Reset:
            {
                m_notificationsList.reset(modifiedNotifications);
                break;
            }

        case ChangeAction::Added:
            {
                m_notificationsList.insert(modifiedNotifications);
                break;
            }

        case ChangeAction::Removed:
            {
                m_notificationsList.remove(modifiedNotifications);
                break;
            }

        case ChangeAction::Updated:
            {
                m_notificationsList.update(modifiedNotifications);
                break;
            }
        
        default:
            assert(false && "Unexpected action");
            break;
    }
    
    emit allNotificationsChanged();
}
