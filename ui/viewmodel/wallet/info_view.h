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

// TODO: add LockedMaturingMP support to the tooltip
#include <QObject>
#include "model/wallet_model.h"
#include "assets_manager.h"
#include "viewmodel/notifications/exchange_rates_manager.h"

class InProgress
{
    Q_GADGET
public:
    Q_PROPERTY(QString receiving          READ getReceiving)
    Q_PROPERTY(QString receivingUnit      READ getReceivingUnit)
    Q_PROPERTY(QString sending            READ getSending)
    Q_PROPERTY(QString locked             READ getLocked)
    Q_PROPERTY(QString lockedMaturing     READ getLockedMaturing)
    Q_PROPERTY(QString lockedMaturingMP   READ getLockedMaturingMP)
    Q_PROPERTY(QString receivingChange    READ getReceivingChange)
    Q_PROPERTY(QString receivingIncoming  READ getReceivingIncoming)
    Q_PROPERTY(QString icon               READ getIcon)
    Q_PROPERTY(QString unitName           READ getUnitName)
    Q_PROPERTY(QString rate               READ getRate)
    Q_PROPERTY(QString rateUnit           READ getRateUnit)

    [[nodiscard]] QString getReceiving() const {
       return receiving;
    }

    [[nodiscard]] QString getReceivingUnit() const {
       return receivingUnit;
    }

    [[nodiscard]] QString getSending() const {
       return sending;
    }

    [[nodiscard]] QString getLocked() const {
       return locked;
    }

    [[nodiscard]] QString getLockedMaturing() const {
       return lockedMaturing;
    }

    [[nodiscard]] QString getReceivingChange() const {
       return receivingChange;
    }

    [[nodiscard]] QString getReceivingIncoming() const {
       return receivingIncoming;
    }

    [[nodiscard]] QString getIcon() const {
        return icon;
    }

    [[nodiscard]] QString getUnitName() const {
        return unitName;
    }

    [[nodiscard]] QString getRate() const {
        return rate;
    }

    [[nodiscard]] QString getRateUnit() const {
        return rateUnit;
    }

    [[nodiscard]] QString getLockedMaturingMP() const {
        return lockedMaturingMP;
    }

    QString receiving;
    QString receivingUnit;
    QString sending;
    QString locked;
    QString lockedMaturing;
    QString lockedMaturingMP;
    QString receivingChange;
    QString receivingIncoming;
    QString icon;
    QString unitName;
    QString rate;
    QString rateUnit;
    beam::Asset::ID assetId;
};

class InfoViewModel : public QObject
{
    Q_OBJECT
public:
    InfoViewModel();
    ~InfoViewModel() override;

    Q_PROPERTY(int selectedAsset                      READ getSelectedAsset              WRITE  setSelectedAsset   NOTIFY assetChanged)
    Q_PROPERTY(QString assetAvailable                 READ assetAvailable                NOTIFY assetChanged)
    Q_PROPERTY(QString assetIcon                      READ assetIcon                     NOTIFY assetChanged)
    Q_PROPERTY(QString assetUnitName                  READ assetUnitName                 NOTIFY assetChanged)
    Q_PROPERTY(QString assetName                      READ assetName                     NOTIFY assetChanged)
    Q_PROPERTY(QString rateUnit                       READ getRateUnit                   NOTIFY assetChanged)
    Q_PROPERTY(QString rate                           READ getRate                       NOTIFY assetChanged)

    Q_PROPERTY(QList<InProgress> progress             READ getProgress                   NOTIFY assetChanged)
    Q_PROPERTY(InProgress progressTotal               READ getProgressTotal              NOTIFY assetChanged)

    QString assetAvailable() const;
    QString assetIcon() const;
    QString assetUnitName() const;
    QString assetName() const;

    QList<InProgress> getProgress() const;
    InProgress getProgressTotal() const;

    int  getSelectedAsset() const;
    void setSelectedAsset(int val);

    QString getRateUnit() const;
    QString getRate() const;

signals:
    void assetChanged();

private slots:
    void onAssetInfo(beam::Asset::ID assetId);
    void onWalletStatus();

private:
    void updateProgress();

    WalletModel&           _wallet;
    mutable AssetsManager  _amgr;
    ExchangeRatesManager   _ermgr;
    int                    _selectedAssetID; // can be -1
    QList<InProgress>      _progress;
    InProgress             _progressTotals;
};
