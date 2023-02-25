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

    connect(_walletModel.get(),
            SIGNAL(chatRemoved(const beam::wallet::WalletID&)),
            SLOT(onChatRemoved(const beam::wallet::WalletID&)));

    _walletModel->getAsync()->getAddresses(true);
}

QString MessengerChat::getAddr() const
{
    return QString::fromStdString(_peerAddr);
}
void MessengerChat::setAddr(const QString& addr)
{
    if (addr.isEmpty())
        return;

    _peerAddr = addr.toStdString();

    auto p = beam::wallet::ParseParameters(_peerAddr);

    if (!p)
        return;

    auto peerAddr = p->GetParameter<beam::wallet::WalletID>(beam::wallet::TxParameterID::PeerAddr);
    if (peerAddr)
    {
        _peerID = *peerAddr;
    }

    emit peerIDChanged();
    _walletModel->getAsync()->getInstantMessages(_peerID);
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

QString MessengerChat::getMyAddress() const
{
    return QString::fromStdString(_myAddr);
}

void MessengerChat::setMyAddress(const QString& myAddr)
{
    if (myAddr.isEmpty())
        return;

    _myAddr = myAddr.toStdString();

    auto p = beam::wallet::ParseParameters(_myAddr);

    if (!p)
        return;

    auto myID = p->GetParameter<beam::wallet::WalletID>(beam::wallet::TxParameterID::PeerAddr);
    if (myID)
    {
        _myID = *myID;
        emit canSendChanged();
    }
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

void MessengerChat::removeChat()
{
    _walletModel->getAsync()->removeChat(_peerID);
}

void MessengerChat::onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addresses)
{
    if (own)
    {
        _myIds = addresses;
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

        std::vector<std::pair<beam::Timestamp, beam::wallet::WalletID>> unreadMessages;
        unreadMessages.emplace_back(std::make_pair(time, counterpart));
        _walletModel->getAsync()->markIMsasRead(std::move(unreadMessages));

        emit messagesChanged();
    }
}

void MessengerChat::onMessages(const std::vector<beam::wallet::InstantMessage>& messages)
{
    if (messages.empty())
        return;

    std::vector<ChatMessage> modifiedItems;
    modifiedItems.reserve(messages.size());

    std::vector<std::pair<beam::Timestamp, beam::wallet::WalletID>> unreadMessages;
    for (const auto& item : messages)
    {
        if (item.m_counterpart == _peerID)
        {
            modifiedItems.emplace_back(
                ChatMessage{item.m_timestamp, item.m_counterpart, item.m_message, item.m_is_income});
            if (!item.m_is_read)
            {
                unreadMessages.emplace_back(std::make_pair(item.m_timestamp, item.m_counterpart));
            }
        }
    }
    _messeges.reset(modifiedItems);
    emit messagesChanged();

    if (!unreadMessages.empty())
    {
        _walletModel->getAsync()->markIMsasRead(std::move(unreadMessages));
    }

    if (_myID.IsValid())
        return;

    auto lastMessageIt = messages.end() - 1;
    for (const auto& myAddr : _myIds)
    {
        if (lastMessageIt->m_mySbbs == myAddr.m_BbsAddr)
        {
            _myID = myAddr.m_BbsAddr;
            emit canSendChanged();
            return;
        }
    }
}

void MessengerChat::onChatRemoved(const beam::wallet::WalletID& counterpart)
{
    if (counterpart == _peerID)
    {
        emit endChat();
    }
}
