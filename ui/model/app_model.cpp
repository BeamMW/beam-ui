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

#include "app_model.h"
#include "wallet/transactions/swaps/swap_transaction.h"
#ifdef BEAM_LELANTUS_SUPPORT
#include "wallet/transactions/lelantus/unlink_transaction.h"
#include "wallet/transactions/lelantus/push_transaction.h"
#include "wallet/transactions/lelantus/pull_transaction.h"
#endif
#include "utility/common.h"
#include "utility/logger.h"
#include "utility/fsutils.h"
#include <boost/filesystem.hpp>
#include <QApplication>
#include <QTranslator>
#include <QFileDialog>
#include <QStandardPaths>

#include "wallet/transactions/swaps/bridges/bitcoin/bitcoin.h"
#include "wallet/transactions/swaps/bridges/litecoin/litecoin.h"
#include "wallet/transactions/swaps/bridges/qtum/qtum.h"
#include "wallet/transactions/swaps/bridges/dogecoin/dogecoin.h"
#include "wallet/transactions/dex/dex_tx.h"
#if defined(BITCOIN_CASH_SUPPORT)
#include "wallet/transactions/swaps/bridges/bitcoin_cash/bitcoin_cash.h"
#endif // BITCOIN_CASH_SUPPORT
#include "wallet/transactions/swaps/bridges/dash/dash.h"

#include "keykeeper/local_private_key_keeper.h"

#if defined(BEAM_HW_WALLET)
#include "core/block_rw.h"
#include "keykeeper/hw_wallet.h"
#include "keykeeper/trezor_key_keeper.h"
#endif

using namespace beam;
using namespace beam::wallet;
using namespace ECC;
using namespace std;

namespace
{
    void generateDefaultAddress(IWalletDB::Ptr db)
    {
        // generate default address
        WalletAddress address;
        db->createAddress(address);
        address.m_label = "default";
        db->saveAddress(address);
    }
}

AppModel* AppModel::s_instance = nullptr;

AppModel& AppModel::getInstance()
{
    assert(s_instance != nullptr);
    return *s_instance;
}

// static
std::string AppModel::getMyName()
{
    return "Beam Wallet UI";
}

const std::string& AppModel::getMyVersion()
{
    static std::string appVersion
#ifdef BEAM_CLIENT_VERSION
        = AppModel::getMyName() + " " + std::string(BEAM_CLIENT_VERSION)
#endif
    ;
    return appVersion;
}

AppModel::AppModel(WalletSettings& settings)
    : m_settings{settings}
    , m_walletReactor(beam::io::Reactor::create())
{
    assert(s_instance == nullptr);
    s_instance = this;
    m_nodeModel.start();
}

AppModel::~AppModel()
{
    s_instance = nullptr;
}

void AppModel::backupDB(const std::string& dbFilePath)
{
    const auto wasInitialized = WalletDB::isInitialized(dbFilePath);
    m_db.reset();

    if (wasInitialized)
    {
        // it seems that we are trying to restore or login to another wallet.
        // Rename/backup existing db
        std::string newName = dbFilePath + "_" + to_string(getTimestamp());

        try
        {
            fsutils::rename(dbFilePath, newName);
            m_walletDBBackupPath = std::move(newName);
        }
        catch(std::runtime_error& err)
        {
            LOG_ERROR() << "failed to backup DB, " << err.what();
        }
    }
}

void AppModel::restoreDBFromBackup(const std::string& dbFilePath)
{
    const auto wasInitialized = WalletDB::isInitialized(dbFilePath);
    m_db.reset();

    if (!wasInitialized && !m_walletDBBackupPath.empty())
    {
        if (fsutils::exists(m_walletDBBackupPath))
        {
            try
            {
                fsutils::rename(m_walletDBBackupPath, dbFilePath);
                m_walletDBBackupPath = {};
            }
            catch(const std::runtime_error& err)
            {
                LOG_ERROR() << "failed to restore DB, " << err.what();
            }
        }
    }
}

bool AppModel::createWallet(const SecString& seed, const SecString& pass)
{
    const auto dbFilePath = m_settings.getWalletStorage();
    backupDB(dbFilePath);
    {
        io::Reactor::Scope s(*m_walletReactor); // do it in main thread
        auto db = WalletDB::init(dbFilePath, pass, seed.hash());
        if (!db) 
            return false;

        generateDefaultAddress(db);
    }

    return openWallet(pass);
}

#if defined(BEAM_HW_WALLET)
bool AppModel::createTrezorWallet(const beam::SecString& pass, beam::wallet::IPrivateKeyKeeper2::Ptr keyKeeper)
{
    const auto dbFilePath = m_settings.getTrezorWalletStorage();
    backupDB(dbFilePath);
    {
        io::Reactor::Scope s(*m_walletReactor); // do it in main thread
        auto db = WalletDB::init(dbFilePath, pass, keyKeeper);
        if (!db)
            return false;

        generateDefaultAddress(db);
    }

    return openWallet(pass, keyKeeper);
}

