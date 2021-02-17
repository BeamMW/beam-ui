// Copyright 2020 The Beam Team
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

#include "swap_settings_item.h"

#include "mnemonic/mnemonic.h"
#include "model/app_model.h"
#include "viewmodel/qml_globals.h"
#include "viewmodel/ui_helpers.h"
#include "seed_phrase_item.h"
#include "viewmodel/settings_helpers.h"

using namespace beam;

namespace
{
    const char ELECTRUM_PHRASES_SEPARATOR = ' ';
}

SwapCoinSettingsItem::SwapCoinSettingsItem(wallet::AtomicSwapCoin swapCoin)
    : m_swapCoin(swapCoin)
    , m_coinClient(AppModel2::getInstance().getSwapCoinClient(swapCoin))
{
    auto coinClient = m_coinClient.lock();
    connect(coinClient.get(), SIGNAL(statusChanged()), this, SLOT(onStatusChanged()));
    connect(coinClient.get(), SIGNAL(connectionErrorChanged()), this, SIGNAL(connectionErrorMsgChanged()));
    LoadSettings();
}

SwapCoinSettingsItem::~SwapCoinSettingsItem()
{
    qDeleteAll(m_seedPhraseItems);
}

QString SwapCoinSettingsItem::getTitle() const
{
    switch (m_settings->GetCurrentConnectionType())
    {
    case beam::bitcoin::Settings::ConnectionType::None:
        return getGeneralTitle();
    case beam::bitcoin::Settings::ConnectionType::Core:
        return getConnectedNodeTitle();
    case beam::bitcoin::Settings::ConnectionType::Electrum:
        return getConnectedElectrumTitle();
    default:
    {
        assert(false && "unexpected connection type");
        return getGeneralTitle();
    }
    }
}

QString SwapCoinSettingsItem::getCoinID() const
{
    return beamui::getCurrencyUnitName(beamui::convertSwapCoinToCurrency(m_swapCoin));
}

QString SwapCoinSettingsItem::getShowSeedDialogTitle() const
{
    switch (m_swapCoin)
    {
    case beam::wallet::AtomicSwapCoin::Bitcoin:
        //% "Bitcoin seed phrase"
        return qtTrId("bitcoin-show-seed-title");
    case beam::wallet::AtomicSwapCoin::Litecoin:
        //% "Litecoin seed phrase"
        return qtTrId("litecoin-show-seed-title");
    case beam::wallet::AtomicSwapCoin::Qtum:
        //% "Qtum seed phrase"
        return qtTrId("qtum-show-seed-title");
    case beam::wallet::AtomicSwapCoin::Dogecoin:
        //% "Dogecoin seed phrase"
        return qtTrId("dogecoin-show-seed-phrase");
#if defined(BITCOIN_CASH_SUPPORT)
    case beam::wallet::AtomicSwapCoin::Bitcoin_Cash:
        //% "Bitcoin Cash seed phrase"
        return qtTrId("bitcoin-cash-show-seed-phrase");
#endif // BITCOIN_CASH_SUPPORT
    case beam::wallet::AtomicSwapCoin::Dash:
        //% "Dash seed phrase"
        return qtTrId("dash-show-seed-phrase");
    default:
    {
        assert(false && "unexpected swap coin!");
        return QString();
    }
    }
}

QString SwapCoinSettingsItem::getShowAddressesDialogTitle() const
{
    switch (m_swapCoin)
    {
    case beam::wallet::AtomicSwapCoin::Bitcoin:
        //% "Bitcoin wallet addresses"
        return qtTrId("bitcoin-show-addresses-title");
    case beam::wallet::AtomicSwapCoin::Litecoin:
        //% "Litecoin wallet addresses"
        return qtTrId("litecoin-show-addresses-title");
    case beam::wallet::AtomicSwapCoin::Qtum:
        //% "Qtum wallet addresses"
        return qtTrId("qtum-show-addresses-title");
    case beam::wallet::AtomicSwapCoin::Dogecoin:
        //% "Dogecoin wallet addresses"
        return qtTrId("dogecoin-show-addresses-title");
#if defined(BITCOIN_CASH_SUPPORT)
    case beam::wallet::AtomicSwapCoin::Bitcoin_Cash:
        //% "Bitcoin Cash wallet addresses"
        return qtTrId("bitcoin-cash-show-addresses-title");
#endif // BITCOIN_CASH_SUPPORT
    case beam::wallet::AtomicSwapCoin::Dash:
        //% "Dash wallet addresses"
        return qtTrId("dash-show-addresses-title");
    default:
    {
        assert(false && "unexpected swap coin!");
        return QString();
    }
    }
}

