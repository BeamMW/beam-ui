// Copyright 2018 The Beam Team
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

#include "address_book_view.h"
#include "ui_helpers.h"
#include <QApplication>
#include <QClipboard>
#include "model/app_model.h"
#include "model/qr.h"

using namespace std;
using namespace beam;
using namespace beamui;

namespace
{
    template<typename T>
    bool compare(const T& lf, const T& rt, Qt::SortOrder sortOrder)
    {
        if (sortOrder == Qt::DescendingOrder)
            return lf > rt;
        return lf < rt;
    }
}

AddressItem::AddressItem(beam::wallet::WalletAddress address)
    : m_walletAddress(std::move(address))
{

}

QString AddressItem::getToken() const
{
    return QString::fromStdString(m_walletAddress.m_Address);
}

QString AddressItem::getWalletID() const
{
    return QString::fromStdString(std::to_string(m_walletAddress.m_walletID));
}

QString AddressItem::getName() const
{
    return QString::fromStdString(m_walletAddress.m_label);
}

QString AddressItem::getCategory() const
{
    return QString::fromStdString(m_walletAddress.m_category);
}

QString AddressItem::getIdentity() const
{
    return beamui::toString(m_walletAddress.m_Identity);
}

QDateTime AddressItem::getExpirationDate() const
{
    QDateTime datetime;
    datetime.setTime_t(m_walletAddress.getExpirationTime());
    
    return datetime;
}

QDateTime AddressItem::getCreateDate() const
{
    QDateTime datetime;
    datetime.setTime_t(m_walletAddress.getCreateTime());
    
    return datetime;
}

bool AddressItem::isNeverExpired() const
{
    return (m_walletAddress.m_duration == 0);
}

bool AddressItem::isExpired() const
{
    return m_walletAddress.isExpired();
}

beam::Timestamp AddressItem::getCreateTimestamp() const
{
    return m_walletAddress.getCreateTime();
}

beam::Timestamp AddressItem::getExpirationTimestamp() const
{
    return m_walletAddress.getExpirationTime();
}

ContactItem::ContactItem(const beam::wallet::WalletAddress& address)
    : m_walletAddress(address)
{

}

QString ContactItem::getWalletID() const
{
    return QString::fromStdString(std::to_string(m_walletAddress.m_walletID));
}

QString ContactItem::getName() const
{
    return QString::fromStdString(m_walletAddress.m_label);
}

QString ContactItem::getCategory() const
{
    return QString::fromStdString(m_walletAddress.m_category);
}

QString ContactItem::getIdentity() const
{
    if (m_walletAddress.m_Identity != Zero)
    {
        return beamui::toString(m_walletAddress.m_Identity);
    }
    return QString();
}

QString ContactItem::getToken() const
{
    return QString::fromStdString(m_walletAddress.m_Address);
}

AddressBookViewModel::AddressBookViewModel()
    : m_model{*AppModel::getInstance().getWalletModel()}
{
    connect(&m_model,
            SIGNAL(addressesChanged(bool, const std::vector<beam::wallet::WalletAddress>&)),
            SLOT(onAddresses(bool, const std::vector<beam::wallet::WalletAddress>&)));
    connect(&m_model,
            SIGNAL(transactionsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>&)),
            SLOT(onTransactions(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>&)));
    connect(&m_model,
            SIGNAL(addressesChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::WalletAddress>&)),
            SLOT(onAddressesChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::WalletAddress>&)));

    getAddressesFromModel();
    m_model.getAsync()->getTransactions();
    startTimer(3 * 1000);
}

AddressBookViewModel::~AddressBookViewModel()
{
    qDeleteAll(m_contacts);
    qDeleteAll(m_activeAddresses);
    qDeleteAll(m_expiredAddresses);
}

QQmlListProperty<ContactItem> AddressBookViewModel::getContacts()
{
    return CreateQmlListProperty<ContactItem>(this, m_contacts);
}

QQmlListProperty<AddressItem> AddressBookViewModel::getActiveAddresses()
{
    return CreateQmlListProperty<AddressItem>(this, m_activeAddresses);
}

QQmlListProperty<AddressItem> AddressBookViewModel::getExpiredAddresses()
{
    return CreateQmlListProperty<AddressItem>(this, m_expiredAddresses);
}

QString AddressBookViewModel::nameRole() const
{
    return "name";
}

QString AddressBookViewModel::walletIDRole() const
{
    return "walletID";
}

QString AddressBookViewModel::categoryRole() const
{
    return "category";
}

QString AddressBookViewModel::identityRole() const
{
    return "identity";
}

QString AddressBookViewModel::expirationRole() const
{
    return "expirationDate";
}

QString AddressBookViewModel::createdRole() const
{
    return "createDate";
}

QString AddressBookViewModel::tokenRole() const
{
    return "token";
}

Qt::SortOrder AddressBookViewModel::activeAddrSortOrder() const
{
    return m_activeAddrSortOrder;
}

