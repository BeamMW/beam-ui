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

#include "start_view.h"
#include <QDateTime>
#include <QMessageBox>
#include <QStringBuilder>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QStandardPaths>
#include <QJSEngine>
#include <QPointer>
#include "settings_view.h"
#include "model/app_model.h"
#include "model/keyboard.h"
#include "version.h"
#include "wallet/core/secstring.h"
#include "wallet/core/default_peers.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <thread>
#include <algorithm>

#if defined(BEAM_HW_WALLET)
#include "core/block_rw.h"
#include "keykeeper/hw_wallet.h"
#endif

using namespace beam;
using namespace ECC;
using namespace std;
namespace fs = boost::filesystem;
namespace
{
    const QChar PHRASES_SEPARATOR = ';';
    const uint8_t kPhraseSize = 12;
    const char kAccountLabel[] = "account/label";
    const char kAccountPicture[] = "account/picture";

    const char kName[] = "name";
    const char kIcon[] = "icon";
    const char kIconWidth[] = "iconWidth";
    const char kIconHeight[] = "iconHeight";
    const char kLabel[] = "label";


    Rules::Network getNetworkFromString(const QString& network)
    {
#define MACRO(name) if (network == #name) return Rules::Network::name;

        RulesNetworks(MACRO)
#undef MACRO
            return Rules::Network::mainnet;
    }

    fs::path pathFromStdString(const std::string& path)
    {
#ifdef WIN32
        fs::path boostPath{ Utf8toUtf16(path.c_str()) };
#else
        fs::path boostPath{ path };
#endif
        return boostPath;
    }

    std::vector<fs::path> findAllWalletDB(const std::string& appPath)
    {
        std::vector<fs::path> walletDBs;
        try
        {
            auto appDataPath = pathFromStdString(appPath);

            if (!fs::exists(appDataPath))
            {
                return {};
            }

            for (fs::recursive_directory_iterator endDirIt, it{ appDataPath }; it != endDirIt; ++it)
            {
                if (it.level() > 1)
                {
                    it.pop();
                    if (it == endDirIt)
                    {
                        break;
                    }
                }
                if (it->path().filename() == WalletSettings::WalletDBFile 
#if defined(BEAM_HW_WALLET)
                    || it->path().filename() == WalletSettings::TrezorWalletDBFile
#endif
                )
                {
                    walletDBs.push_back(it->path());
                }
            }
        }
        catch (std::exception &e)
        {
            LOG_ERROR() << e.what();
        }

        return walletDBs;
    }

    int getNewAccountIndex()
    {
        QDir dir(AppModel::getInstance().getSettings().getAppDataPath().c_str());
        if (!dir.cd(Rules::get().get_NetworkName()))
            return 1;

        int index = 0;
        const char accountName[] = "Account";
        for (const auto& subDirInfo : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::SortFlag::Name))
        {
            auto name = subDirInfo.fileName();
            if (!name.startsWith(accountName))
                continue;

            auto suffix = name.mid((int)std::size(accountName) - 1); // ignore \0
            bool ok = false;
            int accountIndex = suffix.toInt(&ok);
            if (!ok || accountIndex == 0)
                continue;
            accountIndex -= 1;

            if (index != accountIndex)
                break;

            auto paths = findAllWalletDB(subDirInfo.absoluteFilePath().toStdString());
            if (paths.empty())
                break; // folder exists but empty

            ++index;
        }
        return index;
    }


    template<typename T>
    void DoJSCallback(QJSValue& jsCallback, const T& res)
    {
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
        QJSValue v = jsCallback.engine()->toScriptValue(res);
        jsCallback.call(QJSValueList{ v });
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
        #pragma GCC diagnostic pop
#endif
    }

    void DoOpenWallet(QJSValue& jsCallback, std::function<void ()> openFunc)
    {
        try
        {
            openFunc();
            DoJSCallback(jsCallback, QString());
        }
        catch(const beam::wallet::FileIsNotDatabaseException&)
        {
            //% "Invalid password provided"
            DoJSCallback(jsCallback, qtTrId("general-pwd-invalid"));
        }
        catch(std::runtime_error& err)
        {
            QString errmsg = err.what();
            if (errmsg.isEmpty())
            {
                //% "Failed to open wallet, please check logs"
                errmsg = qtTrId("general-open-failed");
            }

            LOG_ERROR() << "Error while trying to open wallet: " << errmsg.toStdString();
            DoJSCallback(jsCallback, errmsg);
        }
    }
}  // namespace

