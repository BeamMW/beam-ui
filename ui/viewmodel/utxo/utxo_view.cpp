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

#include "utxo_view.h"
#include "viewmodel/ui_helpers.h"
#include "model/app_model.h"
using namespace beam;
using namespace std;
using namespace beamui;

UtxoViewModel::UtxoViewModel()
    : m_model{*AppModel::getInstance().getWalletModel()}
{
    connect(&m_model, &WalletModel::normalCoinsChanged,  this, &UtxoViewModel::onNormalCoinsChanged);
    connect(&m_model, &WalletModel::shieldedCoinChanged, this, &UtxoViewModel::onShieldedCoinChanged);
    connect(&m_model, &WalletModel::walletStatusChanged, this, &UtxoViewModel::stateChanged);
    m_model.getAsync()->getAllUtxosStatus();
}

QAbstractItemModel* UtxoViewModel::getAllUtxos()
{
    return &m_allUtxos;
}

QString UtxoViewModel::getCurrentHeight() const
{
    return QString::fromStdString(to_string(m_model.getCurrentStateID().m_Height));
}

QString UtxoViewModel::getCurrentStateHash() const
{
    return QString(beam::to_hex(m_model.getCurrentStateID().m_Hash.m_pData, 10).c_str());
}

bool UtxoViewModel::getMaturingMaxPrivacy() const
{
    return m_maturingMaxPrivacy;
}

void UtxoViewModel::setMaturingMaxPrivacy(bool value)
{
    if (m_maturingMaxPrivacy != value)
    {
        m_maturingMaxPrivacy = value;
        emit maturingMaxPrivacyChanged();
    }
}

unsigned int UtxoViewModel::getAssetId() const
{
    return m_assetId ? *m_assetId : -1;
}

void UtxoViewModel::setAssetId(unsigned int id)
{
    if (!m_assetId || (*m_assetId != id))
    {
        m_assetId = id;
        emit assetIdChanged();

        m_model.getAsync()->getAllUtxosStatus();
    }
}

void UtxoViewModel::onNormalCoinsChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::Coin>& utxos)
{
    using namespace beam::wallet;

    if (getMaturingMaxPrivacy())
        return;

    vector<shared_ptr<BaseUtxoItem>> modifiedItems;
    modifiedItems.reserve(utxos.size());

    for (const auto& t : utxos)
    {
        if (m_assetId)
        {
            if (t.m_ID.m_AssetID != *m_assetId)
            {
                continue;
            }
        }

        modifiedItems.push_back(make_shared<UtxoItem>(t));
    }

    switch (action)
    {
    case ChangeAction::Reset:
    {
        m_allUtxos.reset(modifiedItems);
        break;
    }

    case ChangeAction::Removed:
    {
        m_allUtxos.remove(modifiedItems);
        break;
    }

    case ChangeAction::Added:
    {
        m_allUtxos.insert(modifiedItems);
        break;
    }

    case ChangeAction::Updated:
    {
        m_allUtxos.update(modifiedItems);
        break;
    }

    default:
        assert(false && "Unexpected action");
        break;
    }

    emit allUtxoChanged();
}

void UtxoViewModel::onShieldedCoinChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::ShieldedCoin>& items)
{
    using namespace beam::wallet;

    vector<shared_ptr<BaseUtxoItem>> modifiedItems;
    modifiedItems.reserve(items.size());

    for (const auto& t : items)
    {
        if (m_assetId)
        {
            if (t.m_CoinID.m_AssetID != *m_assetId)
            {
                continue;
            }
        }

        if (getMaturingMaxPrivacy() && t.m_Status != beam::wallet::ShieldedCoin::Status::Maturing)
        {
            continue;
        }

        modifiedItems.push_back(make_shared<ShieldedCoinItem>(t));
    }

    switch (action)
    {
    case ChangeAction::Reset:
    {
        // temporary hack
        vector<shared_ptr<BaseUtxoItem>> toRemove;
        for (const auto& item : m_allUtxos)
        {
            if (item->type() == UtxoViewType::EnType::Shielded)
            {
                toRemove.push_back(item);
            }
        }
        m_allUtxos.remove(toRemove);
        m_allUtxos.insert(modifiedItems);
        break;
    }

    case ChangeAction::Removed:
    {
        m_allUtxos.remove(modifiedItems);
        break;
    }

    case ChangeAction::Added:
    {
        m_allUtxos.insert(modifiedItems);
        break;
    }

    case ChangeAction::Updated:
    {
        m_allUtxos.update(modifiedItems);
        break;
    }

    default:
        assert(false && "Unexpected action");
        break;
    }

    emit allUtxoChanged();
}

