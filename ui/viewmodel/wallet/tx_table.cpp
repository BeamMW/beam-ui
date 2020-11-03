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
#include "tx_table.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QDateTime>
#include <QTextStream>
#include <QTextCodec>
#include <vector>
#include "model/app_model.h"

namespace
{
    const char kTxHistoryFileNamePrefix[] = "transactions_history_";
    const char kTxHistoryFileFormatDesc[] = "Comma-Separated Values (*.csv)";
    const char kTxHistoryFileNameFormat[] = "yyyy_MM_dd_HH_mm_ss";
}

TxTableViewModel::TxTableViewModel()
    : _model(*AppModel::getInstance().getWallet())
{
    connect(&_model, SIGNAL(transactionsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>&)), SLOT(onTransactionsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>&)));
    connect(&_model, SIGNAL(txHistoryExportedToCsv(const QString&)), this, SLOT(onTxHistoryExportedToCsv(const QString&)));
    connect(&_exchangeRatesManager, &ExchangeRatesManager::rateUnitChanged, this, &TxTableViewModel::rateChanged);
    connect(&_exchangeRatesManager, &ExchangeRatesManager::activeRateChanged, this, &TxTableViewModel::rateChanged);
    _model.getAsync()->getTransactions();
}

void TxTableViewModel::exportTxHistoryToCsv()
{
    QDateTime now = QDateTime::currentDateTime();
    QString path = QFileDialog::getSaveFileName(
        nullptr,
        //: transactions history screen, export button tooltip and open file dialog
        //% "Export transactions history"
        qtTrId("wallet-export-tx-history"),
        QDir(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation))
            .filePath(kTxHistoryFileNamePrefix + now.toString(kTxHistoryFileNameFormat)), kTxHistoryFileFormatDesc);

    if (!path.isEmpty())
    {
        _txHistoryToCsvPaths.enqueue(path);
        _model.getAsync()->exportTxHistoryToCsv();
    }
}

void TxTableViewModel::onTxHistoryExportedToCsv(const QString& data)
{
    if (!_txHistoryToCsvPaths.isEmpty())
    {
        const auto& path = _txHistoryToCsvPaths.dequeue();
        QFile file(path);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextCodec *codec = QTextCodec::codecForName("UTF8");
            QTextStream out(&file);
            out.setCodec(codec);
            out << data;
        }
    }
}

QAbstractItemModel* TxTableViewModel::getTransactions()
{
    return &_transactionsList;
}

void TxTableViewModel::onTransactionsChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::TxDescription>& transactions)
{
    using namespace beam::wallet;

    std::vector<std::shared_ptr<TxObject>> modifiedTransactions;
    modifiedTransactions.reserve(transactions.size());
    ExchangeRate::Currency secondCurrency = _exchangeRatesManager.getRateUnitRaw();

    for (const auto& t : transactions)
    {
        if(const auto txType = t.GetParameter<TxType>(TxParameterID::TransactionType))
        {
            switch(*txType)
            {
            case TxType::AtomicSwap:
            case TxType::AssetIssue:
            case TxType::AssetConsume:
            case TxType::AssetReg:
            case TxType::AssetUnreg:
            case TxType::AssetInfo:
            case TxType::PullTransaction:
            case TxType::UnlinkFunds:
            case TxType::VoucherRequest:
            case TxType::VoucherResponse:
            case TxType::Contract:
                continue;
            case TxType::ALL:
                assert(!"This should not happen");
                continue;
            case TxType::PushTransaction:
            case TxType::Simple:
                break;
            }
            modifiedTransactions.push_back(std::make_shared<TxObject>(t, secondCurrency));
        }
    }

    switch (action)
    {
        case ChangeAction::Reset:
            {
                _transactionsList.reset(modifiedTransactions);
                break;
            }

        case ChangeAction::Removed:
            {
                _transactionsList.remove(modifiedTransactions);
                break;
            }

        case ChangeAction::Added:
            {
                _transactionsList.insert(modifiedTransactions);
                break;
            }

        case ChangeAction::Updated:
            {
                _transactionsList.update(modifiedTransactions);
                break;
            }

        default:
            assert(false && "Unexpected action");
            break;
    }

    emit transactionsChanged();
}

QString TxTableViewModel::getRateUnit() const
{
    return beamui::getCurrencyUnitName(_exchangeRatesManager.getRateUnitRaw());
}

QString TxTableViewModel::getRate() const
{
    auto rate = _exchangeRatesManager.getRate(beam::wallet::ExchangeRate::Currency::Beam);
    return beamui::AmountToUIString(rate);
}


void TxTableViewModel::cancelTx(const QVariant& variantTxID)
{
    if (!variantTxID.isNull() && variantTxID.isValid())
    {
        auto txId = variantTxID.value<beam::wallet::TxID>();
        _model.getAsync()->cancelTx(txId);
    }
}

void TxTableViewModel::deleteTx(const QVariant& variantTxID)
{
    if (!variantTxID.isNull() && variantTxID.isValid())
    {
        auto txId = variantTxID.value<beam::wallet::TxID>();
        _model.getAsync()->deleteTx(txId);
    }
}

PaymentInfoItem* TxTableViewModel::getPaymentInfo(const QVariant& variantTxID)
{
    if (!variantTxID.isNull() && variantTxID.isValid())
    {
        auto txId = variantTxID.value<beam::wallet::TxID>();
        return new MyPaymentInfoItem(txId, this);
    }
    else return Q_NULLPTR;
}
