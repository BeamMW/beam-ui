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
#include "settings.h"
#include <algorithm>
#include <utility>
#include <map>
#include <QDataStream>
#include <QFileDialog>
#include <QtGlobal>
#include <QtQuick>
#include "model/app_model.h"
#include "wallet/core/default_peers.h"
#include "version.h"
#include "wallet/client/extensions/news_channels/interface.h"
#include "viewmodel/applications/public.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "wallet/client/apps_api/apps_utils.h"

#ifdef BEAM_IPFS_SUPPORT
#include "utility/cli/options.h"
#endif
using namespace beam;
namespace
{
    const char* kAccountLabel = "account/label";
    const char* kAccountPicture = "account/picture";
    const char* kNodeAddressName = "node/address";
    const char* kLocaleName = "locale";
    const char* kLockTimeoutName = "lock_timeout";
    const char* kRequirePasswordToSpendMoney = "require_password_to_spend_money";
    const char* kIsAlowedBeamMWLink = "beam_mw_links_allowed";
    const char* kDAppsAllowed = "dapps_allowed";
    const char* kshowSwapBetaWarning = "show_swap_beta_warning";
    const char* kRateUnit = "rateUnit";
    const char* kLastAssetSelection = "lastAssetSelection";
    const char* kShowFaucetPromo = "showFaucetPromo";
    const char* kHideSeedValidationPromo = "hideSeedValidationPromo";
    const char* kDevMode ="dev_mode";

    const char* kLocalNodeRun = "localnode/run";
    const char* kLocalNodePort = "localnode/port";
    const char* kLocalNodePeers = "localnode/peers";
    const char* kLocalNodePeersPersistent = "localnode/peers_persistent";
    const char* kDefaultLocale = "en_US";

    const char* kNewVersionActive = "notifications/software_release";
    const char* kBeamNewsActive = "notifications/beam_news";
    const char* kTxStatusActive = "notifications/tx_status";

    const char* kDevAppURL       = "devapp/url";
    const char* kDevAppName      = "devapp/name";
    const char* kDevAppApiVer    = "devapp/api_version";
    const char* kDevAppMinApiVer = "devapp/min_api_version";
    const char* kLocalAppsPort   = "apps/local_port";
    const char* kShadersPrivLvl  = "apps/shaders_privilege";
    const char* kIPFSPrefix      = "ipfsnode/";
    const char* kIPFSNodeStart   = "ipfs_node_start";

    const char* kDappStoreCID = "dappstore/cid";
    const char* kDappStorePath = "dappstore/path";
    const char* kDappStoreUserPublishers = "dappstore/publishers";
    const char* kDappStoreUserUnwantedPublishers = "dappstore/unwanted_publishers";

    const char* kMpAnonymitySet = "max_privacy/anonymity_set";
    const uint8_t kDefaultMaxPrivacyAnonymitySet = 64;

#ifdef BEAM_ASSET_SWAP_SUPPORT
    const char* kAllowedAssets = "assets/allowed";
#endif  // BEAM_ASSET_SWAP_SUPPORT

    std::pair<QString, QString> MakeLanguageCodePair(QString languageCode)
    {
        return std::make_pair(languageCode, QLocale(languageCode).nativeLanguageName());
    }

    const std::map<QString, QString> kSupportedLangs { 
        MakeLanguageCodePair("zh_CN"),
        MakeLanguageCodePair("en_US"),
        MakeLanguageCodePair("es_ES"),
        MakeLanguageCodePair("be_BY"),
        MakeLanguageCodePair("cs_CZ"),
        MakeLanguageCodePair("de_DE"),
        MakeLanguageCodePair("nl_NL"),
        MakeLanguageCodePair("fr_FR"),
        MakeLanguageCodePair("id_ID"),
        MakeLanguageCodePair("it_IT"),
        MakeLanguageCodePair("ja_JP"),
        MakeLanguageCodePair("ru_RU"),
        MakeLanguageCodePair("sr_SR"),
        MakeLanguageCodePair("fi_FI"),
        MakeLanguageCodePair("sv_SE"),
        MakeLanguageCodePair("th_TH"),
        MakeLanguageCodePair("tr_TR"),
        MakeLanguageCodePair("vi_VI"),
        MakeLanguageCodePair("ko_KR"),
        MakeLanguageCodePair("uk_UA"),
        MakeLanguageCodePair("pt_BR")
    };

    const char* kTxFilterInProgress = "tx_filter/inProgress";
    const char* kTxFilterCompleted = "tx_filter/completed";
    const char* kTxFilterCanceled= "tx_filter/canceled";
    const char* kTxFilterFailed = "tx_filter/failed";

    const std::vector<std::string> kOutDatedPeers = beam::getOutdatedDefaultPeers();
    bool isOutDatedPeer(const std::string& peer)
    {
        return find(kOutDatedPeers.begin(), kOutDatedPeers.end(), peer) !=
               kOutDatedPeers.end();
    }

    const beam::wallet::Currency& getDefaultRateUnit()
    {
        return beam::wallet::Currency::USD();
    }

