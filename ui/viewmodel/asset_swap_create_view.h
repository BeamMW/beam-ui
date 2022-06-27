// Copyright 2022 The Beam Team
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
#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>
#include "model/assets_manager.h"

class AssetSwapCreateViewModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QMap<QString, QVariant>> currenciesList READ getCurrenciesList NOTIFY currenciesListChanged)

  public:
    AssetSwapCreateViewModel();

  signals:
    void currenciesListChanged();

  private:
    QList<QMap<QString, QVariant>> getCurrenciesList() const;

    AssetsManager::Ptr _amgr;
};
