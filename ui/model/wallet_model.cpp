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

#include "wallet_model.h"
#include "app_model.h"
#include "utility/logger.h"
#include "utility/bridge.h"
#include "utility/io/asyncevent.h"
#include "utility/helpers.h"
#include "version.h"

using namespace beam;
using namespace beam::io;
using namespace std;


WalletModel::WalletModel(beam::wallet::IWalletDB::Ptr walletDB, const std::string& nodeAddr, beam::io::Reactor::Ptr reactor)
    : WalletClient(Rules::get(), walletDB,  nodeAddr, reactor)
{
    qRegisterMetaType<beam::ByteBuffer>("beam::ByteBuffer");
    qRegisterMetaType<beam::wallet::WalletStatus>("beam::wallet::WalletStatus");
    qRegisterMetaType<beam::wallet::ChangeAction>("beam::wallet::ChangeAction");
    qRegisterMetaType<vector<beam::wallet::TxDescription>>("std::vector<beam::wallet::TxDescription>");
    qRegisterMetaType<vector<beam::wallet::SwapOffer>>("std::vector<beam::wallet::SwapOffer>");
    qRegisterMetaType<beam::Amount>("beam::Amount");
    qRegisterMetaType<vector<beam::wallet::Coin>>("std::vector<beam::wallet::Coin>");
    qRegisterMetaType<vector<beam::wallet::ShieldedCoin>>("std::vector<beam::wallet::ShieldedCoin>");
    qRegisterMetaType<vector<beam::wallet::WalletAddress>>("std::vector<beam::wallet::WalletAddress>");
    qRegisterMetaType<beam::wallet::WalletID>("beam::wallet::WalletID");
    qRegisterMetaType<beam::wallet::WalletAddress>("beam::wallet::WalletAddress");
    qRegisterMetaType<beam::wallet::ErrorType>("beam::wallet::ErrorType");
    qRegisterMetaType<beam::wallet::TxID>("beam::wallet::TxID");
    qRegisterMetaType<beam::wallet::TxParameters>("beam::wallet::TxParameters");
    qRegisterMetaType<std::function<void()>>("std::function<void()>");
    qRegisterMetaType<std::vector<beam::wallet::Notification>>("std::vector<beam::wallet::Notification>");
    qRegisterMetaType<beam::wallet::VersionInfo>("beam::wallet::VersionInfo");
    qRegisterMetaType<beam::wallet::WalletImplVerInfo>("beam::wallet::WalletImplVerInfo");
    qRegisterMetaType<ECC::uintBig>("ECC::uintBig");
    qRegisterMetaType<boost::optional<beam::wallet::WalletAddress>>("boost::optional<beam::wallet::WalletAddress>");
    qRegisterMetaType<beam::wallet::CoinsSelectionInfo>("beam::wallet::CoinsSelectionInfo");
    qRegisterMetaType<vector<beam::wallet::DexOrder>>("std::vector<beam::wallet::DexOrder>");
    qRegisterMetaType<beam::wallet::DexOrder>("beam::wallet::DexOrder");

    connect(this, &WalletModel::walletStatusInternal, this, &WalletModel::onWalletStatusInternal);
    connect(this, SIGNAL(addressesChanged(bool, const std::vector<beam::wallet::WalletAddress>&)),this, SLOT(setAddresses(bool, const std::vector<beam::wallet::WalletAddress>&)));
    connect(this, SIGNAL(functionPosted(const std::function<void()>&)), this, SLOT(doFunction(const std::function<void()>&)));

    getAsync()->getAddresses(true);
}

WalletModel::~WalletModel()
{
    stopReactor();
}

QString WalletModel::GetErrorString(beam::wallet::ErrorType type)
{
    // TODO: add more detailed error description
    switch (type)
    {
    case wallet::ErrorType::NodeProtocolBase:
        //% "Node protocol error!"
        return qtTrId("wallet-model-node-protocol-error");
    case wallet::ErrorType::NodeProtocolIncompatible:
        //% "You are trying to connect to incompatible peer."
        return qtTrId("wallet-model-incompatible-peer-error");
    case wallet::ErrorType::ConnectionBase:
        //% "Connection error"
        return qtTrId("wallet-model-connection-base-error");
    case wallet::ErrorType::ConnectionTimedOut:
        //% "Connection timed out"
        return qtTrId("wallet-model-connection-time-out-error");
    case wallet::ErrorType::ConnectionRefused:
        //% "Connection to %1 node lost"
        return qtTrId("wallet-model-connection-refused-error").arg("BEAM") + ": " +  getNodeAddress().c_str();
    case wallet::ErrorType::ConnectionHostUnreach:
        //% "Node is unreachable"
        return qtTrId("wallet-model-connection-host-unreach-error") + ": " + getNodeAddress().c_str();
    case wallet::ErrorType::ConnectionAddrInUse:
    {
        auto localNodePort = AppModel::getInstance().getSettings().getLocalNodePort();
        //% "The port %1 is already in use. Check if a wallet is already running on this machine or change the port settings."
        return qtTrId("wallet-model-connection-addr-in-use-error").arg(QString::number(localNodePort));
    }
    case wallet::ErrorType::TimeOutOfSync:
        //% "System time not synchronized"
        return qtTrId("wallet-model-time-sync-error");
    case wallet::ErrorType::HostResolvedError:
        //% "Incorrect node name or no Internet connection."
        return qtTrId("wallet-model-host-unresolved-error");
    default:
        //% "Unexpected error!"
        return qtTrId("wallet-model-undefined-error");
    }
}

