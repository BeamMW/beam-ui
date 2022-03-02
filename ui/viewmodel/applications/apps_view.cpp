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
#include "apps_view.h"
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
    QString parseStringField(nlohmann::json& json, const char* fieldName)
    {
        const auto& field = json[fieldName];
        if (!field.is_string())
        {
            std::stringstream ss;
            ss << "Invalid " << fieldName << " of the dapp";
            throw std::runtime_error(ss.str());
        }
        return QString::fromStdString(field.get<std::string>());
    }
}

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

        loadApps();
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

    QString AppsViewModel::getAppCachePath(const QString& appid) const
    {
        return AppSettings().getAppsStoragePath(appid);
    }

    QString AppsViewModel::getAppStoragePath(const QString& appid) const
    {
        return AppSettings().getAppsCachePath(appid);
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

    void AppsViewModel::loadApps()
    {
        // TODO: separate dev/local/"from store DApps"
        // load local apps
        _apps = getLocalApps();

        // load server apps
        QPointer<AppsViewModel> guard(this);
        AppModel::getInstance().getWalletModel()->getAsync()->getAppsList(
            [this, guard](bool isOk, const std::string& response)
            {
                if (!guard)
                {
                    return;
                }

                try
                {
                    if (!isOk)
                    {
                        throw std::runtime_error("unsuccessful request");
                    }

                    auto json = nlohmann::json::parse(response);

                    // parse & verify
                    if (json.empty() || !json.is_array())
                    {
                        throw std::runtime_error("invalid response");
                    }

                    for (auto& item : json.items())
                    {
                        QMap<QString, QVariant> app;
                        auto name = parseStringField(item.value(), "name");
                        auto url = parseStringField(item.value(), "url");
                        const auto appid = beam::wallet::GenerateAppID(name.toStdString(), url.toStdString());
                        
                        app.insert("appid", QString::fromStdString(appid));
                        app.insert("description", parseStringField(item.value(), "description"));
                        app.insert("name", name);
                        app.insert("url", url);
                        app.insert("icon", parseStringField(item.value(), "icon"));

                        bool isSupported = true;

                        app.insert("supported", isSupported);

                        // TODO: check order of the DApps
                        _apps.push_back(app);
                    }
                }
                catch (const std::runtime_error& err)
                {
                    // TODO: mb need to transfer the error to QML(errorMessage)
                    LOG_WARNING() << "Failed to load remote applications list, " << err.what();
                }
                emit appsChanged();
            });
    }

    QList<QMap<QString, QVariant>> AppsViewModel::getApps()
    {
        return _apps;
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

            const auto name  = AppSettings().getDevAppName();
            const auto url   = AppSettings().getDevAppUrl();
            const auto appid = QString::fromStdString(beam::wallet::GenerateAppID(name.toStdString(), url.toStdString()));

            //% "This is your dev application"
            devapp.insert("description",     qtTrId("apps-devapp"));
            devapp.insert("name",            name);
            devapp.insert("url",             url);
            devapp.insert("api_version",     AppSettings().getDevAppApiVer());
            devapp.insert("min_api_version", AppSettings().getDevAppMinApiVer());
            devapp.insert("appid", appid);
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
                app.insert("full_path", fullFolder);
                result.push_back(app);
            }
            catch(std::runtime_error& err)
            {
                LOG_WARNING() << "Error while reading local app from " << mpath.toStdString() << ", " << err.what();
            }
        }

        _lastLocalApps = result;
        return result;
    }

    bool AppsViewModel::isPublisher() const
    {
        // TODO: check after implementation "becomePublisher"
        return _isPublisher;
    }

    QString AppsViewModel::nickname() const
    {
        return _nickname;
    }

    void AppsViewModel::nickname(const QString& name)
    {
        if (name != _nickname)
        {
            _nickname = name;
            emit nicknameChanged();
        }
    }

    QString AppsViewModel::shortTitle() const
    {
        return _shortTitle;
    }

    void AppsViewModel::shortTitle(const QString& value)
    {
        if (value != _shortTitle)
        {
            _shortTitle = value;
            emit shortTitleChanged();
        }
    }

    QString AppsViewModel::aboutMe() const
    {
        return _aboutMe;
    }

    void AppsViewModel::aboutMe(const QString& value)
    {
        if (value != _aboutMe)
        {
            _aboutMe = value;
            emit aboutMeChanged();
        }
    }

    QString AppsViewModel::website() const
    {
        return _website;
    }

    void AppsViewModel::website(const QString& value)
    {
        if (value != _website)
        {
            _website = value;
            emit websiteChanged();
        }
    }

    QString AppsViewModel::twitter() const
    {
        return _twitter;
    }

    void AppsViewModel::twitter(const QString& value)
    {
        if (value != _twitter)
        {
            _twitter = value;
            emit twitterChanged();
        }
    }

    QString AppsViewModel::linkedin() const
    {
        return _linkedin;
    }

    void AppsViewModel::linkedin(const QString& value)
    {
        if (value != _linkedin)
        {
            _linkedin = value;
            emit linkedinChanged();
        }
    }

    QString AppsViewModel::instagram() const
    {
        return _instagram;
    }

    void AppsViewModel::instagram(const QString& value)
    {
        if (value != _instagram)
        {
            _instagram = value;
            emit instagramChanged();
        }
    }

    QString AppsViewModel::telegram() const
    {
        return _telegram;
    }

    void AppsViewModel::telegram(const QString& value)
    {
        if (value != _telegram)
        {
            _telegram = value;
            emit telegramChanged();
        }
    }

    QString AppsViewModel::discord() const
    {
        return _discord;
    }

    void AppsViewModel::discord(const QString& value)
    {
        if (value != _discord)
        {
            _discord = value;
            emit discordChanged();
        }
    }

    QString AppsViewModel::addPublisherByKey(const QString& publicKey)
    {
        // TODO: implement
        return {};
    }

    bool AppsViewModel::uninstallLocalApp(const QString& appid)
    {
        const auto it = std::find_if(_lastLocalApps.begin(), _lastLocalApps.end(), [appid](const auto& props) -> bool {
            const auto ait = props.find("appid");
            if (ait == props.end())
            {
                assert(false);
                return false;
            }
            return ait->toString() == appid;
        });

        if (it == _lastLocalApps.end())
        {
            assert(false);
            return false;
        }

        const auto pathit = it->find("full_path");
        if (pathit == it->end())
        {
            assert(false);
            return false;
        }

        const auto path = pathit->toString();
        if (path.isEmpty())
        {
            assert(false);
            return false;
        }

        LOG_INFO() << "Deleting local app in folder " << path.toStdString();

        QDir dir(path);
        return dir.removeRecursively();
    }

    QString AppsViewModel::expandLocalUrl(const QString& folder, const std::string& url) const
    {
        QString result = QString::fromStdString(url);
        result.replace("localapp", QString("http://") + _serverAddr + "/" + folder);
        return result;
    }

    QString AppsViewModel::expandLocalFile(const QString& folder, const std::string& url) const
    {
        auto path = QDir(AppSettings().getLocalAppsPath()).filePath(folder);
        auto result = QString::fromStdString(url);
        result.replace("localapp", QString("file:///") + path);
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

    QString AppsViewModel::chooseFile()
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

    QString AppsViewModel::installFromFile(const QString& rawFname)
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
                    const auto app = validateAppManifest(in, "");
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
}