QString SwapCoinSettingsItem::getGeneralTitle() const
{
    switch (m_swapCoin)
    {
    case wallet::AtomicSwapCoin::Bitcoin:
        //% "Bitcoin"
        return qtTrId("general-bitcoin");
    case wallet::AtomicSwapCoin::Litecoin:
        //% "Litecoin"
        return qtTrId("general-litecoin");
    case wallet::AtomicSwapCoin::Qtum:
        //% "QTUM"
        return qtTrId("general-qtum");
    case wallet::AtomicSwapCoin::Dogecoin:
        //% "Dogecoin"
        return qtTrId("general-dogecoin");
#if defined(BITCOIN_CASH_SUPPORT)
    case wallet::AtomicSwapCoin::Bitcoin_Cash:
        //% "Bitcoin Cash"
        return qtTrId("general-bitcoin-cash");
#endif // BITCOIN_CASH_SUPPORT
    case wallet::AtomicSwapCoin::Dash:
        //% "DASH"
        return qtTrId("general-dash");
    default:
    {
        assert(false && "unexpected swap coin!");
        return QString();
    }
    }
}

QString SwapCoinSettingsItem::getConnectedNodeTitle() const
{
    // TODO: check, is real need translations?
    switch (m_swapCoin)
    {
    case wallet::AtomicSwapCoin::Bitcoin:
        //% "Bitcoin node"
        return qtTrId("settings-swap-bitcoin-node");
    case wallet::AtomicSwapCoin::Litecoin:
        //% "Litecoin node"
        return qtTrId("settings-swap-litecoin-node");
    case wallet::AtomicSwapCoin::Qtum:
        //% "Qtum node"
        return qtTrId("settings-swap-qtum-node");
    case wallet::AtomicSwapCoin::Dogecoin:
        //% "Dogecoin node"
        return qtTrId("settings-swap-dogecoin-node");
#if defined(BITCOIN_CASH_SUPPORT)
    case wallet::AtomicSwapCoin::Bitcoin_Cash:
        //% "Bitcoin Cash node"
        return qtTrId("settings-swap-bitcoin-cash-node");
#endif // BITCOIN_CASH_SUPPORT
    case wallet::AtomicSwapCoin::Dash:
        //% "Dash node"
        return qtTrId("settings-swap-dash-node");
    default:
    {
        assert(false && "unexpected swap coin!");
        return QString();
    }
    }
}

QString SwapCoinSettingsItem::getConnectedElectrumTitle() const
{
    // TODO: check, is real need translations?
    switch (m_swapCoin)
    {
    case wallet::AtomicSwapCoin::Bitcoin:
        //% "Bitcoin electrum"
        return qtTrId("settings-swap-bitcoin-electrum");
    case wallet::AtomicSwapCoin::Litecoin:
        //% "Litecoin electrum"
        return qtTrId("settings-swap-litecoin-electrum");
    case wallet::AtomicSwapCoin::Qtum:
        //% "Qtum electrum"
        return qtTrId("settings-swap-qtum-electrum");
    case wallet::AtomicSwapCoin::Dogecoin:
        //% "Dogecoin electrum"
        return qtTrId("settings-swap-dogecoin-electrum");
#if defined(BITCOIN_CASH_SUPPORT)
    case wallet::AtomicSwapCoin::Bitcoin_Cash:
        //% "Bitcoin Cash electrum"
        return qtTrId("settings-swap-bitcoin-cash-electrum");
#endif // BITCOIN_CASH_SUPPORT
    case wallet::AtomicSwapCoin::Dash:
        //% "Dash electrum"
        return qtTrId("settings-swap-dash-electrum");
    default:
    {
        assert(false && "unexpected swap coin!");
        return QString();
    }
    }
}

bool SwapCoinSettingsItem::getFolded() const
{
    return m_isFolded;
}

void SwapCoinSettingsItem::setFolded(bool value)
{
    m_isFolded = value;
}

QString SwapCoinSettingsItem::getNodeUser() const
{
    return m_nodeUser;
}

void SwapCoinSettingsItem::setNodeUser(const QString& value)
{
    if (value != m_nodeUser)
    {
        m_nodeUser = value;
        emit nodeUserChanged();
    }
}

QString SwapCoinSettingsItem::getNodePass() const
{
    return m_nodePass;
}

void SwapCoinSettingsItem::setNodePass(const QString& value)
{
    if (value != m_nodePass)
    {
        m_nodePass = value;
        emit nodePassChanged();
    }
}

QString SwapCoinSettingsItem::getNodeAddress() const
{
    return m_nodeAddress;
}