    const std::vector<beam::wallet::Currency>& getSupportedRateUnits()
    {
        static const std::vector<beam::wallet::Currency> supportedUnits {
            beam::wallet::Currency::UNKNOWN(),
            beam::wallet::Currency::USD(),
            beam::wallet::Currency::BTC(),
            beam::wallet::Currency::ETH()
        };
        return supportedUnits;
    }

    uint getNetworkDefaultPort()
    {
        switch (Rules::get().m_Network)
        {
        case Rules::Network::masternet:
        case Rules::Network::mainnet:
            return 10005;
        default:
            return 11005;
        }
    }
    const char* getNetworkDappStoreCID()
    {
        switch (Rules::get().m_Network)
        {
        case Rules::Network::testnet:
            return "c673c2b940d4f6813901165c426ab084e401259c9794d61e1f5f80453ee80317";
        case Rules::Network::dappnet:
            return "59c7b485463eff35c361157038bad32f88a4ef9814f0891298a5e65099b6b50b";
        case Rules::Network::masternet:
            return "b76ca089082e38b23d5e68feeb8b6f459ae74f5012eb520c87169f88ced307e3";
        case Rules::Network::mainnet:
        default:
            return "e2d24b686e8d31a0fe97eade9cd23281e7059b74b5757bdb96c820ef9e2af41c";
        }
    }
}

const char* WalletSettings::WalletCfg = "beam-wallet.cfg";
const char* WalletSettings::LogsFolder = "logs";
const char* WalletSettings::SettingsFile = "settings.ini";
const char* WalletSettings::WalletDBFile = "wallet.db";
const char* WalletSettings::NodeDBFile = "node.db";
#if defined(Q_OS_MACOS)
const char* WalletSettings::DappsStoreWasm = "../Resources/dapps_store_app.wasm";
#else
const char* WalletSettings::DappsStoreWasm = "dapps_store_app.wasm";
#endif

#if defined(BEAM_HW_WALLET)
const char* WalletSettings::TrezorWalletDBFile = "trezor-wallet.db";
#endif

WalletSettings::AccountSettings::AccountSettings(QString settingsPath)
    : m_data{ settingsPath, QSettings::IniFormat }
{
    LOG_INFO() << "User UI Settings file: " << m_data.fileName().toStdString();
    const auto devapp = m_data.value(kDevAppName).toString().toStdString();
    if (!devapp.empty())
    {
        LOG_INFO() << "DevApp Name: " << devapp;
    }
}

WalletSettings::WalletSettings(const QDir& appDataDir, const QString& applicationDirPath)
    : m_accountIndex{0}
    , m_appDataDir{appDataDir}
    , m_accountSettings{getAccountDataDir().filePath(SettingsFile)}
    , m_globalData{ appDataDir.filePath(SettingsFile), QSettings::IniFormat }
    , m_applicationDirPath{applicationDirPath}
{
    LOG_INFO () << "Global UI Settings file: " << m_globalData.fileName().toStdString();
}

void WalletSettings::changeAccount(int accountIndex)
{
    Lock lock(m_mutex);
    m_accountSettings.~AccountSettings();
    m_accountIndex = accountIndex;
    new(&m_accountSettings) AccountSettings(getAccountDataDir().filePath(SettingsFile));
}

#if defined(BEAM_HW_WALLET)
string WalletSettings::getTrezorWalletStorage() const
{
    return getWalletFolder() + "/" + TrezorWalletDBFile;
}
#endif

std::string WalletSettings::getWalletStorage() const
{
    return getWalletFolder() + "/" + WalletDBFile;
}

std::string WalletSettings::getWalletFolder() const
{
    QDir rootDir = getAccountDataDir();

    auto version = QString::fromStdString(PROJECT_VERSION);
    if (!rootDir.exists(version))
    {
        rootDir.mkdir(version);
    }

    return rootDir.filePath(version).toStdString();
}

std::string WalletSettings::getAppDataPath() const
{
    Lock lock(m_mutex);
    return m_appDataDir.path().toStdString();
}

std::string WalletSettings::getUserDataPath() const
{
    return getAccountDataDir().path().toStdString();
}

QDir WalletSettings::getAccountDataDir() const
{
    QDir temp(m_appDataDir);
    const char* networkName = Rules::get().get_NetworkName();
    temp.mkdir(networkName);
    temp.cd(networkName);

    ///
    QString accountName(QString("Account%1").arg(m_accountIndex+1));
    temp.mkdir(accountName);
    temp.cd(accountName);

    return temp;
}

QString WalletSettings::getNodeAddress() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kNodeAddressName).toString();
}

void WalletSettings::setNodeAddress(const QString& addr)
{
    if (addr != getNodeAddress())
    {
        auto walletModel = AppModel::getInstance().getWalletModelUnsafe();
        if (walletModel)
        {
            walletModel->getAsync()->setNodeAddress(addr.toStdString());
        }
        {
            Lock lock(m_mutex);
            m_accountSettings.m_data.setValue(kNodeAddressName, addr);
        }
        
        emit nodeAddressChanged();
    }
    
}

