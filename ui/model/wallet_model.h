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

#include "wallet/client/wallet_client.h"

#ifdef BEAM_HW_WALLET
#include "keykeeper/hw_wallet.h"
#endif

#include <set>

namespace beam::wallet
{
    struct InstantMessage;
}

class WalletModel
    : public QObject
    , public beam::wallet::WalletClient
#ifdef BEAM_HW_WALLET
    , public beam::wallet::HWWallet::IHandler
#endif
{
    Q_OBJECT
public:
    using Ptr = std::shared_ptr<WalletModel>;
    WalletModel(beam::wallet::IWalletDB::Ptr walletDB, const std::string& nodeAddr, beam::io::Reactor::Ptr reactor);
    ~WalletModel() override;

    QString GetErrorString(beam::wallet::ErrorType type);
    bool isOwnAddress(const beam::wallet::WalletID& walletID) const;
    bool isAddressWithCommentExist(const std::string& comment) const;

signals:
    // INTERNAL SIGNALS, DO NOT SUBSCRIBE IN OTHER UI OBJECTS.
    // Subscribe to non-internal counterparts
    // These are used to redirect from reactor thread to the UI thread
    // and cache some data. Due to old designed getters (getAvailable &c.).
    // Better to avoid such internal signals
    void walletStatusInternal(const beam::wallet::WalletStatus& status);

    // Public Signal
    void walletStatusChanged();
    void assetInfoChanged(beam::Asset::ID assetId, const beam::wallet::WalletAsset& info);
    void iwtCallResult(const QString& callId, boost::any);

signals:
    void transactionsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>& items);
    void syncProgressUpdated(int done, int total);
    void changeCalculated(beam::Amount changeAsset, beam::Amount changeBeam, beam::Asset::ID);
    void coinsSelected(const beam::wallet::CoinsSelectionInfo&);
    void normalCoinsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Coin>& utxos);

    #ifdef BEAM_LELANTUS_SUPPORT
    void shieldedCoinChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::ShieldedCoin>& coins);
    #endif // BEAM_LELANTUS_SUPPORT

    void addressesChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::WalletAddress>& addresses);
    void addressesChanged(bool own, const std::vector<beam::wallet::WalletAddress>& addresses);
    void swapOffersChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::SwapOffer>& offers);
    void dexOrdersChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::DexOrder>& orders);
    void dexOrdersFinded(const beam::wallet::DexOrder& order);
    void generatedNewAddress(const beam::wallet::WalletAddress& walletAddr);
    void swapParamsLoaded(const beam::ByteBuffer& params);
    void assetsSwapParamsLoaded(const beam::ByteBuffer& params);
    void newAddressFailed();
    void nodeConnectionChanged(bool isNodeConnected);
    void devStateChanged(const QString& sErr, int);
    void walletError(beam::wallet::ErrorType error);
    void sendMoneyVerified();
    void cantSendToExpired();
    void paymentProofExported(const beam::wallet::TxID& txID, const QString& proof);
    void addressChecked(const QString& addr, bool isValid);
    void functionPosted(const std::function<void()>&);
    void txHistoryExportedToCsv(const QString& data);
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    void atomicSwapTxHistoryExportedToCsv(const QString& data);
#endif // BEAM_ATOMIC_SWAP_SUPPORT
#ifdef BEAM_ASSET_SWAP_SUPPORT
    void assetsSwapTxHistoryExportedToCsv(const QString& data);
#endif  // BEAM_ASSET_SWAP_SUPPORT
    void contractTxHistoryExportedToCsv(const QString& data);

    void fullAssetsListLoaded();
    void instantMessage(beam::Timestamp time, const beam::wallet::WalletID& counterpart, const std::string& message, bool isIncome);
    void chatList(const std::vector<beam::wallet::WalletID>& chats);
    void chatMessages(const std::vector<beam::wallet::InstantMessage>& messages);
    void chatRemoved(const beam::wallet::WalletID& counterpart);

    #if defined(BEAM_HW_WALLET)
    void showTrezorMessage();
    void hideTrezorMessage();
    void showTrezorError(const QString& error);
    #endif

    #ifdef BEAM_IPFS_SUPPORT
    void IPFSStatusChanged(bool running, const QString& error, unsigned int peercnt);
    #endif

    void exchangeRatesUpdate(const std::vector<beam::wallet::ExchangeRate>&);
    void notificationsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Notification>&);
    void publicAddressChanged(const QString& publicAddr);
    void verificationInfoUpdate(const std::vector<beam::wallet::VerificationInfo>&);

