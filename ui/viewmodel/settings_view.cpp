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

#include "settings_view.h"
#include "version.h"
#include <QtQuick>
#include <QApplication>
#include <QClipboard>
#include "model/app_model.h"
#include "model/helpers.h"
#include "model/swap_coin_client_model.h"
#include <thread>
#include "wallet/core/secstring.h"
#include "qml_globals.h"
#include <algorithm>
#include <boost/algorithm/string/trim.hpp>
#include "utility/string_helpers.h"
#include "mnemonic/mnemonic.h"
#include "viewmodel/ui_helpers.h"
#include "atomic_swap/swap_settings_item.h"
#include "settings_helpers.h"

using namespace beam;
using namespace ECC;
using namespace std;

namespace
{
    const std::map<int, uint8_t> kMPAnonymitySetVariants = { {0, 64}, {1, 32}, {2, 16}, {3, 8}, {4, 4}, {5, 2} };
    const std::map<int, uint8_t> kMPLockTimeLimits = { {0, 0}, {1, 72}, {2, 60}, {3, 48}, {4, 36}, {5, 24} };
}  // namespace

SettingsViewModel::SettingsViewModel()
    : m_settings{AppModel::getInstance().getSettings()}
    , m_notificationsSettings(AppModel::getInstance().getSettings())
    , m_isValidNodeAddress{true}
    , m_isNeedToCheckAddress(false)
    , m_isNeedToApplyChanges(false)
    , m_supportedLanguages(WalletSettings::getSupportedLanguages())
    , m_rateCurrency(beam::wallet::Currency::UNKNOWN())
    , m_walletModel(AppModel::getInstance().getWalletModel())
{
    undoChanges();

    m_lockTimeout = m_settings.getLockTimeout();
    m_isPasswordReqiredToSpendMoney = m_settings.isPasswordReqiredToSpendMoney();
    m_isAllowedBeamMWLinks = m_settings.isAllowedBeamMWLinks();
    m_currentLanguageIndex = m_supportedLanguages.indexOf(m_settings.getLanguageName());
    m_rateCurrency = m_settings.getRateCurrency();

    connect(&AppModel::getInstance().getNode(), SIGNAL(startedNode()), SLOT(onNodeStarted()));
    connect(&AppModel::getInstance().getNode(), SIGNAL(stoppedNode()), SLOT(onNodeStopped()));
    connect(m_walletModel.get(), SIGNAL(addressChecked(const QString&, bool)), SLOT(onAddressChecked(const QString&, bool)));
    connect(m_walletModel.get(), SIGNAL(publicAddressChanged(const QString&)), SLOT(onPublicAddressChanged(const QString&)));
    connect(&m_settings, SIGNAL(beamMWLinksChanged()), SIGNAL(beamMWLinksPermissionChanged()));
    connect(&m_settings, &WalletSettings::dappsAllowedChanged, this, &SettingsViewModel::dappsAllowedChanged);
    connect(m_walletModel.get(), &WalletModel::walletStatusChanged, this, &SettingsViewModel::stateChanged);

    m_timerId = startTimer(CHECK_INTERVAL);
}

SettingsViewModel::~SettingsViewModel()
{
    qDeleteAll(m_swapSettings);
}

void SettingsViewModel::onNodeStarted()
{
    emit localNodeRunningChanged();
}

void SettingsViewModel::onNodeStopped()
{
    emit localNodeRunningChanged();
}

void SettingsViewModel::onAddressChecked(const QString& addr, bool isValid)
{
    if (m_nodeAddress == addr && m_isValidNodeAddress != isValid)
    {
        m_isValidNodeAddress = isValid;
        emit validNodeAddressChanged();

        if (m_isNeedToApplyChanges)
        {
            if (m_isValidNodeAddress)
                applyChanges();

            m_isNeedToApplyChanges = false;
        }
    }
}

void SettingsViewModel::onPublicAddressChanged(const QString& publicAddr)
{
    if (m_publicAddress != publicAddr)
    {
        m_publicAddress = publicAddr;
        emit publicAddressChanged();
    }
}

bool SettingsViewModel::isLocalNodeRunning() const
{
    return AppModel::getInstance().getNode().isNodeRunning();
}

bool SettingsViewModel::isValidNodeAddress() const
{
    return m_isValidNodeAddress;
}

QString SettingsViewModel::getNodeAddress() const
{
    return m_nodeAddress;
}

