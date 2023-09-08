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
#include <QQmlListProperty>
#include "model/wallet_model.h"
#include "viewmodel/address_item.h"
#include "wallet/core/common.h"

namespace beam::wallet
{
    struct WalletAddress;
}

class MessengerAddressAdd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString address READ getAddress WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(QString name    READ getName    WRITE setName    NOTIFY nameChanged)
    Q_PROPERTY(bool    error   READ getError                    NOTIFY errorChanged)
    Q_PROPERTY(QString peerID  READ getPeerID                   NOTIFY addressChanged)

    Q_PROPERTY(QQmlListProperty<AddressItem> myAddresses    READ getMyAddresses                               NOTIFY myAddressesChanged)
    Q_PROPERTY(QString                       myAddress      READ getMyAddress                                 NOTIFY myAddressChanged)
    Q_PROPERTY(uint                          myAddressIndex READ getMyAddressIndex  WRITE setMyAddressIndex   NOTIFY myAddressIndexChanged)
public:
    MessengerAddressAdd();

    const QString& getAddress() const;
    const QString& getName() const;
    bool getError() const;
    QString getPeerID() const;

    void setAddress(const QString& addr);
    void setName(const QString& name);

    QQmlListProperty<AddressItem> getMyAddresses();
    QString getMyAddress() const;
    uint getMyAddressIndex() const;
    void setMyAddressIndex(uint value);

    Q_INVOKABLE void saveAddress();

public slots:
    void onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addresses);

signals:
    void addressChanged();
    void myAddressesChanged();
    void myAddressChanged();
    void myAddressIndexChanged();
    void nameChanged();
    void errorChanged();

private:
    WalletModel::Ptr _walletModel;
    std::vector<beam::wallet::WalletAddress> _contacts;
    QList<AddressItem*> _myAddresses;
    uint _myAddressIndex = 0;

    QString _address;
    beam::wallet::WalletID _peerID = beam::Zero;
    QString _name;
    bool _error = false;
};
