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
#pragma once
#include <QObject>
#include "model/wallet_model.h"
#include "viewmodel/helpers/list_model.h"
#include "wallet/core/common.h"

struct ChatItem
{
    beam::wallet::WalletID _counterpartID;
    std::string _name;
    bool _haveUnread = true;
};

class MessengerChatListItem : public ListModel<ChatItem>
{
    Q_OBJECT
public:
    MessengerChatListItem();
    enum class Roles
    {
        CID = Qt::UserRole + 1,
        Name,
        HaveUnread
    };

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
};

class MessengerChatList : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* chats READ getChats NOTIFY listChanged)
public:
    MessengerChatList();
    QAbstractItemModel* getChats();
    Q_INVOKABLE void blockSignals();
    Q_INVOKABLE void unblockSignals();

public slots:
    void onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addresses);
    void onChatList(const std::vector<std::pair<beam::wallet::WalletID, bool>>& chats);
    void onMessage(beam::Timestamp time, const beam::wallet::WalletID& counterpart, const std::string& message, bool isIncome);

signals:
    void listChanged();

private:
    WalletModel::Ptr _walletModel;
    std::vector<beam::wallet::WalletAddress> _contacts;
    MessengerChatListItem _chats;
    bool _signalBlocker = false;
};