bool WalletModel::isOwnAddress(const beam::wallet::WalletID& walletID) const
{
    return m_myWalletIds.count(walletID);
}

bool WalletModel::isAddressWithCommentExist(const std::string& comment) const
{
    if (comment.empty())
    {
        return false;
    }
    return m_myAddrLabels.find(comment) != m_myAddrLabels.end();
}

void WalletModel::onStatus(const beam::wallet::WalletStatus& status)
{
    emit walletStatusInternal(status);
}

void WalletModel::onTxStatus(beam::wallet::ChangeAction action, const std::vector<beam::wallet::TxDescription>& items)
{
    emit transactionsChanged(action, items);
}

void WalletModel::onSyncProgressUpdated(int done, int total)
{
    emit syncProgressUpdated(done, total);
}

void WalletModel::onChangeCalculated(beam::Amount changeAsset, beam::Amount changeBeam, beam::Asset::ID assetId)
{
    emit changeCalculated(changeAsset, changeBeam, assetId);
}

void WalletModel::onCoinsSelected(const beam::wallet::CoinsSelectionInfo& selectionRes)
{
    emit coinsSelected(selectionRes);
}

void WalletModel::onNormalCoinsChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::Coin>& utxos)
{
    emit normalCoinsChanged(action, utxos);
}

void WalletModel::onShieldedCoinChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::ShieldedCoin>& items)
{
#ifdef BEAM_LELANTUS_SUPPORT
    emit shieldedCoinChanged(action, items);
#endif
}

void WalletModel::onAddressesChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::WalletAddress>& items)
{
    emit addressesChanged(action, items);
    for (const auto& item : items)
    {
        if (item.isOwn())
        {
            if (action == beam::wallet::ChangeAction::Removed)
            {
                m_myWalletIds.erase(item.m_walletID);
                m_myAddrLabels.erase(item.m_label);
            }
            else
            {
                m_myWalletIds.emplace(item.m_walletID);
                m_myAddrLabels.emplace(item.m_label);
            }
        }
    }
}

void WalletModel::onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addrs)
{
    emit addressesChanged(own, addrs);
}

#ifdef BEAM_ATOMIC_SWAP_SUPPORT
void WalletModel::onSwapOffersChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::SwapOffer>& offers)
{
    emit swapOffersChanged(action, offers);
}
#endif  // BEAM_ATOMIC_SWAP_SUPPORT

void WalletModel::onDexOrdersChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::DexOrder>& offers)
{
    emit dexOrdersChanged(action, offers);
}

void WalletModel::onFindDexOrder(const beam::wallet::DexOrder& order)
{
    emit dexOrdersFinded(order);
}

void WalletModel::onCoinsByTx(const std::vector<beam::wallet::Coin>& coins)
{
}

void WalletModel::onAddressChecked(const std::string& addr, bool isValid)
{
    emit addressChecked(QString::fromStdString(addr), isValid);
}

void WalletModel::onImportRecoveryProgress(uint64_t done, uint64_t total)
{
}

#if defined(BEAM_HW_WALLET)
void WalletModel::ShowKeyKeeperMessage()
{
    emit showTrezorMessage();
}

void WalletModel::HideKeyKeeperMessage()
{
    emit hideTrezorMessage();
}

void WalletModel::ShowKeyKeeperError(const std::string& error)
{
    emit showTrezorError(QString::fromStdString(error));
}
#endif

void WalletModel::onSwapParamsLoaded(const beam::ByteBuffer& params)
{
    emit swapParamsLoaded(params);
}


void WalletModel::onAssetSwapParamsLoaded(const beam::ByteBuffer& params)
{
    emit assetsSwapParamsLoaded(params);
}

void WalletModel::onGeneratedNewAddress(const beam::wallet::WalletAddress& walletAddr)
{
    emit generatedNewAddress(walletAddr);
}


void WalletModel::onNewAddressFailed()
{
    emit newAddressFailed();
}

