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
#pragma once

#include <QObject>
#include <QAbstractItemModel>
#include "model/assets_list.h"
#include "model/wallet_model.h"

class AssetsViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* assets READ getAssets CONSTANT)

    Q_PROPERTY(int  selectedAsset           READ getSelectedAsset          WRITE setSelectedAsset        NOTIFY selectedAssetChanged)
    Q_PROPERTY(bool showFaucetPromo         READ getShowFaucetPromo        WRITE setShowFaucetPromo      NOTIFY showFaucetPromoChanged)
    Q_PROPERTY(bool showValidationPromo     READ getShowValidationPromo    WRITE setShowValidationPromo  NOTIFY showValidationPromoChanged)
    Q_PROPERTY(bool canHideValidationPromo  READ getCanHideValidationPromo NOTIFY canHideValidationPromoChanged )

public:
    AssetsViewModel();
    ~AssetsViewModel() override = default;

    QAbstractItemModel* getAssets();
    [[nodiscard]] int getSelectedAsset() const;
    void setSelectedAsset(int assetId);

    [[nodiscard]] bool getShowFaucetPromo();
    void setShowFaucetPromo(bool value);

    [[nodiscard]] bool getShowValidationPromo() const;
    void setShowValidationPromo(bool value);

    [[nodiscard]] bool getCanHideValidationPromo() const;

public slots:
    void onNormalCoinsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Coin>& utxos);
    void onShieldedCoinChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::ShieldedCoin>& items);

signals:
    void selectedAssetChanged();
    void showFaucetPromoChanged();
    void showValidationPromoChanged();
    void canHideValidationPromoChanged();

private:
    bool hasBeamAmount() const;

    WalletModel::Ptr _wallet;
    AssetsList::Ptr  _assets;
    WalletSettings&  _settings;
    boost::optional<beam::Asset::ID> _selectedAsset;
};
