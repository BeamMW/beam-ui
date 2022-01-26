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
#include <QObject>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QWebEngineProfile>
#include <QFileDialog>
#include "dapps_store_view.h"
#include "utility/logger.h"
#include "model/app_model.h"
#include "version.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip/JlCompress.h"
#include "viewmodel/qml_globals.h"
#include "wallet/client/apps_api/apps_utils.h"

namespace
{
    const char* kDappCID = "95df85ae1f0e769c9903387180cfcb49fb222819b33b34941c07ff913fa70105";
    const char* kDappStoreShaderPath = "d:/work/dapps-store/beam-dapps-store/shaders/dapps_store_app.wasm";
}

DappsStoreViewModel::DappsStoreViewModel()
{
    LOG_INFO() << "DappsStoreViewModel created";

    auto defaultProfile = QWebEngineProfile::defaultProfile();
    defaultProfile->setHttpCacheType(QWebEngineProfile::HttpCacheType::DiskHttpCache);
    defaultProfile->setPersistentCookiesPolicy(QWebEngineProfile::PersistentCookiesPolicy::AllowPersistentCookies);
    defaultProfile->setCachePath(AppSettings().getAppsCachePath());
    defaultProfile->setPersistentStoragePath(AppSettings().getAppsStoragePath());

    _serverAddr = QString("127.0.0.1:") + QString::number(AppSettings().getAppsServerPort());

    loadApps();
}

DappsStoreViewModel::~DappsStoreViewModel()
{
    LOG_INFO() << "DappsStoreViewModel destroyed";
}

void DappsStoreViewModel::onCompleted(QObject *webView)
{
    assert(webView != nullptr);
}

QMap<QString, QVariant> DappsStoreViewModel::parseAppManifest(QTextStream& in, const QString& appFolder)
{
    QMap<QString, QVariant> app;

    const auto content = in.readAll();
    if (content.isEmpty())
    {
        throw std::runtime_error("Failed to read the manifest file");
    }

    const auto utf = content.toUtf8();

    // do not make json const or it will throw on missing keys
    auto json = nlohmann::json::parse(utf.begin(), utf.end());
    if (!json.is_object() || json.empty())
    {
        throw std::runtime_error("Invalid manifest file");
    }

    const auto& guid = json["guid"];
    if (!guid.is_string() || guid.empty())
    {
        throw std::runtime_error("Invalid GUID in the manifest file");
    }
    app.insert("guid", QString::fromStdString(guid.get<std::string>()));

    const auto& desc = json["description"];
    if (!desc.is_string() || desc.empty())
    {
        throw std::runtime_error("Invalid description in the manifest file");
    }
    app.insert("description", QString::fromStdString(desc.get<std::string>()));

    const auto& name = json["name"];
    if (!name.is_string() || name.empty())
    {
        throw std::runtime_error("Invalid app name in the manifest file");
    }

    const auto sname = name.get<std::string>();
    app.insert("name", QString::fromStdString(sname));

    const auto& url = json["url"];
    if (!url.is_string() || url.empty())
    {
        throw std::runtime_error("Invalid url in the manifest file");
    }

    const auto surl = url.get<std::string>();
    app.insert("url", expandLocalUrl(appFolder, surl));

    const auto& icon = json["icon"];
    if (!icon.empty())
    {
        if (!icon.is_string())
        {
            throw std::runtime_error("Invalid icon in the manifest file");
        }

        const auto ipath = expandLocalFile(appFolder, icon.get<std::string>());
        app.insert("icon", ipath);
        LOG_INFO() << "App: " << sname << ", icon: " << ipath.toStdString();
    }

    const auto& av = json["api_version"];
    if (!av.empty())
    {
        if (!av.is_string())
        {
            throw std::runtime_error("Invalid api_version in the manifest file");
        }
        app.insert("api_version", QString::fromStdString(av.get<std::string>()));
    }

    const auto& mav = json["min_api_version"];
    if (!mav.empty())
    {
        if (!mav.is_string())
        {
            app.insert("min_api_version", QString::fromStdString(mav.get<std::string>()));
        }
        throw std::runtime_error("Invalid min_api_version in the manifest file");
    }

    app.insert("local", true);
    const auto appid = beam::wallet::GenerateAppID(sname, surl);
    app.insert("appid", QString::fromStdString(appid));

    return app;
}

