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

#include "wallet/transactions/swaps/common.h"
#include "wallet/transactions/swaps/bridges/bitcoin/settings.h"

class SwapCoinClientModel;

class SwapCoinSettingsItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString  feeRateLabel             READ getFeeRateLabel                         CONSTANT)
    Q_PROPERTY(QString  showSeedDialogTitle      READ getShowSeedDialogTitle                  CONSTANT)
    Q_PROPERTY(QString  showAddressesDialogTitle READ getShowAddressesDialogTitle             CONSTANT)
    Q_PROPERTY(QString  generalTitle             READ getGeneralTitle                         CONSTANT)
    Q_PROPERTY(QString  title                    READ getTitle                                NOTIFY titleChanged)
    Q_PROPERTY(QString  coinID                   READ getCoinID                               CONSTANT)
    Q_PROPERTY(bool     folded                   READ getFolded       WRITE setFolded         NOTIFY foldedChanged)

    // node settings
    Q_PROPERTY(QString  nodeUser     READ getNodeUser     WRITE setNodeUser       NOTIFY nodeUserChanged)
    Q_PROPERTY(QString  nodePass     READ getNodePass     WRITE setNodePass       NOTIFY nodePassChanged)
    Q_PROPERTY(QString  nodeAddress  READ getNodeAddress  WRITE setNodeAddress    NOTIFY nodeAddressChanged)
    Q_PROPERTY(QString  nodePort     READ getNodePort     WRITE setNodePort       NOTIFY nodePortChanged)
    // electrum settings
    Q_PROPERTY(bool            isSupportedElectrum      READ isSupportedElectrum                                  CONSTANT)
    Q_PROPERTY(QChar           phrasesSeparatorElectrum READ getPhrasesSeparatorElectrum                          CONSTANT)
    Q_PROPERTY(bool            isCurrentSeedValid       READ getIsCurrentSeedValid                                NOTIFY isCurrentSeedValidChanged)
    Q_PROPERTY(bool            isCurrentSeedSegwit      READ getIsCurrentSeedSegwit                               NOTIFY isCurrentSeedSegwitChanged)
    Q_PROPERTY(QList<QObject*> electrumSeedPhrases      READ getElectrumSeedPhrases                               NOTIFY electrumSeedPhrasesChanged)
    Q_PROPERTY(QString         nodeAddressElectrum      READ getNodeAddressElectrum  WRITE setNodeAddressElectrum NOTIFY nodeAddressElectrumChanged)
    Q_PROPERTY(QString         nodePortElectrum         READ getNodePortElectrum     WRITE setNodePortElectrum    NOTIFY nodePortElectrumChanged)
    Q_PROPERTY(bool            selectServerAutomatically      READ getSelectServerAutomatically  WRITE setSelectServerAutomatically NOTIFY selectServerAutomaticallyChanged)

    Q_PROPERTY(bool canEdit      READ getCanEdit                            NOTIFY canEditChanged)

    // connection properties
    Q_PROPERTY(bool isConnected             READ getIsConnected             NOTIFY connectionTypeChanged)
    Q_PROPERTY(bool isNodeConnection        READ getIsNodeConnection        NOTIFY connectionTypeChanged)
    Q_PROPERTY(bool isElectrumConnection    READ getIsElectrumConnection    NOTIFY connectionTypeChanged)
    Q_PROPERTY(QString connectionStatus     READ getConnectionStatus        NOTIFY connectionStatusChanged)
    Q_PROPERTY(QString connectionErrorMsg   READ getConnectionErrorMsg      NOTIFY connectionErrorMsgChanged)

