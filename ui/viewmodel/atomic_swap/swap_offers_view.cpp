// Copyright 2019 The Beam Team
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
#include <qdebug.h>
#include "model/app_model.h"
#include "model/settings.h"
#include "swap_offers_view.h"
#include "viewmodel/ui_helpers.h"
#include "viewmodel/qml_globals.h"

SwapCoinClientWrapper::SwapCoinClientWrapper(beam::wallet::AtomicSwapCoin swapCoin)
    : m_swapCoin(swapCoin)
    , m_coinClient(!beam::ethereum::IsEthereumBased(swapCoin) ? AppModel::getInstance().getSwapCoinClient(swapCoin) : nullptr)
    , m_ethClient(beam::ethereum::IsEthereumBased(swapCoin) ? AppModel::getInstance().getSwapEthClient() : nullptr)
{
    if (beam::ethereum::IsEthereumBased(swapCoin))
    {
        auto coinClient = m_ethClient.lock();
        auto settings = coinClient->GetSettings();
        m_lockTxMinConfirmations = settings.GetLockTxMinConfirmations();
        m_withdrawTxMinConfirmations = settings.GetWithdrawTxMinConfirmations();
        m_blocksPerHour = settings.GetBlocksPerHour();

        connect(coinClient.get(), SIGNAL(balanceChanged()), this, SIGNAL(availableChanged()));
        connect(coinClient.get(), SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));
    }
    else
    {
        auto coinClient = m_coinClient.lock();
        auto settings = coinClient->GetSettings();
        m_lockTxMinConfirmations = settings.GetLockTxMinConfirmations();
        m_withdrawTxMinConfirmations = settings.GetWithdrawTxMinConfirmations();
        m_blocksPerHour = settings.GetBlocksPerHour();

        connect(coinClient.get(), SIGNAL(balanceChanged()), this, SIGNAL(availableChanged()));
        connect(coinClient.get(), SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));
    }

}

void SwapCoinClientWrapper::incrementActiveTxCounter()
{
    ++m_activeTxCounter;
}

void SwapCoinClientWrapper::decrementActiveTxCounter()
{
    --m_activeTxCounter;
}

void SwapCoinClientWrapper::resetActiveTxCounter()
{
    m_activeTxCounter = 0;
}

QString SwapCoinClientWrapper::getAvailableStr() const
{
    return beamui::AmountToUIString(getAvailable(), beamui::convertSwapCoinToCurrency(m_swapCoin), false);
}

bool SwapCoinClientWrapper::getIsConnected() const
{
    if (beam::ethereum::IsEthereumBased(m_swapCoin))
    {
        return m_ethClient.lock()->getStatus() == beam::ethereum::Client::Status::Connected;
    }

    return m_coinClient.lock()->getStatus() == beam::bitcoin::Client::Status::Connected;
}

bool SwapCoinClientWrapper::getIsConnecting() const
{
    if (beam::ethereum::IsEthereumBased(m_swapCoin))
    {
        return m_ethClient.lock()->getStatus() == beam::ethereum::Client::Status::Connecting;
    }

    return m_coinClient.lock()->getStatus() == beam::bitcoin::Client::Status::Connecting;
}

bool SwapCoinClientWrapper::hasActiveTx() const
{
    return m_activeTxCounter > 0;
}

QString SwapCoinClientWrapper::getCoinLabel() const
{
    return beamui::getCurrencyUnitName(beamui::convertSwapCoinToCurrency(m_swapCoin)).toUpper();
}

OldWalletCurrency::OldCurrency SwapCoinClientWrapper::getCurrency() const
{
    return convertSwapCoinToCurrency(m_swapCoin);
}

