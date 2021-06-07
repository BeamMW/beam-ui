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
#include <map>

#include <QFileDialog>
#include <QtQuick>

#include "model/app_model.h"

#include "wallet/core/default_peers.h"

#include "version.h"
#include "wallet/client/extensions/news_channels/interface.h"

#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

using namespace std;

namespace
{
    const char* kNodeAddressName = "node/address";
    const char* kLocaleName = "locale";
    const char* kLockTimeoutName = "lock_timeout";
    const char* kRequirePasswordToSpendMoney = "require_password_to_spend_money";
    const char* kIsAlowedBeamMWLink = "beam_mw_links_allowed";
    const char* kshowSwapBetaWarning = "show_swap_beta_warning";
    const char* kRateUnit = "rateUnit";
    const char* kLastAssetSelection = "lastAssetSelection";

    const char* kLocalNodeRun = "localnode/run";
    const char* kLocalNodePort = "localnode/port";
    const char* kLocalNodePeers = "localnode/peers";
    const char* kLocalNodePeersPersistent = "localnode/peers_persistent";
    const char* kDefaultLocale = "en_US";

    const char* kNewVersionActive = "notifications/software_release";
    const char* kBeamNewsActive = "notifications/beam_news";
    const char* kTxStatusActive = "notifications/tx_status";

    const char* kDevAppURL    = "devapp/url";
    const char* kDevAppName   = "devapp/name";
    const char* kDevAppApiVer = "devapp/api_version";

    const char* kMpAnonymitySet = "max_privacy/anonymity_set";

    const std::map<QString, QString> kSupportedLangs { 
        { "zh_CN", "Chinese Simplified"},
        { "en_US", "English" },
        { "es_ES", "Español"},
        { "be_BY", "Беларуская"},
        { "cs_CZ", "Czech"},
        { "de_DE", "Deutsch"},
        { "nl_NL", "Dutch"},
        { "fr_FR", "Française"},
        { "id_ID", "Bahasa Indonesia"},
        { "it_IT", "Italiano"},
        { "ja_JP", "日本語"},
        { "ru_RU", "Русский" },
        { "sr_SR", "Српски" },
        { "fi_FI", "Suomi" },
        { "sv_SE", "Svenska"},
        { "th_TH", "ภาษาไทย"},
        { "tr_TR", "Türkçe"},
        { "vi_VI", "Tiếng việt"},
        { "ko_KR", "한국어"}
    };

    const vector<string> kOutDatedPeers = beam::getOutdatedDefaultPeers();
    bool isOutDatedPeer(const string& peer)
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
            beam::wallet::Currency::BTC()
        };
        return supportedUnits;
    }

    const uint8_t kDefaultMaxPrivacyAnonymitySet = 64;
}  // namespace

const char* WalletSettings::WalletCfg = "beam-wallet.cfg";
const char* WalletSettings::LogsFolder = "logs";
const char* WalletSettings::SettingsFile = "settings.ini";
const char* WalletSettings::WalletDBFile = "wallet.db";
#if defined(BEAM_HW_WALLET)
const char* WalletSettings::TrezorWalletDBFile = "trezor-wallet.db";
#endif
const char* WalletSettings::NodeDBFile = "node.db";

WalletSettings::WalletSettings(const QDir& appDataDir)
    : m_data{ appDataDir.filePath(SettingsFile), QSettings::IniFormat }
    , m_appDataDir{appDataDir}
{
    LOG_INFO () << "UI Settings file: " << m_data.fileName().toStdString()
                << "\n\tApp Name: " << m_data.value(kDevAppName).toString().toStdString();
}

#if defined(BEAM_HW_WALLET)
string WalletSettings::getTrezorWalletStorage() const
{
    return getWalletFolder() + "/" + TrezorWalletDBFile;
}
#endif

string WalletSettings::getWalletStorage() const
{
    return getWalletFolder() + "/" + WalletDBFile;
}

string WalletSettings::getWalletFolder() const
{
    Lock lock(m_mutex);

    auto version = QString::fromStdString(PROJECT_VERSION);
    if (!m_appDataDir.exists(version))
    {
        m_appDataDir.mkdir(version);
    }

    return m_appDataDir.filePath(version).toStdString();
}

string WalletSettings::getAppDataPath() const
{
    Lock lock(m_mutex);
    return m_appDataDir.path().toStdString();
}

QString WalletSettings::getNodeAddress() const
{
    Lock lock(m_mutex);
    return m_data.value(kNodeAddressName).toString();
}

void WalletSettings::setNodeAddress(const QString& addr)
{
    if (addr != getNodeAddress())
    {
        auto walletModel = AppModel::getInstance().getWalletModel();
        if (walletModel)
        {
            walletModel->getAsync()->setNodeAddress(addr.toStdString());
        }
        {
            Lock lock(m_mutex);
            m_data.setValue(kNodeAddressName, addr);
        }
        
        emit nodeAddressChanged();
    }
    
}

