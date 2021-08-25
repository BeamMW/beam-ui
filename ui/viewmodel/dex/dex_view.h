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
#include "model/wallet_model.h"
#include "dex_orders_list.h"

namespace beamui::dex {
    class DexView : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QAbstractItemModel* orders READ getOrders NOTIFY ordersChanged)

    public:
        DexView();
        ~DexView();

        QAbstractItemModel* getOrders();

        //
        // Methods
        //
        Q_INVOKABLE void buyBEAMX();
        Q_INVOKABLE void sellBEAMX();
        Q_INVOKABLE void acceptOrder(const QString& orderId);

    signals:
        void ordersChanged();

    public slots:
        void onDexOrdersChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::DexOrder>& offers);
        void onNewAddress(const beam::wallet::WalletAddress& addr);

    private:
        WalletModel& _walletModel;
        DexOrdersList _orders;
        beam::wallet::WalletAddress _receiverAddr;
    };
}
