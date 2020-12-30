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
#include <QAbstractItemModel>
#include "model/wallet_model.h"

namespace beamui::dex {
    class DexView : public QObject
    {
        Q_OBJECT

    public:
        DexView();
        ~DexView();

        //
        // Methods
        //
        Q_INVOKABLE void placeOrder();

    public slots:
        void onDexOffersChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::DexOffer>& offers) {}
        void onNewAddress(const beam::wallet::WalletAddress& addr);

    private:
        WalletModel& _walletModel;
        beam::wallet::WalletAddress _receiverAddr;
    };
}