std::shared_ptr<beam::wallet::HWWallet> AppModel::getHardwareWalletClient() const
{
    if (!m_hwWallet)
    {
        m_hwWallet = std::make_shared<beam::wallet::HWWallet>();
    }
    return m_hwWallet;
}

beam::io::Reactor::Ptr AppModel::getWalletReactor() const
{
    return m_walletReactor;
}
#endif

beam::wallet::IWalletDB::Ptr AppModel::getWalletDB() const
{
    return m_db;
}

bool AppModel::openWallet(const beam::SecString& pass, beam::wallet::IPrivateKeyKeeper2::Ptr keyKeeper)
{
    assert(m_db == nullptr);

    try
    {
        if (WalletDB::isInitialized(m_settings.getWalletStorage()))
        {
            m_db = WalletDB::open(m_settings.getWalletStorage(), pass);
        }
#if defined(BEAM_HW_WALLET)
        else if (WalletDB::isInitialized(m_settings.getTrezorWalletStorage()))
        {
            m_db = WalletDB::open(m_settings.getTrezorWalletStorage(), pass, keyKeeper);
        }
#endif

        if (!m_db)
            return false;

        onWalledOpened(pass);
        return true;
    }
    catch (...)
    {
        // TODO: handle the reasons of failure
    }

    return false;
}

void AppModel::onWalledOpened(const beam::SecString& pass)
{
    m_passwordHash = pass.hash();
    start();
}

bool AppModel::exportData()
{
    try
    {
        const auto fileName = beam::wallet::TimestampFile("export.dat");
        const auto path = QFileDialog::getSaveFileName(nullptr, "Export wallet data",
                QDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).filePath(fileName.c_str()),
                "Wallet data (*.dat)");

        const auto jsonData = storage::ExportDataToJson(*m_db);

        FStream fStream;
        return fStream.Open(path.toStdString().c_str(), false) &&
               fStream.write(jsonData.data(), jsonData.size()) == jsonData.size();
    }
    catch(const std::runtime_error&)
    {
        return false;
    }
}

bool AppModel::importData()
{
    try
    {
        const auto path = QFileDialog::getOpenFileName(nullptr, "Import wallet data",
                QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                "Wallet data (*.dat)");

        FStream f;
        ByteBuffer buffer;

        if (!f.Open(path.toStdString().c_str(), true))
        {
            return false;
        }
        const auto size = static_cast<size_t>(f.get_Remaining());
        if (size > 0)
        {
            buffer.resize(size);
            if (f.read(buffer.data(), buffer.size()) != size) 
            {
                return false;
            }
        }
        
        m_wallet->getAsync()->importDataFromJson(std::string(reinterpret_cast<const char*>(buffer.data()), buffer.size()));

        return true;
    }
    catch(const std::runtime_error&)
    {
        return false;
    }
}

void AppModel::resetWallet()
{
    if (m_nodeModel.isNodeRunning())
    {
        m_nsc.disconnect();

        auto dconn = MakeConnectionPtr();
        *dconn = connect(&m_nodeModel, &NodeModel::destroyedNode, [this, dconn]() {
            QObject::disconnect(*dconn);
            emit walletReset();
        });

        m_nodeModel.stopNode();
        return;
    }

    onResetWallet();
}

void AppModel::onResetWallet()
{
    m_walletConnections.disconnect();

    assert(m_assets);
    assert(m_assets.use_count() == 1);
    m_assets.reset();

    assert(m_wallet);
    assert(m_wallet.use_count() == 1);
    m_wallet.reset();

    resetSwapClients();
    assert(m_db);
    m_db.reset();

    try
    {
        fsutils::remove(getSettings().getWalletStorage());

        #if defined(BEAM_HW_WALLET)
        fsutils::remove(getSettings().getTrezorWalletStorage());
        #endif

        fsutils::remove(getSettings().getLocalNodeStorage());
    }
    catch(std::runtime_error& err)
    {
        LOG_ERROR() << "Error while removing files in onResetWallet: " << err.what();
    }

    restoreDBFromBackup(getSettings().getWalletStorage());
    emit walletResetCompleted();
}

