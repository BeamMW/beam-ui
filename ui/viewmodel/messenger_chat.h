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
#include <QLocale>
#include "model/wallet_model.h"
#include "viewmodel/helpers/list_model.h"
#include "wallet/core/common.h"

struct ChatMessage
{
    beam::Timestamp m_timestamp;
    beam::wallet::WalletID m_counterpart;
    std::string m_message;
    bool m_is_income;
};

class MessengesList : public ListModel<ChatMessage>
{
    Q_OBJECT
public:
    MessengesList();
    enum class Roles
    {
        Time = Qt::UserRole + 1,
        CID,
        Message,
        IsIncome
    };

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

private:
    QLocale m_locale; // default locale
};

class MessengerChat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString peerID  READ getPeerID  WRITE setPeerID NOTIFY peerIDChanged)
    Q_PROPERTY(bool    canSend READ getCanSend                 NOTIFY peerIDChanged)

    Q_PROPERTY(QAbstractItemModel* messages READ getMessages   NOTIFY messagesChanged)

public:
    MessengerChat();

    QString getPeerID() const;
    void setPeerID(const QString& peerID);
    bool getCanSend() const;
    QAbstractItemModel* getMessages();

    Q_INVOKABLE void sendMessage(const QString& text);
    Q_INVOKABLE void removeChat();

public slots:
    void onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addresses);
    void onMessage(beam::Timestamp time, const beam::wallet::WalletID& counterpart, const std::string& message, bool isIncome);
    void onMessages(const std::vector<beam::wallet::InstantMessage>& messages);
    void onChatRemoved(const beam::wallet::WalletID& counterpart);

signals:
    void peerIDChanged();
    void messagesChanged();
    void endChat();

private:
    WalletModel::Ptr _walletModel;
    std::vector<beam::wallet::WalletAddress> _myIds;

    beam::wallet::WalletID _peerID = beam::Zero;
    beam::wallet::WalletID _myID = beam::Zero;

    MessengesList _messeges;
};
