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
#include <QQmlListProperty>

#include "model/settings.h"
#include "wallet/transactions/swaps/bridges/bitcoin/client.h"
#include "wallet/transactions/swaps/bridges/bitcoin/settings.h"
#include "viewmodel/notifications/notifications_settings.h"
#include "viewmodel/atomic_swap/swap_eth_settings_item.h"

class SettingsViewModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString      nodeAddress                     READ getNodeAddress                 WRITE setNodeAddress    NOTIFY nodeAddressChanged)
    Q_PROPERTY(QString      version                         READ getVersion                     CONSTANT)
    Q_PROPERTY(bool         localNodeRun                    READ getLocalNodeRun                WRITE setLocalNodeRun   NOTIFY localNodeRunChanged)
    Q_PROPERTY(QString      localNodePort                   READ getLocalNodePort               WRITE setLocalNodePort  NOTIFY localNodePortChanged)
    Q_PROPERTY(QString      remoteNodePort                  READ getRemoteNodePort              WRITE setRemoteNodePort NOTIFY remoteNodePortChanged)
    Q_PROPERTY(bool         isNodeChanged                   READ isNodeChanged                  NOTIFY nodeSettingsChanged)
    Q_PROPERTY(QStringList  localNodePeers                  READ getLocalNodePeers              NOTIFY localNodePeersChanged)
    Q_PROPERTY(int          lockTimeout                     READ getLockTimeout                 WRITE  setLockTimeout NOTIFY lockTimeoutChanged)
    Q_PROPERTY(QString      walletLocation                  READ getWalletLocation              CONSTANT)
    Q_PROPERTY(bool         isLocalNodeRunning              READ isLocalNodeRunning             NOTIFY localNodeRunningChanged)
    Q_PROPERTY(bool         isPasswordReqiredToSpendMoney   READ isPasswordReqiredToSpendMoney  WRITE   setPasswordReqiredToSpendMoney NOTIFY passwordReqiredToSpendMoneyChanged)
    Q_PROPERTY(bool         isAllowedBeamMWLinks            READ isAllowedBeamMWLinks           WRITE   allowBeamMWLinks NOTIFY beamMWLinksPermissionChanged)
    Q_PROPERTY(QStringList  supportedLanguages              READ getSupportedLanguages          NOTIFY  currentLanguageIndexChanged)
    Q_PROPERTY(int          currentLanguageIndex            READ getCurrentLanguageIndex        NOTIFY  currentLanguageIndexChanged)
    Q_PROPERTY(QString      currentLanguage                 READ getCurrentLanguage             WRITE   setCurrentLanguage)
    Q_PROPERTY(bool         isValidNodeAddress              READ isValidNodeAddress             NOTIFY  validNodeAddressChanged)
    Q_PROPERTY(QString      secondCurrency                  READ getSecondCurrency              WRITE   setSecondCurrency NOTIFY secondCurrencyChanged)
    Q_PROPERTY(QString      publicAddress                   READ getPublicAddress               NOTIFY  publicAddressChanged)
    Q_PROPERTY(QString      explorerUrl                     READ getExplorerUrl                 CONSTANT)
    Q_PROPERTY(QString      faucetUrl                       READ getFaucetUrl                   CONSTANT)
    Q_PROPERTY(int          minConfirmations                READ getMinConfirmations            WRITE  setMinConfirmations NOTIFY minConfirmationsChanged)

    Q_PROPERTY(QList<QObject*> swapCoinSettingsList READ getSwapCoinSettings    CONSTANT)
    Q_PROPERTY(QObject* notificationsSettings   READ getNotificationsSettings   CONSTANT)

    Q_PROPERTY(int  maxPrivacyAnonymitySet  READ geMaxPrivacyAnonymitySet   WRITE setMaxPrivacyAnonymitySet NOTIFY maxPrivacyAnonymitySetChanged)
    Q_PROPERTY(int  maxPrivacyLockTimeLimit READ getMaxPrivacyLockTimeLimit WRITE setMaxPrivacyLockTimeLimit NOTIFY maxPrivacyLockTimeLimitChanged)
    Q_PROPERTY(QObject* ethSettings   READ getEthSettings   CONSTANT)
    
