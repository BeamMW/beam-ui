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
#include "token_item.h"
#include "viewmodel/ui_helpers.h"
#include "model/app_model.h"

using namespace beam;
using namespace beamui;

TokenInfoItem::TokenInfoItem(QObject* parent /* = nullptr */)
        : QObject(parent)
{
}

bool TokenInfoItem::isPermanent() const
{
    return m_isPermanent;
}

bool TokenInfoItem::isMaxPrivacy() const
{
    return m_isMaxPrivacy;
}

bool TokenInfoItem::isOffline() const
{
    return m_isOffline;
}

bool TokenInfoItem::isPublicOffline() const
{
    return m_isPublicOffline;
}

QString TokenInfoItem::getTransactionType() const
{
    if (isMaxPrivacy())
    {
        return qtTrId("tx-address-max-privacy");
    }
    if (isPublicOffline())
    {
        return qtTrId("tx-address-public-offline");
    }
    //% "Regular"
    return qtTrId("tx-address-regular");
}

QString TokenInfoItem::getAmount() const
{
    if (m_amountValue)
    {
        if (Asset::s_BeamID == m_assetId)
            return AmountToUIString(m_amountValue, Currencies::Beam);

        auto amgr = AppModel::getInstance().getAssets();
        return amgr->hasAsset(m_assetId)
            ? AmountToUIString(m_amountValue, amgr->getUnitName(m_assetId, AssetsManager::Shorten::ShortenTxt))
            : AmountToUIString(m_amountValue, Currencies::Unknown);
    }
    return "";
}

QString TokenInfoItem::getAmountValue() const
{
    if (m_amountValue)
    {
        return AmountToUIString(m_amountValue, Currencies::Unknown);
    }
    return "";
}

QString TokenInfoItem::getAddress() const
{
    if (m_addressSBBS != Zero)
    {
        return toString(m_addressSBBS);
    }
    return "";
}

QString TokenInfoItem::getIdentity() const
{
    if (m_identity != Zero)
    {
        return toString(m_identity);
    }
    return "";
}


QString TokenInfoItem::getToken() const
{
    return m_token;
}

void TokenInfoItem::setToken(const QString& token)
{
    using namespace beam::wallet;

    auto trimmed = token.trimmed();
    if (trimmed != m_token)
    {
        reset();
        auto p = wallet::ParseParameters(trimmed.toStdString());
        if (p)
        {
            m_token = trimmed;

            const TxParameters& params = *p;

            auto walletID = params.GetParameter<WalletID>(TxParameterID::PeerID);
            m_addressSBBS = walletID ? *walletID : Zero;

            auto amount = params.GetParameter<Amount>(TxParameterID::Amount);
            m_amountValue = amount ? *amount : 0;

            auto assetId = params.GetParameter<beam::Asset::ID>(TxParameterID::AssetID);
            m_assetId = assetId ? *assetId : 0;

            auto identity = params.GetParameter<PeerID>(TxParameterID::PeerWalletIdentity);
            m_identity = identity ? *identity : Zero;

            auto type = params.GetParameter<TxType>(TxParameterID::TransactionType);
            if (type)
            {
                switch (*type)
                {
                case TxType::PushTransaction:
                {
                    auto voucher = params.GetParameter<ShieldedTxo::Voucher>(TxParameterID::Voucher);
                    m_isMaxPrivacy = !!voucher;

                    auto vouchers = params.GetParameter<ShieldedVoucherList>(TxParameterID::ShieldedVoucherList);
                    if (vouchers && !vouchers->empty())
                    {
                        m_isOffline = true;
                        if (getIgnoreStoredVouchers())
                        {
                            setOfflinePayments((int)vouchers->size());
                        }
                        else if (walletID)
                        {
                            AppModel::getInstance().getWalletModel()->getAsync()->saveVouchers(*vouchers, *walletID);
                        }
                    } 
                    else
                    {
                        auto gen = params.GetParameter<ShieldedTxo::PublicGen>(TxParameterID::PublicAddreessGen);
                        if (gen)
                        {
                            m_isPublicOffline = true;
                        }
                    }
                }
                break;
                case TxType::Simple:
                {
                    auto isPermanent = params.GetParameter<bool>(TxParameterID::IsPermanentPeerID);
                    if (isPermanent)
                    {
                        m_isPermanent = *isPermanent;
                    }
                }
                break;
                default:
                    break;
                }
            }
            else
            {
                m_isPermanent = true;
            }

            if (!getIgnoreStoredVouchers())
            {
                AppModel::getInstance().getWalletModel()->getAsync()->getAddress(token.toStdString(), [this](const auto& addr, auto count)
                {
                    setOfflinePayments((int)count);
                });
            }
        }

        emit tokenChanged();
    }
}

int TokenInfoItem::getOfflinePayments() const
{
    return m_offlinePayments;
}

void TokenInfoItem::setOfflinePayments(int value)
{
    if (m_offlinePayments != value)
    {
        m_offlinePayments = value;
        emit offlinePaymentsChanged();
    }
}

bool TokenInfoItem::getIgnoreStoredVouchers() const
{
    return m_ignoreStoredVouchers;
}

void TokenInfoItem::setIgnoreStoredVouchers(bool value)
{
    if (m_ignoreStoredVouchers != value)
    {
        m_ignoreStoredVouchers = value;
        emit ignoreStoredVouchersChanged();
    }
}

void TokenInfoItem::reset()
{
    m_token.clear();
    m_isPermanent = false;
    m_isMaxPrivacy = false;
    m_isOffline = false;
    m_isPublicOffline = false;
    m_amountValue = 0;
    m_addressSBBS = Zero;
    m_identity = Zero;
    m_offlinePayments = 0;
}