private:
    void onStatus(const beam::wallet::WalletStatus& status) override;
    void onTxStatus(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>& items) override;
    void onSyncProgressUpdated(int done, int total) override;
    void onChangeCalculated(beam::Amount changeAsset, beam::Amount changeBeam, beam::Asset::ID assetId) override;
    void onCoinsSelected(const beam::wallet::CoinsSelectionInfo&) override;
    void onNormalCoinsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Coin>& utxos) override;
    void onShieldedCoinChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::ShieldedCoin>& items) override;
    void onAddressesChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::WalletAddress>& items) override;
    void onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addrs) override;
    void onDexOrdersChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::DexOrder>& offers) override;
    void onFindDexOrder(const beam::wallet::DexOrder& order) override;

    #ifdef BEAM_ATOMIC_SWAP_SUPPORT
    void onSwapOffersChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::SwapOffer>& offers) override;
    #endif  // BEAM_ATOMIC_SWAP_SUPPORT

    void onGeneratedNewAddress(const beam::wallet::WalletAddress& walletAddr) override;
    void onSwapParamsLoaded(const beam::ByteBuffer& token) override;
    void onAssetSwapParamsLoaded(const beam::ByteBuffer& params) override;
    void onNewAddressFailed() override;
    void onNodeConnectionChanged(bool isNodeConnected) override;
    void onWalletError(beam::wallet::ErrorType error) override;
    void OnDevState(const std::string& sErr, beam::wallet::HidKeyKeeper::DevState) override;
    void OnDevReject(const beam::wallet::HidKeyKeeper::CallStats&) override;
    void FailedToStartWallet() override;
    void onSendMoneyVerified() override;
    void onCantSendToExpired() override;
    void onPaymentProofExported(const beam::wallet::TxID& txID, const beam::ByteBuffer& proof) override;
    void onCoinsByTx(const std::vector<beam::wallet::Coin>& coins) override;
    void onAddressChecked(const std::string& addr, bool isValid) override;
    void onImportRecoveryProgress(uint64_t done, uint64_t total) override;
    void onNoDeviceConnected() override;
    void onImportDataFromJson(bool isOk) override;
    void onExportDataToJson(const std::string& data) override;
    void onExportTxHistoryToCsv(const std::string& data) override;
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    void onExportAtomicSwapTxHistoryToCsv(const std::string& data) override;
#endif // BEAM_ATOMIC_SWAP_SUPPORT
#ifdef BEAM_ASSET_SWAP_SUPPORT
    void onExportAssetsSwapTxHistoryToCsv(const std::string& data) override;
#endif  // BEAM_ASSET_SWAP_SUPPORT
    void onExportContractTxHistoryToCsv(const std::string& data) override;
    void onExchangeRates(const std::vector<beam::wallet::ExchangeRate>&) override;
    void onVerificationInfo(const std::vector<beam::wallet::VerificationInfo>&) override;
    void onNotificationsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Notification>&) override;
    void onPublicAddress(const std::string& publicAddr) override;
    void onAssetInfo(beam::Asset::ID, const beam::wallet::WalletAsset&) override;
    void onFullAssetsListLoaded() override;
    void onInstantMessage(beam::Timestamp time, const beam::wallet::WalletID& counterpart, const std::string& message, bool isIncome) override;
    void onGetChatList(const std::vector<beam::wallet::WalletID>& chats) override;
    void onGetChatMessages(const std::vector<beam::wallet::InstantMessage>& messages) override;
    void onChatRemoved(const beam::wallet::WalletID& counterpart) override;

    #ifdef BEAM_IPFS_SUPPORT
    virtual void onIPFSStatus(bool running, const std::string& error, unsigned int peercnt) override;
    #endif

    #ifdef BEAM_HW_WALLET
    void ShowKeyKeeperMessage() override;
    void HideKeyKeeperMessage() override;
    void ShowKeyKeeperError(const std::string&) override;
    #endif // BEAM_HW_WALLET

    void onPostFunctionToClientContext(MessageFunction&& func) override;
    beam::Version getLibVersion() const override;
    uint32_t getClientRevision() const override;

private slots:
    void onWalletStatusInternal(const beam::wallet::WalletStatus& status);
    void setAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addrs);
    void doFunction(const std::function<void()>& func);

private:
    std::set<beam::wallet::WalletID> m_myWalletIds;
    std::set<std::string> m_myAddrLabels;
};
