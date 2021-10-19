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
#include "model/exchange_rates_manager.h"

class AssetsManager: public QObject
{
    Q_OBJECT
public:
    typedef std::shared_ptr<AssetsManager> Ptr;

    AssetsManager(WalletModel::Ptr wallet, ExchangeRatesManager::Ptr rates);
    ~AssetsManager() override = default;

    // SYNC
    [[nodiscard]] QString getIcon(beam::Asset::ID);

    enum Shorten {
        ShortenTxt,
        ShortenHtml,
        NoShorten
    };

    [[nodiscard]] QString getUnitName(beam::Asset::ID, Shorten shorten);
    [[nodiscard]] QString getName(beam::Asset::ID);
    [[nodiscard]] QString getSmallestUnitName(beam::Asset::ID);
    [[nodiscard]] QString getShortDesc(beam::Asset::ID);
    [[nodiscard]] QString getLongDesc(beam::Asset::ID);
    [[nodiscard]] QColor  getColor(beam::Asset::ID);
    [[nodiscard]] QColor  getSelectionColor(beam::Asset::ID);
    [[nodiscard]] QString getSiteUrl(beam::Asset::ID);
    [[nodiscard]] QString getPaperUrl(beam::Asset::ID);
    [[nodiscard]] beam::Amount getRate(beam::Asset::ID);
    [[nodiscard]] QString getRateUnit();
    [[nodiscard]] QList<QMap<QString, QVariant>> getAssetsList();
    [[nodiscard]] QMap<QString, QVariant> getAssetsMap(const std::set<beam::Asset::ID>& assets);
    [[nodiscard]] bool hasAsset(beam::Asset::ID) const;
    [[nodiscard]] bool isVerified(beam::Asset::ID) const;

signals:
    void assetInfo(beam::Asset::ID assetId);
    void assetsListChanged();

private slots:
    void onAssetInfo(beam::Asset::ID, const beam::wallet::WalletAsset&);
    void onAssetVerification(const std::vector<beam::wallet::VerificationInfo>&);

private:
    // ASYNC
    void collectAssetInfo(beam::Asset::ID);

    typedef std::shared_ptr<beam::wallet::WalletAssetMeta> MetaPtr;
    typedef std::shared_ptr<beam::wallet::WalletAsset> AssetPtr;
    typedef std::pair<AssetPtr, MetaPtr> InfoPair;
    MetaPtr getAsset(beam::Asset::ID);
    QMap<QString, QVariant> getAssetProps(beam::Asset::ID);

    WalletModel::Ptr _wallet;
    ExchangeRatesManager::Ptr _rates;
    std::map<beam::Asset::ID, beam::wallet::VerificationInfo> m_vi;
    std::map<beam::Asset::ID, InfoPair> _info;
    std::set<beam::Asset::ID> _requested;

    std::map<int, QColor>  _colors;
    std::map<int, QString> _icons;
};