int WalletSettings::getLockTimeout() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kLockTimeoutName, 0).toInt();
}

void WalletSettings::setLockTimeout(int value)
{
    if (value != getLockTimeout())
    {
        {
            Lock lock(m_mutex);
            m_accountSettings.m_data.setValue(kLockTimeoutName, value);
        }
        emit lockTimeoutChanged();
    }
}

bool WalletSettings::isPasswordRequiredToSpendMoney() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kRequirePasswordToSpendMoney, false).toBool();
}

void WalletSettings::setPasswordRequiredToSpendMoney(bool value)
{
    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kRequirePasswordToSpendMoney, value);
}

bool WalletSettings::isAllowedBeamMWLinks() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kIsAlowedBeamMWLink, false).toBool();
}

void WalletSettings::setAllowedBeamMWLinks(bool value)
{
    {
        Lock lock(m_mutex);
        m_accountSettings.m_data.setValue(kIsAlowedBeamMWLink, value);
    }
    emit beamMWLinksChanged();
}

bool WalletSettings::getAppsAllowed() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kDAppsAllowed, false).toBool();
}

bool WalletSettings::getDevMode() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kDevMode, false).toBool();
}

void WalletSettings::setAppsAllowed(bool value)
{
    bool changed = false;

    {
        Lock lock(m_mutex);
        if (m_accountSettings.m_data.value(kDAppsAllowed, false).toBool() != value)
        {
            m_accountSettings.m_data.setValue(kDAppsAllowed, value);
            changed = true;
        }
    }

    if (changed)
    {
        emit dappsAllowedChanged();
    }
}

bool WalletSettings::showSwapBetaWarning()
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kshowSwapBetaWarning, true).toBool();
}

void WalletSettings::setShowSwapBetaWarning(bool value)
{
    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kshowSwapBetaWarning, value);
}

bool WalletSettings::getRunLocalNode() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kLocalNodeRun, false).toBool();
}

void WalletSettings::setRunLocalNode(bool value)
{
    {
        Lock lock(m_mutex);
        m_accountSettings.m_data.setValue(kLocalNodeRun, value);
    }
    emit localNodeRunChanged();
}

uint WalletSettings::getLocalNodePort() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kLocalNodePort, getNetworkDefaultPort()).toUInt();
}

void WalletSettings::setLocalNodePort(uint port)
{
    {
        Lock lock(m_mutex);
        m_accountSettings.m_data.setValue(kLocalNodePort, port);
    }
    emit localNodePortChanged();
}

std::string WalletSettings::getLocalNodeStorage() const
{
    return getAccountDataDir().filePath(NodeDBFile).toStdString();
}

std::string WalletSettings::getTempDir() const
{
    return getAccountDataDir().filePath("./temp").toStdString();
}

static void zipLocalFile(QuaZip& zip, const QString& path, const QString& folder = QString())
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly))
    {
        QuaZipFile zipFile(&zip);

        zipFile.open(QIODevice::WriteOnly, QuaZipNewInfo((folder.isEmpty() ? "" : folder) + QFileInfo(file).fileName(), file.fileName()));
        zipFile.write(file.readAll());
        file.close();
        zipFile.close();
    }
}

QStringList WalletSettings::getLocalNodePeers()
{
    Lock lock(m_mutex);
    auto peers = m_accountSettings.m_data.value(kLocalNodePeers).value<QStringList>();

    size_t outDatedCount = std::count_if(
        peers.begin(),
        peers.end(),
        [] (const QString& peer)
        {
            return isOutDatedPeer(peer.toStdString());
        });

    if (outDatedCount >= static_cast<size_t>(peers.size()) || peers.empty())
    {
        auto defaultPeers = beam::getDefaultPeers();
        peers.clear();
        peers.reserve(static_cast<int>(defaultPeers.size()));
        for (const auto& it : defaultPeers)
        {
            peers << QString::fromStdString(it);
        }
        m_accountSettings.m_data.setValue(kLocalNodePeers, QVariant::fromValue(peers));
    }

    return peers;
}

void WalletSettings::setLocalNodePeers(const QStringList& qPeers)
{
    {
        Lock lock(m_mutex);
        m_accountSettings.m_data.setValue(kLocalNodePeers, QVariant::fromValue(qPeers));
    }
    emit localNodePeersChanged();
}

bool WalletSettings::getPeersPersistent() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kLocalNodePeersPersistent, false).toBool();
}

QString WalletSettings::getLocale() const
{
    QString savedLocale;
    {
        Lock lock(m_mutex);
        savedLocale = m_globalData.value(kLocaleName).toString();
    }

    if (!savedLocale.isEmpty()) {
        const auto& it = kSupportedLangs.find(savedLocale);
        if (it != kSupportedLangs.end())
        {
            return savedLocale;
        }
    }

    return QString::fromUtf8(kDefaultLocale);
}