void SwapCoinSettingsItem::setNodeAddress(const QString& value)
{
    const auto val = value == "0.0.0.0" ? "" : value;
    if (val != m_nodeAddress)
    {
        m_nodeAddress = val;
        emit nodeAddressChanged();
    }
}

QString SwapCoinSettingsItem::getNodePort() const
{
    return m_nodePort;
}

void SwapCoinSettingsItem::setNodePort(const QString& value)
{
    if (value != m_nodePort)
    {
        m_nodePort = value;
        emit nodePortChanged();
    }
}

QList<QObject*> SwapCoinSettingsItem::getElectrumSeedPhrases()
{
    return m_seedPhraseItems;
}

QChar SwapCoinSettingsItem::getPhrasesSeparatorElectrum() const
{
    return QChar(ELECTRUM_PHRASES_SEPARATOR);
}

bool SwapCoinSettingsItem::getIsCurrentSeedValid() const
{
    return m_isCurrentSeedValid;
}

bool SwapCoinSettingsItem::getIsCurrentSeedSegwit() const
{
    return m_isCurrentSeedSegwit;
}

QString SwapCoinSettingsItem::getNodeAddressElectrum() const
{
    return m_nodeAddressElectrum;
}

void SwapCoinSettingsItem::setNodeAddressElectrum(const QString& value)
{
    if (value != m_nodeAddressElectrum)
    {
        m_nodeAddressElectrum = value;
        emit nodeAddressElectrumChanged();
    }
}

QString SwapCoinSettingsItem::getNodePortElectrum() const
{
    return m_nodePortElectrum;
}

void SwapCoinSettingsItem::setNodePortElectrum(const QString& value)
{
    if (value != m_nodePortElectrum)
    {
        m_nodePortElectrum = value;
        emit nodePortElectrumChanged();
    }
}

bool SwapCoinSettingsItem::getSelectServerAutomatically() const
{
    return m_selectServerAutomatically;
}

void SwapCoinSettingsItem::setSelectServerAutomatically(bool value)
{
    if (value != m_selectServerAutomatically)
    {
        m_selectServerAutomatically = value;
        emit selectServerAutomaticallyChanged();
    }
}

bool SwapCoinSettingsItem::isSupportedElectrum() const
{
    return m_settings->IsSupportedElectrum();
}

QStringList SwapCoinSettingsItem::getAddressesElectrum() const
{
    auto electrumSettings = m_settings->GetElectrumConnectionOptions();

    if (electrumSettings.IsInitialized())
    {
        auto addresses = electrum::generateReceivingAddresses(m_swapCoin, electrumSettings.m_secretWords,
            electrumSettings.m_receivingAddressAmount, m_settings->GetAddressVersion());

        QStringList result;
        result.reserve(static_cast<int>(addresses.size()));

        for (const auto& address : addresses)
        {
            result.push_back(QString::fromStdString(address));
        }
        return result;
    }
    return {};
}

void SwapCoinSettingsItem::onStatusChanged()
{
    emit connectionStatusChanged();

    if (m_selectServerAutomatically)
    {
        using beam::bitcoin::Client;

        switch (m_coinClient.lock()->getStatus())
        {
        case Client::Status::Connected:
        case Client::Status::Failed:
        case Client::Status::Unknown:
        {
            auto settings = m_coinClient.lock()->GetSettings();

            if (auto options = settings.GetElectrumConnectionOptions(); options.IsInitialized())
            {
                applyNodeAddressElectrum(str2qstr(options.m_address));
            }
            break;
        }
        default:
        {
            setNodeAddressElectrum("");
            setNodePortElectrum("");
        }
        }
    }
}

bool SwapCoinSettingsItem::canChangeConnection() const
{
    return m_coinClient.lock()->canModifySettings();
}

bool SwapCoinSettingsItem::getIsConnected() const
{
    return m_connectionType != beam::bitcoin::Settings::ConnectionType::None;
}

bool SwapCoinSettingsItem::getIsNodeConnection() const
{
    return m_connectionType == beam::bitcoin::Settings::ConnectionType::Core;
}

bool SwapCoinSettingsItem::getIsElectrumConnection() const
{
    return m_connectionType == beam::bitcoin::Settings::ConnectionType::Electrum;
}

QString SwapCoinSettingsItem::getConnectionStatus() const
{
    using beam::bitcoin::Client;

    switch (m_coinClient.lock()->getStatus())
    {
    case Client::Status::Uninitialized:
        return "uninitialized";

    case Client::Status::Initialized:
    case Client::Status::Connecting:
        return "disconnected";

    case Client::Status::Connected:
        return "connected";

    case Client::Status::Failed:
    case Client::Status::Unknown:
    default:
        return "error";
    }
}