QColor SwapCoinClientWrapper::getGradientColor() const
{
    switch(m_swapCoin)
    {
    case beam::wallet::AtomicSwapCoin::Bitcoin:
        return QColor("#fcaf38");
    case beam::wallet::AtomicSwapCoin::Litecoin:
        return QColor("#bebebe");
    case beam::wallet::AtomicSwapCoin::Qtum:
        return QColor("#2e9ad0");
#if defined(BITCOIN_CASH_SUPPORT)
    case beam::wallet::AtomicSwapCoin::Bitcoin_Cash:
        return QColor("#ff6700");
#endif // BITCOIN_CASH_SUPPORT
    case beam::wallet::AtomicSwapCoin::Dash:
        return QColor("#0092ff");
    case beam::wallet::AtomicSwapCoin::Dogecoin:
        return QColor("#e0cd81");
    case beam::wallet::AtomicSwapCoin::Ethereum:
        return QColor("#8a93b2");
    case beam::wallet::AtomicSwapCoin::Dai:
        return QColor("#f5ac37");
    case beam::wallet::AtomicSwapCoin::Usdt:
        return QColor("#468a77");
    case beam::wallet::AtomicSwapCoin::WBTC:
        return QColor("#f09241");
    default:
        assert(false);
        return QColor("#ff0000");
    }
}

QString SwapCoinClientWrapper::getCoinIcon() const
{
    switch(m_swapCoin)
    {
    case beam::wallet::AtomicSwapCoin::Bitcoin:
        return "qrc:/assets/icon-btc.svg";
    case beam::wallet::AtomicSwapCoin::Litecoin:
        return "qrc:/assets/icon-ltc.svg";
    case beam::wallet::AtomicSwapCoin::Qtum:
        return "qrc:/assets/icon-qtum.svg";
#if defined(BITCOIN_CASH_SUPPORT)
    case beam::wallet::AtomicSwapCoin::Bitcoin_Cash:
        return "qrc:/assets/icon-bch.svg";
#endif // BITCOIN_CASH_SUPPORT
    case beam::wallet::AtomicSwapCoin::Dash:
        return "qrc:/assets/icon-dash.svg";
    case beam::wallet::AtomicSwapCoin::Dogecoin:
        return "qrc:/assets/icon-doge.svg";
    case beam::wallet::AtomicSwapCoin::Ethereum:
        return "qrc:/assets/icon-eth.svg";
    case beam::wallet::AtomicSwapCoin::Dai:
        return "qrc:/assets/icon-dai.svg";
    case beam::wallet::AtomicSwapCoin::Usdt:
        return "qrc:/assets/icon-usdt.svg";
    case beam::wallet::AtomicSwapCoin::WBTC:
        return "qrc:/assets/icon-wbtc.svg";
    default:
        assert(false);
        return "";
    }
}

beam::wallet::AtomicSwapCoin SwapCoinClientWrapper::getSwapCoin() const
{
    return m_swapCoin;
}

uint16_t SwapCoinClientWrapper::getLockTxMinConfirmations() const
{
    return m_lockTxMinConfirmations;
}

uint16_t SwapCoinClientWrapper::getWithdrawTxMinConfirmations() const
{
    return m_withdrawTxMinConfirmations;
}

double SwapCoinClientWrapper::getBlocksPerHour() const
{
    return m_blocksPerHour;
}

beam::Amount SwapCoinClientWrapper::getAvailable() const
{
    if (beam::ethereum::IsEthereumBased(m_swapCoin))
    {
        return m_ethClient.lock()->getAvailable(m_swapCoin);
    }
    return m_coinClient.lock()->getAvailable();
}

SwapOffersViewModel::SwapOffersViewModel()
    :   m_walletModel{*AppModel::getInstance().getWalletModel()}
{
    InitSwapClientWrappers();

    connect(&m_walletModel, &WalletModel::walletStatusChanged, this, &SwapOffersViewModel::beamAvailableChanged);
    connect(&m_walletModel,
            SIGNAL(transactionsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>&)),
            SLOT(onTransactionsDataModelChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>&)));

    connect(&m_walletModel,
            SIGNAL(swapOffersChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::SwapOffer>&)),
            SLOT(onSwapOffersDataModelChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::SwapOffer>&)));

    monitorAllOffersFitBalance();

    m_walletModel.getAsync()->getSwapOffers();
    m_walletModel.getAsync()->getTransactions();    
}

SwapOffersViewModel::~SwapOffersViewModel()
{
    qDeleteAll(m_swapClientWrappers);
}

QAbstractItemModel* SwapOffersViewModel::getAllOffers()
{
    return &m_offersList;
}

