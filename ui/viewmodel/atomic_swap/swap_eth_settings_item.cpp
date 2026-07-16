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

#include <QLocale>

#include "mnemonic/mnemonic.h"
#include "model/app_model.h"
#include "model/swap_eth_client_model.h"
#include "viewmodel/qml_globals.h"
#include "seed_phrase_item.h"
#include "viewmodel/settings_helpers.h"
#include "viewmodel/ui_helpers.h"

#include "wallet/transactions/swaps/bridges/ethereum/common.h"
#include "wallet/transactions/swaps/common.h"
#include "model/settings.h"

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
    connect(coinClient.get(), SIGNAL(connectionErrorChanged()), this, SIGNAL(connectionErrorChanged()));
    connect(coinClient.get(), &SwapEthClientModel::endpointValidated, this, &SwapEthSettingsItem::onEndpointValidated);
    connect(coinClient.get(), &SwapEthClientModel::gotTokenInfo, this, &SwapEthSettingsItem::onGotTokenInfo);
    LoadSettings();
    loadCustomTokens();
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
    m_settings->m_projectID = m_infuraProjectID.toStdString();
    m_settings->m_secretWords = GetSeedPhraseFromSeedItems();
    m_settings->m_useCustomRpc = m_useCustomRpc;
    m_settings->m_customRpcUrl = m_customRpcUrl.trimmed().toStdString();

    coinClient->SetSettings(*m_settings);
    clearEndpointCheck();
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

void SwapEthSettingsItem::validateEndpoint()
{
    if (auto c = m_coinClient.lock())
    {
        c->validateEndpoint();
    }
}

void SwapEthSettingsItem::lookupToken(const QString& contractAddress)
{
    const auto address = contractAddress.trimmed().toStdString();
    if (!wallet::IsValidEthContractAddress(address))
    {
        //% "Invalid contract address"
        emit tokenInfoReady(contractAddress, QString(), 0, qtTrId("settings-swap-token-invalid-address"));
        return;
    }

    if (auto c = m_coinClient.lock())
    {
        c->requestTokenInfo(address);
    }
}

void SwapEthSettingsItem::onGotTokenInfo(const QString& contract, const QString& symbol, uint decimals, const QString& error)
{
    emit tokenInfoReady(contract, symbol, decimals, error);
}

void SwapEthSettingsItem::addCustomToken(const QString& contractAddress, const QString& symbol, uint decimals)
{
    const auto address = contractAddress.trimmed().toStdString();
    if (!wallet::IsValidEthContractAddress(address) || symbol.isEmpty())
    {
        return;
    }

    const auto normalized = QString::fromStdString(address).toLower();
    if (isBuiltinTokenContract(normalized))
    {
        //% "This token is already supported"
        emit tokenInfoReady(contractAddress, QString(), 0, qtTrId("settings-swap-token-already-added"));
        return;
    }
    for (const auto& token : m_customTokens)
    {
        if (token.value("contract").toString().toLower() == normalized)
        {
            //% "This token is already supported"
            emit tokenInfoReady(contractAddress, QString(), 0, qtTrId("settings-swap-token-already-added"));
            return; // already added
        }
    }

    QMap<QString, QVariant> token;
    token.insert("contract", QString::fromStdString(address));
    token.insert("symbol", symbol);
    token.insert("decimals", decimals);
    m_customTokens.push_back(token);

    saveCustomTokens();
    emit customTokensChanged();
}

void SwapEthSettingsItem::removeCustomToken(const QString& contractAddress)
{
    const auto normalized = contractAddress.trimmed().toLower();
    for (int i = 0; i < m_customTokens.size(); ++i)
    {
        if (m_customTokens[i].value("contract").toString().toLower() == normalized)
        {
            m_customTokens.removeAt(i);
            saveCustomTokens();
            emit customTokensChanged();
            return;
        }
    }
}

QList<QMap<QString, QVariant>> SwapEthSettingsItem::getCustomTokens() const
{
    QList<QMap<QString, QVariant>> result;
    for (auto token : m_customTokens)
    {
        token.insert("color", beamui::ColorFromString(token.value("contract").toString()));
        result.push_back(token);
    }
    return result;
}

QList<QMap<QString, QVariant>> SwapEthSettingsItem::getBuiltinTokens() const
{
    QList<QMap<QString, QVariant>> result;
    static const std::tuple<wallet::AtomicSwapCoin, const char*, const char*> builtins[] = {
        { wallet::AtomicSwapCoin::Usdt, "USDT", "qrc:/assets/icon-usdt.svg" },
        { wallet::AtomicSwapCoin::Dai,  "DAI",  "qrc:/assets/icon-dai.svg" },
        { wallet::AtomicSwapCoin::WBTC, "WBTC", "qrc:/assets/icon-wbtc.svg" },
    };

    for (const auto& [coin, symbol, icon] : builtins)
    {
        QMap<QString, QVariant> token;
        const auto contract = QString::fromStdString(m_settings->GetTokenContractAddress(coin));
        token.insert("symbol", QString(symbol));
        token.insert("contract", contract);
        token.insert("color", beamui::ColorFromString(contract));
        token.insert("icon", QString(icon));
        result.push_back(token);
    }
    return result;
}

bool SwapEthSettingsItem::isBuiltinTokenContract(const QString& normalizedAddress) const
{
    for (const auto& token : getBuiltinTokens())
    {
        if (token.value("contract").toString().toLower() == normalizedAddress)
        {
            return true;
        }
    }
    return false;
}

void SwapEthSettingsItem::loadCustomTokens()
{
    m_customTokens = AppModel::getInstance().getSettings().getEthCustomTokens();
}

