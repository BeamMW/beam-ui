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

#include "swap_eth_settings_item.h"

#include "mnemonic/mnemonic.h"
#include "model/app_model.h"
#include "viewmodel/qml_globals.h"
#include "seed_phrase_item.h"
#include "viewmodel/settings_helpers.h"

#include "wallet/transactions/swaps/bridges/ethereum/common.h"

using namespace beam;

namespace
{
    const char PHRASES_SEPARATOR = ' ';
}

SwapEthSettingsItem::SwapEthSettingsItem()
    : m_coinClient(AppModel::getInstance().getSwapEthClient())
{
    auto coinClient = m_coinClient.lock();
    connect(coinClient.get(), SIGNAL(statusChanged()), this, SIGNAL(connectionStatusChanged()));
    connect(coinClient.get(), SIGNAL(connectionErrorChanged()), this, SIGNAL(connectionErrorMsgChanged()));
    LoadSettings();
}

SwapEthSettingsItem::~SwapEthSettingsItem()
{
    qDeleteAll(m_seedPhraseItems);
}

void SwapEthSettingsItem::disconnect()
{
    m_settings->m_shouldConnect = false;
    m_coinClient.lock()->SetSettings(*m_settings);
    shouldConnect(false);
}

void SwapEthSettingsItem::applySettings()
{
    auto coinClient = m_coinClient.lock();

    m_settings->m_accountIndex = m_accountIndex;
    m_settings->m_shouldConnect = m_shouldConnect;
    m_settings->m_address = m_nodeAddress.toStdString();//formatAddress(m_nodeAddress, m_nodePort).toStdString();
    m_settings->m_secretWords = GetSeedPhraseFromSeedItems();
    // TODO roman.strilets hash or aggregate
    m_settings->m_swapContractAddress = m_contractAddress.toStdString();
    m_settings->m_erc20SwapContractAddress = m_erc20ContractAddress.toStdString();

    m_settings->m_daiContractAddress = m_daiContractAddress.toStdString();
    m_settings->m_usdtContractAddress = m_usdtContractAddress.toStdString();
    m_settings->m_wbtcContractAddress = m_wbtcContractAddress.toStdString();

    m_settings->m_shouldConnectToDai = m_activateDai;
    m_settings->m_shouldConnectToUsdt = m_activateUsdt;
    m_settings->m_shouldConnectToWBTC = m_activateWBTC;

    coinClient->SetSettings(*m_settings);
}

void SwapEthSettingsItem::clearSettings()
{
    SetDefaultSettings();
    applySettings();
}

void SwapEthSettingsItem::connectToNode()
{
    m_settings->m_shouldConnect = true;
    m_coinClient.lock()->SetSettings(*m_settings);
    shouldConnect(true);
}

void SwapEthSettingsItem::newSeedPhrases()
{
    // TODO check this
    auto secretsWords = beam::createMnemonic(beam::getEntropy(), beam::language::en);
    SetSeedPhrase(secretsWords);
}

void SwapEthSettingsItem::restoreSeedPhrases()
{
    SetSeedPhrase(m_settings->m_secretWords);
}

void SwapEthSettingsItem::copySeedPhrases()
{
    auto seedPhrases = GetSeedPhraseFromSeedItems();
    auto seedString = vec2str(seedPhrases, PHRASES_SEPARATOR);
    QMLGlobals::copyToClipboard(QString::fromStdString(seedString));
}

void SwapEthSettingsItem::validateCurrentSeedPhrase()
{
    std::vector<std::string> seedPhrases;
    seedPhrases.reserve(WORD_COUNT);

    // extract seed phrase from user input
    for (const auto phraseItem : m_seedPhraseItems)
    {
        auto word = static_cast<SeedPhraseItem*>(phraseItem)->getValue().toStdString();
        seedPhrases.push_back(word);
    }

    setIsCurrentSeedValid(isValidMnemonic(seedPhrases, language::en));
}

