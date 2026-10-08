// Copyright 2022 The Beam Team
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

#include "messenger_chat_list.h"

#include "model/app_model.h"

MessengerChatListItem::MessengerChatListItem() {}

QVariant MessengerChatListItem::data(const QModelIndex &index, int role) const
{
if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size())
    {
       return QVariant();
    }

    auto& value = m_list[index.row()];
    switch (static_cast<Roles>(role))
    {
        case Roles::CID:
            return QString::fromStdString(std::to_string(value._counterpartID));
        case Roles::Name:
            return QString::fromStdString(value._name);
        case Roles::HaveUnread:
            return value._haveUnread;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> MessengerChatListItem::roleNames() const
{
    static const auto roles = QHash<int, QByteArray>
    {
        { static_cast<int>(Roles::CID), "cid" },
        { static_cast<int>(Roles::Name), "name" },
        { static_cast<int>(Roles::HaveUnread), "haveUnread" },
    };
    return roles;
}

MessengerChatList::MessengerChatList() :
    _walletModel(AppModel::getInstance().getWalletModel())
{
    const auto ignored = AppModel::getInstance().getSettings().getMessengerIgnoredAddresses();
    _ignoredAddresses = QSet<QString>(ignored.begin(), ignored.end());

    connect(_walletModel,
            SIGNAL(addressesChanged(bool, const std::vector<beam::wallet::WalletAddress>&)),
            SLOT(onAddresses(bool, const std::vector<beam::wallet::WalletAddress>&)));
    connect(_walletModel,
            SIGNAL(chatList(const std::vector<beam::wallet::ChatInfo>&)),
            SLOT(onChatList(const std::vector<beam::wallet::ChatInfo>&)));

    connect(_walletModel,
            SIGNAL(instantMessage(beam::Timestamp, const beam::wallet::WalletID&, const std::string&, bool)),
            SLOT(onMessage(beam::Timestamp, const beam::wallet::WalletID&, const std::string&, bool)));

    _walletModel->getAsync()->getAddresses(true);
    _walletModel->getAsync()->getAddresses(false);
}

QAbstractItemModel* MessengerChatList::getChats()
{
    return &_chats;
}

QVariantList MessengerChatList::getOwnAddresses() const
{
    QVariantList result;
    for (const auto& addr : _ownAddresses)
    {
        auto id = QString::fromStdString(std::to_string(addr.m_BbsAddr));
        const bool ignored = _ignoredAddresses.contains(id);
        if (addr.isExpired() && !ignored)
            continue;

        QVariantMap item;
        item["address"] = id;
        item["label"] = QString::fromStdString(addr.m_label);
        item["ignored"] = ignored;
        result.push_back(item);
    }
    return result;
}

void MessengerChatList::setAddressIgnored(const QString& address, bool ignored)
{
    if (ignored == _ignoredAddresses.contains(address))
        return;

    if (ignored)
        _ignoredAddresses.insert(address);
    else
        _ignoredAddresses.remove(address);

    AppModel::getInstance().getSettings().setMessengerIgnoredAddresses(QStringList(_ignoredAddresses.begin(), _ignoredAddresses.end()));
    emit ownAddressesChanged();
    updateChats();
}

void MessengerChatList::blockSignals()
{
    _signalBlocker = true;
}

void MessengerChatList::unblockSignals()
{
    _signalBlocker = false;
    _walletModel->getAsync()->getAddresses(true);
    _walletModel->getAsync()->getAddresses(false);
}

void MessengerChatList::onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addresses)
{
    if (_signalBlocker) return;

    if (own)
    {
        _ownAddresses = addresses;
        emit ownAddressesChanged();
        return;
    }

    _contacts = addresses;
    _walletModel->getAsync()->getChats();
}

void MessengerChatList::onChatList(const std::vector<beam::wallet::ChatInfo>& chats)
{
    if (_signalBlocker) return;

    _allChats = chats;
    updateChats();
}

void MessengerChatList::updateChats()
{
    std::vector<ChatItem> modifiedItems;
    modifiedItems.reserve(_allChats.size());

    for (const auto& chat : _allChats)
    {
        const bool isIgnored = !chat.m_myAddresses.empty() &&
            std::all_of(chat.m_myAddresses.begin(), chat.m_myAddresses.end(),
                        [this] (const auto& myAddr) { return _ignoredAddresses.contains(QString::fromStdString(std::to_string(myAddr))); });
        if (isIgnored)
            continue;

        std::string name = "Anonymous";
        auto it = std::find_if(_contacts.begin(), _contacts.end(),
                               [&chat] (const auto& addr) { return addr.m_BbsAddr == chat.m_counterpart;});
        if (it != _contacts.end())
            name = it->m_label;
        modifiedItems.emplace_back(ChatItem{chat.m_counterpart, name, chat.m_hasUnread});
    }
    _chats.reset(modifiedItems);
    emit listChanged();
}

void MessengerChatList::onMessage(beam::Timestamp time, const beam::wallet::WalletID& counterpart,
                                  const std::string& message, bool isIncome)
{
    if (_signalBlocker) return;

    auto it = std::find_if(_chats.begin(), _chats.end(),
                           [&counterpart] (const auto& item) { return counterpart == item._counterpartID;});
    if (it == _chats.end())
    {
        _walletModel->getAsync()->getChats();
    }
}