void SettingsViewModel::setNodeAddress(const QString& value)
{
    if (value != m_nodeAddress)
    {
        m_nodeAddress = value;

        if (!m_isNeedToCheckAddress)
        {
            m_isNeedToCheckAddress = true;
            m_timerId = startTimer(CHECK_INTERVAL);
        }

        emit nodeAddressChanged();
        emit nodeSettingsChanged();
    }
}

QString SettingsViewModel::getVersion() const
{
    return QString::fromStdString(PROJECT_VERSION);
}

bool SettingsViewModel::getLocalNodeRun() const
{
    return m_localNodeRun;
}

void SettingsViewModel::setLocalNodeRun(bool value)
{
    if (value != m_localNodeRun)
    {
        m_localNodeRun = value;

        if (!m_localNodeRun && !m_isNeedToCheckAddress)
        {
            m_isNeedToCheckAddress = true;
            m_timerId = startTimer(CHECK_INTERVAL);
        }

        emit localNodeRunChanged();
        emit nodeSettingsChanged();
    }
}

QString SettingsViewModel::getLocalNodePort() const
{
    return m_localNodePort;
}

void SettingsViewModel::setLocalNodePort(const QString& value)
{
    if (value != m_localNodePort)
    {
        m_localNodePort = value;
        emit localNodePortChanged();
        emit nodeSettingsChanged();
    }
}

int SettingsViewModel::getAppsPort() const
{
    return static_cast<int>(AppSettings().getAppsServerPort());
}

void SettingsViewModel::setAppsPort(int port)
{
    if (AppSettings().getAppsServerPort() != port)
    {
        AppSettings().setAppsServerPort(port);
        emit appsPortChanged();
    }
}

QString SettingsViewModel::getRemoteNodePort() const
{
    return m_remoteNodePort;
}

void SettingsViewModel::setRemoteNodePort(const QString& value)
{
    if (value != m_remoteNodePort)
    {
        m_remoteNodePort = value;
        emit remoteNodePortChanged();
        emit nodeSettingsChanged();
    }
}

int SettingsViewModel::getLockTimeout() const
{
    return m_lockTimeout;
}

void SettingsViewModel::setLockTimeout(int value)
{
    if (value != m_lockTimeout)
    {
        m_lockTimeout = value;
        m_settings.setLockTimeout(m_lockTimeout);
        emit lockTimeoutChanged();
    }
}

bool SettingsViewModel::isPasswordReqiredToSpendMoney() const
{
    return m_isPasswordReqiredToSpendMoney;
}

void SettingsViewModel::setPasswordReqiredToSpendMoney(bool value)
{
    if (value != m_isPasswordReqiredToSpendMoney)
    {
        m_isPasswordReqiredToSpendMoney = value;
        m_settings.setPasswordReqiredToSpendMoney(m_isPasswordReqiredToSpendMoney);
        emit passwordReqiredToSpendMoneyChanged();
    }
}

bool SettingsViewModel::isAllowedBeamMWLinks()
{
    m_isAllowedBeamMWLinks = m_settings.isAllowedBeamMWLinks();
    return m_isAllowedBeamMWLinks;
}

void SettingsViewModel::allowBeamMWLinks(bool value)
{
    if (value != m_isAllowedBeamMWLinks)
    {
        m_settings.setAllowedBeamMWLinks(value);
    }
}

bool SettingsViewModel::getDAppsAllowed () const
{
    return m_settings.getAppsAllowed();
}

void SettingsViewModel::setDAppsAllowed (bool val)
{
    m_settings.setAppsAllowed(val);
}

QString SettingsViewModel::getCurrentHeight() const
{
    return QString::fromStdString(to_string(m_walletModel->getCurrentStateID().m_Height));
}

QStringList SettingsViewModel::getSupportedLanguages() const
{
    return m_supportedLanguages;
}

int SettingsViewModel::getCurrentLanguageIndex() const
{
    return m_currentLanguageIndex;
}

void SettingsViewModel::setCurrentLanguageIndex(int value)
{
    m_currentLanguageIndex = value;
    m_settings.setLocaleByLanguageName(
            m_supportedLanguages[m_currentLanguageIndex]);
    emit currentLanguageIndexChanged();
}

QString SettingsViewModel::getCurrentLanguage() const
{
    return m_supportedLanguages[m_currentLanguageIndex];
}