QString WalletSettings::getLanguageName() const
{
    return kSupportedLangs.at(getLocale());
}

void WalletSettings::setLocaleByLanguageName(const QString& language)
{
    const auto& it = std::find_if(
            kSupportedLangs.begin(),
            kSupportedLangs.end(),
            [language] (const auto& mapedObject) -> bool
            {
                return mapedObject.second == language;
            });
    auto locale = 
            it != kSupportedLangs.end()
                ? it->first
                : QString::fromUtf8(kDefaultLocale);
    {
        Lock lock(m_mutex);
        m_globalData.setValue(kLocaleName, locale);
    }
    emit localeChanged();
}

beam::wallet::Currency WalletSettings::getRateCurrency() const
{
    const auto& defaultUnit = getDefaultRateUnit();
    const auto& supportedUnits = getSupportedRateUnits();
    Lock lock(m_mutex);

    auto rawUnitValue = m_accountSettings.m_data.value(kRateUnit, QString::fromStdString(defaultUnit.m_value)).toString();
    beam::wallet::Currency savedAmountUnit(rawUnitValue.toStdString());

    const auto it = find(std::begin(supportedUnits), std::cend(supportedUnits), savedAmountUnit);
    if (it == std::cend(supportedUnits))
    {
        return defaultUnit;
    }
    else
    {
        return savedAmountUnit;
    }
}

void WalletSettings::setRateCurrency(const beam::wallet::Currency& curr)
{
    const auto& supportedUnits = getSupportedRateUnits();

    const auto& it = std::find(supportedUnits.begin(), supportedUnits.end(), curr);
    auto unit = it != supportedUnits.end() ? curr : getDefaultRateUnit();

    {
        Lock lock(m_mutex);
        m_accountSettings.m_data.setValue(kRateUnit, QString::fromStdString(unit.m_value));
        emit secondCurrencyChanged();
    }
}

bool WalletSettings::isNewVersionActive() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kNewVersionActive, true).toBool();
}

bool WalletSettings::isBeamNewsActive() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kBeamNewsActive, true).toBool();
}

bool WalletSettings::isTxStatusActive() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kTxStatusActive, true).toBool();
}

void WalletSettings::setNewVersionActive(bool isActive)
{
    if (isActive != isNewVersionActive())
    {
        auto walletModel = AppModel::getInstance().getWalletModel();
        if (walletModel)
        {
            walletModel->getAsync()->switchOnOffNotifications(
                beam::wallet::Notification::Type::SoftwareUpdateAvailable,
                isActive);
        }
        Lock lock(m_mutex);
        m_accountSettings.m_data.setValue(kNewVersionActive, isActive);
    }
}

void WalletSettings::setBeamNewsActive(bool isActive)
{
    if (isActive != isBeamNewsActive())
    {
        auto walletModel = AppModel::getInstance().getWalletModel();
        if (walletModel)
        {
            walletModel->getAsync()->switchOnOffNotifications(
                beam::wallet::Notification::Type::BeamNews,
                isActive);
        }
        Lock lock(m_mutex);
        m_accountSettings.m_data.setValue(kBeamNewsActive, isActive);
    }
}

void WalletSettings::setTxStatusActive(bool isActive)
{
    if (isActive != isTxStatusActive())
    {
        auto walletModel = AppModel::getInstance().getWalletModel();
        if (walletModel)
        {
            auto asyncModel = walletModel->getAsync();
            asyncModel->switchOnOffNotifications(
                beam::wallet::Notification::Type::TransactionCompleted,
                isActive);
            asyncModel->switchOnOffNotifications(
                beam::wallet::Notification::Type::TransactionFailed,
                isActive);
        }
        Lock lock(m_mutex);
        m_accountSettings.m_data.setValue(kTxStatusActive, isActive);
    }
}

uint8_t WalletSettings::getMaxPrivacyAnonymitySet() const
{
    Lock lock(m_mutex);
    return static_cast<uint8_t>(m_accountSettings.m_data.value(kMpAnonymitySet, kDefaultMaxPrivacyAnonymitySet).toUInt());
}

void WalletSettings::setMaxPrivacyAnonymitySet(uint8_t anonymitySet)
{
    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kMpAnonymitySet, anonymitySet);
}

void WalletSettings::maxPrivacyLockTimeLimitInit()
{
    auto walletModel = AppModel::getInstance().getWalletModel();
    if (walletModel)
    {
        walletModel->getAsync()->getMaxPrivacyLockTimeLimitHours([this] (uint8_t limit)
        {
            Lock lock(m_mutex);
            m_mpLockTimeLimit = limit;
        });
    }
}

uint8_t WalletSettings::getMaxPrivacyLockTimeLimitHours() const
{
    Lock lock(m_mutex);
    return m_mpLockTimeLimit;
}

