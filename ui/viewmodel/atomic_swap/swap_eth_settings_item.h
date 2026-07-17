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
#include <QMap>
#include <QVariant>
#include <memory>
#include <boost/optional.hpp>
#include "wallet/transactions/swaps/bridges/ethereum/settings.h"

class SwapEthClientModel;

class SwapEthSettingsItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString         showSeedDialogTitle      READ getShowSeedDialogTitle                                 CONSTANT)
    Q_PROPERTY(QString         showAddressesDialogTitle READ getShowAddressesDialogTitle                            CONSTANT)
    Q_PROPERTY(QString         generalTitle             READ getGeneralTitle                                        CONSTANT)
    Q_PROPERTY(QString         title                    READ getTitle                                               CONSTANT)
    Q_PROPERTY(QString         coinID                   READ getCoinID                                              CONSTANT)
    Q_PROPERTY(bool            folded                   READ getFolded               WRITE setFolded                NOTIFY foldedChanged)
                                                        
    Q_PROPERTY(QChar           phrasesSeparator         READ getPhrasesSeparator                                    CONSTANT)
    Q_PROPERTY(QList<QObject*> seedPhrases              READ getSeedPhrases                                         NOTIFY seedPhrasesChanged)
    Q_PROPERTY(bool            isCurrentSeedValid       READ isCurrentSeedValid                                     NOTIFY isCurrentSeedValidChanged)
    Q_PROPERTY(QString         infuraProjectID          READ infuraProjectID          WRITE infuraProjectID         NOTIFY infuraProjectIDChanged)
    Q_PROPERTY(unsigned int    accountIndex             READ getAccountIndex          WRITE setAccountIndex         NOTIFY accountIndexChanged)

    Q_PROPERTY(bool            canChangeConnection      READ canChangeConnection                                    NOTIFY canChangeConnectionChanged)
    Q_PROPERTY(bool            isConnected              READ getIsConnected                                         NOTIFY connectionChanged)
    Q_PROPERTY(QString         connectionStatus         READ getConnectionStatus                                    NOTIFY connectionStatusChanged)
    Q_PROPERTY(QString         connectionError          READ getConnectionError                                     NOTIFY connectionErrorChanged)
    Q_PROPERTY(bool            useCustomRpc             READ useCustomRpc             WRITE setUseCustomRpc         NOTIFY useCustomRpcChanged)
    Q_PROPERTY(QString         customRpcUrl             READ customRpcUrl             WRITE setCustomRpcUrl         NOTIFY customRpcUrlChanged)
    Q_PROPERTY(QString         endpointCheckResult      READ endpointCheckResult                                    NOTIFY endpointCheckResultChanged)
    Q_PROPERTY(bool            endpointCheckOk          READ endpointCheckOk                                        NOTIFY endpointCheckResultChanged)

    Q_PROPERTY(QList<QMap<QString, QVariant>> customTokens READ getCustomTokens                                     NOTIFY customTokensChanged)
    // fixed DAI/USDT/WBTC contracts the wallet already supports natively, shown as
    // non-removable rows above the user-added custom tokens
    Q_PROPERTY(QList<QMap<QString, QVariant>> builtinTokens READ getBuiltinTokens                                   CONSTANT)

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
    Q_INVOKABLE QStringList getEthereumAddresses() const;
    Q_INVOKABLE void validateEndpoint();

    Q_INVOKABLE void lookupToken(const QString& contractAddress);
    Q_INVOKABLE void addCustomToken(const QString& contractAddress, const QString& symbol, uint decimals);
    Q_INVOKABLE void removeCustomToken(const QString& contractAddress);

private:
    QString getTitle() const;
    QString getShowSeedDialogTitle() const;
    QString getShowAddressesDialogTitle() const;
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
    QString infuraProjectID() const;
    void infuraProjectID(const QString& value);
    unsigned int getAccountIndex() const;
    void setAccountIndex(unsigned int value);

    bool canChangeConnection() const;
    bool getIsConnected() const;
    QString getConnectionStatus() const;
    QString getConnectionError() const;

    bool useCustomRpc() const;
    void setUseCustomRpc(bool value);
    QString customRpcUrl() const;
    void setCustomRpcUrl(const QString& value);
    QString endpointCheckResult() const;
    bool endpointCheckOk() const;
    void onEndpointValidated(quint64 chainID, quint64 blockNumber, bool ok, bool wrongNetwork, const QString& errorMsg);

    std::vector<std::string> GetSeedPhraseFromSeedItems() const;
    static QString getChainName(quint64 chainID);
    void clearEndpointCheck();

    QList<QMap<QString, QVariant>> getCustomTokens() const;
    QList<QMap<QString, QVariant>> getBuiltinTokens() const;
    bool isBuiltinTokenContract(const QString& normalizedAddress) const;
    void loadCustomTokens();
    void saveCustomTokens();

signals:
    void infuraProjectIDChanged();
    void seedPhrasesChanged();
    void isCurrentSeedValidChanged();
    void canChangeConnectionChanged();
    void connectionChanged();
    void accountIndexChanged();
    void connectionStatusChanged();
    void connectionErrorChanged();
    void foldedChanged();
    void useCustomRpcChanged();
    void customRpcUrlChanged();
    void endpointCheckResultChanged();
    void customTokensChanged();
    void tokenInfoReady(const QString& contract, const QString& symbol, uint decimals, const QString& error);

private:
    std::weak_ptr<SwapEthClientModel> m_coinClient;
    boost::optional<beam::ethereum::Settings> m_settings;
    bool m_shouldConnect = false;
    QList<QObject*> m_seedPhraseItems;
    bool m_isCurrentSeedValid = false;
    QString m_infuraProjectID;
    unsigned int m_accountIndex;
    bool m_isFolded = true;
    bool m_useCustomRpc = false;
    QString m_customRpcUrl;
    QString m_endpointCheckResult;
    bool m_endpointCheckOk = false;
    QList<QMap<QString, QVariant>> m_customTokens; // {"contract","symbol","decimals"}, persisted per wallet
};