void AppModel::startWallet()
{
    assert(!m_wallet->isRunning());

    auto additionalTxCreators = std::make_shared<std::unordered_map<TxType, BaseTransaction::Creator::Ptr>>();
    auto swapTransactionCreator = std::make_shared<beam::wallet::AtomicSwapTransaction::Creator>(m_db);

    registerSwapFactory<BitcoinSide, bitcoin::Electrum, bitcoin::ISettingsProvider>(AtomicSwapCoin::Bitcoin, *swapTransactionCreator);
    registerSwapFactory<LitecoinSide, litecoin::Electrum, litecoin::ISettingsProvider>(AtomicSwapCoin::Litecoin, *swapTransactionCreator);
    registerSwapFactory<QtumSide, qtum::Electrum, qtum::ISettingsProvider>(AtomicSwapCoin::Qtum, *swapTransactionCreator);
#if defined(BITCOIN_CASH_SUPPORT)
    registerSwapFactory<BitcoinCashSide, bitcoin_cash::Electrum, bitcoin_cash::ISettingsProvider>(AtomicSwapCoin::Bitcoin_Cash, *swapTransactionCreator);
#endif // BITCOIN_CASH_SUPPORT
    registerSwapFactory<DogecoinSide, dogecoin::Electrum, dogecoin::ISettingsProvider>(AtomicSwapCoin::Dogecoin, *swapTransactionCreator);
    registerSwapFactory<DashSide, dash::Electrum, dash::ISettingsProvider>(AtomicSwapCoin::Dash, *swapTransactionCreator);

    additionalTxCreators->emplace(TxType::AtomicSwap, swapTransactionCreator);

    std::map<Notification::Type,bool> activeNotifications {

        { Notification::Type::SoftwareUpdateAvailable, false },
        { Notification::Type::WalletImplUpdateAvailable, m_settings.isNewVersionActive() },
        { Notification::Type::AddressStatusChanged, false },    // turned off
        { Notification::Type::BeamNews, m_settings.isBeamNewsActive() },
        { Notification::Type::TransactionFailed, m_settings.isTxStatusActive() },
        { Notification::Type::TransactionCompleted, m_settings.isTxStatusActive() }
    };

#ifdef BEAM_LELANTUS_SUPPORT
    additionalTxCreators->emplace(TxType::PushTransaction, std::make_shared<lelantus::PushTransaction::Creator>(m_db));
#endif

    additionalTxCreators->emplace(TxType::DexSimpleSwap, std::make_shared<DexTransaction::Creator>(m_db));

    bool displayRate = m_settings.getSecondCurrency().toStdString() != exchangeRateOffStr;
    m_wallet->start(activeNotifications, displayRate, additionalTxCreators);
}

template<typename BridgeSide, typename Bridge, typename SettingsProvider>
void AppModel::registerSwapFactory(AtomicSwapCoin swapCoin, beam::wallet::AtomicSwapTransaction::Creator& swapTxCreator)
{
    if (auto client = getSwapCoinClient(swapCoin); client)
    {
        auto bridgeHolder = m_swapBridgeHolders[swapCoin];
        auto bridgeCreator = [bridgeHolder, reactor = m_walletReactor, settingsProvider = client]() -> bitcoin::IBridge::Ptr
        {
            return bridgeHolder->Get(*reactor, *settingsProvider);
        };

        auto secondSideFactory = wallet::MakeSecondSideFactory<BridgeSide, Bridge, SettingsProvider>(bridgeCreator, *client);
        swapTxCreator.RegisterFactory(swapCoin, secondSideFactory);
    }
}

void AppModel::applySettingsChanges()
{
    if (m_nodeModel.isNodeRunning())
    {
        m_nsc.disconnect();
        m_nodeModel.stopNode();
    }

    if (m_settings.getRunLocalNode())
    {
        startNode();

        io::Address nodeAddr = io::Address::LOCALHOST;
        nodeAddr.port(m_settings.getLocalNodePort());
        m_wallet->getAsync()->setNodeAddress(nodeAddr.str());
    }
    else
    {
        auto nodeAddr = m_settings.getNodeAddress().toStdString();
        m_wallet->getAsync()->setNodeAddress(nodeAddr);
    }
}

void AppModel::nodeSettingsChanged()
{
    applySettingsChanges();
    if (!m_settings.getRunLocalNode())
    {
        if (!m_wallet->isRunning())
        {
            startWallet();
        }
    }
}

void AppModel::onStartedNode()
{
    m_nsc.disconnect();
    assert(m_wallet);

    if (!m_wallet->isRunning())
    {
        startWallet();
    }
}

void AppModel::onFailedToStartNode(beam::wallet::ErrorType errorCode)
{
    m_nsc.disconnect();

    if (errorCode == beam::wallet::ErrorType::ConnectionAddrInUse && m_wallet)
    {
        emit m_wallet->walletError(errorCode);
        return;
    }

    if (errorCode == beam::wallet::ErrorType::TimeOutOfSync && m_wallet)
    {
        //% "Failed to start the integrated node: the timezone settings of your machine are out of sync. Please fix them and restart the wallet."
        getMessages().addMessage(qtTrId("appmodel-failed-time-not-synced"));
        return;
    }

    //% "Failed to start node. Please check your node configuration"
    getMessages().addMessage(qtTrId("appmodel-failed-start-node"));
}