RecoveryPhraseItem::RecoveryPhraseItem(int index, QString phrase)
    : m_index(index)
    , m_phrase(std::move(phrase))
{
}

bool RecoveryPhraseItem::isCorrect() const
{
    return m_userInput == m_phrase;
}

bool RecoveryPhraseItem::isAllowed() const
{
    return  isAllowedWord(m_userInput.toStdString(), language::en);
}

const QString& RecoveryPhraseItem::getValue() const
{
    return m_userInput;
}

void RecoveryPhraseItem::setValue(const QString& value)
{
    if (m_userInput != value)
    {
        m_userInput = value;
        emit valueChanged();
        emit isCorrectChanged();
        emit isAllowedChanged();
    }
}

const QString& RecoveryPhraseItem::getPhrase() const
{
    return m_phrase;
}

int RecoveryPhraseItem::getIndex() const
{
    return m_index;
}

WalletDBPathItem::WalletDBPathItem(
    const QString& walletDBPath,
    uintmax_t fileSize,
    QDateTime lastWriteTime,
    QDateTime creationTime,
    bool defaultLocated)
    : m_fullPath{walletDBPath}
    , m_fileSize(fileSize)
    , m_lastWriteTime(lastWriteTime)
    , m_creationTime(creationTime)
    , m_defaultLocated(defaultLocated)
{
}

WalletDBPathItem::~WalletDBPathItem()
{
}

int WalletDBPathItem::getFileSize() const
{
    return m_fileSize;
}

const QString& WalletDBPathItem::getFullPath() const
{
    return m_fullPath;
}

QString WalletDBPathItem::getShortPath() const
{
    return QString();
}

QString WalletDBPathItem::getLastWriteDateString() const
{
    return m_lastWriteTime.toString(QLocale().dateTimeFormat(QLocale::ShortFormat));
}

QString WalletDBPathItem::getCreationDateString() const
{
    return m_creationTime.toString(QLocale().dateTimeFormat(QLocale::ShortFormat));
}

QDateTime WalletDBPathItem::getLastWriteDate() const
{
    return m_lastWriteTime;
}

bool WalletDBPathItem::locatedByDefault() const
{
    return m_defaultLocated;
}

void WalletDBPathItem::setPreferred(bool isPreferred)
{
    m_isPreferred = isPreferred;
}

bool WalletDBPathItem::isPreferred() const
{
    return m_isPreferred;
}

StartViewModel::StartViewModel()
    : m_isRecoveryMode{false}
#if defined(BEAM_HW_WALLET)
    , m_hwWallet(AppModel::getInstance().getHardwareWalletClient())
    , m_trezorTimer(this)
    , m_trezorThread(*this)
#endif

{
    findExistingWalletDBIfNeeded();

#if defined(BEAM_HW_WALLET)
    connect(&m_trezorThread, SIGNAL(ownerKeyImported()), this, SLOT(onTrezorOwnerKeyImported()));
    connect(&m_trezorTimer, SIGNAL(timeout()), this, SLOT(checkTrezor()));
    m_trezorTimer.start(1000);
#endif
}

StartViewModel::~StartViewModel()
{
    qDeleteAll(m_walletDBpaths);
}

bool StartViewModel::walletExists() const
{
    return wallet::WalletDB::isInitialized(AppModel::getInstance().getSettings().getWalletStorage())
#if defined(BEAM_HW_WALLET)
        || wallet::WalletDB::isInitialized(AppModel::getInstance().getSettings().getTrezorWalletStorage())
#endif
    ;
}

bool StartViewModel::isTrezorEnabled() const
{
#if defined(BEAM_HW_WALLET)
    return true;
#else
    return false;
#endif
}

bool StartViewModel::useHWWallet() const
{
    return m_useHWWallet;
}

void StartViewModel::setUseHWWallet(bool value)
{
    if (m_useHWWallet != value)
    {
        m_useHWWallet = value;
        emit isUseHWWalletChanged();
    }
}

bool StartViewModel::getSaveSeed() const
{
    return m_saveSeed;
}

