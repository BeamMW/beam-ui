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
#include <QMessageBox>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QWebEngineProfile>
#include <QFileDialog>
#include "apps_view.h"
#include "utility/logger.h"
#include "model/settings.h"
#include "model/app_model.h"
#include "version.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip/JlCompress.h"
#include "viewmodel/qml_globals.h"

namespace beamui::applications
{
    AppsViewModel::AppsViewModel()
    {
        LOG_INFO() << "AppsViewModel created";

        auto defaultProfile = QWebEngineProfile::defaultProfile();
        defaultProfile->setHttpCacheType(QWebEngineProfile::HttpCacheType::DiskHttpCache);
        defaultProfile->setPersistentCookiesPolicy(QWebEngineProfile::PersistentCookiesPolicy::AllowPersistentCookies);
        defaultProfile->setCachePath(AppSettings().getAppsCachePath());
        defaultProfile->setPersistentStoragePath(AppSettings().getAppsStoragePath());

        _userAgent  = defaultProfile->httpUserAgent() + " BEAM/" + QString::fromStdString(PROJECT_VERSION);
        _serverAddr = QString("127.0.0.1:") + QString::number(AppSettings().getAppsServerPort());
    }

    AppsViewModel::~AppsViewModel()
    {
        if (_server)
        {
            _server.reset();
        }
        LOG_INFO() << "AppsViewModel destroyed";
    }

    void AppsViewModel::onCompleted(QObject *webView)
    {
        assert(webView != nullptr);
    }

    QString AppsViewModel::getAppsUrl() const
    {
        auto& settings = AppModel::getInstance().getSettings();
        return settings.getAppsUrl();
    }

    QString AppsViewModel::getAppCachePath(const QString& appname) const
    {
        return AppSettings().getAppsStoragePath(appname);
    }

    QString AppsViewModel::getAppStoragePath(const QString& appname) const
    {
        return AppSettings().getAppsCachePath(appname);
    }

    QString AppsViewModel::getUserAgent() const
    {
        return _userAgent;
    }

    QMap<QString, QVariant> AppsViewModel::validateAppManifest(QTextStream& in, const QString& appFolder)
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
        app.insert("name", QString::fromStdString(name.get<std::string>()));

        const auto& url = json["url"];
        if (!url.is_string() || url.empty())
        {
            throw std::runtime_error("Invalid url in the manifest file");
        }
        app.insert("url", expandLocalUrl(appFolder, url.get<std::string>()));

        const auto& icon = json["icon"];
        if (!url.empty())
        {
            if (!icon.is_string())
            {
                throw std::runtime_error("Invalid icon in the manifest file");
            }
            app.insert("icon", expandLocalUrl(appFolder, icon.get<std::string>()));
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

        return app;
    }

    QList<QMap<QString, QVariant>> AppsViewModel::getLocalApps()
    {
        QList<QMap<QString, QVariant>> result;

        //
        // Dev App
        //
        if (!AppSettings().getDevAppName().isEmpty())
        {
            QMap<QString, QVariant> devapp;
            //% "This is your dev application"
            devapp.insert("description",     qtTrId("apps-devapp"));
            devapp.insert("name",            AppSettings().getDevAppName());
            devapp.insert("url",             AppSettings().getAppsUrl());
            devapp.insert("api_version",     AppSettings().getDevAppApiVer());
            devapp.insert("min_api_version", AppSettings().getDevAppMinApiVer());
            result.push_back(devapp);
        }

        QDir appsdir(AppSettings().getLocalAppsPath());
        auto list = appsdir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

        for (const auto& finfo: list)
        {
            const auto fullFolder = finfo.absoluteFilePath();
            const auto justFolder = finfo.fileName();
            auto mpath = QDir(fullFolder).absoluteFilePath("manifest.json");

            try
            {
                QFile file(mpath);
                if (!file.open(QFile::ReadOnly | QFile::Text))
                {
                    throw std::runtime_error("Cannot open file");
                }

                QTextStream in(&file);
                auto app = validateAppManifest(in, justFolder);
                app.insert("local", true);
                result.push_back(app);
            }
            catch(std::runtime_error& err)
            {
                LOG_WARNING() << "Error while reading local app from " << mpath.toStdString() << ", " << err.what();
            }
        }

        if (!result.empty())
        {
            launchAppServer();
        }

        return result;
    }

    QString AppsViewModel::expandLocalUrl(const QString& folder, const std::string& url) const
    {
        QString result = QString::fromStdString(url);
        result.replace("localapp", QString("http://") + _serverAddr + "/" + folder);
        return result;
    }

    void AppsViewModel::launchAppServer()
    {
        if(!_server)
        {
            try
            {
                _server = std::make_unique<AppsServer>(AppSettings().getLocalAppsPath(),
                                                       AppSettings().getAppsServerPort());
            }
            catch(std::runtime_error& err)
            {
                LOG_WARNING() << "Failed to launch local apps server: " << err.what();
            }
        }
    }

    bool AppsViewModel::installFromFile()
    {
        QFileDialog dialog(nullptr,
                        //% "Select application to install"
                        qtTrId("applications-install-title"),
                        "",
                        "BEAM DApp files (*.dapp)");
        dialog.setWindowModality(Qt::WindowModality::ApplicationModal);
        if (!dialog.exec())
        {
            return false;
        }

        try
        {
            QString archiveName = dialog.selectedFiles().value(0);
            QuaZip zip(archiveName);
            if(!zip.open(QuaZip::Mode::mdUnzip))
            {
                throw std::runtime_error("Failed to open the DApp file");
            }

            QString guid, appName;
            for (bool ok = zip.goToFirstFile(); ok; ok = zip.goToNextFile())
            {
                const auto fname = zip.getCurrentFileName();
                if (fname == "manifest.json")
                {
                    QuaZipFile mfile(zip.getZipName(), fname);
                    if (!mfile.open(QIODevice::ReadOnly))
                    {
                        throw std::runtime_error("Failed to read the DApp file");
                    }

                    QTextStream in(&mfile);
                    const auto app = validateAppManifest(in, "");
                    guid = app["guid"].value<QString>();
                    appName = app["name"].value<QString>();
                }
            }

            if (guid.isEmpty())
            {
                throw std::runtime_error("Invalid DAPP file");
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
            if(JlCompress::extractDir(archiveName, appFolder).isEmpty())
            {
                //cleanupFolder(appFolder)
                throw std::runtime_error("DAPP Installation failed");
            }

            //% "'%1' is successfully installed"
            QMLGlobals::showMessage(qtTrId("appliactions-install-ok").arg(appName));
            return true;
        }
        catch(std::exception& err)
        {
            //% "Failed to install DAPP: %1"
            const auto errMsg = qtTrId("appliactions-install-fail").arg(err.what());
            LOG_ERROR() << errMsg.toStdString();
            QMLGlobals::showMessage(errMsg);
            return false;
        }
    }
}