QStringList SwapEthSettingsItem::getEthereumAddresses() const
{
    QStringList result;

    if (m_settings->IsInitialized())
    {
        auto rawAddress = ethereum::GenerateEthereumAddress(m_settings->m_secretWords, m_settings->m_accountIndex);
        auto address = ethereum::ConvertEthAddressToStr(rawAddress);
        
        if (!address.empty())
        {
            result.push_back(QString::fromStdString(address));
        }
    }

    return result;
}

QString SwapEthSettingsItem::getTitle() const
{
    if (m_settings->m_shouldConnect)
    {
        //% "Ethereum node"
        return qtTrId("settings-swap-ethereum-node");
    }
    
    return getGeneralTitle();
}

QString SwapEthSettingsItem::getShowSeedDialogTitle() const
{
    //% "Ethereum seed phrase"
    return qtTrId("ethereum-show-seed-title");
}

QString SwapEthSettingsItem::getGeneralTitle() const
{
    //% "Ethereum"
    return qtTrId("general-ethereum");
}

QString SwapEthSettingsItem::getCoinID() const
{
    return beamui::getCurrencyLabel(beamui::Currencies::Ethereum);
}

bool SwapEthSettingsItem::getFolded() const
{
    return m_isFolded;
}

void SwapEthSettingsItem::setFolded(bool value)
{
    m_isFolded = value;
}

QList<QObject*> SwapEthSettingsItem::getSeedPhrases()
{
    return m_seedPhraseItems;
}

QChar SwapEthSettingsItem::getPhrasesSeparator() const
{
    return QChar(PHRASES_SEPARATOR);
}

bool SwapEthSettingsItem::isCurrentSeedValid() const
{
    return m_isCurrentSeedValid;
}

void SwapEthSettingsItem::setIsCurrentSeedValid(bool value)
{
    if (m_isCurrentSeedValid != value)
    {
        m_isCurrentSeedValid = value;
        emit isCurrentSeedValidChanged();
    }
}

void SwapEthSettingsItem::LoadSettings()
{
    SetDefaultSettings();

    m_settings = m_coinClient.lock()->GetSettings();

    if (m_settings->IsInitialized())
    {
        SetSeedPhrase(m_settings->m_secretWords);
        applyNodeAddress(str2qstr(m_settings->m_address));
        setAccountIndex(m_settings->m_accountIndex);
        // TODO roman.strilets hash or aggregate
        setContractAddress(str2qstr(m_settings->m_swapContractAddress));
        setERC20ContractAddress(str2qstr(m_settings->m_erc20SwapContractAddress));
        shouldConnect(m_settings->m_shouldConnect);

        setDaiContractAddress(str2qstr(m_settings->m_daiContractAddress));
        setUsdtContractAddress(str2qstr(m_settings->m_usdtContractAddress));
        setWbtcContractAddress(str2qstr(m_settings->m_wbtcContractAddress));

        activateDai(m_settings->m_shouldConnectToDai);
        activateUsdt(m_settings->m_shouldConnectToUsdt);
        activateWBTC(m_settings->m_shouldConnectToWBTC);
    }
}

void SwapEthSettingsItem::shouldConnect(bool value)
{
    if (value != m_shouldConnect)
    {
        m_shouldConnect = value;
        emit connectionChanged();
    }
}

void SwapEthSettingsItem::SetSeedPhrase(const std::vector<std::string>& seedElectrum)
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

    setIsCurrentSeedValid(isValidMnemonic(seedElectrum, language::en));
    emit seedPhrasesChanged();
}

void SwapEthSettingsItem::SetDefaultSettings(bool clearSeed)
{
    setNodeAddress("");
    setNodePort("");
    setAccountIndex(0);
    setContractAddress("");
    setERC20ContractAddress("");
    setDaiContractAddress("");
    setUsdtContractAddress("");
    setWbtcContractAddress("");

    activateDai(false);
    activateUsdt(false);
    activateWBTC(false);

    if (clearSeed)
    {
        SetSeedPhrase({});
    }
}

QString SwapEthSettingsItem::getNodeAddress() const
{
    return m_nodeAddress;
}

void SwapEthSettingsItem::setNodeAddress(const QString& value)
{
    if (value != m_nodeAddress)
    {
        m_nodeAddress = value;
        emit nodeAddressChanged();
    }
}

QString SwapEthSettingsItem::getNodePort() const
{
    return m_nodePort;
}