void DappsStoreViewModel::loadApps()
{
    // settings ? - load whitelist of the publishers
    // callShader - getAppsByPublisher
    // add Apps to AppList
    // install app from IPFS

    std::string args = "role=manager,action=view_dapps,cid=";
    args += kDappCID;

    QPointer<DappsStoreViewModel> guard(this);

    AppModel::getInstance().getWalletModel()->getAsync()->callShader(kDappStoreShaderPath, args,
        [this, guard](const std::string& err, const std::string& output, const beam::wallet::TxID& id)
        {
            if (!guard)
            {
                return;
            }

            try
            {
                auto json = nlohmann::json::parse(output);

                if (json.empty() || !json.is_object())
                {
                    throw std::runtime_error("Invalid response of the view_dapps method");
                }

                LOG_INFO() << json.dump(4);

                QList<QMap<QString, QVariant>> apps;

                for (auto& item : json.items())
                {
                    // TODO: add publisherKey to error messages
                    if (!item.value().is_object())
                    {
                        throw std::runtime_error("Invalid body of the dapp " + item.key());
                    }
                    const auto& name = item.value()["label"];
                    if (!name.is_string())
                    {
                        throw std::runtime_error("Invalid name of the dapp " + item.key());
                    }
                    const auto& ipfsCID = item.value()["ipfs_id"];
                    if (!ipfsCID.is_string())
                    {
                        throw std::runtime_error("Invalid ipfsCID of the dapp " + item.key());
                    }

                    QMap<QString, QVariant> app;
                    // TODO: change description
                    app.insert("description", "stored on IPFS");
                    app.insert("name", QString::fromStdString(name.get<std::string>()));
                    app.insert("url", QString::fromStdString(ipfsCID.get<std::string>()));
                    // TODO: read api_version from contract
                    app.insert("api_version", AppSettings().getDevAppApiVer());
                    app.insert("min_api_version", AppSettings().getDevAppMinApiVer());
                    //app.insert("appid", appid);

                    // TODO: check
                    app.insert("supported", true);

                    apps.push_back(app);
                }

                _apps = apps;
                emit appsChanged();
            }
            catch (std::runtime_error& err)
            {
                LOG_WARNING() << "Error while parsing app from contract" << ", " << err.what();
            }
        }
    );
}

QList<QMap<QString, QVariant>> DappsStoreViewModel::getApps()
{
    return _apps;
}

QString DappsStoreViewModel::getPublisherKey()
{
    if (_publisherKey.isEmpty())
    {
        std::string args = "role=manager,action=get_pk,cid=";
        args += kDappCID;
        QPointer<DappsStoreViewModel> guard(this);

        AppModel::getInstance().getWalletModel()->getAsync()->callShader(kDappStoreShaderPath, args,
            [this, guard](const std::string& err, const std::string& output, const beam::wallet::TxID& id)
            {
                if (!guard)
                {
                    return;
                }

                try
                {
                    auto json = nlohmann::json::parse(output);

                    if (json.empty() || !json.is_object() || !json["pk"].is_string())
                    {
                        throw std::runtime_error("Invalid response of the get_pk method");
                    }

                    _publisherKey = QString::fromStdString(json["pk"].get<std::string>());
                    emit publisherKeyChanged();
                }
                catch (std::runtime_error& err)
                {
                    LOG_WARNING() << "Failed to get publisherKey from contract" << ", " << err.what();
                }
            }
        );
    }
    return _publisherKey;
}

QString DappsStoreViewModel::expandLocalUrl(const QString& folder, const std::string& url) const
{
    QString result = QString::fromStdString(url);
    result.replace("localapp", QString("http://") + _serverAddr + "/" + folder);
    return result;
}

QString DappsStoreViewModel::expandLocalFile(const QString& folder, const std::string& url) const
{
    auto path = QDir(AppSettings().getLocalAppsPath()).filePath(folder);
    auto result = QString::fromStdString(url);
    result.replace("localapp", QString("file:///") + path);
    return result;
}

QString DappsStoreViewModel::chooseFile()
{
    QFileDialog dialog(nullptr,
                        //% "Select application to install"
                        qtTrId("applications-install-title"),
                        "",
                        "BEAM DApp files (*.dapp)");

    dialog.setWindowModality(Qt::WindowModality::ApplicationModal);
    if (!dialog.exec())
    {
        return "";
    }
    return dialog.selectedFiles().value(0);
}

