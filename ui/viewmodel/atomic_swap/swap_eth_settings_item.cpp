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
    : m_coinClient(AppModel2::getInstance().getSwapEthClient())
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
    m_settings->m_projectID = m_infuraProjectID.toStdString();
    m_settings->m_secretWords = GetSeedPhraseFromSeedItems();

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