QString SwapCoinSettingsItem::getConnectionErrorMsg() const
{
    using beam::bitcoin::IBridge;

    switch (m_coinClient.lock()->getConnectionError())
    {
    case IBridge::ErrorType::InvalidCredentials:
        //% "Cannot connect to node. Invalid credentials"
        return qtTrId("swap-invalid-credentials-error");

    case IBridge::ErrorType::IOError:
        //% "Cannot connect to node. Please check your network connection."
        return qtTrId("swap-connection-error");

    case IBridge::ErrorType::InvalidGenesisBlock:
        //% "Cannot connect to node. Invalid genesis block"
        return qtTrId("swap-invalid-genesis-block-error");

    default:
        return QString();
    }
}

void SwapCoinSettingsItem::applyNodeSettings()
{
    auto coinClient = m_coinClient.lock();
    bitcoin::BitcoinCoreSettings connectionSettings = coinClient->GetSettings().GetConnectionOptions();
    connectionSettings.m_pass = m_nodePass.toStdString();
    connectionSettings.m_userName = m_nodeUser.toStdString();

    if (!m_nodeAddress.isEmpty())
    {
        const std::string address = m_nodeAddress.toStdString();
        connectionSettings.m_address.resolve(address.c_str());
        connectionSettings.m_address.port(m_nodePort.toInt());
    }

    m_settings->SetConnectionOptions(connectionSettings);

    coinClient->SetSettings(*m_settings);
}

void SwapCoinSettingsItem::applyElectrumSettings()
{
    auto coinClient = m_coinClient.lock();
    bitcoin::ElectrumSettings electrumSettings = coinClient->GetSettings().GetElectrumConnectionOptions();

    if (!m_selectServerAutomatically && !m_nodeAddressElectrum.isEmpty())
    {
        electrumSettings.m_address = formatAddress(m_nodeAddressElectrum, m_nodePortElectrum).toStdString();
    }

    electrumSettings.m_automaticChooseAddress = m_selectServerAutomatically;
    electrumSettings.m_secretWords = GetSeedPhraseFromSeedItems();

    m_settings->SetElectrumConnectionOptions(electrumSettings);

    coinClient->SetSettings(*m_settings);
}

void SwapCoinSettingsItem::resetNodeSettings()
{
    SetDefaultNodeSettings();
    applyNodeSettings();
}

void SwapCoinSettingsItem::resetElectrumSettings()
{
    bool clearSeed = canChangeConnection();
    SetDefaultElectrumSettings(clearSeed);
    applyElectrumSettings();
}

void SwapCoinSettingsItem::newElectrumSeed()
{
    auto secretWords = electrum::createMnemonic(getEntropy());
    SetSeedElectrum(secretWords);
}

void SwapCoinSettingsItem::restoreSeedElectrum()
{
    SetSeedElectrum(m_settings->GetElectrumConnectionOptions().m_secretWords);
}

void SwapCoinSettingsItem::disconnect()
{
    auto connectionType = bitcoin::Settings::ConnectionType::None;

    m_settings->ChangeConnectionType(connectionType);
    m_coinClient.lock()->SetSettings(*m_settings);
    setConnectionType(connectionType);
}

void SwapCoinSettingsItem::connectToNode()
{
    auto connectionType = bitcoin::Settings::ConnectionType::Core;

    m_settings->ChangeConnectionType(connectionType);
    m_coinClient.lock()->SetSettings(*m_settings);
    setConnectionType(connectionType);
}

void SwapCoinSettingsItem::connectToElectrum()
{
    auto connectionType = bitcoin::Settings::ConnectionType::Electrum;

    m_settings->ChangeConnectionType(connectionType);
    m_coinClient.lock()->SetSettings(*m_settings);
    setConnectionType(connectionType);
}

void SwapCoinSettingsItem::copySeedElectrum()
{
    auto seedElectrum = GetSeedPhraseFromSeedItems();
    auto seedString = vec2str(seedElectrum, ELECTRUM_PHRASES_SEPARATOR);
    QMLGlobals::copyToClipboard(QString::fromStdString(seedString));
}

void SwapCoinSettingsItem::validateCurrentElectrumSeedPhrase()
{
    std::vector<std::string> seedElectrum;
    seedElectrum.reserve(WORD_COUNT);

    // extract seed phrase from user input
    for (const auto phraseItem : m_seedPhraseItems)
    {
        auto word = static_cast<SeedPhraseItem*>(phraseItem)->getValue().toStdString();
        seedElectrum.push_back(word);
    }

    setIsCurrentSeedValid(electrum::validateMnemonic(seedElectrum));
    setIsCurrentSeedSegwit(electrum::validateMnemonic(seedElectrum, true));
}