void WalletSettings::setMaxPrivacyLockTimeLimitHours(uint8_t lockTimeLimit)
{
    if (m_mpLockTimeLimit != lockTimeLimit)
    {
        auto walletModel = AppModel::getInstance().getWalletModel();
        if (walletModel)
        {
            {
                Lock lock(m_mutex);
                m_mpLockTimeLimit = lockTimeLimit;
            }
            walletModel->getAsync()->setMaxPrivacyLockTimeLimitHours(lockTimeLimit);
        }
    }
}

QStringList WalletSettings::getSupportedLanguages()
{
    QStringList languagesNames;
    std::transform(kSupportedLangs.begin(),
                   kSupportedLangs.end(),
                   std::back_inserter(languagesNames),
                   [] (const auto& lang) -> QString {
                       return lang.second;
                   });
    return languagesNames;
}

void WalletSettings::openFolder(const QString& path)
{
    QFileInfo fileInfo(path);
    QDesktopServices::openUrl(
        QUrl::fromLocalFile(
            fileInfo.isFile() ? fileInfo.absolutePath() : path));
}

void WalletSettings::reportProblem()
{
    auto logsFolder = QString::fromStdString(LogsFolder) + "/";

    QDir dataDir = getAccountDataDir();

    QFile zipFile = dataDir.filePath("beam v" + QString::fromStdString(PROJECT_VERSION)
        + " " + QSysInfo::productType().toLower() + " report.zip");

    QuaZip zip(zipFile.fileName());
    zip.open(QuaZip::mdCreate);

    // save settings.ini
    zipLocalFile(zip, dataDir.filePath(SettingsFile));

    // save .cfg
    zipLocalFile(zip, QDir(QDir::currentPath()).filePath(WalletCfg));

    // create 'logs' folder
    {
        QuaZipFile zipLogsFile(&zip);
        zipLogsFile.open(QIODevice::WriteOnly, QuaZipNewInfo(logsFolder, logsFolder));
        zipLogsFile.close();
    }

    {
        QDirIterator it(dataDir.filePath(LogsFolder));

        while (it.hasNext())
        {
            zipLocalFile(zip, it.next(), logsFolder);
        }
    }

    {
        QDirIterator it(dataDir);

        while (it.hasNext())
        {
            const auto& name = it.next();
            if (QFileInfo(name).completeSuffix() == "dmp")
            {
                zipLocalFile(zip, dataDir.filePath(name));
            }
        }
    }

    zip.close();

    QString path = QFileDialog::getSaveFileName(nullptr, "Save problem report", 
        QDir(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).filePath(QFileInfo(zipFile).fileName()),
        "Archives (*.zip)");

    if (path.isEmpty())
    {
        zipFile.remove();
    }
    else
    {
        {
            QFile file(path);
            if(file.exists())
                file.remove();
        }

        zipFile.rename(path);
    }
}

void WalletSettings::applyNodeChanges()
{
    AppModel::getInstance().applyNodeChanges();
}

QString WalletSettings::getExplorerUrl() const
{
    switch (Rules::get().m_Network)
    {
    case Rules::Network::dappnet:
        return "https://dappnet.explorer.beam.mw/";
    case Rules::Network::testnet:
        return "https://testnet.explorer.beam.mw/";
    case Rules::Network::masternet:
        return "https://master-net.explorer.beam.mw/";
    case Rules::Network::mainnet:
    default:
        return "https://explorer.beam.mw/";
    }
}

QString WalletSettings::getFaucetUrl() const
{
    switch (Rules::get().m_Network)
    {
    case Rules::Network::dappnet:
    case Rules::Network::testnet:
    case Rules::Network::masternet:
    case Rules::Network::mainnet:
    default:
        return "https://faucet.beamprivacy.community/";
    }
}

QString WalletSettings::getAppsUrl() const
{
    switch (Rules::get().m_Network)
    {
    case Rules::Network::dappnet:
        return "https://apps-dappnet.beam.mw/app/appslist.json";
    case Rules::Network::testnet:
        return "https://apps-testnet.beam.mw/appslist.json";
    case Rules::Network::masternet:
        return "http://3.19.32.148/app/appslist.json";
    case Rules::Network::mainnet:
    default:
        return "https://apps.beam.mw/appslist.json";
    }
}

bool WalletSettings::showFaucetPromo() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kShowFaucetPromo, true).toBool();
}

void WalletSettings::setShowFacetPromo(bool value)
{
    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kShowFaucetPromo, value);
}

bool WalletSettings::hideSeedValidationPromo() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kHideSeedValidationPromo, false).toBool();
}

void WalletSettings::setHideSeedValidationPromo(bool value)
{
    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kHideSeedValidationPromo, value);
}

QString WalletSettings::getDevAppUrl() const
{
    return m_accountSettings.m_data.value(kDevAppURL).toString();
}

QString WalletSettings::getDevAppName() const
{
    return m_accountSettings.m_data.value(kDevAppName).toString();
}

QString WalletSettings::getDevAppApiVer() const
{
    return m_accountSettings.m_data.value(kDevAppApiVer).toString();
}

QString WalletSettings::getDevAppMinApiVer() const
{
    return m_accountSettings.m_data.value(kDevAppMinApiVer).toString();
}

