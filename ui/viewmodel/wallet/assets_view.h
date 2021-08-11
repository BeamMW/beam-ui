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
#include "assets_list.h"
#include "model/wallet_model.h"

class AssetsViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* assets READ getAssets CONSTANT)
    Q_PROPERTY(int selectedAsset READ getSelectedAsset WRITE setSelectedAsset NOTIFY selectedAssetChanged)
    Q_PROPERTY(bool showFaucetPromo READ getShowFaucetPromo WRITE setShowFaucetPromo NOTIFY showFaucetPromoChanged)
    Q_PROPERTY(bool hideSeedValidationPromo READ getHideSeedValidationPromo WRITE setHideSeedValidationPromo NOTIFY hideSeedValidationPromoChanged)
    Q_PROPERTY(bool canHideSeedValidationPromo READ getCanHideSeedValidationPromo NOTIFY canHideSeedValidationPromoChanged)
    Q_PROPERTY(bool isSeedValidated READ getIsSeedValidated NOTIFY isSeedValidatedChanged)

public:
    AssetsViewModel();
    ~AssetsViewModel() override = default;

    QAbstractItemModel* getAssets();
    [[nodiscard]] int getSelectedAsset() const;
    void setSelectedAsset(int assetId);
    bool getShowFaucetPromo();
    void setShowFaucetPromo(bool value);
    bool getIsSeedValidated() const;
    bool getHideSeedValidationPromo() const;
    void setHideSeedValidationPromo(bool value);
    bool getCanHideSeedValidationPromo() const;

public slots:
    void onNormalCoinsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Coin>& utxos);
    void onShieldedCoinChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::ShieldedCoin>& items);

signals:
    void selectedAssetChanged();
    void showFaucetPromoChanged();
    void isSeedValidatedChanged();
    void hideSeedValidationPromoChanged();
    void canHideSeedValidationPromoChanged();

private:
    bool hasBeamAmount() const;

    AssetsList _assets;
    WalletSettings& _settings;
    WalletModel& _wallet;
    boost::optional<beam::Asset::ID> _selectedAsset;
    std::string _seed;
};
