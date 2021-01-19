// Copyright 2020 The Beam Team
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
#include <QColor>
#include "model/wallet_model.h"

// TODO: consider singleton for AssetsManager
class AssetsManager: public QObject
{
    Q_OBJECT
public:
    AssetsManager();
    ~AssetsManager() override = default;

    // ASYNC
    void collectAssetInfo(beam::Asset::ID);

    // SYNC
    QString getIcon(beam::Asset::ID);
    QString getUnitName(beam::Asset::ID, bool shorten);
    QString getName(beam::Asset::ID);
    QColor  getColor(beam::Asset::ID);
    QColor  getSelectionColor(beam::Asset::ID);

signals:
    void assetInfo(beam::Asset::ID assetId);

private slots:
    void onAssetInfo(beam::Asset::ID, const beam::wallet::WalletAsset&);

private:
    typedef std::unique_ptr<beam::wallet::WalletAssetMeta> MetaPtr;
    MetaPtr getAsset(beam::Asset::ID);

    WalletModel& _wallet;
    std::map<beam::Asset::ID, beam::wallet::WalletAsset> _info;

    std::map<int, QColor> _colors;
    std::map<int, QString> _icons;
};
