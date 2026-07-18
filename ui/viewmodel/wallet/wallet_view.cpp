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
#include "wallet_view.h"
#include "model/app_model.h"


WalletViewModel::WalletViewModel()
    : _model(AppModel::getInstance().getWalletModel())
{
    connect(_model, &WalletModel::slatepackReady, this,
            [this](const beam::wallet::TxID&, const QString& armored) { emit slatepackProduced(armored); });
    connect(_model, &WalletModel::slatepackImportResult, this,
            [this](bool ok, const QString& error, const QVariantMap& info) {
                // Runs on the UI thread — safe to touch AssetsManager to resolve the ticker + icon.
                QVariantMap m = info;
                if (auto amgr = AppModel::getInstance().getAssets())
                {
                    const auto aid = static_cast<beam::Asset::ID>(info.value("assetId").toUInt());
                    m["unitName"] = amgr->getUnitName(aid, AssetsManager::NoShorten);
                    m["icon"]     = amgr->getIcon(aid);
                }
                emit slatepackImported(ok, error, m);
            });
}

void WalletViewModel::importSlatepack(const QString& text)
{
    _model->getAsync()->importSlatepack(text.toStdString());
}

void WalletViewModel::commitSlatepack(const QString& txId)
{
    _model->getAsync()->commitSlatepack(txId.toStdString());
}

void WalletViewModel::cancelSlatepack(const QString& txId)
{
    _model->getAsync()->cancelSlatepack(txId.toStdString());
}

