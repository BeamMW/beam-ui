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
#include "model/wallet_model.h"
#include "model/settings.h"
#include "viewmodel/notifications/exchange_rates_manager.h"

class WalletViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString beamAvailable                 READ beamAvailable              NOTIFY beamChanged)
    Q_PROPERTY(QString beamReceiving                 READ beamReceiving              NOTIFY beamChanged)
    Q_PROPERTY(QString beamSending                   READ beamSending                NOTIFY beamChanged)
    Q_PROPERTY(QString beamLocked                    READ beamLocked                 NOTIFY beamChanged)
    Q_PROPERTY(QString beamLockedMaturing            READ beamLockedMaturing         NOTIFY beamChanged)
    Q_PROPERTY(QString beamReceivingChange           READ beamReceivingChange        NOTIFY beamChanged)
    Q_PROPERTY(QString beamReceivingIncoming         READ beamReceivingIncoming      NOTIFY beamChanged)
    Q_PROPERTY(QString beamIcon                      READ beamIcon                   CONSTANT)
    Q_PROPERTY(QString beamName                      READ beamName                   CONSTANT)

    Q_PROPERTY(int selectedAsset                      READ getSelectedAsset         WRITE setSelectedAsset   NOTIFY assetChanged)
    Q_PROPERTY(QString assetAvailable                 READ assetAvailable                                    NOTIFY assetChanged)
    Q_PROPERTY(QString assetReceiving                 READ assetReceiving                                    NOTIFY assetChanged)
    Q_PROPERTY(QString assetSending                   READ assetSending                                      NOTIFY assetChanged)
    Q_PROPERTY(QString assetLocked                    READ assetLocked                                       NOTIFY assetChanged)
    Q_PROPERTY(QString assetLockedMaturing            READ assetLockedMaturing                               NOTIFY assetChanged)
    Q_PROPERTY(QString assetReceivingChange           READ assetReceivingChange                              NOTIFY assetChanged)
    Q_PROPERTY(QString assetReceivingIncoming         READ assetReceivingIncoming                            NOTIFY assetChanged)
    Q_PROPERTY(QString assetIcon                      READ assetIcon                                         NOTIFY assetChanged)
    Q_PROPERTY(QString assetUnitName                  READ assetUnitName                                     NOTIFY assetChanged)
    Q_PROPERTY(QString assetName                      READ assetName                                         NOTIFY assetChanged)

    Q_PROPERTY(QString secondCurrencyUnitName        READ getSecondCurrencyUnitName     NOTIFY secondCurrencyUnitNameChanged)
    Q_PROPERTY(QString secondCurrencyRate            READ getSecondCurrencyRate         NOTIFY secondCurrencyRateChanged)
    Q_PROPERTY(bool isAllowedBeamMWLinks             READ isAllowedBeamMWLinks          WRITE allowBeamMWLinks      NOTIFY beamMWLinksAllowed)


public:
    WalletViewModel();

    QString beamAvailable() const;
    QString beamReceiving() const;
    QString beamSending() const;
    QString beamLocked() const;
    QString beamLockedMaturing() const;
    QString beamReceivingChange() const;
    QString beamReceivingIncoming() const;
    QString beamIcon() const;
    QString beamName() const;
    QString assetAvailable() const;
    QString assetReceiving() const;
    QString assetSending() const;
    QString assetLocked() const;
    QString assetLockedMaturing() const;
    QString assetReceivingChange() const;
    QString assetReceivingIncoming() const;
    QString assetIcon() const;
    QString assetUnitName() const;
    QString assetName() const;
    QString getSecondCurrencyUnitName() const;
    QString getSecondCurrencyRate() const;

    int  getSelectedAsset() const;
    void setSelectedAsset(int val);

    bool getIsOfflineStatus() const;
    bool getIsFailedStatus() const;
    QString getWalletStatusErrorMsg() const;
    void allowBeamMWLinks(bool value);

    Q_INVOKABLE bool isAllowedBeamMWLinks() const;

signals:
    void beamChanged();
    void assetChanged();
    void secondCurrencyUnitNameChanged();
    void secondCurrencyRateChanged();
    void beamMWLinksAllowed();

private:
    WalletModel&         _model;
    WalletSettings&      _settings;
    ExchangeRatesManager _exchangeRatesManager1;
    beam::Asset::ID      _selectedAsset;
};