QAbstractItemModel* SwapOffersViewModel::getAllOffersFitBalance()
{
    return &m_offersListFitBalance;
}

QString SwapOffersViewModel::beamAvailable() const
{
    auto available = beam::AmountBig::get_Lo(m_walletModel.getAvailable(beam::Asset::s_BeamID));
    return beamui::AmountToUIString(available);
}

QAbstractItemModel* SwapOffersViewModel::getTransactions()
{
    return &m_transactionsList;
}

void SwapOffersViewModel::cancelOffer(const QVariant& variantTxID)
{
    if (!variantTxID.isNull() && variantTxID.isValid())
    {
        auto txId = variantTxID.value<beam::wallet::TxID>();
        LOG_INFO() << txId << " Cancel offer";
        m_walletModel.getAsync()->cancelTx(txId);
    }
}

void SwapOffersViewModel::cancelTx(const QVariant& variantTxID)
{
    if (!variantTxID.isNull() && variantTxID.isValid())
    {
        auto txId = variantTxID.value<beam::wallet::TxID>();
        m_walletModel.getAsync()->cancelTx(txId);
    }
}

void SwapOffersViewModel::deleteTx(const QVariant& variantTxID)
{
    if (!variantTxID.isNull() && variantTxID.isValid())
    {
        auto txId = variantTxID.value<beam::wallet::TxID>();
        m_walletModel.getAsync()->deleteTx(txId);
    }
}

PaymentInfoItem* SwapOffersViewModel::getPaymentInfo(const QVariant& variantTxID)
{
    if (!variantTxID.isNull() && variantTxID.isValid())
    {
        auto txId = variantTxID.value<beam::wallet::TxID>();
        return new MyPaymentInfoItem(txId, this);
    }
    else return Q_NULLPTR;
}

void SwapOffersViewModel::onTransactionsDataModelChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::TxDescription>& transactions)
{
    using namespace beam::wallet;

    std::vector<std::shared_ptr<SwapTxObject>> swapTransactions;
    std::vector<std::shared_ptr<SwapTxObject>> activeTransactions;
    std::vector<std::shared_ptr<SwapTxObject>> inactiveTransactions;
    swapTransactions.reserve(transactions.size());

    for (const auto& t : transactions)
    {
        if (t.GetParameter<TxType>(TxParameterID::TransactionType) == TxType::AtomicSwap)
        {
            auto swapCoinType = t.GetParameter<AtomicSwapCoin>(TxParameterID::AtomicSwapCoin);
            uint32_t lockTxMinConfirmations = swapCoinType ? getLockTxMinConfirmations(*swapCoinType) : 0;
            uint32_t withdrawTxMinConfirmations = swapCoinType ? getWithdrawTxMinConfirmations(*swapCoinType) : 0;
            double blocksPerHour = swapCoinType ? getBlocksPerHour(*swapCoinType) : 0;
            auto newItem = std::make_shared<SwapTxObject>(t, lockTxMinConfirmations, withdrawTxMinConfirmations, blocksPerHour);
            swapTransactions.push_back(newItem);
            if (!newItem->isPending() && newItem->isInProgress())
            {
                activeTransactions.push_back(newItem);
            }
            else
            {
                inactiveTransactions.push_back(newItem);
            }
        }
    }

    if (swapTransactions.empty())
    {
        return;
    }
    
    auto eraseActive = [this](auto tx)
    {
        if (m_activeTx.erase(tx->getTxID()) > 0) // item was erased
        {
            auto swapCoinType = tx->getSwapCoinType();
            decrementActiveTxCounter(swapCoinType);
        }
    };

    auto insertActive = [this](auto tx)
    {
        auto swapCoinType = tx->getSwapCoinType();
        auto p = m_activeTx.emplace(tx->getTxID(), swapCoinType);
        if (p.second) // new item was inserted
        {
            incrementActiveTxCounter(swapCoinType);
        }
    };

    switch (action)
    {
        case ChangeAction::Reset:
            {
                m_transactionsList.reset(swapTransactions);
                m_activeTx.clear();
                resetActiveTxCounters();
                for (auto tx : activeTransactions)
                {
                    auto swapCoinType = tx->getSwapCoinType();
                    m_activeTx.emplace(tx->getTxID(), swapCoinType);
                    incrementActiveTxCounter(swapCoinType);
                }
                break;
            }

        case ChangeAction::Removed:
            {
                m_transactionsList.remove(swapTransactions);
                for (auto tx : swapTransactions)
                {
                    eraseActive(tx);
                }
                break;
            }

        case ChangeAction::Added:
            {
                m_transactionsList.insert(swapTransactions);
                for (auto tx : activeTransactions)
                {
                    insertActive(tx);
                }
                break;
            }
        
        case ChangeAction::Updated:
            {
                m_transactionsList.update(swapTransactions);
                for (auto tx : activeTransactions)
                {
                    insertActive(tx);
                }
                for (auto tx : inactiveTransactions)
                {
                    eraseActive(tx);
                }

                break;
            }

        default:
            assert(false && "Unexpected action");
            break;
    }
    m_activeTxCount = static_cast<int>(m_activeTx.size());
    emit allTransactionsChanged();
}

