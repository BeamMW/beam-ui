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

#pragma once

#include <QObject>

class SeedPhraseItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool    isModified READ isModified               NOTIFY isModifiedChanged)
    Q_PROPERTY(bool    isAllowed  READ isAllowed                NOTIFY isAllowedChanged)
    Q_PROPERTY(QString value      READ getValue  WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString phrase     READ getPhrase                NOTIFY phraseChanged)
    Q_PROPERTY(int     index      READ getIndex                 CONSTANT)
public:
    SeedPhraseItem(int index, const QString& phrase);

    bool isAllowed() const;
    bool isModified() const;
    const QString& getValue() const;
    void setValue(const QString& value);
    const QString& getPhrase() const;
    int getIndex() const;

    Q_INVOKABLE void applyChanges();
    Q_INVOKABLE void revertChanges();

signals:
    void isModifiedChanged();
    void isAllowedChanged();
    void valueChanged();
    void phraseChanged();

protected:
    int m_index;
    QString m_phrase;
    QString m_userInput;
};