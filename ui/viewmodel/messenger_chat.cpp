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

#include "messenger_chat.h"

#include "model/app_model.h"

MessengesList::MessengesList() {}

QVariant MessengesList::data(const QModelIndex &index, int role) const
{
if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size())
    {
       return QVariant();
    }

    auto& value = m_list[index.row()];
    switch (static_cast<Roles>(role))
    {
        case Roles::Time:
        {
            QDateTime datetime;
            datetime.setTime_t(value.m_timestamp);
            return datetime.time().toString(m_locale.timeFormat(QLocale::ShortFormat));
        }
        case Roles::CID:
            return QString::fromStdString(std::to_string(value.m_counterpart));
        case Roles::Message:
            return QString::fromStdString(value.m_message);
        case Roles::IsIncome:
            return value.m_is_income;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> MessengesList::roleNames() const
{
    static const auto roles = QHash<int, QByteArray>
    {
        { static_cast<int>(Roles::Time), "time" },
        { static_cast<int>(Roles::CID), "cid" },
        { static_cast<int>(Roles::Message), "message" },
        { static_cast<int>(Roles::IsIncome), "is_income" }
    };
    return roles;
}

MessengerChat::MessengerChat() :
    _walletModel(AppModel::getInstance().getWalletModel())
{
    connect(_walletModel.get(),
            SIGNAL(addressesChanged(bool, const std::vector<beam::wallet::WalletAddress>&)),
            SLOT(onAddresses(bool, const std::vector<beam::wallet::WalletAddress>&)));

    connect(_walletModel.get(),
            SIGNAL(instantMessage(beam::Timestamp, const beam::wallet::WalletID&, const std::string&, bool)),
            SLOT(onMessage(beam::Timestamp, const beam::wallet::WalletID&, const std::string&, bool)));

    connect(_walletModel.get(),
            SIGNAL(chatMessages(const std::vector<beam::wallet::InstantMessage>&)),
            SLOT(onMessages(const std::vector<beam::wallet::InstantMessage>&)));

    _walletModel->getAsync()->getAddresses(true);
}

QString MessengerChat::getPeerID() const
{
    return _peerID.IsValid() ? QString::fromStdString(std::to_string(_peerID)) : "";
}

void MessengerChat::setPeerID(const QString& peerID)
{
    _peerID.FromHex(peerID.toStdString());
    emit peerIDChanged();
    _walletModel->getAsync()->getInstantMessages(_peerID);
}

bool MessengerChat::getCanSend() const
{
    return _myID.IsValid() && _peerID.IsValid();
}

QAbstractItemModel* MessengerChat::getMessages()
{
    return &_messeges;
}

void MessengerChat::sendMessage(const QString& text)
{

    if (getCanSend())
    {
        auto textStr = text.toStdString();
        _walletModel->getAsync()->sendInstantMessage(_peerID, _myID, beam::ByteBuffer(textStr.begin(), textStr.end()));
    }
}

void MessengerChat::onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addresses)
{
    if (own)
    {
        _myIds = addresses;

        for (const auto& myAddr : _myIds)
        {
            if (!myAddr.isExpired() && myAddr.m_walletID.IsValid())
            {
                _myID = myAddr.m_walletID;
                break;
            }
        }
        emit peerIDChanged();
    }
}

void MessengerChat::onMessage(beam::Timestamp time, const beam::wallet::WalletID& counterpart,
                              const std::string& message, bool isIncome)
{
    if (counterpart == _peerID)
    {
        std::vector<ChatMessage> modifiedItems;
        modifiedItems.emplace_back(
                ChatMessage{time, counterpart, message, isIncome});

        _messeges.insert(modifiedItems);
        emit messagesChanged();
    }
}

void MessengerChat::onMessages(const std::vector<beam::wallet::InstantMessage>& messages)
{
    std::vector<ChatMessage> modifiedItems;
    modifiedItems.reserve(messages.size());

    for (const auto& item : messages)
    {
        if (item.m_counterpart == _peerID)
        {
            modifiedItems.emplace_back(
                ChatMessage{item.m_timestamp, item.m_counterpart, item.m_message, item.m_is_income});
        }
    }
    _messeges.reset(modifiedItems);
    emit messagesChanged();
}