void SwapOffersViewModel::onSwapOffersDataModelChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::SwapOffer>& offers)
{
    using namespace beam::wallet;

    std::vector<std::shared_ptr<SwapOfferItem>> modifiedOffers;
    modifiedOffers.reserve(offers.size());

    for (const auto& offer : offers)
    {

        // Offers without publisherID don't pass validation
        auto peerResponseTime = offer.peerResponseHeight();
        auto minHeight = offer.minHeight();
        auto currentHeight = m_walletModel.getCurrentHeight();
        auto currentHeightTimestamp = m_walletModel.getCurrentHeightTimestamp();

        QDateTime timeExpiration;
        if (currentHeight && peerResponseTime && minHeight)
        {
            auto expiresHeight = minHeight + peerResponseTime;
            timeExpiration = beamui::CalculateExpiresTime(currentHeightTimestamp, currentHeight, expiresHeight);
        }

        modifiedOffers.push_back(std::make_shared<SwapOfferItem>(offer, timeExpiration));
    }

    switch (action)
    {
        case ChangeAction::Reset:
            {
                m_offersList.reset(modifiedOffers);
                resetAllOffersFitBalance();
                break;
            }

        case ChangeAction::Added:
            {
                m_offersList.insert(modifiedOffers);
                insertAllOffersFitBalance(modifiedOffers);
                break;
            }

        case ChangeAction::Removed:
            {
                for (const auto& modifiedOffer: modifiedOffers)
                {
                    emit offerRemovedFromTable(
                        QVariant::fromValue(modifiedOffer->getTxID()));
                }
                m_offersList.remove(modifiedOffers);
                removeAllOffersFitBalance(modifiedOffers);
                break;
            }
        
        default:
            assert(false && "Unexpected action");
            break;
    }
    
    emit allOffersChanged();
    setIsOffersLoaded(true);
}

void SwapOffersViewModel::resetAllOffersFitBalance()
{
    auto offersCount = m_offersList.rowCount();
    std::vector<std::shared_ptr<SwapOfferItem>> offersListFitBalance;
    offersListFitBalance.reserve(offersCount);

    for(int i = 0; i < offersCount; ++i)
    {
        const auto&it = m_offersList.get(i);
        if (isOfferFitBalance(*it))
            offersListFitBalance.push_back(it);
    }
    m_offersListFitBalance.reset(offersListFitBalance);
    emit allOffersFitBalanceChanged();
}

bool SwapOffersViewModel::showBetaWarning() const
{
    auto& settings = AppModel::getInstance().getSettings();
    bool showWarning = settings.showSwapBetaWarning();
    if (showWarning)
    {
        settings.setShowSwapBetaWarning(false);
    }
    return showWarning;
}

bool SwapOffersViewModel::isOffersLoaded() const
{
    return m_isOffersLoaded;
}

void SwapOffersViewModel::setIsOffersLoaded(bool isOffersLoaded)
{
    if (m_isOffersLoaded != isOffersLoaded)
    {
        m_isOffersLoaded = isOffersLoaded;
        emit offersLoaded();
    }
}

