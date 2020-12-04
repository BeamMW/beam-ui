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
#include <memory>
#include <boost/optional.hpp>
#include "wallet/transactions/swaps/bridges/ethereum/settings.h"

class SwapEthClientModel;

class SwapEthSettingsItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString         showSeedDialogTitle  READ getShowSeedDialogTitle                                 CONSTANT)
    Q_PROPERTY(QString         generalTitle         READ getGeneralTitle                                        CONSTANT)
    Q_PROPERTY(QString         title                READ getTitle                                               CONSTANT)
    Q_PROPERTY(QString         coinID               READ getCoinID                                              CONSTANT)
    Q_PROPERTY(bool            folded               READ getFolded               WRITE setFolded                NOTIFY foldedChanged)

    Q_PROPERTY(QChar           phrasesSeparator     READ getPhrasesSeparator                                    CONSTANT)
    Q_PROPERTY(QList<QObject*> seedPhrases          READ getSeedPhrases                                         NOTIFY seedPhrasesChanged)
    Q_PROPERTY(bool            isCurrentSeedValid   READ isCurrentSeedValid                                     NOTIFY isCurrentSeedValidChanged)
    Q_PROPERTY(QString         nodeAddress          READ getNodeAddress           WRITE setNodeAddress          NOTIFY nodeAddressChanged)
    Q_PROPERTY(QString         nodePort             READ getNodePort              WRITE setNodePort             NOTIFY nodePortChanged)
    Q_PROPERTY(unsigned int    accountIndex         READ getAccountIndex          WRITE setAccountIndex         NOTIFY accountIndexChanged)
    Q_PROPERTY(QString         contractAddress      READ getContractAddress       WRITE setContractAddress      NOTIFY contractAddressChanged)
    Q_PROPERTY(QString         erc20ContractAddress READ getERC20ContractAddress  WRITE setERC20ContractAddress NOTIFY erc20ContractAddressChanged)

    Q_PROPERTY(QString         daiContractAddress   READ getDaiContractAddress    WRITE setDaiContractAddress   NOTIFY daiContractAddressChanged)
    Q_PROPERTY(QString         usdtContractAddress  READ getUsdtContractAddress   WRITE setUsdtContractAddress  NOTIFY usdtContractAddressChanged)
    Q_PROPERTY(QString         wbtcContractAddress  READ getWbtcContractAddress   WRITE setWbtcContractAddress  NOTIFY wbtcContractAddressChanged)
    Q_PROPERTY(bool            activateDai          READ activateDai              WRITE activateDai             NOTIFY activateDaiChanged)
    Q_PROPERTY(bool            activateUsdt         READ activateUsdt             WRITE activateUsdt            NOTIFY activateUsdtChanged)
    Q_PROPERTY(bool            activateWBTC         READ activateWBTC             WRITE activateWBTC            NOTIFY activateWBTCChanged)

    Q_PROPERTY(bool            canEdit              READ getCanEdit                                             NOTIFY canEditChanged)
    Q_PROPERTY(bool            isConnected          READ getIsConnected                                         NOTIFY connectionChanged)
    Q_PROPERTY(QString         connectionStatus     READ getConnectionStatus                                    NOTIFY connectionStatusChanged)
    Q_PROPERTY(QString         connectionErrorMsg   READ getConnectionErrorMsg                                  NOTIFY connectionErrorMsgChanged)


public:
    SwapEthSettingsItem();
    ~SwapEthSettingsItem() override;

    Q_INVOKABLE void disconnect();
    Q_INVOKABLE void applySettings();
    Q_INVOKABLE void clearSettings();
    Q_INVOKABLE void connectToNode();
    Q_INVOKABLE void newSeedPhrases();
    Q_INVOKABLE void restoreSeedPhrases();
    Q_INVOKABLE void copySeedPhrases();
    Q_INVOKABLE void validateCurrentSeedPhrase();

private:
    QString getTitle() const;
    QString getShowSeedDialogTitle() const;
    QString getGeneralTitle() const;
    QString getCoinID() const;
    bool getFolded() const;
    void setFolded(bool value);
    QList<QObject*> getSeedPhrases();
    QChar getPhrasesSeparator() const;
    bool isCurrentSeedValid() const;
    void setIsCurrentSeedValid(bool value);
    void LoadSettings();
    void shouldConnect(bool value);
    void SetSeedPhrase(const std::vector<std::string>& secretWords);
    void SetDefaultSettings(bool clearSeed = true);
    QString getNodeAddress() const;
    void setNodeAddress(const QString& value);
    QString getNodePort() const;
    void setNodePort(const QString& value);
    unsigned int getAccountIndex() const;
    void setAccountIndex(unsigned int value);
    QString getContractAddress() const;
    QString getERC20ContractAddress() const;
    QString getDaiContractAddress() const;
    QString getUsdtContractAddress() const;
    QString getWbtcContractAddress() const;
    void setContractAddress(const QString& value);
    void setERC20ContractAddress(const QString& value);
    void setDaiContractAddress(const QString& value);
    void setUsdtContractAddress(const QString& value);
    void setWbtcContractAddress(const QString& value);
    bool activateDai() const;
    void activateDai(bool value);
    bool activateUsdt() const;
    void activateUsdt(bool value);
    bool activateWBTC() const;
    void activateWBTC(bool value);

    bool getCanEdit() const;
    bool getIsConnected() const;
    QString getConnectionStatus() const;
    QString getConnectionErrorMsg() const;

    void applyNodeAddress(const QString& address);
    std::vector<std::string> GetSeedPhraseFromSeedItems() const;

signals:
    void nodeAddressChanged();
    void nodePortChanged();
    void seedPhrasesChanged();
    void isCurrentSeedValidChanged();
    void canEditChanged();
    void connectionChanged();
    void accountIndexChanged();
    void contractAddressChanged();
    void erc20ContractAddressChanged();
    void daiContractAddressChanged();
    void usdtContractAddressChanged();
    void wbtcContractAddressChanged();
    void activateDaiChanged();
    void activateUsdtChanged();
    void activateWBTCChanged();
    void connectionStatusChanged();
    void connectionErrorMsgChanged();
    void foldedChanged();

private:
    std::weak_ptr<SwapEthClientModel> m_coinClient;
    boost::optional<beam::ethereum::Settings> m_settings;
    bool m_shouldConnect = false;
    QList<QObject*> m_seedPhraseItems;
    bool m_isCurrentSeedValid = false;
    QString m_nodeAddress;
    QString m_nodePort;
    unsigned int m_accountIndex;
    QString m_contractAddress;
    QString m_erc20ContractAddress;
    QString m_daiContractAddress;
    QString m_usdtContractAddress;
    QString m_wbtcContractAddress;
    bool m_activateDai;
    bool m_activateUsdt;
    bool m_activateWBTC;
    bool m_isFolded = true;
};