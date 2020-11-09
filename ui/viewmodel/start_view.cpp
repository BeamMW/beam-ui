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
#include <QVariant>
#include <QStandardPaths>
#include <QJSEngine>
#include "settings_view.h"
#include "model/app_model.h"
#include "model/keyboard.h"
#include "version.h"
#include "wallet/core/secstring.h"
#include "wallet/core/default_peers.h"

#include <boost/filesystem.hpp>
#include <algorithm>
#include <thread>

#if defined(BEAM_HW_WALLET)
#include "core/block_rw.h"
#include "keykeeper/hw_wallet.h"
#endif

using namespace beam;
using namespace ECC;
using namespace std;

namespace
{
    const QChar PHRASES_SEPARATOR = ';';

    boost::filesystem::path pathFromStdString(const std::string& path)
    {
#ifdef WIN32
        boost::filesystem::path boostPath{ Utf8toUtf16(path.c_str()) };
#else
        boost::filesystem::path boostPath{ path };
#endif
        return boostPath;
    }

    std::vector<boost::filesystem::path> findAllWalletDB(const std::string& appPath)
    {
        std::vector<boost::filesystem::path> walletDBs;
        try
        {
            auto appDataPath = pathFromStdString(appPath);

            if (!boost::filesystem::exists(appDataPath))
            {
                return {};
            }

            for (boost::filesystem::recursive_directory_iterator endDirIt, it{ appDataPath }; it != endDirIt; ++it)
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

    void DoJSCallback(QJSValue& jsCallback, bool res)
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
}

RecoveryPhraseItem::RecoveryPhraseItem(int index, const QString& phrase)
    : m_index(index)
    , m_phrase(phrase)
{

}

RecoveryPhraseItem::~RecoveryPhraseItem()
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
    return m_lastWriteTime.date().toString(QLocale().dateTimeFormat(QLocale::ShortFormat));
}

QString WalletDBPathItem::getCreationDateString() const
{
    return m_creationTime.date().toString(QLocale().dateTimeFormat(QLocale::ShortFormat));
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
    , m_useHWWallet(wallet::WalletDB::isInitialized(AppModel::getInstance().getSettings().getTrezorWalletStorage()))
    , m_hwWallet(AppModel::getInstance().getHardwareWalletClient())
    , m_trezorTimer(this)
    , m_trezorThread(*this)
#else 
    , m_useHWWallet(false)
#endif

{
    if (!walletExists())
    {
        // find all wallet.db in appData and defaultAppData
        findExistingWalletDB();
    }

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

    SecString secretPass = m_password;
    if (m_creating)
    {
        DoJSCallback(m_callback, m_HWKeyKeeper && AppModel::getInstance().createTrezorWallet(secretPass, m_HWKeyKeeper));
    }
    else
    {
        DoJSCallback(m_callback, AppModel::getInstance().openWallet(secretPass, m_HWKeyKeeper));
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
    if (m_recoveryPhrases.empty())
    {
        if (!m_isRecoveryMode)
        {
            m_generatedPhrases = beam::createMnemonic(beam::getEntropy(), beam::language::en);
        }
        else
        {
            m_generatedPhrases.resize(12);
        }
        assert(m_generatedPhrases.size() == 12);
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

bool StartViewModel::isCapsLockOn() const
{
    return keyboard::isCapsLockOn();
}

void StartViewModel::setupLocalNode(int port, const QString& localNodePeer)
{
    auto& settings = AppModel::getInstance().getSettings();
    auto localAddress = QString::asprintf("127.0.0.1:%d", port);
    settings.setNodeAddress(localAddress);
    settings.setLocalNodePort(port);
    settings.setRunLocalNode(true);
    QStringList peers;
    
    for (const auto& peer : getDefaultPeers())
    {
        if (localNodePeer != peer.c_str())
        {
            peers.push_back(peer.c_str());
        }
    }

    peers.push_back(localNodePeer);
    settings.setLocalNodePeers(peers);
}

void StartViewModel::setupRemoteNode(const QString& nodeAddress)
{
    AppModel::getInstance().getSettings().setRunLocalNode(false);
    AppModel::getInstance().getSettings().setNodeAddress(nodeAddress);
}

void StartViewModel::setupRandomNode()
{
    AppModel::getInstance().getSettings().setRunLocalNode(false);
    AppModel::getInstance().getSettings().setNodeAddress(chooseRandomNode());
}

uint StartViewModel::coreAmount() const
{
    return std::thread::hardware_concurrency();
}

void StartViewModel::copyPhrasesToClipboard()
{
    QString phrases;
    for (const auto& p : m_generatedPhrases)
    {
        phrases = phrases % p.c_str() % PHRASES_SEPARATOR;
    }
    QApplication::clipboard()->setText(phrases);
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

    SecString secretSeed;
    secretSeed.assign(buf.data(), buf.size());
    SecString sectretPass = m_password;
    DoJSCallback(m_callback, AppModel::getInstance().createWallet(secretSeed, sectretPass));
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
            DoJSCallback(m_callback, false);
        }
        return;
    }
#endif
    // TODO make this secure
    SecString secretPass = pass.toStdString();
    DoJSCallback(m_callback, AppModel::getInstance().openWallet(secretPass));
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

void StartViewModel::findExistingWalletDB()
{
    auto appDataPath = AppModel::getInstance().getSettings().getAppDataPath();
    auto defaultAppDataPath = QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation)).path().toStdString();

    auto walletDBs = findAllWalletDB(appDataPath);

    if (appDataPath != defaultAppDataPath)
    {
        auto additionnalWalletDBs = findAllWalletDB(defaultAppDataPath);
        walletDBs.reserve(walletDBs.size() + additionnalWalletDBs.size());
        walletDBs.insert(std::end(walletDBs), std::begin(additionnalWalletDBs), std::end(additionnalWalletDBs));
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
        walletDBpaths.push_back(new WalletDBPathItem(
                absoluteFilePath,
                fileInfo.size(),
                fileInfo.lastModified(),
                fileInfo.birthTime(),
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

bool StartViewModel::isFindExistingWalletDB()
{
    return !m_walletDBpaths.empty();
}

void StartViewModel::deleteCurrentWalletDB()
{
    try
    {
        {
            auto pathToDB = pathFromStdString(AppModel::getInstance().getSettings().getWalletStorage());
            if (boost::filesystem::exists(pathToDB))
                boost::filesystem::remove(pathToDB);
        }

#if defined(BEAM_HW_WALLET)
        {
            auto pathToDB = pathFromStdString(AppModel::getInstance().getSettings().getTrezorWalletStorage());
            if (boost::filesystem::exists(pathToDB))
                boost::filesystem::remove(pathToDB);
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
        boost::filesystem::copy_file(pathSrc, pathDst);
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
#ifdef BEAM_TESTNET
    return "11005";
#else
    return "10005";
#endif  // BEAM_TESTNET
}

QString StartViewModel::defaultRemoteNodeAddr() const
{
#ifdef BEAM_TESTNET
    return "127.0.0.1:11005";
#else
    return "127.0.0.1:10005";
#endif // BEAM_TESTNET
}

void StartViewModel::checkCapsLock()
{
    emit capsLockStateMayBeChanged();
}

void StartViewModel::openFolder(const QString& path) const
{
    WalletSettings::openFolder(path);
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