public:
    SwapCoinSettingsItem() = default;
    SwapCoinSettingsItem(beam::wallet::AtomicSwapCoin swapCoin);
    virtual ~SwapCoinSettingsItem();

    QString getFeeRateLabel() const;

    QString getTitle() const;
    QString getShowSeedDialogTitle() const;
    QString getShowAddressesDialogTitle() const;
    QString getCoinID() const;

    bool getFolded() const;
    void setFolded(bool value);
    QString getNodeUser() const;
    void setNodeUser(const QString& value);
    QString getNodePass() const;
    void setNodePass(const QString& value);
    QString getNodeAddress() const;
    void setNodeAddress(const QString& value);
    QString getNodePort() const;
    void setNodePort(const QString& value);

    bool getIsCurrentSeedValid() const;
    bool getIsCurrentSeedSegwit() const;
    QList<QObject*> getElectrumSeedPhrases();
    QChar getPhrasesSeparatorElectrum() const;
    QString getNodeAddressElectrum() const;
    void setNodeAddressElectrum(const QString& value);
    QString getNodePortElectrum() const;
    void setNodePortElectrum(const QString& value);
    bool getSelectServerAutomatically() const;
    void setSelectServerAutomatically(bool value);
    bool isSupportedElectrum() const;

    bool getCanEdit() const;

    bool getIsConnected() const;
    bool getIsNodeConnection() const;
    bool getIsElectrumConnection() const;
    QString getConnectionStatus() const;
    QString getConnectionErrorMsg() const;

    Q_INVOKABLE void applyNodeSettings();
    Q_INVOKABLE void applyElectrumSettings();

    Q_INVOKABLE void resetNodeSettings();
    Q_INVOKABLE void resetElectrumSettings();

    Q_INVOKABLE void newElectrumSeed();
    Q_INVOKABLE void restoreSeedElectrum();

    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void connectToNode();
    Q_INVOKABLE void connectToElectrum();
    Q_INVOKABLE void copySeedElectrum();
    Q_INVOKABLE void validateCurrentElectrumSeedPhrase();

    Q_INVOKABLE QStringList getAddressesElectrum() const;

private:

    void applyNodeAddress(const QString& address);
    void applyNodeAddressElectrum(const QString& address);

signals:

    void foldedChanged();
    void titleChanged();
    void nodeUserChanged();
    void nodePassChanged();
    void nodeAddressChanged();
    void nodePortChanged();

    void isCurrentSeedValidChanged();
    void isCurrentSeedSegwitChanged();
    void electrumSeedPhrasesChanged();
    void nodeAddressElectrumChanged();
    void nodePortElectrumChanged();
    void selectServerAutomaticallyChanged();

    void canEditChanged();
    void connectionTypeChanged();
    void connectionStatusChanged();
    void connectionErrorMsgChanged();

public slots:

    void onStatusChanged();

private:
    QString getGeneralTitle() const;
    QString getConnectedNodeTitle() const;
    QString getConnectedElectrumTitle() const;

    void LoadSettings();
    void SetSeedElectrum(const std::vector<std::string>& secretWords);
    void SetDefaultNodeSettings();
    void SetDefaultElectrumSettings(bool clearSeed = true);
    void setConnectionType(beam::bitcoin::Settings::ConnectionType type);
    void setIsCurrentSeedValid(bool value);
    void setIsCurrentSeedSegwit(bool value);

    std::vector<std::string> GetSeedPhraseFromSeedItems() const;

private:
    beam::wallet::AtomicSwapCoin m_swapCoin;
    std::weak_ptr<SwapCoinClientModel> m_coinClient;

    boost::optional<beam::bitcoin::Settings> m_settings;

    beam::bitcoin::Settings::ConnectionType
        m_connectionType = beam::bitcoin::Settings::ConnectionType::None;
    QString m_nodeUser;
    QString m_nodePass;
    QString m_nodeAddress;
    QString m_nodePort;

    QList<QObject*> m_seedPhraseItems;
    QString m_nodeAddressElectrum;
    QString m_nodePortElectrum;
    bool m_selectServerAutomatically;
    bool m_isCurrentSeedValid = false;
    // "true" if current seed valid and segwit type
    bool m_isCurrentSeedSegwit = false;
    bool m_isFolded = true;
};