void StartViewModel::setSaveSeed(bool value)
{
    if (m_saveSeed != value)
    {
        m_saveSeed = value;
        emit saveSeedChanged();
    }
}

#if defined(BEAM_HW_WALLET)
bool StartViewModel::isTrezorConnected() const
{
    return m_isTrezorConnected;
}

void StartViewModel::checkTrezor()
{
    bool foundDevice = !m_hwWallet->getDevices().empty();

    if (m_isTrezorConnected != foundDevice)
    {
        m_isTrezorConnected = foundDevice;
        emit isTrezorConnectedChanged();
        emit trezorDeviceNameChanged();
    }
}

QString StartViewModel::getTrezorDeviceName() const
{
    auto devices = m_hwWallet->getDevices();
    assert(!devices.empty());
    return QString(devices[0].c_str());
}

bool StartViewModel::isOwnerKeyImported() const
{
    return bool(m_HWKeyKeeper);
}

TrezorThread::TrezorThread(StartViewModel& vm)
    : m_vm(vm)
{

}

void TrezorThread::run()
{
    auto reactor = AppModel::getInstance().getWalletReactor();
    io::Reactor::Scope s(*reactor); 
    auto hwWallet = std::make_shared<beam::wallet::HWWallet>();//AppModel::AppModel::getInstance().getHardwareWalletClient();//std::make_shared<beam::wallet::HWWallet>();
    struct UIHandlerProxy : public beam::wallet::HWWallet::IHandler
    {
        beam::wallet::HWWallet::IHandler::Ptr getHandler()
        {
            return std::static_pointer_cast<beam::wallet::HWWallet::IHandler>(AppModel::getInstance().getWallet());
        }

        void ShowKeyKeeperMessage() override
        {
            if (auto h = getHandler(); h)
            {
                h->ShowKeyKeeperMessage();
            }
        }

        void HideKeyKeeperMessage() override
        {
            if (auto h = getHandler(); h)
            {
                h->HideKeyKeeperMessage();
            }
        }

        void ShowKeyKeeperError(const std::string& m) override
        {
            if (auto h = getHandler(); h)
            {
                h->ShowKeyKeeperError(m);
            }
        }
    };
    auto proxy = std::make_shared<UIHandlerProxy>();
    beam::wallet::HWWallet::IHandler::Ptr p = std::static_pointer_cast<beam::wallet::HWWallet::IHandler>(proxy);
    auto keyKeeper = hwWallet->getKeyKeeper(hwWallet->getDevices().front(), p);
    using namespace beam::wallet;
    
    {
        // cache sbbs kdf
        struct MyHandler : IPrivateKeyKeeper2::Handler
        {
            io::Reactor::Ptr m_Reactor;
            IPrivateKeyKeeper2::Method::get_Kdf m_Method;
            MyHandler(io::Reactor::Ptr reactor)
                : m_Reactor(reactor)
            {}
    
            void OnDone(IPrivateKeyKeeper2::Status::Type s) override
            {
                if (s != IPrivateKeyKeeper2::Status::Success)
                {
                    m_Reactor->stop();
                }
            }
        };
        auto handler = std::make_shared<MyHandler>(reactor);
        beam::wallet::IPrivateKeyKeeper2::Method::get_Kdf& m = handler->m_Method;
        m.m_Root = false;
        m.m_iChild = Key::Index(-1);
        keyKeeper->InvokeAsync(m, handler);
    }

    {
        // cache slots
        struct MyHandler : IPrivateKeyKeeper2::Handler
        {
            io::Reactor::Ptr m_Reactor;
            IPrivateKeyKeeper2::Method::get_NumSlots m_Method;
            MyHandler(io::Reactor::Ptr reactor)
                : m_Reactor(reactor)
            {}

            virtual ~MyHandler()
            {

            }

            void OnDone(IPrivateKeyKeeper2::Status::Type s) override
            {
                if (s != IPrivateKeyKeeper2::Status::Success)
                {
                    m_Reactor->stop();
                }
            }
        };
        auto handler = std::make_shared<MyHandler>(reactor);
        keyKeeper->InvokeAsync(handler->m_Method, handler);
    }
    
    {
        // cache master kdf
        struct MyHandler : IPrivateKeyKeeper2::Handler
        {
            StartViewModel* m_ViewModel;
            IPrivateKeyKeeper2::Ptr m_KeyKeeper;
            io::Reactor::Ptr m_Reactor;
            IPrivateKeyKeeper2::Method::get_Kdf m_Method;
    
            MyHandler(StartViewModel* vm, IPrivateKeyKeeper2::Ptr keyKeeper, io::Reactor::Ptr reactor)
                : m_ViewModel(vm)
                , m_KeyKeeper(keyKeeper)
                , m_Reactor(reactor)
            {}
    
            void OnDone(IPrivateKeyKeeper2::Status::Type s) override
            {
                if (s == IPrivateKeyKeeper2::Status::Success)
                {
                    m_ViewModel->m_HWKeyKeeper = m_KeyKeeper;
                }
                m_Reactor->stop();
            }
        };
    
        auto handler = std::make_shared<MyHandler>(&m_vm, keyKeeper, reactor);
        IPrivateKeyKeeper2::Method::get_Kdf& m = handler->m_Method;
        m.m_Root = true;
        keyKeeper->InvokeAsync(m, handler);
    }
    
    reactor->run();
    emit ownerKeyImported();
}

