// Copyright 2021 The Beam Team
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

class SeedValidationHelper: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isSeedValidatiomMode READ getIsSeedValidatiomMode WRITE setIsSeedValidationMode NOTIFY isSeedValidatiomModeChanged)
    Q_PROPERTY(bool isTriggeredFromSettings READ getIsTriggeredFromSettings WRITE setIsTriggeredFromSettings NOTIFY isTriggeredFromSettingsChanged)
    Q_PROPERTY(bool isSeedValidated READ getIsSeedValidated NOTIFY isSeedValidatedChanged)

  public:
    SeedValidationHelper();
    ~SeedValidationHelper();

    bool getIsSeedValidatiomMode() const;
    void setIsSeedValidationMode(bool value);
    bool getIsTriggeredFromSettings() const;
    void setIsTriggeredFromSettings(bool value);
    bool getIsSeedValidated() const;

    Q_INVOKABLE void validate();

  signals:
    void isSeedValidatiomModeChanged();
    void isTriggeredFromSettingsChanged();
    void isSeedValidatedChanged();

  private:
    std::string m_seed;
};