void SwapEthSettingsItem::setNodePort(const QString& value)
{
    if (value != m_nodePort)
    {
        m_nodePort = value;
        emit nodePortChanged();
    }
}

unsigned int SwapEthSettingsItem::getAccountIndex() const
{
    return m_accountIndex;
}

void SwapEthSettingsItem::setAccountIndex(unsigned int value)
{
    if (value != m_accountIndex)
    {
        m_accountIndex = value;
        emit accountIndexChanged();
    }
}

QString SwapEthSettingsItem::getContractAddress() const
{
    return m_contractAddress;
}

QString SwapEthSettingsItem::getERC20ContractAddress() const
{
    return m_erc20ContractAddress;
}

QString SwapEthSettingsItem::getDaiContractAddress() const
{
    return m_daiContractAddress;
}

QString SwapEthSettingsItem::getUsdtContractAddress() const
{
    return m_usdtContractAddress;
}

QString SwapEthSettingsItem::getWbtcContractAddress() const
{
    return m_wbtcContractAddress;
}

void SwapEthSettingsItem::setContractAddress(const QString& value)
{
    if (value != m_contractAddress)
    {
        m_contractAddress = value;
        emit contractAddressChanged();
    }
}

void SwapEthSettingsItem::setERC20ContractAddress(const QString& value)
{
    if (value != m_erc20ContractAddress)
    {
        m_erc20ContractAddress = value;
        emit erc20ContractAddressChanged();
    }
}

void SwapEthSettingsItem::setDaiContractAddress(const QString& value)
{
    if (value != m_daiContractAddress)
    {
        m_daiContractAddress = value;
        emit daiContractAddressChanged();
    }
}

void SwapEthSettingsItem::setUsdtContractAddress(const QString& value)
{
    if (value != m_usdtContractAddress)
    {
        m_usdtContractAddress = value;
        emit usdtContractAddressChanged();
    }
}

void SwapEthSettingsItem::setWbtcContractAddress(const QString& value)
{
    if (value != m_wbtcContractAddress)
    {
        m_wbtcContractAddress = value;
        emit wbtcContractAddressChanged();
    }
}

bool SwapEthSettingsItem::activateDai() const
{
    return m_activateDai;
}

void SwapEthSettingsItem::activateDai(bool value)
{
    if (value != m_activateDai)
    {
        m_activateDai = value;
        emit activateDaiChanged();
    }
}

bool SwapEthSettingsItem::activateUsdt() const
{
    return m_activateUsdt;
}

void SwapEthSettingsItem::activateUsdt(bool value)
{
    if (value != m_activateUsdt)
    {
        m_activateUsdt = value;
        emit activateUsdtChanged();
    }
}

bool SwapEthSettingsItem::activateWBTC() const
{
    return m_activateWBTC;
}

void SwapEthSettingsItem::activateWBTC(bool value)
{
    if (value != m_activateWBTC)
    {
        m_activateWBTC = value;
        emit activateWBTCChanged();
    }
}

bool SwapEthSettingsItem::getCanEdit() const
{
    return m_coinClient.lock()->canModifySettings();
}

bool SwapEthSettingsItem::getIsConnected() const
{
    return m_shouldConnect;
}

QString SwapEthSettingsItem::getConnectionStatus() const
{
    using beam::ethereum::Client;

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

QString SwapEthSettingsItem::getConnectionErrorMsg() const
{
    using beam::ethereum::IBridge;

    switch (m_coinClient.lock()->getConnectionError())
    {
    case IBridge::ErrorType::IOError:
        //% "Cannot connect to node. Please check your network connection."
        return qtTrId("swap-connection-error");

    default:
        return QString();
    }
}

void SwapEthSettingsItem::applyNodeAddress(const QString& address)
{
    setNodeAddress(address);
    /*auto unpackedAddress = parseAddress(address);
    setNodeAddress(unpackedAddress.address);
    if (unpackedAddress.port > 0)
    {
        setNodePort(unpackedAddress.port);
    }*/
}

std::vector<std::string> SwapEthSettingsItem::GetSeedPhraseFromSeedItems() const
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