int SwapOffersViewModel::getActiveTxCount() const
{
    return m_activeTxCount;
}

void SwapOffersViewModel::monitorAllOffersFitBalance()
{
    connect(this, SIGNAL(beamAvailableChanged()), SLOT(resetAllOffersFitBalance()));

    for (auto swapClientWrapper : m_swapClientWrappers)
    {
        connect(swapClientWrapper, SIGNAL(availableChanged()), this, SLOT(resetAllOffersFitBalance()));
        connect(swapClientWrapper, SIGNAL(statusChanged()), this, SLOT(resetAllOffersFitBalance()));
        connect(this, SIGNAL(allTransactionsChanged()), swapClientWrapper, SIGNAL(activeTxChanged()));
    }
}

// TODO: check logic
bool SwapOffersViewModel::isOfferFitBalance(const SwapOfferItem& offer)
{
    if (offer.isOwnOffer())
        return true;

    bool isSendBeam = offer.isSendBeam();
    beam::AmountBig::Type beamOfferAmount = isSendBeam ? offer.rawAmountSend() : offer.rawAmountReceive();

    if (isSendBeam && beamOfferAmount > m_walletModel.getAvailable(beam::Asset::s_BeamID))
        return false;
    
    auto swapCoinOfferAmount = isSendBeam ? offer.rawAmountReceive() : offer.rawAmountSend();
    // TODO: find better solution to get AtomicSwapCoin
    auto swapCoin = offer.getTxParameters().GetParameter<beam::wallet::AtomicSwapCoin>(beam::wallet::TxParameterID::AtomicSwapCoin);
    auto swapCoinClientWrapper = getSwapCoinClientWrapper(*swapCoin);

    if (swapCoinClientWrapper->getIsConnected())
    {
        return isSendBeam ? true : swapCoinOfferAmount <= swapCoinClientWrapper->getAvailable();
    }

    return false;
}

void SwapOffersViewModel::insertAllOffersFitBalance(
    const std::vector<std::shared_ptr<SwapOfferItem>>& offers)
{
    std::vector<std::shared_ptr<SwapOfferItem>> fitBalanceOffers;
    fitBalanceOffers.reserve(offers.size());

    std::copy_if(offers.begin(), offers.end(),
                 std::back_inserter(fitBalanceOffers),
                 [this] (const std::shared_ptr<SwapOfferItem>& it)
                 {
                     return isOfferFitBalance(*it);
                 });
    m_offersListFitBalance.insert(fitBalanceOffers);
    emit allOffersFitBalanceChanged();
}

void SwapOffersViewModel::removeAllOffersFitBalance(
    const std::vector<std::shared_ptr<SwapOfferItem>>& offers)
{
    std::vector<std::shared_ptr<SwapOfferItem>> fitBalanceOffers;
    fitBalanceOffers.reserve(offers.size());

    std::copy_if(offers.begin(), offers.end(),
                 std::back_inserter(fitBalanceOffers),
                 [this] (const std::shared_ptr<SwapOfferItem>& it)
                 {
                     return isOfferFitBalance(*it);
                 });
    m_offersListFitBalance.remove(fitBalanceOffers);
    emit allOffersFitBalanceChanged();    
}

bool SwapOffersViewModel::hasActiveTx(const std::string& swapCoin) const
{
    for (int i = 0; i < m_transactionsList.rowCount(); ++i)
    {
        auto index = m_transactionsList.index(i, 0);
        try
        {
            bool isPending = m_transactionsList.data(index, static_cast<int>(SwapTxObjectList::Roles::IsPending)).toBool();
            if (!isPending)
            {
                bool isInProgress = m_transactionsList.data(index, static_cast<int>(SwapTxObjectList::Roles::IsInProgress)).toBool();
                if (isInProgress)
                {
                    auto mySwapCoin = m_transactionsList.data(index, static_cast<int>(SwapTxObjectList::Roles::SwapCoin)).toString().toStdString();
                    if (mySwapCoin == swapCoin)
                    {
                        return true;
                    }
                }
            }
        }
        catch(...)
        {
            qDebug() << "Wrong ROLE data";
        }
    }

    return false;
}