void StartViewModel::onTrezorOwnerKeyImported()
{
    LOG_INFO() << "Trezor Key imported";

    if (m_creating)
    {
        if (!m_HWKeyKeeper)
        {
            //% "Hardware keykeeper is not initialized"
            reutrn DoJSCallback(m_callback, qtTrId("start-no-hwkeeper"));
        }

        SecString secretPass
        if(!AppModel::getInstance().createTrezorWallet(secretPass, m_HWKeyKeeper))
        {
            //% "Failed to create trezor wallet"
            reutrn DoJSCallback(m_callback, qtTrId("start-trezor-error");
        }

        DoJSCallback(m_callback, "");
    }
    else
    {
        DoOpenWallet(m_callback, [this] () {
            SecString secretPass = m_password;
            AppModel::getInstance().openWalletThrow(secretPass, m_HWKeyKeeper));
        });
    }

    emit isOwnerKeyImportedChanged();
}

void StartViewModel::startOwnerKeyImporting(bool creating)
{
    m_creating = creating;
    if (!m_HWKeyKeeper)
    {
        m_trezorThread.start();
    }
    else
    {
        onTrezorOwnerKeyImported();
    }
}

#endif

bool StartViewModel::getIsRecoveryMode() const
{
    return m_isRecoveryMode;
}

void StartViewModel::setIsRecoveryMode(bool value)
{
    if (value != m_isRecoveryMode)
    {
        m_isRecoveryMode = value;
        qDeleteAll(m_recoveryPhrases);
        m_recoveryPhrases.clear();
        emit isRecoveryModeChanged();
    }
}

const QList<QObject*>& StartViewModel::getRecoveryPhrases()
{
    if (m_recoveryPhrases.empty() && !AppModel::getInstance().isSeedValidationMode())
    {
        if (!m_isRecoveryMode)
        {
            m_generatedPhrases = beam::createMnemonic(beam::getEntropy(), beam::language::en);
        }
        else
        {
            m_generatedPhrases.resize(kPhraseSize);
        }
        assert(m_generatedPhrases.size() == kPhraseSize);
        m_recoveryPhrases.reserve(static_cast<int>(m_generatedPhrases.size()));
        int i = 0;
        for (const auto& p : m_generatedPhrases)
        {
            m_recoveryPhrases.push_back(new RecoveryPhraseItem(i++, QString::fromStdString(p)));
        }
    }
    return m_recoveryPhrases;
}

const QList<QObject*>& StartViewModel::getCheckPhrases()
{
    if (m_checkPhrases.empty())
    {
        srand(time(0));
        set<int> indecies;
        while (indecies.size() < 6)
        {
            int index = rand() % m_recoveryPhrases.size();
            auto it = indecies.insert(index);
            if (it.second)
            {
                m_checkPhrases.push_back(new RecoveryPhraseItem(index, QString::fromStdString(m_generatedPhrases[index])));
            }
        }
    }

    return m_checkPhrases;
}

QChar StartViewModel::getPhrasesSeparator()
{
    return PHRASES_SEPARATOR;
}

