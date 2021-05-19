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


class WalletSettings : public QObject
{
    Q_OBJECT
public:
    WalletSettings(const QDir& appDataDir);

    QString getNodeAddress() const;
    void setNodeAddress(const QString& value);

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
    std::string getWalletStorage() const;
    std::string getWalletFolder() const;
    std::string getAppDataPath() const;
    void reportProblem();

    bool getRunLocalNode() const;
    void setRunLocalNode(bool value);

    uint getLocalNodePort() const;
    void setLocalNodePort(uint port);
    std::string getLocalNodeStorage() const;
    std::string getTempDir() const;

    QStringList getLocalNodePeers();
    void setLocalNodePeers(const QStringList& qPeers);

    bool getPeersPersistent() const;

    QString getLocale() const;
    QString getLanguageName() const;
    void setLocaleByLanguageName(const QString& language);
    static QStringList getSupportedLanguages();

    // Second currency settings
    beam::wallet::Currency getRateCurrency() const;
    void setRateCurrency(const beam::wallet::Currency&);

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
    QString getDevAppApiVer();

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

public:
    static const char* WalletCfg;
    static const char* LogsFolder;
    static const char* SettingsFile;
    static const char* WalletDBFile;
#if defined(BEAM_HW_WALLET)
    static const char* TrezorWalletDBFile;
#endif
    static const char* NodeDBFile;

    void applyChanges();

signals:
    void nodeAddressChanged();
    void lockTimeoutChanged();
    void localNodeRunChanged();
    void localNodePortChanged();
    void localNodePeersChanged();
    void localeChanged();
    void beamMWLinksChanged();
    void secondCurrencyChanged();

private:
    QSettings m_data;
    QDir m_appDataDir;
    uint8_t m_mpLockTimeLimit = 0;
    mutable std::recursive_mutex m_mutex;
    using Lock = std::unique_lock<decltype(m_mutex)>;
};