Qt::SortOrder AddressBookViewModel::expiredAddrSortOrder() const
{
    return m_expiredAddrSortOrder;
}

Qt::SortOrder AddressBookViewModel::contactSortOrder() const
{
    return m_contactSortOrder;
}

void AddressBookViewModel::setActiveAddrSortOrder(Qt::SortOrder value)
{
    if (m_activeAddrSortOrder != value)
    {
        m_activeAddrSortOrder = value;
        sortActiveAddresses();
    }
}

void AddressBookViewModel::setExpiredAddrSortOrder(Qt::SortOrder value)
{
    if (m_expiredAddrSortOrder != value)
    {
        m_expiredAddrSortOrder = value;
        sortExpiredAddresses();
    }
}

void AddressBookViewModel::setContactSortOrder(Qt::SortOrder value)
{
    if (m_contactSortOrder != value)
    {
        m_contactSortOrder = value;
        sortContacts();
    }
}

QString AddressBookViewModel::activeAddrSortRole() const
{
    return m_activeAddrSortRole;
}

QString AddressBookViewModel::expiredAddrSortRole() const
{
    return m_expiredAddrSortRole;
}

QString AddressBookViewModel::contactSortRole() const
{
    return m_contactSortRole;
}

void AddressBookViewModel::setActiveAddrSortRole(QString value)
{
    if (m_activeAddrSortRole != value)
    {
        m_activeAddrSortRole = value;
        sortActiveAddresses();
    }
}

void AddressBookViewModel::setExpiredAddrSortRole(QString value)
{
    if (m_expiredAddrSortRole != value)
    {
        m_expiredAddrSortRole = value;
        sortExpiredAddresses();
    }
}

void AddressBookViewModel::setContactSortRole(QString value)
{
    if (m_contactSortRole != value)
    {
        m_contactSortRole = value;
        sortContacts();
    }
}

bool AddressBookViewModel::isWIDBusy(const QString& wid)
{
    beam::wallet::WalletID walletID;
    walletID.FromHex(wid.toStdString());
    return find(m_busyAddresses.cbegin(), m_busyAddresses.cend(), walletID) != m_busyAddresses.cend();
}

void AddressBookViewModel::deleteAddress(const QString& token)
{
    m_model.getAsync()->deleteAddressByToken(token.toStdString());
}

void AddressBookViewModel::saveChanges(const QString& wid, const QString& name, QDateTime expiration)
{
    beam::Timestamp expirationStamp = expiration.toSecsSinceEpoch();
    beam::wallet::WalletID walletID;
    walletID.FromHex(wid.toStdString());
    m_model.getAsync()->updateAddress(walletID, name.toStdString(), expirationStamp);
}

// static
QString AddressBookViewModel::generateQR(
        const QString& addr, uint width, uint height)
{
    QR qr(addr, width, height);
    return qr.getEncoded();
}

// static
bool AddressBookViewModel::commentValid(const QString& comment) const
{
    return !m_model.isAddressWithCommentExist(comment.toStdString());
}

void AddressBookViewModel::onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addresses)
{
    if (own)
    {
        qDeleteAll(m_activeAddresses); m_activeAddresses.clear();
        qDeleteAll(m_expiredAddresses); m_expiredAddresses.clear();

        for (const auto& addr : addresses)
        {
            assert(!addr.m_Address.empty());
            if (addr.isExpired())
            {
                m_expiredAddresses.push_back(new AddressItem(addr));
            }
            else
            {
                m_activeAddresses.push_back(new AddressItem(addr));
            }
        }

        sortActiveAddresses();
        sortExpiredAddresses();
    }
    else
    {
        qDeleteAll(m_contacts); m_contacts.clear();

        for (const auto& addr : addresses)
        {
            m_contacts.push_back(new ContactItem(addr));
        }

        sortContacts();
    }
}

void AddressBookViewModel::onAddressesChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::WalletAddress>& addresses)
{
    // TODO: refactor this
    getAddressesFromModel();
}

void AddressBookViewModel::onTransactions(beam::wallet::ChangeAction action, const std::vector<beam::wallet::TxDescription>& transactions)
{
    using namespace beam::wallet;

    switch (action)
    {
        case ChangeAction::Reset:
            {
                m_busyAddresses.clear();
                // no beak!
            }

        case ChangeAction::Added:
            {
                for (const auto& tx : transactions)
                {
                    if (!tx.canDelete())    // only active transactions
                    {
                        m_busyAddresses.push_back(tx.m_myId);
                    }
                }
                break;
            }
        
        case ChangeAction::Updated:
            {
                for (const auto& tx : transactions)
                {
                    auto it = find(m_busyAddresses.cbegin(), m_busyAddresses.cend(), tx.m_myId);
                    if (it != m_busyAddresses.cend() && tx.canDelete())
                    {
                        m_busyAddresses.erase(it);
                    }
                }
                break;
            }

        case ChangeAction::Removed:
            {
                for (const auto& tx : transactions)
                {
                    auto it = find(m_busyAddresses.cbegin(), m_busyAddresses.cend(), tx.m_myId);
                    if (it != m_busyAddresses.cend())
                    {
                        m_busyAddresses.erase(it);
                    }
                }
                break;
            }

        default:
            assert(false && "Unexpected action");
            break;
    }
}

