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

#include "seed_phrase_item.h"

#include "wallet/transactions/swaps/bridges/bitcoin/common.h"

SeedPhraseItem::SeedPhraseItem(int index, const QString& phrase)
    : m_index(index)
    , m_phrase(phrase)
    , m_userInput(phrase)
{
}

bool SeedPhraseItem::isModified() const
{
    return m_userInput != m_phrase;
}

const QString& SeedPhraseItem::getValue() const
{
    return m_userInput;
}

void SeedPhraseItem::setValue(const QString& value)
{
    if (m_userInput != value)
    {
        m_userInput = value;
        emit valueChanged();
        emit isModifiedChanged();
        emit isAllowedChanged();
    }
}

const QString& SeedPhraseItem::getPhrase() const
{
    return m_phrase;
}

int SeedPhraseItem::getIndex() const
{
    return m_index;
}

bool SeedPhraseItem::isAllowed() const
{
    return beam::bitcoin::isAllowedWord(m_userInput.toStdString());
}

void SeedPhraseItem::applyChanges()
{
    m_phrase = m_userInput;
    emit phraseChanged();
}

void SeedPhraseItem::revertChanges()
{
    setValue(m_phrase);
}