uint32_t WalletSettings::getShadersPrivilegeLvl() const
{
    return m_accountSettings.m_data.value(kShadersPrivLvl, 2).toUInt();
}

std::string WalletSettings::getDappStoreCID() const
{
    auto cid = m_accountSettings.m_data.value(kDappStoreCID).toString();
    // TODO roman.strilets default cid value should be set for mainnet and testnet
    // for masternet
    return cid.isEmpty() ? getNetworkDappStoreCID()
        : cid.toStdString();
}

std::string WalletSettings::getDappStorePath() const
{
    auto path = m_accountSettings.m_data.value(kDappStorePath).toString();
    return path.isEmpty()
        ? QDir(m_applicationDirPath).filePath(DappsStoreWasm).toStdString()
        : path.toStdString();
}

QStringList WalletSettings::getDappStoreUserPublishers() const
{
    Lock lock(m_mutex);
    auto publishersList = m_accountSettings.m_data.value(kDappStoreUserPublishers).value<QStringList>();
    return publishersList;
}

void WalletSettings::setDappStoreUserPublishers(const QStringList& publishersList)
{
    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kDappStoreUserPublishers, QVariant::fromValue(publishersList));
}

QStringList WalletSettings::getDappStoreUserUnwantedPublishers() const
{
    Lock lock(m_mutex);
    auto publishersList = m_accountSettings.m_data.value(kDappStoreUserUnwantedPublishers).value<QStringList>();
    return publishersList;
}

void WalletSettings::setDappStoreUserUnwantedPublishers(const QStringList& publishersList)
{
    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kDappStoreUserUnwantedPublishers, QVariant::fromValue(publishersList));
}

QString WalletSettings::getAppsCachePath(const QString& appid) const
{
    const QString kCacheFolder = "appcache";
    auto rawAppID = beam::wallet::StripAppIDPrefix(appid.toStdString());

    QString cachePath = QDir::cleanPath(
            QStandardPaths::writableLocation(QStandardPaths::StandardLocation::CacheLocation) +
            QDir::separator() + kCacheFolder +
            QDir::separator() + QString::fromStdString(rawAppID)
    );

    auto adir = getAccountDataDir();
    adir.mkpath(cachePath);

    return cachePath;
}

QString WalletSettings::getAppsStoragePath(const QString& appid) const
{
    Lock lock(m_mutex);
    const char* kStorageFolder = "appstorage";
    auto rawAppID = beam::wallet::StripAppIDPrefix(appid.toStdString());
    auto dataDir = getAccountDataDir();
    QString storagePath = QDir::cleanPath(
            dataDir.path() +
            QDir::separator() + kStorageFolder +
            QDir::separator() + QString::fromStdString(rawAppID)
    );

    dataDir.mkpath(storagePath);

    return storagePath;
}

QString WalletSettings::getLocalAppsPath() const
{
    QDir dataDir = getAccountDataDir();
    const char* kLocalAppsFolder = "localapps";
    QString localAppsPath = QDir::cleanPath(
            dataDir.path() +
            QDir::separator() + kLocalAppsFolder
    );

    dataDir.mkpath(localAppsPath);

    return localAppsPath;
}

#ifdef BEAM_IPFS_SUPPORT
void WalletSettings::applyIPFSChanges()
{
    AppModel::getInstance().applyIPFSChanges();
}