int WalletSettings::getLockTimeout() const
{
    Lock lock(m_mutex);
    return m_data.value(kLockTimeoutName, 0).toInt();
}

void WalletSettings::setLockTimeout(int value)
{
    if (value != getLockTimeout())
    {
        {
            Lock lock(m_mutex);
            m_data.setValue(kLockTimeoutName, value);
        }
        emit lockTimeoutChanged();
    }
}

bool WalletSettings::isPasswordReqiredToSpendMoney() const
{
    Lock lock(m_mutex);
    return m_data.value(kRequirePasswordToSpendMoney, false).toBool();
}

void WalletSettings::setPasswordReqiredToSpendMoney(bool value)
{
    Lock lock(m_mutex);
    m_data.setValue(kRequirePasswordToSpendMoney, value);
}

bool WalletSettings::isAllowedBeamMWLinks() const
{
    Lock lock(m_mutex);
    return m_data.value(kIsAlowedBeamMWLink, false).toBool();
}

void WalletSettings::setAllowedBeamMWLinks(bool value)
{
    {
        Lock lock(m_mutex);
        m_data.setValue(kIsAlowedBeamMWLink, value);
    }
    emit beamMWLinksChanged();
}

bool WalletSettings::showSwapBetaWarning()
{
    Lock lock(m_mutex);
    return m_data.value(kshowSwapBetaWarning, true).toBool();
}

void WalletSettings::setShowSwapBetaWarning(bool value)
{
    Lock lock(m_mutex);
    m_data.setValue(kshowSwapBetaWarning, value);
}

bool WalletSettings::getRunLocalNode() const
{
    Lock lock(m_mutex);
    return m_data.value(kLocalNodeRun, false).toBool();
}

void WalletSettings::setRunLocalNode(bool value)
{
    {
        Lock lock(m_mutex);
        m_data.setValue(kLocalNodeRun, value);
    }
    emit localNodeRunChanged();
}

uint WalletSettings::getLocalNodePort() const
{
    Lock lock(m_mutex);
#ifdef BEAM_TESTNET
    return m_data.value(kLocalNodePort, 11005).toUInt();
#else
    return m_data.value(kLocalNodePort, 10005).toUInt();
#endif // BEAM_TESTNET
}

void WalletSettings::setLocalNodePort(uint port)
{
    {
        Lock lock(m_mutex);
        m_data.setValue(kLocalNodePort, port);
    }
    emit localNodePortChanged();
}

string WalletSettings::getLocalNodeStorage() const
{
    Lock lock(m_mutex);
    return m_appDataDir.filePath(NodeDBFile).toStdString();
}

string WalletSettings::getTempDir() const
{
    Lock lock(m_mutex);
    return m_appDataDir.filePath("./temp").toStdString();
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
    QStringList peers = m_data.value(kLocalNodePeers).value<QStringList>();
    size_t outDatedCount = count_if(
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
        m_data.setValue(kLocalNodePeers, QVariant::fromValue(peers));
    }
    return peers;
}

void WalletSettings::setLocalNodePeers(const QStringList& qPeers)
{
    {
        Lock lock(m_mutex);
        m_data.setValue(kLocalNodePeers, QVariant::fromValue(qPeers));
    }
    emit localNodePeersChanged();
}

bool WalletSettings::getPeersPersistent() const
{
    Lock lock(m_mutex);
    return m_data.value(kLocalNodePeersPersistent, false).toBool();
}

QString WalletSettings::getLocale() const
{
    QString savedLocale;
    {
        Lock lock(m_mutex);
        savedLocale = m_data.value(kLocaleName).toString();
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
        m_data.setValue(kLocaleName, locale);
    }
    emit localeChanged();
}

beam::wallet::Currency WalletSettings::getRateCurrency() const
{
    const auto& defaultUnit = getDefaultRateUnit();
    const auto& supportedUnits = getSupportedRateUnits();
    Lock lock(m_mutex);

    auto rawUnitValue = m_data.value(kRateUnit, QString::fromStdString(defaultUnit.m_value)).toString();
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
        m_data.setValue(kRateUnit, QString::fromStdString(unit.m_value));
        emit secondCurrencyChanged();
    }
}

bool WalletSettings::isNewVersionActive() const
{
    Lock lock(m_mutex);
    return m_data.value(kNewVersionActive, true).toBool();
}

bool WalletSettings::isBeamNewsActive() const
{
    Lock lock(m_mutex);
    return m_data.value(kBeamNewsActive, true).toBool();
}

bool WalletSettings::isTxStatusActive() const
{
    Lock lock(m_mutex);
    return m_data.value(kTxStatusActive, true).toBool();
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
        m_data.setValue(kNewVersionActive, isActive);
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
        m_data.setValue(kBeamNewsActive, isActive);
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
        m_data.setValue(kTxStatusActive, isActive);
    }
}