void WalletModel::onNoDeviceConnected()
{
#if defined(BEAM_HW_WALLET)
    //% "There is no Trezor device connected. Please, connect and try again."
    showTrezorError(qtTrId("wallet-model-device-not-connected"));
#endif
}

void WalletModel::onImportDataFromJson(bool isOk)
{
}

void WalletModel::onExportDataToJson(const std::string& data)
{
}

void WalletModel::onExportTxHistoryToCsv(const std::string& data)
{
    emit txHistoryExportedToCsv(QString::fromStdString(data));
}

#ifdef BEAM_ATOMIC_SWAP_SUPPORT
void WalletModel::onExportAtomicSwapTxHistoryToCsv(const std::string& data)
{
    emit atomicSwapTxHistoryExportedToCsv(QString::fromStdString(data));
}
#endif // BEAM_ATOMIC_SWAP_SUPPORT

#ifdef BEAM_ASSET_SWAP_SUPPORT
void WalletModel::onExportAssetsSwapTxHistoryToCsv(const std::string& data)
{
    emit assetsSwapTxHistoryExportedToCsv(QString::fromStdString(data));
}
#endif  // BEAM_ASSET_SWAP_SUPPORT

void WalletModel::onExportContractTxHistoryToCsv(const std::string& data)
{
    emit contractTxHistoryExportedToCsv(QString::fromStdString(data));
}

void WalletModel::onNodeConnectionChanged(bool isNodeConnected)
{
    emit nodeConnectionChanged(isNodeConnected);
}

#ifdef BEAM_IPFS_SUPPORT
void WalletModel::onIPFSStatus(bool running, const std::string& error, unsigned int peercnt)
{
    emit IPFSStatusChanged(running, QString::fromStdString(error), peercnt);
}
#endif

void WalletModel::onWalletError(beam::wallet::ErrorType error)
{
    emit walletError(error);
}

void WalletModel::FailedToStartWallet()
{
    //% "Failed to start wallet. Please check your wallet data location"
    AppModel::getInstance().getMessages().addMessage(qtTrId("wallet-model-data-location-error"));
}

void WalletModel::onSendMoneyVerified()
{
    emit sendMoneyVerified();
}

void WalletModel::onCantSendToExpired()
{
    emit cantSendToExpired();
}

void WalletModel::onPaymentProofExported(const beam::wallet::TxID& txID, const beam::ByteBuffer& proof)
{
    string str;
    str.resize(proof.size() * 2);

    beam::to_hex(str.data(), proof.data(), proof.size());
    emit paymentProofExported(txID, QString::fromStdString(str));
}

void WalletModel::onPostFunctionToClientContext(MessageFunction&& func)
{
    emit functionPosted(func);
}

void WalletModel::onExchangeRates(const std::vector<beam::wallet::ExchangeRate>& rates)
{
    emit exchangeRatesUpdate(rates);
}

void WalletModel::onVerificationInfo(const std::vector<beam::wallet::VerificationInfo>& info)
{
    emit verificationInfoUpdate(info);
}

void WalletModel::onNotificationsChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::Notification>& notifications)
{
    emit notificationsChanged(action, notifications);
}

void WalletModel::onPublicAddress(const std::string& publicAddr)
{
    emit publicAddressChanged(QString::fromStdString(publicAddr));
}

void WalletModel::onAssetInfo(beam::Asset::ID assetId, const beam::wallet::WalletAsset& info)
{
    emit assetInfoChanged(assetId, info);
}

void WalletModel::onFullAssetsListLoaded()
{
    emit fullAssetsListLoaded();
}

void WalletModel::onInstantMessage(Timestamp time, const beam::wallet::WalletID& counterpart, const std::string& message, bool isIncome)
{
    emit instantMessage(time, counterpart, message, isIncome);
}

void WalletModel::onGetChatList(const std::vector<beam::wallet::WalletID>& chats)
{
    emit chatList(chats);
}

void WalletModel::onGetChatMessages(const std::vector<beam::wallet::InstantMessage>& messages)
{
    emit chatMessages(messages);
}

beam::Version WalletModel::getLibVersion() const
{
    beam::Version ver;
    return ver.from_string(BEAM_VERSION) ? ver : beam::Version();
}

uint32_t WalletModel::getClientRevision() const
{
    return VERSION_REVISION;
}

void WalletModel::onWalletStatusInternal(const beam::wallet::WalletStatus& newStatus)
{
    emit walletStatusChanged();
}

void WalletModel::setAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addrs)
{
    if (own)
    {
        m_myWalletIds.clear();
        m_myAddrLabels.clear();

        for (const auto& addr : addrs)
        {
            m_myWalletIds.emplace(addr.m_walletID);
            m_myAddrLabels.emplace(addr.m_label);
        }
    }
}

void WalletModel::doFunction(const std::function<void()>& func)
{
    func();
}