asio_ipfs::config WalletSettings::getIPFSConfig() const
{
    namespace cli = beam::cli;

    QDir dataDir = getAccountDataDir();
    Lock lock(m_mutex);
    asio_ipfs::config cfg(asio_ipfs::config::Mode::Desktop);

    const QString keyStorage = QString(kIPFSPrefix) + cli::IPFS_STORAGE;
    if (m_accountSettings.m_data.contains(keyStorage))
    {
        cfg.repo_root = m_accountSettings.m_data.value(keyStorage).toString().toStdString();
    }
    else
    {
        cfg.repo_root = QDir::cleanPath(
            dataDir.path() + QDir::separator() +
            QDir(cfg.repo_root.c_str()
        ).dirName()).toStdString();
    }

    cfg.low_water = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_LOW_WATER, cfg.low_water).toUInt();
    cfg.high_water = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_HIGH_WATER, cfg.high_water).toUInt();
    cfg.grace_period = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_GRACE, cfg.grace_period).toUInt();
    cfg.swarm_port = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_SWARM_PORT, cfg.swarm_port).toUInt();
    cfg.auto_relay = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_AUTO_RELAY, cfg.auto_relay).toUInt();
    cfg.relay_hop = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_RELAY_HOP, cfg.relay_hop).toUInt();
    cfg.storage_max = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_STORAGE_MAX, QString::fromStdString(cfg.storage_max)).toString().toStdString();
    cfg.api_address = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_API_ADDR, QString::fromStdString(cfg.api_address)).toString().toStdString();
    cfg.gateway_address = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_GATEWAY_ADDR, QString::fromStdString(cfg.gateway_address)).toString().toStdString();
    cfg.autonat = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_AUTONAT, cfg.autonat).toBool();
    cfg.autonat_limit = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_AUTONAT_LIMIT, cfg.autonat_limit).toUInt();
    cfg.autonat_peer_limit = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_AUTONAT_PEER_LIMIT, cfg.autonat_peer_limit).toUInt();
    cfg.routing_type = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_ROUTING_TYPE, QString::fromStdString(cfg.routing_type)).toString().toStdString();
    cfg.run_gc = m_accountSettings.m_data.value(QString(kIPFSPrefix) + cli::IPFS_RUN_GC, cfg.run_gc).toBool();

    const QString keyBootstrap = QString(kIPFSPrefix) + cli::IPFS_BOOTSTRAP;
    if (m_accountSettings.m_data.contains(keyBootstrap))
    {
        auto list = m_accountSettings.m_data.value(keyBootstrap).toStringList();
        decltype(cfg.bootstrap)().swap(cfg.bootstrap);
        decltype(cfg.peering)().swap(cfg.peering);

        for (const auto& qsval : list)
        {
            cfg.bootstrap.push_back(qsval.toStdString());
            cfg.peering.push_back(qsval.toStdString());
        }
    }

    const QString keyPeering = QString(kIPFSPrefix) + cli::IPFS_PEERING;
    if (m_accountSettings.m_data.contains(keyPeering))
    {
        auto list = m_accountSettings.m_data.value(keyPeering).toStringList();
        decltype(cfg.peering)().swap(cfg.peering);

        for (const auto& qsval : list)
        {
            cfg.peering.push_back(qsval.toStdString());
        }
    }

    const QString keySwarm = QString(kIPFSPrefix) + cli::IPFS_SWARM_KEY;
    if (m_accountSettings.m_data.contains(keySwarm))
    {
        cfg.swarm_key = m_accountSettings.m_data.value(keySwarm).toString().toStdString();
    }

    return cfg;
}

void WalletSettings::setIPFSPort(uint32_t port)
{
    namespace cli = beam::cli;
    const QString keySwarmPort = QString(kIPFSPrefix) + cli::IPFS_SWARM_PORT;

    Lock lock(m_mutex);
    if (m_accountSettings.m_data.contains(keySwarmPort) && m_accountSettings.m_data.value(keySwarmPort).toUInt() == port) {
        return;
    }

    m_accountSettings.m_data.setValue(keySwarmPort, port);
    emit IPFSSettingsChanged();
}

void WalletSettings::setIPFSNodeStart(const QString& start)
{
    const QString keyNodeStart = QString(kIPFSPrefix) + kIPFSNodeStart;

    Lock lock(m_mutex);
    if (m_accountSettings.m_data.contains(keyNodeStart) && m_accountSettings.m_data.value(keyNodeStart).toString() == start) {
        return;
    }

    m_accountSettings.m_data.setValue(keyNodeStart, start);
    emit IPFSSettingsChanged();
}

QString WalletSettings::getIPFSNodeStart() const
{
    const QString keyNodeStart = QString(kIPFSPrefix) + kIPFSNodeStart;
    Lock lock(m_mutex);

    QString defStart("clientstart");
    auto start = m_accountSettings.m_data.value(keyNodeStart, defStart).toString();

    if (start != "clientstart" && start != "dapps" && start != "never") {
        LOG_WARNING() << "Unknown IPFS start setting '" << start.toStdString()
                      << "'. Defaulting to '" << defStart.toStdString() << "'";
        m_accountSettings.m_data.setValue(keyNodeStart, defStart);
        return defStart;
    }

    return start;
}

WalletSettings::IPFSLaunch WalletSettings::getIPFSNodeLaunch() const
{
    const auto start = getIPFSNodeStart();

    if (start == "clientstart") {
        return IPFSLaunch::AtStart;
    }

    if (start == "dapps") {
        return IPFSLaunch::AtDApps;
    }

    return IPFSLaunch::Never;
}
#endif

#ifdef BEAM_ASSET_SWAP_SUPPORT
QVector<beam::Asset::ID> WalletSettings::getAllowedAssets() const
{
    if (!m_accountSettings.m_allowedAssets.empty()) return m_accountSettings.m_allowedAssets;

    Lock lock(m_mutex);

    auto ser = m_accountSettings.m_data.value(kAllowedAssets).value<QByteArray>();
    QDataStream in(&ser, QIODevice::ReadOnly);
    in >> m_accountSettings.m_allowedAssets;

    if (m_accountSettings.m_allowedAssets.empty())
    {
        if (Rules::get().m_Network == Rules::Network::mainnet)
        {
            m_accountSettings.m_allowedAssets = { 0, 2, 3, 4, 6, 7, 9, 10, 23 };
        }
        else
        {
            for (int i = 0; i <100; ++i)
                m_accountSettings.m_allowedAssets.push_back(i);
        }
    }

    return m_accountSettings.m_allowedAssets;
}