bool StartViewModel::getIsRunLocalNode() const
{
    return AppModel::getInstance().getSettings().getRunLocalNode();
}

QString StartViewModel::chooseRandomNode() const
{
    auto peers = getDefaultPeers();
    srand(time(0));
    return QString(peers[rand() % peers.size()].c_str());
}

QString StartViewModel::walletVersion() const
{
    return QString::fromStdString(PROJECT_VERSION);
}

int StartViewModel::getLocalPort() const
{
    return AppModel::getInstance().getSettings().getLocalNodePort();
}

QString StartViewModel::getRemoteNodeAddress() const
{
    return AppModel::getInstance().getSettings().getNodeAddress();
}

QString StartViewModel::getLocalNodePeer() const
{
    auto peers = AppModel::getInstance().getSettings().getLocalNodePeers();
    return !peers.empty() ? peers.first() : "";
}

const QList<QObject*>& StartViewModel::getWalletDBpaths()
{
    return *&m_walletDBpaths;
}

QList<QVariantMap> StartViewModel::getNetworks() const
{
    QList<QVariantMap> networks;
#define MACRO(name) \
    { QVariantMap network; \
    network[kName] = #name; \
    network[kIcon] = "qrc:/assets/icon_" #name ".png"; \
    network[kIconWidth] = 16; \
    network[kIconHeight] = 16; \
    networks.push_back(network); }

    RulesNetworks(MACRO)
#undef MACRO
    return networks;
}

bool StartViewModel::isCapsLockOn() const
{
    return keyboard::isCapsLockOn();
}

void StartViewModel::setupLocalNode(int port, const QString& localNodePeer)
{
    m_connectToLocalNode = true;
    m_localNodePort = port;
    m_remoteNodeAddress = localNodePeer;
}

void StartViewModel::setupNode()
{
    auto& settings = AppModel::getInstance().getSettings();
    if (m_localNodePort > 0)
    {
        settings.setLocalNodePort(m_localNodePort);
    }
    if (m_connectToLocalNode) 
    {
        auto localAddress = defaultRemoteNodeAddr();
        settings.setNodeAddress(localAddress);
    }
    else
    {
        settings.setNodeAddress(m_remoteNodeAddress);
    }

    settings.setRunLocalNode(true/*m_connectToLocalNode*/);
    QStringList peers;

    for (const auto& peer : getDefaultPeers())
    {
        if (m_remoteNodeAddress != peer.c_str())
        {
            peers.push_back(peer.c_str());
        }
    }

    peers.push_back(m_remoteNodeAddress);
    settings.setLocalNodePeers(peers);
}

void StartViewModel::setupRemoteNode(const QString& nodeAddress)
{
    m_connectToLocalNode = false;
    m_remoteNodeAddress = nodeAddress;
}

void StartViewModel::setupRandomNode()
{
    setupRemoteNode(chooseRandomNode());
}

uint StartViewModel::coreAmount() const
{
    return std::thread::hardware_concurrency();
}

void StartViewModel::copyPhrasesToClipboard()
{
    QApplication::clipboard()->setText(getPhrases());
}

void StartViewModel::resetPhrases()
{
    qDeleteAll(m_recoveryPhrases);
    m_recoveryPhrases.clear();
    m_generatedPhrases.clear();
    qDeleteAll(m_checkPhrases);
    m_checkPhrases.clear();
    emit recoveryPhrasesChanged();
}

