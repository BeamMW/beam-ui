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

    beam::Amount getAvailable(beam::Asset::ID) const;
    beam::Amount getReceiving(beam::Asset::ID) const;
    beam::Amount getReceivingIncoming(beam::Asset::ID) const;
    beam::Amount getReceivingChange(beam::Asset::ID) const;
    beam::Amount getSending(beam::Asset::ID) const;
    beam::Amount getMaturing(beam::Asset::ID) const;
    bool hasShielded(beam::Asset::ID) const;

    beam::Height getCurrentHeight() const;
    beam::Timestamp getCurrentHeightTimestamp() const;
    beam::Block::SystemState::ID getCurrentStateID() const;

signals:
    // INTERNAL SIGNALS, DO NOT SUBSCRIBE IN OTHER UI OBJECTS.
    // Subscribe to non-internal counterparts
    // These are used to redirect from reactor thread to the UI thread
    void walletStatusInternal(const beam::wallet::WalletStatus& status);
    void walletStatusChanged();

signals:
    void transactionsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>& items);
    void syncProgressUpdated(int done, int total);
    void changeCalculated(beam::Amount change);
    void shieldedCoinsSelectionCalculated(const beam::wallet::ShieldedCoinsSelectionInfo& selectionRes);
    void needExtractShieldedCoins(bool val);
    void allUtxoChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Coin>& utxos);
#ifdef BEAM_LELANTUS_SUPPORT
    void shieldedCoinChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::ShieldedCoin>& coins);
#endif // BEAM_LELANTUS_SUPPORT
    void addressesChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::WalletAddress>& addresses);
    void addressesChanged(bool own, const std::vector<beam::wallet::WalletAddress>& addresses);
    void swapOffersChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::SwapOffer>& offers);
    void generatedNewAddress(const beam::wallet::WalletAddress& walletAddr);
    void swapParamsLoaded(const beam::ByteBuffer& params);
    void newAddressFailed();
    void getAddressReturned(const beam::wallet::WalletID& id, const boost::optional<beam::wallet::WalletAddress>& address, int offlinePayments);
    void nodeConnectionChanged(bool isNodeConnected);
    void walletError(beam::wallet::ErrorType error);
    void sendMoneyVerified();
    void cantSendToExpired();
    void paymentProofExported(const beam::wallet::TxID& txID, const QString& proof);
    void addressChecked(const QString& addr, bool isValid);
    void functionPosted(const std::function<void()>&);
#if defined(BEAM_HW_WALLET)
    void showTrezorMessage();
    void hideTrezorMessage();
    void showTrezorError(const QString& error);
#endif
    void txHistoryExportedToCsv(const QString& data);

    void exchangeRatesUpdate(const std::vector<beam::wallet::ExchangeRate>&);
    void notificationsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Notification>&);

private:
    void onStatus(const beam::wallet::WalletStatus& status) override;
    void onTxStatus(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>& items) override;
    void onSyncProgressUpdated(int done, int total) override;
    void onChangeCalculated(beam::Amount change) override;
    void onShieldedCoinsSelectionCalculated(const beam::wallet::ShieldedCoinsSelectionInfo& selectionRes) override;
    void onNeedExtractShieldedCoins(bool val) override;
    void onAllUtxoChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Coin>& utxos) override;
    void onShieldedCoinChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::ShieldedCoin>& items) override;
    void onAddressesChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::WalletAddress>& items) override;
    void onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addrs) override;
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    void onSwapOffersChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::SwapOffer>& offers) override;
#endif  // BEAM_ATOMIC_SWAP_SUPPORT
    void onGeneratedNewAddress(const beam::wallet::WalletAddress& walletAddr) override;
    void onGetAddress(const beam::wallet::WalletID& id, const boost::optional<beam::wallet::WalletAddress>& address, size_t offlinePayments) override;
    void onSwapParamsLoaded(const beam::ByteBuffer& token) override;
    void onNewAddressFailed() override;
    void onNodeConnectionChanged(bool isNodeConnected) override;
    void onWalletError(beam::wallet::ErrorType error) override;
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
    void onExchangeRates(const std::vector<beam::wallet::ExchangeRate>&) override;
    void onNotificationsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Notification>&) override;
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
    beam::wallet::WalletStatus m_status;
};
