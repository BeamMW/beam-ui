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
#include "apps_view.h"
#include "utility/logger.h"
#include "model/settings.h"
#include "model/app_model.h"
#include "version.h"

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
        stopServer();
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
        bool hasLocalApps = false;
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
                const auto content = in.readAll();
                if (content.isEmpty())
                {
                    throw std::runtime_error("File is empty or failed to read");
                }

                QMap<QString, QVariant> app;
                const auto utf = content.toUtf8();

                // do not make json const or it will throw on missing keys
                auto json = nlohmann::json::parse(utf.begin(), utf.end());
                if (!json.is_object() || json.empty())
                {
                    throw std::runtime_error("Invalid json");
                }

                const auto& desc = json["description"];
                if (!desc.is_string() || desc.empty())
                {
                    throw std::runtime_error("Invalid description");
                }
                app.insert("description", QString::fromStdString(desc.get<std::string>()));

                const auto& name = json["name"];
                if (!name.is_string() || name.empty())
                {
                    throw std::runtime_error("Invalid name");
                }
                app.insert("name", QString::fromStdString(name.get<std::string>()));

                const auto& url = json["url"];
                if (!url.is_string() || url.empty())
                {
                    throw std::runtime_error("Invalid url");
                }
                app.insert("url", expandLocalUrl(justFolder, url.get<std::string>()));

                const auto& icon = json["icon"];
                if (!url.empty())
                {
                    if (!icon.is_string())
                    {
                        throw std::runtime_error("Invalid icon");
                    }
                    app.insert("icon", expandLocalUrl(justFolder, icon.get<std::string>()));
                }

                const auto& av = json["api_version"];
                if (!av.empty())
                {
                    if (!av.is_string())
                    {
                        throw std::runtime_error("Invalid api_version");
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
                    throw std::runtime_error("Invalid min_api_version");
                }

                hasLocalApps = true;
                result.push_back(app);
            }
            catch(std::runtime_error& err)
            {
                LOG_WARNING() << "Error while reading local app from " << mpath.toStdString() << ", " << err.what();
            }
        }

        if(hasLocalApps)
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

    void AppsViewModel::stopServer()
    {
        if(_server)
        {
            _server->Stop();
        }
    }
}
