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
#include "wallet_view.h"
#include "model/app_model.h"
#include <QUrl>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>


WalletViewModel::WalletViewModel()
    : _model(AppModel::getInstance().getWalletModel())
{
    connect(_model, &WalletModel::slatepackReady, this,
            [this](const beam::wallet::TxID& txId, const QString& armored) {
                emit slatepackProduced(QString::fromStdString(std::to_string(txId)), armored);
            });
    connect(_model, &WalletModel::slatepackImportResult, this,
            [this](bool ok, const QString& error, const QVariantMap& info) {
                // Runs on the UI thread — safe to touch AssetsManager to resolve the ticker + icon.
                QVariantMap m = info;
                if (auto amgr = AppModel::getInstance().getAssets())
                {
                    const auto aid = static_cast<beam::Asset::ID>(info.value("assetId").toUInt());
                    m["unitName"] = amgr->getUnitName(aid, AssetsManager::NoShorten);
                    m["icon"]     = amgr->getIcon(aid);
                }
                emit slatepackImported(ok, error, m);
            });
}

void WalletViewModel::importSlatepack(const QString& text)
{
    _model->getAsync()->importSlatepack(text.toStdString());
}

void WalletViewModel::commitSlatepack(const QString& txId)
{
    _model->getAsync()->commitSlatepack(txId.toStdString());
}

void WalletViewModel::cancelSlatepack(const QString& txId)
{
    _model->getAsync()->cancelSlatepack(txId.toStdString());
}

void WalletViewModel::saveSlatepackToFile(const QString& txId, const QString& armored)
{
    // Constructed dialog + exec() with a null parent — the pattern AppsViewModel::chooseFile
    // uses to open a native file dialog from inside a modal QML popup.
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QFileDialog dialog(nullptr, "Save Slatepack", QDir(dir).filePath(txId + ".slatepack"),
                       "Slatepack files (*.slatepack)");
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDefaultSuffix("slatepack");
    dialog.setWindowModality(Qt::WindowModality::ApplicationModal);
    if (!dialog.exec())
        return;
    const QString path = dialog.selectedFiles().value(0);
    if (path.isEmpty())
        return;
    QFile f(path);
    if (f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        f.write(armored.toUtf8());
}

QString WalletViewModel::openSlatepackFromFile()
{
    QFileDialog dialog(nullptr, "Load Slatepack", "", "Slatepack files (*.slatepack)");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setWindowModality(Qt::WindowModality::ApplicationModal);
    if (!dialog.exec())
        return QString();
    const QString path = dialog.selectedFiles().value(0);
    if (path.isEmpty())
        return QString();
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return QString();
    return QString::fromUtf8(f.readAll());
}

QString WalletViewModel::readSlatepackFile(const QUrl& file)
{
    const QString path = file.toLocalFile();
    if (path.isEmpty())
        return QString();
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return QString();
    return QString::fromUtf8(f.readAll());
}

