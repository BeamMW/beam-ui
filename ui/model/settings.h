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
#include <QSettings>
#include <QDir>
#include <QStringList>
#include <mutex>
#include "model/wallet_model.h"

#ifdef BEAM_MAINNET
#define BEAM_MAIN_BLOCKCHAIN_NAME ""
#else
#define BEAM_MAIN_BLOCKCHAIN_NAME BRANCH_NAME
#endif

#define BEAM_BLOCKCHAINS_MAP(macro) \
     macro(Main,       "master",       "") \
     macro(Sidechain1, "sidechain #1", "Sidechain1") \
     macro(Sidechain2, "sidechain #2", "Sidechain2") 



class WalletSettings : public QObject
{
    Q_OBJECT
public:
    WalletSettings(const QDir& appDataDir);

    QString getNodeAddress() const;
    QString getNodeAddress(const QString& blockchainName) const;

    void setNodeAddress(const QString& value);
    void setNodeAddress(const QString& blockchainName, const QString& value);

    int getLockTimeout() const;
    void setLockTimeout(int value);

    bool isPasswordReqiredToSpendMoney() const;
    void setPasswordReqiredToSpendMoney(bool value);

    bool isAllowedBeamMWLinks() const;
    void setAllowedBeamMWLinks(bool value);

    bool showSwapBetaWarning();
    void setShowSwapBetaWarning(bool value);

#if defined(BEAM_HW_WALLET)
    std::string getTrezorWalletStorage() const;
#endif

#define MACRO(name, name2, suffix) \
    std::string getWalletStorage##name() const; 
    BEAM_BLOCKCHAINS_MAP(MACRO)
#undef MACRO

    std::string getWalletFolder() const;
    std::string getAppDataPath() const;
    void reportProblem();

    bool getRunLocalNode(const QString& blockchain) const;
    void setRunLocalNode(const QString& blockchain, bool value);

    uint getLocalNodePort(const QString& blockchain) const;
    void setLocalNodePort(const QString& blockchain, uint port);
    std::string getLocalNodeStorage(const QString& blockchain) const;
    std::string getTempDir() const;

    QStringList getLocalNodePeers(const QString& blockchain);
    void setLocalNodePeers(const QString& blockchain, const QStringList& qPeers);

    bool getPeersPersistent(const QString& blockchain) const;

    QString getLocale() const;
    QString getLanguageName() const;
    void setLocaleByLanguageName(const QString& language);
    static QStringList getSupportedLanguages();
    static QStringList getSupportedRateUnits();

    // Second currency settings
    QString getSecondCurrency() const;
    void setSecondCurrency(const QString&);

    // Notifications settings
    bool isNewVersionActive() const;
    bool isBeamNewsActive() const;
    bool isTxStatusActive() const;
    void setNewVersionActive(bool isActive);
    void setBeamNewsActive(bool isActive);
    void setTxStatusActive(bool isActive);

    static void openFolder(const QString& path);

    // dev BEAM Apps
    QString getDevBeamAppUrl();
    QString getDevBeamAppName();

    uint8_t getMaxPrivacyAnonymitySet() const;
    void setMaxPrivacyAnonymitySet(uint8_t anonymitySet);

    void maxPrivacyLockTimeLimitInit();
    uint8_t getMaxPrivacyLockTimeLimitHours() const;
    void setMaxPrivacyLockTimeLimitHours(uint8_t lockTimeLimit);

    QString getExplorerUrl() const;
    QString getFaucetUrl() const;
    QString getAppsUrl() const;
    QString getAppsCachePath() const;
    QString getAppsStoragePath() const;

    const QList<QString>& getSupportedBlockchains();
    QString getBlockchainInFocus() const;
    void setBlockchainInFocus(const QString& name);
    void enableBlockchain(const QString& name, bool value);
    bool isBlockchainEnabled(const QString& name) const;

public:
    static const char* WalletCfg;
    static const char* LogsFolder;
    static const char* SettingsFile;
#define MACRO(name, name2, suffix) \
    static const char* WalletDBFile##name; \
    static const char* NodeDBFile##name;
    BEAM_BLOCKCHAINS_MAP(MACRO)
#undef MACRO
#if defined(BEAM_HW_WALLET)
    static const char* TrezorWalletDBFile;
#endif
    

    void applyChanges();

signals:
    void nodeAddressChanged(const QString& blockchainName, const QString& addr);
    void lockTimeoutChanged();
    void localNodeRunChanged();
    void localNodePortChanged();
    void localNodePeersChanged();
    void localNodeSynchronizedChanged();
    void localeChanged();
    void beamMWLinksChanged();
    void secondCurrencyChanged();
    void blockchainChanged();

private:
    QSettings m_data;
    QDir m_appDataDir;
    uint8_t m_mpLockTimeLimit = 0;
    mutable std::recursive_mutex m_mutex;
    using Lock = std::unique_lock<decltype(m_mutex)>;
};