void WalletSettings::addAllowedAsset(beam::Asset::ID asset)
{
    Lock lock(m_mutex);

    if (m_accountSettings.m_allowedAssets.contains(asset)) return;

    m_accountSettings.m_allowedAssets.push_back(asset);
    QByteArray ser;
    QDataStream out(&ser, QIODevice::WriteOnly);
    out << m_accountSettings.m_allowedAssets;

    m_accountSettings.m_data.setValue(kAllowedAssets, QVariant::fromValue<QByteArray>(ser));
}

void WalletSettings::removeAllowedAsset(beam::Asset::ID asset)
{
    Lock lock(m_mutex);

    if (m_accountSettings.m_allowedAssets.contains(asset))
    {
        m_accountSettings.m_allowedAssets.removeOne(asset);
        QByteArray ser;
        QDataStream out(&ser, QIODevice::WriteOnly);
        out << m_accountSettings.m_allowedAssets;

        m_accountSettings.m_data.setValue(kAllowedAssets, QVariant::fromValue<QByteArray>(ser));
    }
}
#endif  // BEAM_ASSET_SWAP_SUPPORT

int WalletSettings::getAppsServerPort() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kLocalAppsPort, 34700).toInt();
}

void WalletSettings::setAppsServerPort(int port)
{
    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kLocalAppsPort, port);
}

void WalletSettings::minConfirmationsInit()
{
    auto walletModel = AppModel::getInstance().getWalletModel();
    if (walletModel)
    {
        walletModel->getAsync()->getCoinConfirmationsOffset([this] (uint32_t count)
        {
            Lock lock(m_mutex);
            m_minConfirmations = count;
        });
    }
}

uint32_t WalletSettings::getMinConfirmations() const
{
    Lock lock(m_mutex);
    return m_minConfirmations;
}

void WalletSettings::setMinConfirmations(uint32_t value)
{
    if (m_minConfirmations != value)
    {
        auto walletModel = AppModel::getInstance().getWalletModel();
        if (walletModel)
        {
            {
                Lock lock(m_mutex);
                m_minConfirmations = value;
            }
            walletModel->getAsync()->setCoinConfirmationsOffset(m_minConfirmations);
        }
    }
}

QVector<beam::Asset::ID> WalletSettings::getLastAssetSelection() const
{
    Lock lock(m_mutex);

    auto ser = m_accountSettings.m_data.value(kLastAssetSelection).value<QByteArray>();
    QDataStream in(&ser, QIODevice::ReadOnly);
    QVector<beam::Asset::ID> res;
    in >> res;

    return res;
}

void WalletSettings::setLastAssetSelection(QVector<beam::Asset::ID> selection)
{
    Lock lock(m_mutex);

    QByteArray ser;
    QDataStream out(&ser, QIODevice::WriteOnly);
    out << selection;

    m_accountSettings.m_data.setValue(kLastAssetSelection, QVariant::fromValue<QByteArray>(ser));
}

bool WalletSettings::getShowInProgress() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kTxFilterInProgress, true).toBool();
}

void WalletSettings::setShowInProgress(bool value)
{
    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kTxFilterInProgress, value);
}

bool WalletSettings::getShowCompleted() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kTxFilterCompleted, true).toBool();
}

void WalletSettings::setShowCompleted(bool value)
{
    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kTxFilterCompleted, value);
}

bool WalletSettings::getShowCanceled() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kTxFilterCanceled, true).toBool();
}

void WalletSettings::setShowCanceled(bool value)
{
    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kTxFilterCanceled, value);
}

bool WalletSettings::getShowFailed() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kTxFilterFailed, true).toBool();
}

void WalletSettings::setShowFailed(bool value)
{
    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kTxFilterFailed, value);
}

bool WalletSettings::isAppActive() const
{
    Lock lock(m_mutex);
    auto curTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return curTime - m_activateTime > 300 && m_isActive;
}

void WalletSettings::setAppActive(bool value)
{
    Lock lock(m_mutex);
    m_isActive = value;
    m_activateTime = m_isActive
        ? std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()
        : 0;
}


QString WalletSettings::getAccountLabel() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kAccountLabel, qtTrId("new-account-label").arg(1)).toString();
}

void  WalletSettings::setAccountLabel(const QString& label)
{
    if (getAccountLabel() == label)
        return;

    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kAccountLabel, label);
}

int WalletSettings::getAccountPictureIndex() const
{
    Lock lock(m_mutex);
    return m_accountSettings.m_data.value(kAccountPicture).toInt();
}

void  WalletSettings::setAccountPictureIndex(int value)
{
    if (getAccountPictureIndex() == value)
        return;

    Lock lock(m_mutex);
    m_accountSettings.m_data.setValue(kAccountPicture, value);
}

bool WalletSettings::isConnectedToLocalNode() const
{
    return getRunLocalNode() && getNodeAddress() == QString("127.0.0.1:%1").arg(getLocalNodePort());
}

QString WalletSettings::getAccountPictureByIndex(int index)
{
    return QString("qrc:/assets/asset-%1.svg").arg(index);
}