QString DappsStoreViewModel::installFromFile(const QString& rawFname)
{
    try
    {
        QString fname = rawFname;

        // Some shells/systems provide incorrect count of '/' after file:
        // For example in gnome on linux one '/' is missing. So this ugly code is necessary
        if (fname.startsWith("file:"))
        {
            fname = fname.remove(0, 5);
            while(fname.startsWith("/"))
            {
                fname = fname.remove(0, 1);
            }

            #ifndef WIN32
            fname = QString("/") + fname;
            #endif
        }

        LOG_DEBUG() << "Installing DApp from file " << rawFname.toStdString() << " | " << fname.toStdString();

        QuaZip zip(fname);
        if(!zip.open(QuaZip::Mode::mdUnzip))
        {
            throw std::runtime_error("Failed to open the DApp file");
        }

        QString guid, appName;
        for (bool ok = zip.goToFirstFile(); ok; ok = zip.goToNextFile())
        {
            const auto zipFname = zip.getCurrentFileName();
            if (zipFname == "manifest.json")
            {
                QuaZipFile mfile(zip.getZipName(), zipFname);
                if (!mfile.open(QIODevice::ReadOnly))
                {
                    throw std::runtime_error("Failed to read the DApp file");
                }

                QTextStream in(&mfile);
                const auto app = parseAppManifest(in, "");
                guid = app["guid"].value<QString>();
                appName = app["name"].value<QString>();
            }
        }

        if (guid.isEmpty())
        {
            throw std::runtime_error("Invalid DApp file");
        }

        const auto appsPath = AppSettings().getLocalAppsPath();
        const auto appFolder = QDir(appsPath).filePath(guid);

        if (QDir(appFolder).exists())
        {
            if(!QDir(appFolder).removeRecursively())
            {
                throw std::runtime_error("Failed to prepare folder");
            }
        }

        QDir(appsPath).mkdir(guid);
        if(JlCompress::extractDir(fname, appFolder).isEmpty())
        {
            //cleanupFolder(appFolder)
            throw std::runtime_error("DApp Installation failed");
        }

        return appName;
    }
    catch(std::exception& err)
    {
        LOG_ERROR() << "Failed to install DApp: " << err.what();
        return "";
    }
}

void DappsStoreViewModel::uploadApp()
{
    try
    {
        // select file
        const auto appPath = chooseFile();

        // unpack and verify
        QuaZip zip(appPath);
        if (!zip.open(QuaZip::Mode::mdUnzip))
        {
            throw std::runtime_error("Failed to open the DApp file");
        }

        QString appName;
        for (bool ok = zip.goToFirstFile(); ok; ok = zip.goToNextFile())
        {
            const auto zipFname = zip.getCurrentFileName();
            if (zipFname == "manifest.json")
            {
                QuaZipFile mfile(zip.getZipName(), zipFname);
                if (!mfile.open(QIODevice::ReadOnly))
                {
                    throw std::runtime_error("Failed to read the DApp file");
                }

                QTextStream in(&mfile);
                const auto app = parseAppManifest(in, "");
                appName = app["name"].value<QString>();
            }
        }

        // add to IPFS
        QFile file(appPath);

        if (!file.open(QFile::ReadOnly))
        {
            throw std::runtime_error("Failed to read the DApp file");
        }

        QDataStream in(&file);
        auto buffer = file.readAll();
        auto ipfs = AppModel::getInstance().getWalletModel()->getIPFS();
        QPointer<DappsStoreViewModel> guard(this);

        ipfs->AnyThread_add(beam::ByteBuffer(buffer.cbegin(), buffer.cend()),
            [this, guard, appName] (std::string&& ipfsCID) {

                LOG_INFO() << "IPFSCID: " << ipfsCID;

                // save result to contract
                addAppToStore(appName, ipfsCID);
            },
            [this, guard] (std::string&& err) {
                LOG_ERROR() << "Failed to add to ipfs: " << err;
            }
        );
    }
    catch (std::runtime_error& err)
    {
        LOG_ERROR() << "Failed to upload DApp: " << err.what();
    }
}

void DappsStoreViewModel::addAppToStore(const QString& appName, const std::string& ipfsCID)
{
    std::string args = "role=manager,action=add_dapp,cid=";
    args += kDappCID;
    args += ",ipfs_id=" + ipfsCID + ",label=" + appName.toStdString();

    QPointer<DappsStoreViewModel> guard(this);

    AppModel::getInstance().getWalletModel()->getAsync()->callShader(kDappStoreShaderPath, args,
        [this, guard](const std::string& err, const std::string& output, const beam::wallet::TxID& id)
        {
            if (!guard)
            {
                return;
            }

            try
            {
                // TODO: check TX status

                LOG_INFO() << "App added!";
            }
            catch (std::runtime_error& err)
            {
                LOG_WARNING() << "Failed to get publisherKey from contract" << ", " << err.what();
            }
        }
    );
}