void SwapOffersViewModel::InitSwapClientWrappers()
{
    using namespace beam::wallet;

    if (m_swapClientWrappers.empty())
    {
        m_swapClientWrappers.push_back(new SwapCoinClientWrapper(AtomicSwapCoin::Bitcoin));
        m_swapClientWrappers.push_back(new SwapCoinClientWrapper(AtomicSwapCoin::Litecoin));
        m_swapClientWrappers.push_back(new SwapCoinClientWrapper(AtomicSwapCoin::Qtum));
#if defined(BITCOIN_CASH_SUPPORT)
        m_swapClientWrappers.push_back(new SwapCoinClientWrapper(AtomicSwapCoin::Bitcoin_Cash));
#endif // BITCOIN_CASH_SUPPORT
        m_swapClientWrappers.push_back(new SwapCoinClientWrapper(AtomicSwapCoin::Dogecoin));
        m_swapClientWrappers.push_back(new SwapCoinClientWrapper(AtomicSwapCoin::Dash));
        m_swapClientWrappers.push_back(new SwapCoinClientWrapper(AtomicSwapCoin::Ethereum));
        m_swapClientWrappers.push_back(new SwapCoinClientWrapper(AtomicSwapCoin::Dai));
        m_swapClientWrappers.push_back(new SwapCoinClientWrapper(AtomicSwapCoin::Usdt));
        m_swapClientWrappers.push_back(new SwapCoinClientWrapper(AtomicSwapCoin::WBTC));
    }
}

QQmlListProperty<SwapCoinClientWrapper> SwapOffersViewModel::getSwapClients()
{
    return beamui::CreateQmlListProperty<SwapCoinClientWrapper>(this, m_swapClientWrappers);
}

SwapCoinClientWrapper* SwapOffersViewModel::getSwapCoinClientWrapper(beam::wallet::AtomicSwapCoin swapCoinType) const
{
    auto it = std::find_if(m_swapClientWrappers.cbegin(), m_swapClientWrappers.cend(),
        [swapCoinType](SwapCoinClientWrapper* wrapper)
        {
            return wrapper->getSwapCoin() == swapCoinType;
        });
    return (it != m_swapClientWrappers.end()) ? (*it) : nullptr;
}

uint32_t SwapOffersViewModel::getLockTxMinConfirmations(beam::wallet::AtomicSwapCoin swapCoinType) const
{
    auto swapClientWrapper = getSwapCoinClientWrapper(swapCoinType);

    if (swapClientWrapper)
    {
        return swapClientWrapper->getLockTxMinConfirmations();
    }
    return 0;
}

uint32_t SwapOffersViewModel::getWithdrawTxMinConfirmations(beam::wallet::AtomicSwapCoin swapCoinType) const
{
    auto swapClientWrapper = getSwapCoinClientWrapper(swapCoinType);

    if (swapClientWrapper)
    {
        return swapClientWrapper->getWithdrawTxMinConfirmations();
    }
    return 0;
}

double SwapOffersViewModel::getBlocksPerHour(beam::wallet::AtomicSwapCoin swapCoinType) const
{
    auto swapClientWrapper = getSwapCoinClientWrapper(swapCoinType);

    if (swapClientWrapper)
    {
        return swapClientWrapper->getBlocksPerHour();
    }
    return 0;
}

void SwapOffersViewModel::incrementActiveTxCounter(beam::wallet::AtomicSwapCoin swapCoinType)
{
    auto swapClientWrapper = getSwapCoinClientWrapper(swapCoinType);
    if (swapClientWrapper)
    {
        swapClientWrapper->incrementActiveTxCounter();
    }
}

void SwapOffersViewModel::decrementActiveTxCounter(beam::wallet::AtomicSwapCoin swapCoinType)
{
    auto swapClientWrapper = getSwapCoinClientWrapper(swapCoinType);
    if (swapClientWrapper)
    {
        swapClientWrapper->decrementActiveTxCounter();
    }
}

void SwapOffersViewModel::resetActiveTxCounters()
{
    for (auto swapClientWrapper : m_swapClientWrappers)
    {
        swapClientWrapper->resetActiveTxCounter();
    }
}