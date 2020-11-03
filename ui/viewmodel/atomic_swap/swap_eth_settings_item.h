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
    Q_PROPERTY(QString  showSeedDialogTitle      READ getShowSeedDialogTitle                  CONSTANT)
    Q_PROPERTY(QString  generalTitle             READ getGeneralTitle                         CONSTANT)
    Q_PROPERTY(QString  title                    READ getTitle                                CONSTANT)
    Q_PROPERTY(QChar           phrasesSeparator READ getPhrasesSeparator                          CONSTANT)
    Q_PROPERTY(QList<QObject*> seedPhrases      READ getSeedPhrases                               NOTIFY seedPhrasesChanged)
    Q_PROPERTY(bool            isCurrentSeedValid       READ isCurrentSeedValid                                NOTIFY isCurrentSeedValidChanged)
    Q_PROPERTY(QString         nodeAddress      READ getNodeAddress  WRITE setNodeAddress NOTIFY nodeAddressChanged)
    Q_PROPERTY(QString         nodePort         READ getNodePort     WRITE setNodePort    NOTIFY nodePortChanged)
    Q_PROPERTY(unsigned int    accountIndex     READ getAccountIndex WRITE setAccountIndex NOTIFY accountIndexChanged)
    Q_PROPERTY(QString         contractAddress  READ getContractAddress WRITE setContractAddress    NOTIFY contractAddressChanged)

    Q_PROPERTY(bool canEdit      READ getCanEdit                            NOTIFY canEditChanged)
    Q_PROPERTY(bool isConnected             READ getIsConnected             NOTIFY connectionChanged)


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
    void setContractAddress(const QString& value);

    bool getCanEdit() const;
    bool getIsConnected() const;

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
};