void StartViewModel::createWallet(const QJSValue& callback)
{
    m_callback = callback;
#if defined(BEAM_HW_WALLET)
    if (m_hwWallet->isConnected())
    {
        startOwnerKeyImporting(true);
        return;
    }
#endif


    SecString secretSeed;
    SecString secretPass = m_password;
    std::string rawSeed;

    if (!m_isRecoveryMode)
        m_useHWWallet = false;

    if (!m_useHWWallet)
    {
        if (m_isRecoveryMode)
        {
            assert(m_generatedPhrases.size() == static_cast<size_t>(m_recoveryPhrases.size()));
            for (int i = 0; i < m_recoveryPhrases.size(); ++i)
            {
                QString s = static_cast<RecoveryPhraseItem*>(m_recoveryPhrases[i])->getValue();
                m_generatedPhrases[i] = s.toStdString();
            }
        }

        auto buf = beam::decodeMnemonic(m_generatedPhrases);
        secretSeed.assign(buf.data(), buf.size());

        if (m_saveSeed)
            rawSeed = getPhrases().toStdString();
    }

    if (AppModel::getInstance().isOnlyOneInstanceStarted())
    {
        AppModel::resetInstance(AppModel::getInstance().getSettings(), getNewAccountIndex());
        auto& settings = AppModel::getInstance().getSettings();
        settings.setAccountLabel(getNewAccountLabel());
        settings.setAccountPictureIndex(m_newAccountPictureIndex);
        setupNode();
        try
        {
            AppModel::getInstance().createWalletThrow(m_useHWWallet ? nullptr : &secretSeed, secretPass, rawSeed);
            DoJSCallback(m_callback, QString());
        }
        catch (const std::exception& exc)
        {
            QString errmsg = exc.what();
            if (errmsg.isEmpty())
            {
                //% "Failed to open wallet, please check logs"
                errmsg = qtTrId("general-open-failed");
            }
            DoJSCallback(m_callback, errmsg);
        }
    }
}

void StartViewModel::openWallet(const QString& pass, const QJSValue& callback)
{
    m_callback = callback;
#if defined(BEAM_HW_WALLET)
    if (m_useHWWallet)
    {
        if (m_hwWallet->isConnected())
        {
            setPassword(pass);
            startOwnerKeyImporting(false);
        }
        else
        {
            //% "Hardware wallet is not connected"
            DoJSCallback(m_callback, qtTrId("start-hw-not-connected"));
        }
        return;
    }
#endif
    // TODO make this secure
    if (AppModel::getInstance().isOnlyOneInstanceStarted())
    {
        DoOpenWallet(m_callback, [pass] () {
            SecString secret = pass.toStdString();
            AppModel::getInstance().openWalletThrow(secret);
        });
    }
}

bool StartViewModel::checkWalletPassword(const QString& password) const
{
    SecString secretPassword = password.toStdString();
    return AppModel::getInstance().checkWalletPassword(secretPassword);
}

void StartViewModel::setPassword(const QString& pass)
{
    m_password = pass.toStdString();
}

void StartViewModel::onNodeSettingsChanged()
{
    AppModel::getInstance().nodeSettingsChanged();
}

void StartViewModel::findExistingWalletDBIfNeeded()
{
    if (walletExists() || isFoundExistingWalletDB())
        return;

    // find all wallet.db in appData and defaultAppData
    findExistingWalletDB();
}

void StartViewModel::findExistingWalletDB()
{
    std::set<std::string> pathsToCheck;

    auto appDataPath = AppModel::getInstance().getSettings().getUserDataPath();
    pathsToCheck.insert(appDataPath);

    auto defaultAppDataPath = QStandardPaths::writableLocation(QStandardPaths::DataLocation).toStdString();
    pathsToCheck.insert(defaultAppDataPath);

    #ifdef Q_OS_LINUX
    {
        // Some 5.2 & 5.3 created folders without ' ' in name (BeamWallet instead of 'Beam Wallet')
        // As of 6.0 this is fixed, but we need to take care of these
        auto checkAlso = [&] (const std::string& spath)
        {
            fs::path path(spath);
            if (path.empty()) return;

            auto dirname = path.filename().string();
            std::string::iterator end_pos = std::remove(dirname.begin(), dirname.end(), ' ');
            dirname.erase(end_pos, dirname.end());

            auto nspath = path.parent_path().append(dirname);
            pathsToCheck.insert(nspath.string());
        };

        checkAlso(appDataPath);
        checkAlso(defaultAppDataPath);
    }
    #endif

    std::vector<fs::path> walletDBs;
    for(const auto& path: pathsToCheck)
    {
        auto additionalWalletDBs = findAllWalletDB(path);
        walletDBs.reserve(walletDBs.size() + additionalWalletDBs.size());
        walletDBs.insert(std::end(walletDBs), std::begin(additionalWalletDBs), std::end(additionalWalletDBs));
    }

    QList<WalletDBPathItem*> walletDBpaths;
    for (auto& walletDBPath : walletDBs)
    {
#ifdef WIN32
        QFileInfo fileInfo(QString::fromStdWString(walletDBPath.wstring()));
#else
        QFileInfo fileInfo(QString::fromStdString(walletDBPath.string()));
#endif
        QString absoluteFilePath = fileInfo.absoluteFilePath();
        bool isDefaultLocated = absoluteFilePath.contains(
            QString::fromStdString(defaultAppDataPath));

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
        auto birthTime = fileInfo.birthTime();
        if(!birthTime.isValid()) birthTime = fileInfo.metadataChangeTime();
#else
        auto birthTime = fileInfo.created();
#endif
        walletDBpaths.push_back(new WalletDBPathItem(
                absoluteFilePath,
                fileInfo.size(),
                fileInfo.lastModified(),
                birthTime,
                isDefaultLocated));
    }

    std::sort(walletDBpaths.begin(), walletDBpaths.end(),
              [] (WalletDBPathItem* left, WalletDBPathItem* right) {
                  if (left->locatedByDefault() && !right->locatedByDefault()) {
                      return false;
                  }
                  return left->getLastWriteDate() > right->getLastWriteDate();
              });

    if (!walletDBpaths.empty()) {
        walletDBpaths.first()->setPreferred();
    }
    std::copy(walletDBpaths.begin(), walletDBpaths.end(), std::back_inserter(m_walletDBpaths));
}

