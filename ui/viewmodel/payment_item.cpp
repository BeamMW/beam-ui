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
#include "payment_item.h"
#include "ui_helpers.h"
#include "wallet/core/wallet.h"
#include "model/app_model.h"

using namespace beam;
using namespace beam::wallet;
using namespace beamui;

PaymentInfoItem::PaymentInfoItem(QObject* parent /* = nullptr */)
        : QObject(parent)
{
}

QString PaymentInfoItem::getSender() const
{
    if (m_paymentInfo)
        return toString(m_paymentInfo->m_Sender);
    else if (m_shieldedPaymentInfo)
        return toString(m_shieldedPaymentInfo->m_Sender);

    return "";
}

QString PaymentInfoItem::getReceiver() const
{
    if (m_paymentInfo)
        return toString(m_paymentInfo->m_Receiver);
    else if (m_shieldedPaymentInfo)
        return toString(m_shieldedPaymentInfo->m_Receiver);

    return "";
}

QString PaymentInfoItem::getAmount() const
{
    if (m_paymentInfo)
        return AmountToUIString(m_paymentInfo->m_Amount, Currencies::Beam);
    else if (m_shieldedPaymentInfo)
        return AmountToUIString(m_shieldedPaymentInfo->m_Amount, Currencies::Beam);

    return "";
}

QString PaymentInfoItem::getAmountValue() const
{
    if (m_paymentInfo)
        return AmountToUIString(m_paymentInfo->m_Amount, Currencies::Unknown);
    else if (m_shieldedPaymentInfo)
        return AmountToUIString(m_shieldedPaymentInfo->m_Amount, Currencies::Unknown);

    return "";
}

QString PaymentInfoItem::getKernelID() const
{
    if (m_paymentInfo)
        return toString(m_paymentInfo->m_KernelID);
    else if (m_shieldedPaymentInfo)
        return toString(m_shieldedPaymentInfo->m_KernelID);

    return "";
}

bool PaymentInfoItem::isValid() const
{
    if (m_paymentInfo)
        return m_paymentInfo->IsValid();
    else if (m_shieldedPaymentInfo)
        return m_shieldedPaymentInfo->IsValid();

    return false;
}

QString PaymentInfoItem::getPaymentProof() const
{
    return m_paymentProof;
}

void PaymentInfoItem::setPaymentProof(const QString& value)
{
    if (m_paymentProof != value)
    {
        m_paymentProof = value;
        auto buffer = beam::from_hex(m_paymentProof.toStdString());
        try
        {
            m_paymentInfo = beam::wallet::storage::PaymentInfo::FromByteBuffer(buffer);
            emit paymentProofChanged();
            return;
        }
        catch (...)
        {
            reset();
        }
        try
        {
            m_shieldedPaymentInfo = beam::wallet::storage::ShieldedPaymentInfo::FromByteBuffer(buffer);
            emit paymentProofChanged();
            return;
        }
        catch (...)
        {
            reset();
        }
    }
}

void PaymentInfoItem::reset()
{
    m_paymentInfo.reset();
    m_shieldedPaymentInfo.reset();
    emit paymentProofChanged();
}


MyPaymentInfoItem::MyPaymentInfoItem(const TxID& txID, QObject* parent/* = nullptr*/)
        : PaymentInfoItem(parent)
{
    auto model = AppModel::getInstance().getWalletModel();
    connect(model.get(), SIGNAL(paymentProofExported(const beam::wallet::TxID&, const QString&)), SLOT(onPaymentProofExported(const beam::wallet::TxID&, const QString&)));
    model->getAsync()->exportPaymentProof(txID);
}

void MyPaymentInfoItem::onPaymentProofExported(const beam::wallet::TxID& txID, const QString& proof)
{
    setPaymentProof(proof);
}