void SwapCoinSettingsItem::LoadSettings()
{
    SetDefaultElectrumSettings();
    SetDefaultNodeSettings();

    m_settings = m_coinClient.lock()->GetSettings();

    setConnectionType(m_settings->GetCurrentConnectionType());

    if (auto options = m_settings->GetConnectionOptions(); options.IsInitialized())
    {
        setNodeUser(str2qstr(options.m_userName));
        setNodePass(str2qstr(options.m_pass));
        applyNodeAddress(AddressToQstring(options.m_address));
    }

    if (auto options = m_settings->GetElectrumConnectionOptions(); options.IsInitialized())
    {
        SetSeedElectrum(options.m_secretWords);
        setSelectServerAutomatically(options.m_automaticChooseAddress);

        if (m_settings->IsElectrumActivated() || !options.m_automaticChooseAddress)
        {
            applyNodeAddressElectrum(str2qstr(options.m_address));
        }
    }
}

void SwapCoinSettingsItem::SetSeedElectrum(const std::vector<std::string>& seedElectrum)
{
    if (!m_seedPhraseItems.empty())
    {
        qDeleteAll(m_seedPhraseItems);
        m_seedPhraseItems.clear();
    }

    m_seedPhraseItems.reserve(static_cast<int>(WORD_COUNT));

    if (seedElectrum.empty())
    {
        for (int index = 0; index < static_cast<int>(WORD_COUNT); ++index)
        {
            m_seedPhraseItems.push_back(new SeedPhraseItem(index, QString()));
        }
    }
    else
    {
        assert(seedElectrum.size() == WORD_COUNT);
        int index = 0;
        for (auto& word : seedElectrum)
        {
            m_seedPhraseItems.push_back(new SeedPhraseItem(index++, QString::fromStdString(word)));
        }
    }

    setIsCurrentSeedValid(electrum::validateMnemonic(seedElectrum));
    setIsCurrentSeedSegwit(electrum::validateMnemonic(seedElectrum, true));
    emit electrumSeedPhrasesChanged();
}

void SwapCoinSettingsItem::SetDefaultNodeSettings()
{
    setNodePort(0);
    setNodeAddress("");
    setNodePass("");
    setNodeUser("");
}

void SwapCoinSettingsItem::SetDefaultElectrumSettings(bool clearSeed)
{
    setNodeAddressElectrum("");
    setNodePortElectrum("");
    setSelectServerAutomatically(true);

    if (clearSeed)
    {
        SetSeedElectrum({});
    }
}

void SwapCoinSettingsItem::setConnectionType(beam::bitcoin::Settings::ConnectionType type)
{
    if (type != m_connectionType)
    {
        m_connectionType = type;
        emit connectionTypeChanged();
    }
}

void SwapCoinSettingsItem::setIsCurrentSeedValid(bool value)
{
    if (m_isCurrentSeedValid != value)
    {
        m_isCurrentSeedValid = value;
        emit isCurrentSeedValidChanged();
    }
}

void SwapCoinSettingsItem::setIsCurrentSeedSegwit(bool value)
{
    if (m_isCurrentSeedSegwit != value)
    {
        m_isCurrentSeedSegwit = value;
        emit isCurrentSeedSegwitChanged();
    }
}

std::vector<std::string> SwapCoinSettingsItem::GetSeedPhraseFromSeedItems() const
{
    assert(static_cast<size_t>(m_seedPhraseItems.size()) == WORD_COUNT);

    std::vector<std::string> seedElectrum;

    for (const auto phraseItem : m_seedPhraseItems)
    {
        auto item = static_cast<SeedPhraseItem*>(phraseItem);
        auto word = item->getPhrase().toStdString();

        // TODO need to wath this code. fixed ui bug #58
        // secret word can not empty
        if (!word.empty())
            seedElectrum.push_back(word);
    }

    return seedElectrum;
}

void SwapCoinSettingsItem::applyNodeAddress(const QString& address)
{
    auto unpackedAddress = parseAddress(address);
    setNodeAddress(unpackedAddress.address);
    if (unpackedAddress.port > 0)
    {
        setNodePort(unpackedAddress.port);
    }
}

void SwapCoinSettingsItem::applyNodeAddressElectrum(const QString& address)
{
    auto unpackedAddress = parseAddress(address);
    setNodeAddressElectrum(unpackedAddress.address);
    if (unpackedAddress.port > 0)
    {
        setNodePortElectrum(unpackedAddress.port);
    }
}