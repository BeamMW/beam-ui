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
        beam::wallet::TxID id;
        LOG_INFO() << id;

         connect(&_walletModel, &WalletModel::dexOrdersChanged, this, &DexView::onDexOrdersChanged);
         connect(&_walletModel, &WalletModel::generatedNewAddress, this, &DexView::onNewAddress);

         _walletModel.getAsync()->getDexOrders();

         // TODO:DEX move address to the board?
         _walletModel.getAsync()->generateNewAddress();

         //_orders.insert(std::make_shared<DexOrderObject>());
         //_orders.insert(std::make_shared<DexOrderObject>());
         //_orders.insert(std::make_shared<DexOrderObject>());
         emit ordersChanged();
    }

    DexView::~DexView()
    {
    }

    void DexView::buyBEAM()
    {
        using namespace beam::wallet;

        _walletModel.getAsync()->saveAddress(_receiverAddr, true);

        DexOrder order(DexOrderID::generate(), _receiverAddr.m_walletID, _receiverAddr.m_OwnID, 1, 0, 100);
        _walletModel.getAsync()->publishDexOrder(order);
    }

    void DexView::sellBEAM()
    {
        using namespace beam::wallet;

        _walletModel.getAsync()->saveAddress(_receiverAddr, true);

        DexOrder order(DexOrderID::generate(), _receiverAddr.m_walletID, _receiverAddr.m_OwnID, 0, 1, 100);
        _walletModel.getAsync()->publishDexOrder(order);
    }

    void DexView::onNewAddress(const beam::wallet::WalletAddress& addr)
    {
       _receiverAddr = addr;
       _orders.selfID = addr.m_Identity;
    }

    QAbstractItemModel* DexView::getOrders()
    {
        return &_orders;
    }

    void DexView::onDexOrdersChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::DexOrder>& orders)
    {
        using ChangeAction = beam::wallet::ChangeAction;

        switch (action)
        {
            case ChangeAction::Reset:
                _orders.reset(orders);
                break;

            case ChangeAction::Added:
                _orders.insert(orders);
                break;

            case ChangeAction::Removed:
                _orders.remove(orders);
                break;

            case ChangeAction::Updated:
                _orders.update(orders);
                break;

            default:
                assert(false);
                break;
        }
    }

    void DexView::acceptOrder(QString orderId)
    {
        try
        {
            beam::wallet::DexOrderID dexOrderId(orderId.toStdString());
            _walletModel.getAsync()->acceptDexOrder(dexOrderId);
        }
        catch(std::runtime_error& err)
        {
            LOG_ERROR() << "DexView::acceptOrder failed, " << err.what();
        }
    }
}