void SettingsViewModel::setCurrentLanguage(QString value)
{
    auto index = m_supportedLanguages.indexOf(value);
    if (index != -1 )
    {
        setCurrentLanguageIndex(index);
    }
}

QString SettingsViewModel::getSecondCurrency() const
{
    return QString::fromStdString(m_rateCurrency.m_value);
}

void SettingsViewModel::setSecondCurrency(const QString& value)
{
    const auto currency = beam::wallet::Currency(value.toStdString());

    m_rateCurrency = currency;
    m_settings.setRateCurrency(currency);

    emit secondCurrencyChanged();
}

const QString& SettingsViewModel::getPublicAddress() const
{
    if (m_publicAddress.isEmpty())
    {
        AppModel::getInstance().getWalletModel()->getAsync()->getPublicAddress();
    }
    return m_publicAddress;
}

uint SettingsViewModel::coreAmount() const
{
    return std::thread::hardware_concurrency();
}

bool SettingsViewModel::hasPeer(const QString& peer) const
{
    return m_localNodePeers.contains(peer, Qt::CaseInsensitive);
}

void SettingsViewModel::addLocalNodePeer(const QString& localNodePeer)
{
    m_localNodePeers.push_back(localNodePeer);
    emit localNodePeersChanged();
    emit nodeSettingsChanged();
}

void SettingsViewModel::deleteLocalNodePeer(int index)
{
    m_localNodePeers.removeAt(index);
    emit localNodePeersChanged();
    emit nodeSettingsChanged();
}

