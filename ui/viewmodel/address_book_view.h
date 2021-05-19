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


#pragma once

#include <QObject>
#include <QtCore/qvariant.h>
#include <QDateTime>
#include <QQmlListProperty>
#include "wallet/core/wallet_db.h"
#include "model/wallet_model.h"

class AddressItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString   walletID         READ getWalletID       CONSTANT)
    Q_PROPERTY(QString   token            READ getToken          CONSTANT)
    Q_PROPERTY(QString   name             READ getName           CONSTANT)
    Q_PROPERTY(QString   category         READ getCategory       CONSTANT)
    Q_PROPERTY(QString   identity         READ getIdentity       CONSTANT)
    Q_PROPERTY(QDateTime expirationDate   READ getExpirationDate CONSTANT)
    Q_PROPERTY(QDateTime createDate       READ getCreateDate     CONSTANT)
    Q_PROPERTY(bool      neverExpired     READ isNeverExpired    CONSTANT)
    Q_PROPERTY(bool      isExpired        READ isExpired         CONSTANT)

public:

    AddressItem() = default;
    AddressItem(beam::wallet::WalletAddress);

    QString getWalletID() const;
    QString getToken() const;
    QString getName() const;
    QString getCategory() const;
    QString getIdentity() const;
    QDateTime getExpirationDate() const;
    QDateTime getCreateDate() const;
    bool isNeverExpired() const;

    bool isExpired() const;
    beam::Timestamp getCreateTimestamp() const;
    beam::Timestamp getExpirationTimestamp() const;

private:
    beam::wallet::WalletAddress m_walletAddress;
};

class ContactItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString walletID      READ getWalletID   CONSTANT)
    Q_PROPERTY(QString name          READ getName       CONSTANT)
    Q_PROPERTY(QString category      READ getCategory   CONSTANT)
    Q_PROPERTY(QString identity      READ getIdentity   CONSTANT)
    Q_PROPERTY(QString token         READ getToken      CONSTANT)

public:
    ContactItem() = default;
    ContactItem(const beam::wallet::WalletAddress&);

    QString getWalletID() const;
    QString getName() const;
    QString getCategory() const;
    QString getIdentity() const;
    QString getToken() const;

private:
    beam::wallet::WalletAddress m_walletAddress;
};

class AddressBookViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<ContactItem> contacts          READ getContacts          NOTIFY contactsChanged)
    Q_PROPERTY(QQmlListProperty<AddressItem> activeAddresses   READ getActiveAddresses   NOTIFY activeAddressesChanged)
    Q_PROPERTY(QQmlListProperty<AddressItem> expiredAddresses  READ getExpiredAddresses  NOTIFY expiredAddressesChanged)

    Q_PROPERTY(QString nameRole READ nameRole CONSTANT)
    Q_PROPERTY(QString tokenRole READ tokenRole CONSTANT)
    Q_PROPERTY(QString walletIDRole READ walletIDRole CONSTANT)
    Q_PROPERTY(QString categoryRole READ categoryRole CONSTANT)
    Q_PROPERTY(QString identityRole READ identityRole CONSTANT)
    Q_PROPERTY(QString expirationRole READ expirationRole CONSTANT)
    Q_PROPERTY(QString createdRole READ createdRole CONSTANT)

    Q_PROPERTY(Qt::SortOrder activeAddrSortOrder  READ activeAddrSortOrder  WRITE setActiveAddrSortOrder)
    Q_PROPERTY(Qt::SortOrder expiredAddrSortOrder READ expiredAddrSortOrder WRITE setExpiredAddrSortOrder)
    Q_PROPERTY(Qt::SortOrder contactSortOrder     READ contactSortOrder     WRITE setContactSortOrder)

    Q_PROPERTY(QString activeAddrSortRole  READ activeAddrSortRole  WRITE setActiveAddrSortRole)
    Q_PROPERTY(QString expiredAddrSortRole READ expiredAddrSortRole WRITE setExpiredAddrSortRole)
    Q_PROPERTY(QString contactSortRole     READ contactSortRole     WRITE setContactSortRole)

public:
    Q_INVOKABLE bool    isWIDBusy(const QString& walletID);
    Q_INVOKABLE bool    commentValid(const QString& comment) const;
    Q_INVOKABLE void    deleteAddress(const QString& walletID);
    Q_INVOKABLE void    saveChanges(const QString& walletID, const QString& name, QDateTime expirationDate);
    Q_INVOKABLE QString generateQR(const QString& addr, uint width, uint height);

public:
    AddressBookViewModel();
    ~AddressBookViewModel();

    QQmlListProperty<ContactItem> getContacts();
    QQmlListProperty<AddressItem> getActiveAddresses();
    QQmlListProperty<AddressItem> getExpiredAddresses();

    [[nodiscard]] QString nameRole() const;
    [[nodiscard]] QString walletIDRole() const;
    [[nodiscard]] QString categoryRole() const;
    [[nodiscard]] QString identityRole() const;
    [[nodiscard]] QString expirationRole() const;
    [[nodiscard]] QString createdRole() const;
    [[nodiscard]] QString tokenRole() const;

    [[nodiscard]] Qt::SortOrder activeAddrSortOrder() const;
    [[nodiscard]] Qt::SortOrder expiredAddrSortOrder() const;
    [[nodiscard]] Qt::SortOrder contactSortOrder() const;
    void setActiveAddrSortOrder(Qt::SortOrder);
    void setExpiredAddrSortOrder(Qt::SortOrder);
    void setContactSortOrder(Qt::SortOrder);

    [[nodiscard]] QString activeAddrSortRole() const;
    [[nodiscard]] QString expiredAddrSortRole() const;
    [[nodiscard]] QString contactSortRole() const;
    void setActiveAddrSortRole(QString);
    void setExpiredAddrSortRole(QString);
    void setContactSortRole(QString);

public slots:
    void onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addresses);
    void onTransactions(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>&);
    void onAddressesChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::WalletAddress>& addresses);

signals:
    void contactsChanged();
    void activeAddressesChanged();
    void expiredAddressesChanged();

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    void getAddressesFromModel();
    void sortActiveAddresses();
    void sortExpiredAddresses();
    void sortContacts();

    std::function<bool(const AddressItem*, const AddressItem*)> generateAddrComparer(QString, Qt::SortOrder);
    std::function<bool(const ContactItem*, const ContactItem*)> generateContactComparer();

private:
    WalletModel& m_model;
    QList<ContactItem*> m_contacts;
    QList<AddressItem*> m_activeAddresses;
    QList<AddressItem*> m_expiredAddresses;
    Qt::SortOrder m_activeAddrSortOrder = Qt::AscendingOrder;
    Qt::SortOrder m_expiredAddrSortOrder = Qt::AscendingOrder;
    Qt::SortOrder m_contactSortOrder = Qt::AscendingOrder;
    QString m_activeAddrSortRole;
    QString m_expiredAddrSortRole;
    QString m_contactSortRole;
    std::vector<beam::wallet::WalletID> m_busyAddresses;
};