void AddressBookViewModel::timerEvent(QTimerEvent *event)
{
    auto firstExpired = std::remove_if(
        m_activeAddresses.begin(), m_activeAddresses.end(), 
        [](const AddressItem* addr) { return addr->isExpired(); });

    if (firstExpired != m_activeAddresses.end())
    {
        for (auto it = firstExpired; it != m_activeAddresses.end(); ++it)
        {
            m_expiredAddresses.push_back(*it);
        }

        m_activeAddresses.erase(firstExpired, m_activeAddresses.end());

        emit activeAddressesChanged();
        sortExpiredAddresses();
    }
}

void AddressBookViewModel::getAddressesFromModel()
{
    m_model.getAsync()->getAddresses(true);
    m_model.getAsync()->getAddresses(false);
}

void AddressBookViewModel::sortActiveAddresses()
{
    auto cmp = generateAddrComparer(m_activeAddrSortRole, m_activeAddrSortOrder);
    std::sort(m_activeAddresses.begin(), m_activeAddresses.end(), cmp);

    emit activeAddressesChanged();
}

void AddressBookViewModel::sortExpiredAddresses()
{
    auto cmp = generateAddrComparer(m_expiredAddrSortRole, m_expiredAddrSortOrder);
    std::sort(m_expiredAddresses.begin(), m_expiredAddresses.end(), cmp);

    emit expiredAddressesChanged();
}

void AddressBookViewModel::sortContacts()
{
    auto cmp = generateContactComparer();
    std::sort(m_contacts.begin(), m_contacts.end(), cmp);

    emit contactsChanged();
}

std::function<bool(const AddressItem*, const AddressItem*)> AddressBookViewModel::generateAddrComparer(QString role, Qt::SortOrder order)
{
    if (role == nameRole())
        return [sortOrder = order](const AddressItem* lf, const AddressItem* rt)
    {
        return compare(lf->getName(), rt->getName(), sortOrder);
    };

    if (role == tokenRole())
        return [sortOrder = order](const AddressItem* lf, const AddressItem* rt)
    {
        return compare(lf->getToken(), rt->getToken(), sortOrder);
    };

    if (role == walletIDRole())
        return [sortOrder = order](const AddressItem* lf, const AddressItem* rt)
    {
        return compare(lf->getWalletID(), rt->getWalletID(), sortOrder);
    };

    if (role == categoryRole())
        return [sortOrder = order](const AddressItem* lf, const AddressItem* rt)
    {
        return compare(lf->getCategory(), rt->getCategory(), sortOrder);
    };

    if (role == identityRole())
        return [sortOrder = order](const AddressItem* lf, const AddressItem* rt)
    {
        return compare(lf->getIdentity(), rt->getIdentity(), sortOrder);
    };

    if (role == expirationRole())
        return [sortOrder = order](const AddressItem* lf, const AddressItem* rt)
    {
        return compare(lf->getExpirationTimestamp(), rt->getExpirationTimestamp(), sortOrder);
    };

    // default for createdRole
    return [sortOrder = order](const AddressItem* lf, const AddressItem* rt)
    {
        return compare(lf->getCreateTimestamp(), rt->getCreateTimestamp(), sortOrder);
    };
}

std::function<bool(const ContactItem*, const ContactItem*)> AddressBookViewModel::generateContactComparer()
{
    if (m_contactSortRole == walletIDRole())
        return [sortOrder = m_contactSortOrder](const ContactItem* lf, const ContactItem* rt)
    {
        return compare(lf->getWalletID(), rt->getWalletID(), sortOrder);
    };

    if (m_contactSortRole == tokenRole())
        return [sortOrder = m_contactSortOrder](const ContactItem* lf, const ContactItem* rt)
    {
        return compare(lf->getToken(), rt->getToken(), sortOrder);
    };

    if (m_contactSortRole == categoryRole())
        return [sortOrder = m_contactSortOrder](const ContactItem* lf, const ContactItem* rt)
    {
        return compare(lf->getCategory(), rt->getCategory(), sortOrder);
    };

    if (m_contactSortRole == identityRole())
        return [sortOrder = m_contactSortOrder](const ContactItem* lf, const ContactItem* rt)
    {
        return compare(lf->getIdentity(), rt->getIdentity(), sortOrder);
    };

    // default for nameRole
    return [sortOrder = m_contactSortOrder](const ContactItem* lf, const ContactItem* rt)
    {
        return compare(lf->getName(), rt->getName(), sortOrder);
    };
}