void SettingsViewModel::openUrl(const QString& url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void SettingsViewModel::refreshWallet()
{
    AppModel::getInstance().getWalletModel()->getAsync()->rescan();
}

void SettingsViewModel::openFolder(const QString& path)
{
    WalletSettings::openFolder(path);
}

bool SettingsViewModel::checkWalletPassword(const QString& oldPass) const
{
    SecString secretPass = oldPass.toStdString();
    return AppModel::getInstance().checkWalletPassword(secretPass);
}

QString SettingsViewModel::getOwnerKey(const QString& password) const
{
    SecString secretPass = password.toStdString();
    const auto& ownerKey = AppModel::getInstance().getWalletModel()->exportOwnerKey(secretPass);
    return QString::fromStdString(ownerKey);
}

bool SettingsViewModel::isNodeChanged() const
{
    return formatAddress(m_nodeAddress, m_remoteNodePort) != m_settings.getNodeAddress()
        || m_localNodeRun != m_settings.getRunLocalNode()
        || static_cast<uint>(m_localNodePort.toInt()) != m_settings.getLocalNodePort()
        || m_localNodePeers != m_settings.getLocalNodePeers();
}

void SettingsViewModel::applyChanges()
{
    if (!m_localNodeRun && m_isNeedToCheckAddress)
    {
        m_isNeedToApplyChanges = true;
        return;
    }

    m_settings.setNodeAddress(formatAddress(m_nodeAddress, m_remoteNodePort));
    m_settings.setRunLocalNode(m_localNodeRun);
    m_settings.setLocalNodePort(m_localNodePort.toInt());
    m_settings.setLocalNodePeers(m_localNodePeers);
    m_settings.applyChanges();
    emit nodeSettingsChanged();
}

QStringList SettingsViewModel::getLocalNodePeers() const
{
    return m_localNodePeers;
}

void SettingsViewModel::setLocalNodePeers(const QStringList& localNodePeers)
{
    m_localNodePeers = localNodePeers;
    emit localNodePeersChanged();
    emit nodeSettingsChanged();
}

QString SettingsViewModel::getWalletLocation() const
{
    return QString::fromStdString(m_settings.getAppDataPath());
}

void SettingsViewModel::undoChanges()
{
    auto unpackedAddress = parseAddress(m_settings.getNodeAddress());
    setNodeAddress(unpackedAddress.address);
    if (unpackedAddress.port > 0)
    {
        setRemoteNodePort(unpackedAddress.port);
    }

    setLocalNodeRun(m_settings.getRunLocalNode());
    setLocalNodePort(formatPort(m_settings.getLocalNodePort()));
    setLocalNodePeers(m_settings.getLocalNodePeers());
}

void SettingsViewModel::reportProblem()
{
    m_settings.reportProblem();
}

bool SettingsViewModel::exportData() const
{
    return AppModel::getInstance().exportData();
}

bool SettingsViewModel::importData() const
{
    return AppModel::getInstance().importData();
}

void SettingsViewModel::changeWalletPassword(const QString& pass)
{
    AppModel::getInstance().changeWalletPassword(pass.toStdString());
}

void SettingsViewModel::timerEvent(QTimerEvent *event)
{
    if (m_isNeedToCheckAddress && !m_localNodeRun)
    {
        m_isNeedToCheckAddress = false;
        AppModel::getInstance().getWalletModel()->getAsync()->checkNetworkAddress(m_nodeAddress.toStdString());
        killTimer(m_timerId);
    }
}

const QList<QObject*>& SettingsViewModel::getSwapCoinSettings()
{
    if (m_swapSettings.empty())
    {
        m_swapSettings.push_back(new SwapCoinSettingsItem(beam::wallet::AtomicSwapCoin::Bitcoin));
        m_swapSettings.push_back(new SwapCoinSettingsItem(beam::wallet::AtomicSwapCoin::Litecoin));
        m_swapSettings.push_back(new SwapCoinSettingsItem(beam::wallet::AtomicSwapCoin::Qtum));
#if defined(BITCOIN_CASH_SUPPORT)
        m_swapSettings.push_back(new SwapCoinSettingsItem(beam::wallet::AtomicSwapCoin::Bitcoin_Cash));
#endif // BITCOIN_CASH_SUPPORT
        m_swapSettings.push_back(new SwapCoinSettingsItem(beam::wallet::AtomicSwapCoin::Dogecoin));
        m_swapSettings.push_back(new SwapCoinSettingsItem(beam::wallet::AtomicSwapCoin::Dash));
    }
    return m_swapSettings;
}

QObject* SettingsViewModel::getNotificationsSettings()
{
    return &m_notificationsSettings;
}

int SettingsViewModel::geMaxPrivacyAnonymitySet() const
{
    auto anonymitySetValue = m_settings.getMaxPrivacyAnonymitySet();
    const auto it = std::find_if(
          kMPAnonymitySetVariants.begin(),
          kMPAnonymitySetVariants.end(),
          [anonymitySetValue](const auto& mo) {return mo.second == anonymitySetValue; });
    if (it != kMPAnonymitySetVariants.end())
        m_mpAnonymitySetIndex = it->first;
    return m_mpAnonymitySetIndex;
}

void SettingsViewModel::setMaxPrivacyAnonymitySet(int mpAnonymitySetIndex)
{
    if (m_mpAnonymitySetIndex != mpAnonymitySetIndex)
    {
        const auto it = kMPAnonymitySetVariants.find(mpAnonymitySetIndex);
        if (it != kMPAnonymitySetVariants.end())
        {
            m_mpAnonymitySetIndex = mpAnonymitySetIndex;
            m_settings.setMaxPrivacyAnonymitySet(it->second);
            emit maxPrivacyAnonymitySetChanged();
        }
    }
}

int SettingsViewModel::getMaxPrivacyLockTimeLimit() const
{
    auto limit = m_settings.getMaxPrivacyLockTimeLimitHours();
    const auto it = std::find_if(
          kMPLockTimeLimits.begin(),
          kMPLockTimeLimits.end(),
          [limit](const auto& mo) {return mo.second == limit; });
    if (it != kMPLockTimeLimits.end())
        m_mpLockTimeLimitIndex = it->first;
    return m_mpLockTimeLimitIndex;
}

void SettingsViewModel::setMaxPrivacyLockTimeLimit(int limit)
{
    if (m_mpLockTimeLimitIndex != limit)
    {
        const auto it = kMPLockTimeLimits.find(limit);
        if (it != kMPLockTimeLimits.end())
        {
            m_mpLockTimeLimitIndex = limit;
            m_settings.setMaxPrivacyLockTimeLimitHours(it->second);
            emit maxPrivacyLockTimeLimitChanged();
        }
    }
}

int SettingsViewModel::getMinConfirmations() const
{
    return m_settings.getMinConfirmations();
}

void SettingsViewModel::setMinConfirmations(int value)
{
    m_settings.setMinConfirmations(value);
}

QString SettingsViewModel::getExplorerUrl() const
{
    return m_settings.getExplorerUrl();
}

QString SettingsViewModel::getFaucetUrl() const
{
    return m_settings.getFaucetUrl();
}

QObject* SettingsViewModel::getEthSettings()
{
    return &m_ethSettings;
}