void AppModel::start()
{
    m_walletConnections << connect(this, &AppModel::walletReset, this, &AppModel::onResetWallet);

    m_nodeModel.setKdf(m_db->get_MasterKdf());
    m_nodeModel.setOwnerKey(m_db->get_OwnerKdf());

    std::string nodeAddrStr = m_settings.getNodeAddress().toStdString();
    if (m_settings.getRunLocalNode())
    {
        io::Address nodeAddr = io::Address::LOCALHOST;
        nodeAddr.port(m_settings.getLocalNodePort());
        nodeAddrStr = nodeAddr.str();
    }

    initSwapClients();

    m_wallet = std::make_shared<WalletModel>(m_db, nodeAddrStr, m_walletReactor);
    m_assets = std::make_shared<AssetsManager>(m_wallet);

    if (m_settings.getRunLocalNode())
    {
        startNode();
    }
    else
    {
        startWallet();
    }
}

void AppModel::startNode()
{
    m_nsc
        << connect(&m_nodeModel, &NodeModel::startedNode, this, &AppModel::onStartedNode)
        << connect(&m_nodeModel, &NodeModel::failedToStartNode, this, &AppModel::onFailedToStartNode)
        << connect(&m_nodeModel, &NodeModel::failedToSyncNode, this, &AppModel::onFailedToStartNode);

    m_nodeModel.startNode();
}

bool AppModel::checkWalletPassword(const beam::SecString& pass) const
{
    auto passwordHash = pass.hash();
    return passwordHash.V == m_passwordHash.V;
}

void AppModel::changeWalletPassword(const std::string& pass)
{
    beam::SecString t = pass;
    m_passwordHash.V = t.hash().V;
    m_wallet->getAsync()->changeWalletPassword(pass);
}

WalletModel::Ptr AppModel::getWalletModel() const
{
    return m_wallet;
}

WalletSettings& AppModel::getSettings() const
{
    return m_settings;
}

AssetsManager::Ptr AppModel::getAssets() const
{
    return m_assets;
}

MessageManager& AppModel::getMessages()
{
    return m_messages;
}

NodeModel& AppModel::getNode()
{
    return m_nodeModel;
}

SwapCoinClientModel::Ptr AppModel::getSwapCoinClient(beam::wallet::AtomicSwapCoin swapCoin) const
{
    auto it = m_swapClients.find(swapCoin);
    if (it != m_swapClients.end())
    {
        return it->second;
    }
    return nullptr;
}

void AppModel::initSwapClients()
{
    initSwapClient<bitcoin::BitcoinCore017, bitcoin::Electrum, bitcoin::SettingsProvider>(AtomicSwapCoin::Bitcoin);
    initSwapClient<litecoin::LitecoinCore017, litecoin::Electrum, litecoin::SettingsProvider>(AtomicSwapCoin::Litecoin);
    initSwapClient<qtum::QtumCore017, qtum::Electrum, qtum::SettingsProvider>(AtomicSwapCoin::Qtum);
    initSwapClient<dash::DashCore014, dash::Electrum, dash::SettingsProvider>(AtomicSwapCoin::Dash);
#if defined(BITCOIN_CASH_SUPPORT)
    initSwapClient<bitcoin_cash::BitcoinCashCore, bitcoin_cash::Electrum, bitcoin_cash::SettingsProvider>(AtomicSwapCoin::Bitcoin_Cash);
#endif // BITCOIN_CASH_SUPPORT
    initSwapClient<dogecoin::DogecoinCore014, dogecoin::Electrum, dogecoin::SettingsProvider>(AtomicSwapCoin::Dogecoin);
}

template<typename CoreBridge, typename ElectrumBridge, typename SettingsProvider>
void AppModel::initSwapClient(beam::wallet::AtomicSwapCoin swapCoin)
{
    auto bridgeHolder = std::make_shared<bitcoin::BridgeHolder<ElectrumBridge, CoreBridge>>();
    auto settingsProvider = std::make_unique<SettingsProvider>(m_db);
    settingsProvider->Initialize();
    auto client = std::make_shared<SwapCoinClientModel>(bridgeHolder, std::move(settingsProvider), *m_walletReactor);
    m_swapClients.emplace(std::make_pair(swapCoin, client));
    m_swapBridgeHolders.emplace(std::make_pair(swapCoin, bridgeHolder));
}

void AppModel::resetSwapClients()
{
    m_swapClients.clear();
}