public:

    SettingsViewModel();
    ~SettingsViewModel() override;

    QString getNodeAddress() const;
    void setNodeAddress(const QString& value);
    QString getVersion() const;
    bool getLocalNodeRun() const;
    void setLocalNodeRun(bool value);
    QString getLocalNodePort() const;
    void setLocalNodePort(const QString& value);
    QString getRemoteNodePort() const;
    void setRemoteNodePort(const QString& value);
    int getLockTimeout() const;
    void setLockTimeout(int value);
    bool isPasswordReqiredToSpendMoney() const;
    void setPasswordReqiredToSpendMoney(bool value);
    bool isAllowedBeamMWLinks();
    void allowBeamMWLinks(bool value);
    QStringList getSupportedLanguages() const;
    int getCurrentLanguageIndex() const;
    void setCurrentLanguageIndex(int value);
    QString getCurrentLanguage() const;
    void setCurrentLanguage(QString value);
    QString getExplorerUrl() const;
    QString getFaucetUrl() const;
    // Amount in second currency
    QString getSecondCurrency() const;
    void setSecondCurrency(const QString&);

    const QString& getPublicAddress() const;

    QStringList getLocalNodePeers() const;
    void setLocalNodePeers(const QStringList& localNodePeers);
    QString getWalletLocation() const;
    bool isLocalNodeRunning() const;
    bool isValidNodeAddress() const;

    bool isNodeChanged() const;

    const QList<QObject*>& getSwapCoinSettings();
    QObject* getNotificationsSettings();
    QObject* getEthSettings();

    int geMaxPrivacyAnonymitySet() const;
    void setMaxPrivacyAnonymitySet(int mpAnonymitySetIndex);

    int getMaxPrivacyLockTimeLimit() const;
    void setMaxPrivacyLockTimeLimit(int limit);

    int getMinConfirmations() const;
    void setMinConfirmations(int value);

    Q_INVOKABLE uint coreAmount() const;
    Q_INVOKABLE void addLocalNodePeer(const QString& localNodePeer);
    Q_INVOKABLE void deleteLocalNodePeer(int index);
    Q_INVOKABLE void openUrl(const QString& url);
    Q_INVOKABLE void refreshWallet();
    Q_INVOKABLE void openFolder(const QString& path);
    Q_INVOKABLE bool checkWalletPassword(const QString& password) const;
    Q_INVOKABLE QString getOwnerKey(const QString& password) const;
    Q_INVOKABLE bool exportData() const;
    Q_INVOKABLE bool importData() const;
    Q_INVOKABLE bool hasPeer(const QString& peer) const;

public slots:
    void applyChanges();
    void undoChanges();
	void reportProblem();

    void changeWalletPassword(const QString& pass);
    void onNodeStarted();
    void onNodeStopped();
    void onAddressChecked(const QString& addr, bool isValid);
private slots:
    void onPublicAddressChanged(const QString& publicAddr);

signals:
    void nodeAddressChanged();
    void localNodeRunChanged();
    void localNodePortChanged();
    void remoteNodePortChanged();
    void localNodePeersChanged();
    void nodeSettingsChanged();
    void lockTimeoutChanged();
    void localNodeRunningChanged();
    void passwordReqiredToSpendMoneyChanged();
    void validNodeAddressChanged();
    void currentLanguageIndexChanged();
    void secondCurrencyChanged();
    void beamMWLinksPermissionChanged();
    void publicAddressChanged();
    void maxPrivacyAnonymitySetChanged();
    void maxPrivacyLockTimeLimitChanged();
    void minConfirmationsChanged();
protected:
    void timerEvent(QTimerEvent *event) override;

private:
    WalletSettings& m_settings;
    QList<QObject*> m_swapSettings;
    NotificationsSettings m_notificationsSettings;
    SwapEthSettingsItem m_ethSettings;

    QString m_nodeAddress;
    bool m_localNodeRun;
    QString m_localNodePort;
    QString m_remoteNodePort;
    QStringList m_localNodePeers;
    int m_lockTimeout;
    bool m_isPasswordReqiredToSpendMoney;
    bool m_isAllowedBeamMWLinks;
    bool m_isValidNodeAddress;
    bool m_isNeedToCheckAddress;
    bool m_isNeedToApplyChanges;
    QStringList m_supportedLanguages;
    QStringList m_supportedAmountUnits;
    int m_currentLanguageIndex;
    beam::wallet::Currency m_rateCurrency;
    int m_timerId;
    QString m_publicAddress;
    mutable int m_mpAnonymitySetIndex = 0;
    mutable int m_mpLockTimeLimitIndex = 1;

    const int CHECK_INTERVAL = 1000;
};
