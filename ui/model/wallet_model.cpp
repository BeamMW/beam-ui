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
using namespace beam::wallet;
using namespace beam::io;
using namespace std;


WalletModel::WalletModel(const Rules& rules, IWalletDB::Ptr walletDB, const std::string& nodeAddr, beam::io::Reactor::Ptr reactor, const std::string& blockchain)
    : WalletClient(rules, walletDB, nodeAddr, reactor)
    , m_blockchainName(QString::fromStdString(blockchain))
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
    qRegisterMetaType<beam::wallet::ShieldedCoinsSelectionInfo>("beam::wallet::ShieldedCoinsSelectionInfo");
    qRegisterMetaType<vector<beam::wallet::DexOrder>>("std::vector<beam::wallet::DexOrder>");

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
        //% "Cannot connect to node"
        return qtTrId("wallet-model-connection-refused-error") + ": " +  getNodeAddress().c_str();
    case wallet::ErrorType::ConnectionHostUnreach:
        //% "Node is unreachable"
        return qtTrId("wallet-model-connection-host-unreach-error") + ": " + getNodeAddress().c_str();
    case wallet::ErrorType::ConnectionAddrInUse:
    {
        auto localNodePort = AppModel2::getInstance().getSettings().getLocalNodePort(m_blockchainName);
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

bool WalletModel::isOwnAddress(const WalletID& walletID) const
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

void WalletModel::onShieldedCoinsSelectionCalculated(const ShieldedCoinsSelectionInfo& selectionRes)
{
    emit shieldedCoinsSelectionCalculated(selectionRes);
}

void WalletModel::onAllUtxoChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::Coin>& utxos)
{
    emit allUtxoChanged(action, utxos);
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
            if (action == ChangeAction::Removed)
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

void WalletModel::onGeneratedNewAddress(const beam::wallet::WalletAddress& walletAddr)
{
    emit generatedNewAddress(walletAddr);
}

void WalletModel::onGetAddress(const WalletID& id, const boost::optional<beam::wallet::WalletAddress>& address, size_t offlinePayments)
{
    emit getAddressReturned(id, address, (int)offlinePayments);
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

void WalletModel::onNodeConnectionChanged(bool isNodeConnected)
{
    emit nodeConnectionChanged(isNodeConnected);
}

void WalletModel::onWalletError(beam::wallet::ErrorType error)
{
    emit walletError(error);
}

void WalletModel::FailedToStartWallet()
{
    //% "Failed to start wallet. Please check your wallet data location"
    AppModel2::getInstance().getMessages().addMessage(qtTrId("wallet-model-data-location-error"));
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

void WalletModel::onNotificationsChanged(beam::wallet::ChangeAction action, const std::vector<Notification>& notifications)
{
    emit notificationsChanged(action, notifications);
}

void WalletModel::onPublicAddress(const std::string& publicAddr)
{
    emit publicAddressChanged(QString::fromStdString(publicAddr));
}

void WalletModel::onAssetInfo(beam::Asset::ID assetId, const WalletAsset& info)
{
    emit assetInfoChanged(assetId, info);
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

std::set<beam::Asset::ID> WalletModel::getAssetsNZ() const
{
    std::set<beam::Asset::ID> assets;

    // always have BEAM, even if zero
    assets.insert(Asset::s_BeamID);

    for(const auto& status: m_status.all)
    {
        const auto& totals = status.second;
        if (totals.available != Zero || totals.maturing != Zero || totals.maturingMP != Zero ||
            totals.receiving != Zero || totals.receivingChange != Zero || totals.receivingIncoming != Zero ||
            totals.sending   != Zero || totals.shielded != Zero)
        {
            assets.insert(status.first);
        }
    }

    return assets;
}

beam::AmountBig::Type WalletModel::getAvailable(beam::Asset::ID id) const
{
    const auto& status = m_status.GetStatus(id);

    auto result = status.available;
    result += status.shielded;

    return result;
}

beam::AmountBig::Type WalletModel::getReceiving(beam::Asset::ID id) const
{
    const auto& status = m_status.GetStatus(id);
    return status.receiving;
}

beam::AmountBig::Type WalletModel::getReceivingIncoming(beam::Asset::ID id) const
{
    const auto& status = m_status.GetStatus(id);
    return status.receivingIncoming;
}

beam::AmountBig::Type WalletModel::getMatutingMP(beam::Asset::ID id) const
{
    const auto& status = m_status.GetStatus(id);
    return status.maturingMP;
}

beam::AmountBig::Type WalletModel::getReceivingChange(beam::Asset::ID id) const
{
    const auto& status = m_status.GetStatus(id);
    return status.receivingChange;
}

beam::AmountBig::Type WalletModel::getSending(beam::Asset::ID id) const
{
    const auto& status = m_status.GetStatus(id);
    return status.sending;
}

beam::AmountBig::Type WalletModel::getMaturing(beam::Asset::ID id) const
{
    const auto& status = m_status.GetStatus(id);
    return status.maturing;
}

beam::Height WalletModel::getCurrentHeight() const
{
    return m_status.stateID.m_Height;
}

beam::Height WalletModel::getCurrentHeightTimestamp() const
{
    return m_status.update.lastTime;
}

beam::Block::SystemState::ID WalletModel::getCurrentStateID() const
{
    return m_status.stateID;
}

bool WalletModel::hasShielded(beam::Asset::ID id) const
{
    const auto& status = m_status.GetStatus(id);
    return status.shielded != Zero;
}

void WalletModel::onWalletStatusInternal(const beam::wallet::WalletStatus& newStatus)
{
    m_status = newStatus;
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
