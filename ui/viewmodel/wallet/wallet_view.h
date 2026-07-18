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
#include <QUrl>
#include "model/wallet_model.h"

class WalletViewModel : public QObject
{
    Q_OBJECT
public:
    WalletViewModel();
    ~WalletViewModel() = default;

    Q_INVOKABLE void importSlatepack(const QString& text);
    Q_INVOKABLE void commitSlatepack(const QString& txId);
    Q_INVOKABLE void cancelSlatepack(const QString& txId);
    Q_INVOKABLE void saveSlatepackToFile(const QString& txId, const QString& armored);
    Q_INVOKABLE QString openSlatepackFromFile();
    Q_INVOKABLE QString readSlatepackFile(const QUrl& file);  // drag-and-drop

signals:
    void slatepackProduced(const QString& txId, const QString& armored);
    void slatepackImported(bool ok, const QString& error, const QVariantMap& info);

private:
    WalletModel::Ptr _model;
};
