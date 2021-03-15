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
#include <QMap>
#include <QList>
#include <QVariant>
#include "model/wallet_model.h"
#include "viewmodel/notifications/exchange_rates_manager.h"

class AssetsManager: public QObject
{
    Q_OBJECT
public:
    typedef std::shared_ptr<AssetsManager> Ptr;

    explicit AssetsManager(WalletModel::Ptr wallet);
    ~AssetsManager() override = default;

    // SYNC
    QString getIcon(beam::Asset::ID);

    enum Shorten {
        ShortenTxt,
        ShortenHtml,
        NoShorten
    };

    QString getUnitName(beam::Asset::ID, Shorten shorten);
    QString getName(beam::Asset::ID);
    QString getSmallestUnitName(beam::Asset::ID);
    QString getShortDesc(beam::Asset::ID);
    QString getLongDesc(beam::Asset::ID);
    QColor  getColor(beam::Asset::ID);
    QColor  getSelectionColor(beam::Asset::ID);
    QString getSiteUrl(beam::Asset::ID);
    QString getPaperUrl(beam::Asset::ID);
    QList<QMap<QString, QVariant>> getAssetsList();
    bool hasAsset(beam::Asset::ID) const;

signals:
    void assetInfo(beam::Asset::ID assetId);
    void assetsListChanged();

private slots:
    void onAssetInfo(beam::Asset::ID, const beam::wallet::WalletAsset&);

private:
    // ASYNC
    void collectAssetInfo(beam::Asset::ID);

    typedef std::shared_ptr<beam::wallet::WalletAssetMeta> MetaPtr;
    typedef std::shared_ptr<beam::wallet::WalletAsset> AssetPtr;
    typedef std::pair<AssetPtr, MetaPtr> InfoPair;
    MetaPtr getAsset(beam::Asset::ID);

    WalletModel::Ptr _wallet;
    ExchangeRatesManager _exchangeRatesManager;
    std::map<beam::Asset::ID, InfoPair> _info;
    std::set<beam::Asset::ID> _requested;

    std::map<int, QColor>  _colors;
    std::map<int, QString> _icons;
};