uint8_t WalletSettings::getMaxPrivacyAnonymitySet() const
{
    Lock lock(m_mutex);
    return static_cast<uint8_t>(m_data.value(kMpAnonymitySet, kDefaultMaxPrivacyAnonymitySet).toUInt());
}

void WalletSettings::setMaxPrivacyAnonymitySet(uint8_t anonymitySet)
{
    Lock lock(m_mutex);
    m_data.setValue(kMpAnonymitySet, anonymitySet);
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

// static
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

// static
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

    QFile zipFile = m_appDataDir.filePath("beam v" + QString::fromStdString(PROJECT_VERSION) 
        + " " + QSysInfo::productType().toLower() + " report.zip");

    QuaZip zip(zipFile.fileName());
    zip.open(QuaZip::mdCreate);

    // save settings.ini
    zipLocalFile(zip, m_appDataDir.filePath(SettingsFile));

    // save .cfg
    zipLocalFile(zip, QDir(QDir::currentPath()).filePath(WalletCfg));

    // create 'logs' folder
    {
        QuaZipFile zipLogsFile(&zip);
        zipLogsFile.open(QIODevice::WriteOnly, QuaZipNewInfo(logsFolder, logsFolder));
        zipLogsFile.close();
    }

    {
        QDirIterator it(m_appDataDir.filePath(LogsFolder));

        while (it.hasNext())
        {
            zipLocalFile(zip, it.next(), logsFolder);
        }
    }

    {
        QDirIterator it(m_appDataDir);

        while (it.hasNext())
        {
            const auto& name = it.next();
            if (QFileInfo(name).completeSuffix() == "dmp")
            {
                zipLocalFile(zip, m_appDataDir.filePath(name));
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

void WalletSettings::applyChanges()
{
    AppModel::getInstance().applySettingsChanges();
}

QString WalletSettings::getDevBeamAppUrl()
{
    return m_data.value(kDevAppURL).toString();
}

QString WalletSettings::getDevBeamAppName()
{
    return m_data.value(kDevAppName).toString();
}

QString WalletSettings::getDevAppApiVer()
{
    return m_data.value(kDevAppApiVer).toString();
}

QString WalletSettings::getExplorerUrl() const
{
    #ifdef BEAM_BEAMX
    return "https://beamx.explorer.beam.mw/";
    #elif defined(BEAM_TESTNET)
    return "https://testnet.explorer.beam.mw/";
    #elif defined(BEAM_MAINNET)
    return "https://explorer.beam.mw/";
    #else
    return "https://master-net.explorer.beam.mw/";
    #endif
}

QString WalletSettings::getFaucetUrl() const
{
    #ifdef BEAM_BEAMX
    return "https://faucet.beamprivacy.community/";
    #elif defined(BEAM_TESTNET)
    return "https://faucet.beamprivacy.community/";
    #elif defined(BEAM_MAINNET)
    return "https://faucet.beamprivacy.community/";
    #else
    return "https://faucet.beamprivacy.community/";
    #endif
}

QString WalletSettings::getAppsUrl() const
{
    #ifdef BEAM_BEAMX
    return "";
    #elif defined(BEAM_TESTNET)
    return "https://apps-testnet.beam.mw/appslist.json";
    #elif defined(BEAM_MAINNET)
    return "https://apps.beam.mw/appslist.json";
    #else
    return "http://3.136.182.25:80/app/appslist.json";
    #endif
}

QString WalletSettings::getAppsCachePath() const
{
    Lock lock(m_mutex);

    const char* kCacheFolder = "appcache";
    if (!m_appDataDir.exists(kCacheFolder))
    {
        m_appDataDir.mkdir(kCacheFolder);
    }

    return m_appDataDir.filePath(kCacheFolder);
}

QString WalletSettings::getAppsStoragePath() const
{
    Lock lock(m_mutex);

    const char* kStorageFolder = "appstorage";
    if (!m_appDataDir.exists(kStorageFolder))
    {
        m_appDataDir.mkdir(kStorageFolder);
    }

    return m_appDataDir.filePath(kStorageFolder);
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

boost::optional<beam::Asset::ID> WalletSettings::getLastAssetSelection() const
{
    Lock lock(m_mutex);

    if (m_data.contains(kLastAssetSelection))
    {
        return m_data.value(kLastAssetSelection).toInt();
    }
    else
    {
        return boost::none;
    }
}

void WalletSettings::setLastAssetSelection(boost::optional<beam::Asset::ID> selection)
{
    Lock lock(m_mutex);
    if (selection.is_initialized())
    {
        m_data.setValue(kLastAssetSelection, *selection);
    }
    else
    {
        m_data.remove(kLastAssetSelection);
    }
}