bool StartViewModel::isFoundExistingWalletDB()
{
    return !m_walletDBpaths.empty();
}

void StartViewModel::deleteCurrentWalletDB()
{
    try
    {
        {
            auto pathToDB = pathFromStdString(AppModel::getInstance().getSettings().getWalletStorage());
            if (fs::exists(pathToDB))
                fs::remove(pathToDB);
        }

#if defined(BEAM_HW_WALLET)
        {
            auto pathToDB = pathFromStdString(AppModel::getInstance().getSettings().getTrezorWalletStorage());
            if (fs::exists(pathToDB))
                fs::remove(pathToDB);
        }
#endif
    }
    catch (std::exception& e)
    {
        LOG_ERROR() << e.what();
    }
}

void StartViewModel::migrateWalletDB(const QString& path)
{
    try
    {
        auto pathSrc = pathFromStdString(path.toStdString());
        auto pathDst = pathFromStdString(AppModel::getInstance().getSettings().getWalletFolder() + "/" + pathSrc.filename().string());
        fs::copy_file(pathSrc, pathDst);
    }
    catch (std::exception& e)
    {
        LOG_ERROR() << e.what();
    }
}

QString StartViewModel::selectCustomWalletDB()
{
    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        //% "Select the wallet database file"
        qtTrId("general-select-db"),
        //% "SQLite database file (*.db)"
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), qtTrId("start-view-db-file-filter"));

    return filePath;
}

QString StartViewModel::defaultPortToListen() const
{
    return QString("%1").arg(AppModel::getInstance().getSettings().getLocalNodePort());
}

QString StartViewModel::defaultRemoteNodeAddr() const
{
    return QString("127.0.0.1:%1").arg(defaultPortToListen());
}

void StartViewModel::checkCapsLock()
{
    emit capsLockStateMayBeChanged();
}

void StartViewModel::openFolder(const QString& path) const
{
    WalletSettings::openFolder(path);
}

void StartViewModel::loadRecoveryPhraseForValidation()
{
    auto walletModel = AppModel::getInstance().getWalletModel();
    if (walletModel)
    {
        QPointer<StartViewModel> guard(this);
        walletModel->getAsync()->readRawSeedPhrase([guard, this] (const std::string& savedSeed)
        {
            if (!guard) return;
            if (savedSeed.empty()) return;

            m_generatedPhrases.clear();
            m_recoveryPhrases.clear();

            std::vector<std::string> savedPhrases;
            boost::split(savedPhrases, savedSeed, [](char c){return c == PHRASES_SEPARATOR;});
            if (savedPhrases.size() > kPhraseSize)
                savedPhrases.erase(savedPhrases.begin() + kPhraseSize, savedPhrases.end());

            m_recoveryPhrases.reserve(kPhraseSize);
            int i = 0;
            for (const auto& p : savedPhrases)
            {
                m_generatedPhrases.push_back(p);
                m_recoveryPhrases.push_back(new RecoveryPhraseItem(i++, QString::fromStdString(p)));
            }

            emit recoveryPhrasesChanged();
        });
    }

}