void SwapEthSettingsItem::saveCustomTokens()
{
    AppModel::getInstance().getSettings().setEthCustomTokens(m_customTokens);
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
        //% "Ethereum"
        return qtTrId("settings-swap-ethereum-node");
    }
    
    return getGeneralTitle();
}

QString SwapEthSettingsItem::getShowSeedDialogTitle() const
{
    //% "Ethereum seed phrase"
    return qtTrId("ethereum-show-seed-title");
}


QString SwapEthSettingsItem::getShowAddressesDialogTitle() const
{
    //% "Ethereum wallet addresses"
    return qtTrId("ethereum-show-addresses-title");
}

QString SwapEthSettingsItem::getGeneralTitle() const
{
    //% "Ethereum"
    return qtTrId("general-ethereum");
}

QString SwapEthSettingsItem::getCoinID() const
{
    return beamui::getCurrencyUnitName(beamui::Currencies::Ethereum);
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
        infuraProjectID(str2qstr(m_settings->m_projectID));
        setUseCustomRpc(m_settings->m_useCustomRpc);
        setCustomRpcUrl(str2qstr(m_settings->m_customRpcUrl));

        setAccountIndex(m_settings->m_accountIndex);
        shouldConnect(m_settings->m_shouldConnect);
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

void SwapEthSettingsItem::SetSeedPhrase(const std::vector<std::string>& seedPhrase)
{
    if (!m_seedPhraseItems.empty())
    {
        qDeleteAll(m_seedPhraseItems);
        m_seedPhraseItems.clear();
    }

    m_seedPhraseItems.reserve(static_cast<int>(WORD_COUNT));

    if (seedPhrase.empty())
    {
        for (int index = 0; index < static_cast<int>(WORD_COUNT); ++index)
        {
            m_seedPhraseItems.push_back(new SeedPhraseItem(index, QString()));
        }
    }
    else
    {
        assert(seedPhrase.size() == WORD_COUNT);
        int index = 0;
        for (auto& word : seedPhrase)
        {
            m_seedPhraseItems.push_back(new SeedPhraseItem(index++, QString::fromStdString(word)));
        }
    }

    setIsCurrentSeedValid(isValidMnemonic(seedPhrase, language::en));
    emit seedPhrasesChanged();
}

void SwapEthSettingsItem::SetDefaultSettings(bool clearSeed)
{
    infuraProjectID("");
    setAccountIndex(0);
    setUseCustomRpc(false);
    setCustomRpcUrl("");

    if (clearSeed)
    {
        SetSeedPhrase({});
    }
}

QString SwapEthSettingsItem::infuraProjectID() const
{
    return m_infuraProjectID;
}

void SwapEthSettingsItem::infuraProjectID(const QString& value)
{
    if (value != m_infuraProjectID)
    {
        m_infuraProjectID = value;
        emit infuraProjectIDChanged();
        clearEndpointCheck();
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

bool SwapEthSettingsItem::canChangeConnection() const
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

QString SwapEthSettingsItem::getConnectionError() const
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

bool SwapEthSettingsItem::useCustomRpc() const
{
    return m_useCustomRpc;
}

void SwapEthSettingsItem::setUseCustomRpc(bool value)
{
    if (value != m_useCustomRpc)
    {
        m_useCustomRpc = value;
        emit useCustomRpcChanged();
        clearEndpointCheck();
    }
}

QString SwapEthSettingsItem::customRpcUrl() const
{
    return m_customRpcUrl;
}

void SwapEthSettingsItem::setCustomRpcUrl(const QString& value)
{
    if (value != m_customRpcUrl)
    {
        m_customRpcUrl = value;
        emit customRpcUrlChanged();
        clearEndpointCheck();
    }
}

QString SwapEthSettingsItem::endpointCheckResult() const
{
    return m_endpointCheckResult;
}

bool SwapEthSettingsItem::endpointCheckOk() const
{
    return m_endpointCheckOk;
}

QString SwapEthSettingsItem::getChainName(quint64 chainID)
{
    switch (chainID)
    {
    //% "Ethereum Mainnet"
    case 1:  return qtTrId("settings-eth-chain-mainnet");
    //% "Sepolia Testnet"
    case 11155111: return qtTrId("settings-eth-chain-sepolia");
    default:
        //% "chain %1"
        return qtTrId("settings-eth-chain-other").arg(chainID);
    }
}

void SwapEthSettingsItem::onEndpointValidated(quint64 chainID, quint64 blockNumber, bool ok, bool wrongNetwork, const QString& errorMsg)
{
    m_endpointCheckOk = ok;
    if (!ok)
    {
        if (wrongNetwork)
        {
            //% "Connected to %1, but Ethereum Mainnet is required"
            m_endpointCheckResult = qtTrId("settings-eth-endpoint-wrong-network").arg(getChainName(chainID));
        }
        else
        {
            //% "Unable to connect"
            m_endpointCheckResult = qtTrId("settings-eth-endpoint-failed");
            if (!errorMsg.isEmpty())
            {
                m_endpointCheckResult += " (" + errorMsg + ")";
            }
        }
    }
    else
    {
        //% "Connected to %1. Latest block: %2"
        m_endpointCheckResult = qtTrId("settings-eth-endpoint-ok")
            .arg(getChainName(chainID)).arg(QLocale().toString((qulonglong)blockNumber));
    }
    emit endpointCheckResultChanged();
}

void SwapEthSettingsItem::clearEndpointCheck()
{
    if (!m_endpointCheckResult.isEmpty() || m_endpointCheckOk)
    {
        m_endpointCheckResult.clear();
        m_endpointCheckOk = false;
        emit endpointCheckResultChanged();
    }
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
