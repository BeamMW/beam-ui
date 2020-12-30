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
#include "dex_view.h"
#include "utility/logger.h"
#include "model/app_model.h"

namespace beamui::dex
{
    DexView::DexView()
        : _walletModel(*AppModel::getInstance().getWalletModel())
    {
         connect(&_walletModel, &WalletModel::dexOffersChanged, this, &DexView::onDexOffersChanged);
         connect(&_walletModel, &WalletModel::generatedNewAddress, this, &DexView::onNewAddress);

         _walletModel.getAsync()->getSwapOffers();
         _walletModel.getAsync()->generateNewAddress();
    }

    DexView::~DexView()
    {
    }

    void DexView::placeOrder()
    {
        _walletModel.getAsync()->saveAddress(_receiverAddr, true);
        beam::wallet::DexOffer offer(_receiverAddr.m_walletID, 0, 1, 10000);
        _walletModel.getAsync()->publishDexOffer(offer);
    }

     void DexView::onNewAddress(const beam::wallet::WalletAddress& addr)
     {
        _receiverAddr = addr;
     }
}