bool StartViewModel::getValidateDictionary() const
{
    return m_validateDictionary;
}

void StartViewModel::setValidateDictionary(bool value)
{
    if (m_validateDictionary != value)
    {
        m_validateDictionary = value;
        emit validateDictionaryChanged();
    }
}

bool StartViewModel::isOnlyOneInstanceStarted() const
{
    return AppModel::getInstance().isOnlyOneInstanceStarted();
}

QString StartViewModel::getPhrases() const
{
    QString phrases;
    for (const auto& p : m_generatedPhrases)
    {
        phrases = phrases % p.c_str() % PHRASES_SEPARATOR;
    }
    return phrases;
}

QString StartViewModel::getCurrentNetwork() const
{
    return Rules::get().get_NetworkName();
}

void StartViewModel::setCurrentNetwork(const QString& network)
{
    auto value = getNetworkFromString(network);
    if (value == Rules::get().m_Network)
        return;

    Rules::get().m_Network = value;
    m_accounts.clear();
    setCurrentAccountIndexForced(0);
    emit currentNetworkChanged();
}

int StartViewModel::getCurrentNetworkIndex() const
{
    return (int)Rules::get().m_Network;
}

const QList<QVariantMap>& StartViewModel::getAccounts() const
{
    if (!m_accounts.isEmpty())
        return m_accounts;

    QDir dir(AppModel::getInstance().getSettings().getAppDataPath().c_str());
    if (!dir.cd(Rules::get().get_NetworkName()))
        return m_accounts;

    for (const auto& subDirInfo : dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        auto paths = findAllWalletDB(subDirInfo.absoluteFilePath().toStdString());
        if (paths.empty())
            continue;
        QString accountDir = subDirInfo.fileName();
        QSettings settings(QDir(subDirInfo.absoluteFilePath()).filePath(WalletSettings::SettingsFile), QSettings::IniFormat);
        auto label = settings.value(kAccountLabel, accountDir).toString();
        QVariantMap account;
        account[kLabel] = label;
        account[kIcon] = getAccountPictureByIndex(settings.value(kAccountPicture, 0).toInt());
        account[kIconWidth] = 16;
        account[kIconHeight] = 16;

        m_accounts.push_back(account);
    }

    return m_accounts;
}

int StartViewModel::getCurrentAccountIndex() const
{
    return m_accountIndex;
}

void StartViewModel::setCurrentAccountIndex(int value)
{
    if (m_accountIndex == value)
        return;

    setCurrentAccountIndexForced(value);
}

void StartViewModel::setCurrentAccountIndexForced(int value)
{
    m_accountIndex = value;
    m_walletDBpaths.clear();
    AppModel::resetInstance(AppModel::getInstance().getSettings(), value);
    findExistingWalletDBIfNeeded();
    emit currentAccountChanged();
    emit walletExistsChanged();
}

QString StartViewModel::getNewAccountLabel() const
{
    return m_newAccountLabel;
}

QString StartViewModel::getDefaultNewAccountLabel() const
{
    int accountIndex = getNewAccountIndex();
    //% "Account %1"
    return qtTrId("new-account-label").arg(accountIndex + 1);
}

void StartViewModel::setNewAccountLabel(const QString& value)
{
    if (m_newAccountLabel == value)
        return;

    const auto& accounts = getAccounts();
    auto it = find_if(std::begin(accounts), std::end(accounts),
        [&value](const auto& account)
    {
        return account[kLabel] == value;
    });
    if (it != std::end(accounts))
    {
        setAccountLabelExists(true);
        return;
    }
    m_newAccountLabel = value;
    emit newAccountLabelChanged();
}

bool StartViewModel::getAccountLabelExists() const
{
    return m_accountLabelExists;
}

void StartViewModel::setAccountLabelExists(bool value)
{
    if (m_accountLabelExists == value)
        return;
    m_accountLabelExists = value;
    emit accountLabelExistsChanged();
}

void StartViewModel::setNewAccountPictureIndex(int value)
{
    m_newAccountPictureIndex = value;
}

QString StartViewModel::getAccountPictureByIndex(int index) const
{
    return WalletSettings::getAccountPictureByIndex(index);
}
