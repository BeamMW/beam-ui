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
#include <qbuffer.h>
#include "apps_view.h"
#include "utility/logger.h"
#include "model/app_model.h"
#include "version.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip/JlCompress.h"
#include "viewmodel/qml_globals.h"
#include "wallet/api/i_wallet_api.h"
#include "wallet/client/apps_api/apps_utils.h"

namespace
{
    const uint8_t kCountApiVersionParts = 2;
    const uint8_t kCountDAppVersionParts = 4;
    const QString kBeamPublisherName = "Beam Development Limited";
    const QString kBeamPublisherKey = "";
    const QString kLocalapp = "localapp";
    const QString kManifestFile = "manifest.json";

    namespace DApp
    {
        const char kName[] = "name";
        const char kDescription[] = "description";
        const char kIpfsId[] = "ipfs_id";
        const char kUrl[] = "url";
        const char kApiVersion[] = "api_version";
        const char kMinApiVersion[] = "min_api_version";
        const char kGuid[] = "guid";
        const char kId[] = "id";
        const char kPublisherKey[] = "publisher";
        const char kPublisherName[] = "publisherName";
        const char kCategory[] = "category";
        const char kCategoryName[] = "categoryName";
        const char kCategoryColor[] = "categoryColor";
        const char kSupported[] = "supported";
        const char kNotInstalled[] = "notInstalled";
        const char kIcon[] = "icon";
        const char kVersion[] = "version";
        const char kFullPath[] = "fullPath";
        const char kAppid[] = "appid";
        const char kMajor[] = "major";
        const char kMinor[] = "minor";
        const char kRelease[] = "release";
        const char kBuild[] = "build";
        const char kFromServer[] = "isFromServer";
        const char kDevApp[] = "devApp";
        const char kHasUpdate[] = "hasUpdate";
        const char kReleaseDate[] = "release_date";
        const char kLocal[] = "local";

        const int kNameMaxSize = 30;
        const int kDescriptionMaxSize = 1024;
        const int kApiVersionMaxSize = 10;
        const int kIconMaxSize = 10240; // 10kb
    } // namespace DApp

    namespace Publisher
    {
        const char kPubkey[] = "pubkey";
        const char kName[] = "name";
        const char kShortTitle[] = "short_title";
        const char kAboutMe[] = "about_me";
        const char kWebsite[] = "website";
        const char kTwitter[] = "twitter";
        const char kLinkedin[] = "linkedin";
        const char kInstagramp[] = "instagram";
        const char kTelegram[] = "telegram";
        const char kDiscord[] = "discord";
    }

    namespace Actions
    {
        const char kViewDapps[] = "view_dapps";
        const char kViewPublishers[] = "view_publishers";
        const char kMyPublisherInfo[] = "my_publisher_info";
        const char kAddPublisher[] = "add_publisher";
        const char kUpdatePublisher[] = "update_publisher";
        const char kUpdateDapp[] = "update_dapp";
        const char kAddDapp[] = "add_dapp";
        const char kDeleteDapp[] = "delete_dapp";
    }

    class ContractArgs
    {
    public:
        ContractArgs(const std::string& action)
        {
            _stream << "action=" << action;
            _stream << ",cid=" << AppSettings().getDappStoreCID();
        }

        void append(const std::string& arg, const std::string& value)
        {
            _stream << "," << arg << "=" << value;
        }

        void append(const std::string& arg, uint32_t value)
        {
            _stream << "," << arg << "=" << value;
        }

        std::string args() const
        {
            return _stream.str();
        }
    private:
        ContractArgs()
        {}

        std::stringstream _stream;
    };

    QString fromHex(const std::string& value)
    {
        auto tmp = beam::from_hex(value);

        return QString::fromUtf8(reinterpret_cast<char*>(tmp.data()), static_cast<int>(tmp.size()));
    }

    std::string extractStringField(nlohmann::json& json, const char* fieldName)
    {
        const auto& field = json[fieldName];
        if (!field.is_string())
        {
            std::stringstream ss;
            ss << "Invalid " << fieldName << " of the dapp";
            throw std::runtime_error(ss.str());
        }
        return field.get<std::string>();
    }

    QString parseStringField(nlohmann::json& json, const char* fieldName)
    {
        return QString::fromStdString(extractStringField(json, fieldName));
    }

    QString decodeStringField(nlohmann::json& json, const char* fieldName)
    {
        return fromHex(extractStringField(json, fieldName));
    }

    std::string toHex(const QString& value)
    {
        std::string tmp = value.toStdString();
        return beam::to_hex(tmp.data(), tmp.size());
    }

    QVariantMap parsePublisherInfo(nlohmann::json& info)
    {
        QVariantMap tmp;

        tmp[Publisher::kPubkey] = QString::fromStdString(info[Publisher::kPubkey].get<std::string>());
        tmp[Publisher::kName] = fromHex(info[Publisher::kName].get<std::string>());
        tmp[Publisher::kShortTitle] = fromHex(info[Publisher::kShortTitle].get<std::string>());
        tmp[Publisher::kAboutMe] = fromHex(info[Publisher::kAboutMe].get<std::string>());
        tmp[Publisher::kWebsite] = fromHex(info[Publisher::kWebsite].get<std::string>());
        tmp[Publisher::kTwitter] = fromHex(info[Publisher::kTwitter].get<std::string>());
        tmp[Publisher::kLinkedin] = fromHex(info[Publisher::kLinkedin].get<std::string>());
        tmp[Publisher::kInstagramp] = fromHex(info[Publisher::kInstagramp].get<std::string>());
        tmp[Publisher::kTelegram] = fromHex(info[Publisher::kTelegram].get<std::string>());
        tmp[Publisher::kDiscord] = fromHex(info[Publisher::kDiscord].get<std::string>());

        return tmp;
    }

    QString removeFilePrefix(QString fname)
    {
        // Some shells/systems provide incorrect count of '/' after file:
        // For example in gnome on linux one '/' is missing. So this ugly code is necessary
        if (fname.startsWith("file:"))
        {
            fname = fname.remove(0, 5);
            while (fname.startsWith("/"))
            {
                fname = fname.remove(0, 1);
            }

#ifndef WIN32
            fname = QString("/") + fname;
#endif
        }
        return fname;
    }

    int16_t compareDAppVersion(const QString& first, const QString& second)
    {
        auto fillDAppVersion = [] (QStringList& versionList) {
            while (versionList.length() < kCountDAppVersionParts)
            {
                versionList.append("0");
            }
        };

        auto firstStringList = first.split(".");
        auto secondStringList = second.split(".");
        fillDAppVersion(firstStringList);
        fillDAppVersion(secondStringList);

        for (uint8_t i = 0; i < firstStringList.length(); ++i)
        {
            if (firstStringList[i] == secondStringList[i])
            {
                continue;
            }
            
            return firstStringList[i].toInt() > secondStringList[i].toInt() ? 1 : -1;
        }
        return 0;
    }

    QString converToString(beamui::applications::AppsViewModel::Category category)
    {
        switch (category)
        {
        case beamui::applications::AppsViewModel::Category::Other:
            return "Other";
        case beamui::applications::AppsViewModel::Category::Finance:
            return "Finance";
        case beamui::applications::AppsViewModel::Category::Games:
            return "Games";
        case beamui::applications::AppsViewModel::Category::Technology:
            return "Technology";
        case beamui::applications::AppsViewModel::Category::Governance:
            return "Governance";
        default:
            return "";
        }
    }

    QString getCategoryColor(beamui::applications::AppsViewModel::Category category)
    {
        switch (category)
        {
        case beamui::applications::AppsViewModel::Category::Other:
            return "#FFB13D";
        case beamui::applications::AppsViewModel::Category::Finance:
            return "#0BCCF7";
        case beamui::applications::AppsViewModel::Category::Games:
            return "#B29FFF";
        case beamui::applications::AppsViewModel::Category::Technology:
            return "#FF57BF";
        case beamui::applications::AppsViewModel::Category::Governance:
            return "#C5FF7A";
        default:
            return "#FF57BF";
        }
    }

    bool apiSupported(const QString& apiVersion)
    {
        return beam::wallet::IWalletApi::ValidateAPIVersion(apiVersion.toStdString());
    }

    bool isAppSupported(const QVariantMap& app)
    {
        return apiSupported(app.contains(DApp::kApiVersion) ? app[DApp::kApiVersion].toString() : "current") ||
            apiSupported(app.contains(DApp::kMinApiVersion) ? app[DApp::kMinApiVersion].toString() : "");
    }

    void checkDAppFieldSize(const QVariantMap& app)
    {
        if (app.contains(DApp::kName) && app[DApp::kName].toString().size() > DApp::kNameMaxSize)
        {
            std::stringstream stream;
            stream << "'"<< DApp::kName << "' must be less than " << DApp::kNameMaxSize << " characters";
            throw std::runtime_error(stream.str());
        }

        if (app.contains(DApp::kDescription) && app[DApp::kDescription].toString().size() > DApp::kDescriptionMaxSize)
        {
            std::stringstream stream;
            stream << "'" << DApp::kDescription << "' must be less than " << DApp::kDescriptionMaxSize << " characters";
            throw std::runtime_error(stream.str());
        }

        if (app.contains(DApp::kApiVersion) && app[DApp::kApiVersion].toString().size() > DApp::kApiVersionMaxSize)
        {
            std::stringstream stream;
            stream << "'" << DApp::kApiVersion << "' must be less than " << DApp::kApiVersionMaxSize << " characters";
            throw std::runtime_error(stream.str());
        }

        if (app.contains(DApp::kMinApiVersion) && app[DApp::kMinApiVersion].toString().size() > DApp::kApiVersionMaxSize)
        {
            std::stringstream stream;
            stream << "'" << DApp::kMinApiVersion << "' must be less than " << DApp::kApiVersionMaxSize << " characters";
            throw std::runtime_error(stream.str());
        }

        if (app.contains(DApp::kIcon) && app[DApp::kIcon].toString().size() > DApp::kIconMaxSize)
        {
            std::stringstream stream;
            stream << "'" << DApp::kIcon << "' must be less than " << DApp::kIconMaxSize << " bytes";
            throw std::runtime_error(stream.str());
        }
    }

    void checkVersion(const QString& version, uint8_t count = kCountApiVersionParts)
    {
        QStringList list = version.split(".");

        if (list.length() > static_cast<int>(count))
        {
            throw std::runtime_error("Invalid version");
        }

        for (const auto& item : list)
        {
            bool ok;
            item.toLongLong(&ok);

            if (!ok)
            {
                throw std::runtime_error("Invalid version");
            }
        }
    }
}

namespace beamui::applications
{
    AppsViewModel::AppsViewModel()
        : m_walletModel(AppModel::getInstance().getWalletModel())
    {
        LOG_INFO() << "AppsViewModel created";
    }

    AppsViewModel::~AppsViewModel()
    {
        if (_server)
        {
            _server.reset();
        }
        LOG_INFO() << "AppsViewModel destroyed";
    }

    void AppsViewModel::init(bool runApp)
    {
        auto defaultProfile = QWebEngineProfile::defaultProfile();
        defaultProfile->setHttpCacheType(QWebEngineProfile::HttpCacheType::DiskHttpCache);
        defaultProfile->setPersistentCookiesPolicy(QWebEngineProfile::PersistentCookiesPolicy::AllowPersistentCookies);
        defaultProfile->setCachePath(AppSettings().getAppsCachePath());
        defaultProfile->setPersistentStoragePath(AppSettings().getAppsStoragePath());

        _userAgent = defaultProfile->httpUserAgent() + " BEAM/" + QString::fromStdString(PROJECT_VERSION);
        _serverAddr = QString("127.0.0.1:") + QString::number(AppSettings().getAppsServerPort());

        if (runApp)
        {
            _runApp = true;
            loadLocalApps();
            loadAppsFromServer();
        }
        else
        {
            _runApp = false;
            connect(m_walletModel.get(), &WalletModel::transactionsChanged, this, &AppsViewModel::onTransactionsChanged);
            connect(m_walletModel.get(), &WalletModel::walletStatusChanged, this, &AppsViewModel::loadPublishers);
            connect(m_walletModel.get(), &WalletModel::walletStatusChanged, this, &AppsViewModel::loadApps);
            // update the application info because the list of tracked publishers has changed
            connect(this, &AppsViewModel::userPublishersChanged, this, &AppsViewModel::onUserPublishersChanged);

#ifdef BEAM_IPFS_SUPPORT
            connect(m_walletModel.get(), &WalletModel::IPFSStatusChanged, this, &AppsViewModel::onIPFSStatus);
            m_walletModel->getAsync()->getIPFSStatus();
#endif

            loadMyPublisherInfo();
            loadUserPublishers();

            loadPublishers();
            loadApps();
        }
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

    QVariantMap AppsViewModel::parseAppManifest(QTextStream& in, const QString& appFolder, bool needExpandIcon)
    {
        QVariantMap app;

        const auto content = in.readAll();
        if (content.isEmpty())
        {
            throw std::runtime_error("Failed to read the manifest file");
        }

        const auto utf = content.toUtf8();

        try
        {
            // do not make json const or it will throw on missing keys
            auto json = nlohmann::json::parse(utf.begin(), utf.end());
            if (!json.is_object() || json.empty())
            {
                throw std::runtime_error("Invalid manifest file");
            }

            const auto& guid = json[DApp::kGuid];
            if (!guid.is_string() || guid.empty())
            {
                throw std::runtime_error("Invalid GUID in the manifest file");
            }
            app.insert(DApp::kGuid, QString::fromStdString(guid.get<std::string>()));

            const auto& desc = json[DApp::kDescription];
            if (!desc.is_string() || desc.empty())
            {
                throw std::runtime_error("Invalid description in the manifest file");
            }
            app.insert(DApp::kDescription, QString::fromStdString(desc.get<std::string>()));

            const auto& name = json[DApp::kName];
            if (!name.is_string() || name.empty())
            {
                throw std::runtime_error("Invalid app name in the manifest file");
            }

            const auto sname = name.get<std::string>();
            app.insert(DApp::kName, QString::fromStdString(sname));

            const auto& url = json[DApp::kUrl];
            if (!url.is_string() || url.empty())
            {
                throw std::runtime_error("Invalid url in the manifest file");
            }

            const auto surl = url.get<std::string>();
            app.insert(DApp::kUrl, expandLocalUrl(appFolder, surl));

            const auto& icon = json[DApp::kIcon];
            if (!icon.empty())
            {
                if (!icon.is_string())
                {
                    throw std::runtime_error("Invalid icon in the manifest file");
                }
                QString ipath = "";

                if (needExpandIcon)
                {
                    ipath = expandLocalFile(appFolder, icon.get<std::string>());
                }
                else
                {
                    ipath = QString::fromStdString(json[DApp::kIcon].get<std::string>());

                    if (ipath.startsWith(kLocalapp))
                    {
                        // remove "localapp/" substring
                        ipath = ipath.remove(0, kLocalapp.size() + 1);
                    }
                }

                app.insert(DApp::kIcon, ipath);

                LOG_INFO() << "App: " << sname << ", icon: " << ipath.toStdString();
            }

            const auto& av = json[DApp::kApiVersion];
            if (!av.empty())
            {
                if (!av.is_string())
                {
                    throw std::runtime_error("Invalid api_version in the manifest file");
                }
                QString version = QString::fromStdString(av.get<std::string>());

                checkVersion(version);
                app.insert(DApp::kApiVersion, version);
            }

            const auto& mav = json[DApp::kMinApiVersion];
            if (!mav.empty())
            {
                if (!mav.is_string())
                {
                    throw std::runtime_error("Invalid min_api_version in the manifest file");
                }
                QString version = QString::fromStdString(mav.get<std::string>());

                checkVersion(version);
                app.insert(DApp::kMinApiVersion, version);
            }

            const auto& v = json[DApp::kVersion];
            if (!v.empty())
            {
                if (!v.is_string())
                {
                    throw std::runtime_error("Invalid version in the manifest file");
                }
                QString version = QString::fromStdString(v.get<std::string>());

                checkVersion(version, kCountDAppVersionParts);
                app.insert(DApp::kVersion, version);
            }

            const auto& categoryObj = json[DApp::kCategory];
            if (!categoryObj.empty())
            {
                if (!categoryObj.is_number_unsigned())
                {
                    throw std::runtime_error("Invalid category in the manifest file");
                }
                app.insert(DApp::kCategory, categoryObj.get<uint32_t>());

                Category category = static_cast<Category>(categoryObj.get<uint32_t>());
                app.insert(DApp::kCategoryName, converToString(category));
                app.insert(DApp::kCategoryColor, getCategoryColor(category));
            }

            const auto& publisherObj = json[DApp::kPublisherKey];
            if (!publisherObj.empty())
            {
                if (!publisherObj.is_string())
                {
                    throw std::runtime_error("Invalid publisher in the manifest file");
                }
                app.insert(DApp::kPublisherKey, QString::fromStdString(publisherObj.get<std::string>()));
            }

            app.insert(DApp::kLocal, true);
            // TODO: check why we used surl instead of extended url - app["url"]
            const auto appid = beam::wallet::GenerateAppID(sname, app[DApp::kUrl].toString().toStdString());
            app.insert(DApp::kAppid, QString::fromStdString(appid));
        }
        catch (const std::exception& ex)
        {
            LOG_ERROR() << "Invalid manifest file. exception: " << ex.what();
            throw std::runtime_error("Invalid manifest file.");
        }

        return app;
    }

    void AppsViewModel::loadApps()
    {
        loadLocalApps();
        loadDevApps();
        loadAppsFromServer();
        loadAppsFromStore();
    }

    void AppsViewModel::loadLocalApps()
    {
        QList<QVariantMap> result;
        QDir appsdir(AppSettings().getLocalAppsPath());
        auto list = appsdir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

        for (const auto& finfo : list)
        {
            const auto fullFolder = finfo.absoluteFilePath();
            const auto justFolder = finfo.fileName();
            auto mpath = QDir(fullFolder).absoluteFilePath(kManifestFile);

            try
            {
                QFile file(mpath);
                if (!file.open(QFile::ReadOnly | QFile::Text))
                {
                    throw std::runtime_error("Cannot open file");
                }

                QTextStream in(&file);

                auto app = parseAppManifest(in, justFolder);
                app.insert(DApp::kFullPath, fullFolder);
                app.insert(DApp::kSupported, isAppSupported(app));

                result.push_back(app);
            }
            catch (std::runtime_error& err)
            {
                LOG_ERROR() << "Error while reading local app from " << mpath.toStdString() << ", " << err.what();
            }
        }

        _localApps = result;

        if (!_runApp)
            emit appsChanged();
    }

    void AppsViewModel::loadDevApps()
    {
        QList<QVariantMap> result;

        if (!AppSettings().getDevAppName().isEmpty())
        {
            QVariantMap devapp;

            const auto name = AppSettings().getDevAppName();
            const auto url = AppSettings().getDevAppUrl();
            const auto appid = QString::fromStdString(beam::wallet::GenerateAppID(name.toStdString(), url.toStdString()));

            //% "This is your dev application"
            devapp.insert(DApp::kDescription, qtTrId("apps-devapp"));
            devapp.insert(DApp::kName, name);
            devapp.insert(DApp::kUrl, url);
            devapp.insert(DApp::kApiVersion, AppSettings().getDevAppApiVer());
            devapp.insert(DApp::kMinApiVersion, AppSettings().getDevAppMinApiVer());
            devapp.insert(DApp::kAppid, appid);
            devapp.insert(DApp::kSupported, true);
            devapp.insert(DApp::kDevApp, true);
            result.push_back(devapp);
        }

        if (_devApps != result)
        {
            _devApps = result;
            emit appsChanged();
        }
    }

    void AppsViewModel::loadAppsFromServer()
    {
        // load apps from server
        QPointer<AppsViewModel> guard(this);
        AppModel::getInstance().getWalletModel()->getAsync()->getAppsList(
            [this, guard](bool isOk, const std::string& response)
            {
                if (!guard)
                {
                    return;
                }

                QList<QVariantMap> result;

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
                        QVariantMap app;
                        auto name = parseStringField(item.value(), DApp::kName);
                        auto url = parseStringField(item.value(), DApp::kUrl);
                        const auto appid = beam::wallet::GenerateAppID(name.toStdString(), url.toStdString());

                        app.insert(DApp::kAppid, QString::fromStdString(appid));
                        app.insert(DApp::kDescription, parseStringField(item.value(), DApp::kDescription));
                        app.insert(DApp::kName, name);
                        app.insert(DApp::kUrl, url);
                        app.insert(DApp::kIcon, parseStringField(item.value(), DApp::kIcon));
                        app.insert(DApp::kPublisherName, kBeamPublisherName);
                        app.insert(DApp::kPublisherKey, kBeamPublisherKey);
                        app.insert(DApp::kSupported, isAppSupported(app));
                        app.insert(DApp::kFromServer, true);

                        result.push_back(app);
                    }
                }
                catch (const std::runtime_error& err)
                {
                    // TODO: mb need to transfer the error to QML(errorMessage)
                    LOG_ERROR() << "Failed to load remote applications list, " << err.what();
                }

                if (_runApp || result != _remoteApps)
                {
                    _remoteApps = result;
                    emit appsChanged();
                }
            });
    }

    void AppsViewModel::loadAppsFromStore()
    {
        ContractArgs args(Actions::kViewDapps);

        QPointer<AppsViewModel> guard(this);

        AppModel::getInstance().getWalletModel()->getAsync()->callShaderAndStartTx(AppSettings().getDappStorePath(), args.args(),
            [this, guard](const std::string& err, const std::string& output, const beam::wallet::TxID& id)
            {
                if (!guard)
                {
                    return;
                }

                if (!err.empty())
                {
                    LOG_ERROR() << "Failed to load dapps list from DApp Store" << ", " << err;
                    return;
                }

                try
                {
                    auto json = nlohmann::json::parse(output);

                    if (json.empty() || !json.is_object() || !json["dapps"].is_array())
                    {
                        throw std::runtime_error("Invalid response of the view_dapps method");
                    }

                    QList<QVariantMap> result;
                    for (auto& item : json["dapps"].items())
                    {
                        try
                        {
                            if (!item.value().is_object())
                            {
                                throw std::runtime_error("Invalid body of the dapp " + item.key());
                            }
                            auto guid = parseStringField(item.value(), DApp::kId);
                            auto publisherKey = parseStringField(item.value(), DApp::kPublisherKey);

                            // parse DApps only of the user publishers + own
                            if (!_userPublishersKeys.contains(publisherKey, Qt::CaseInsensitive) &&
                                !(isPublisher() && publisherKey.compare(_publisherInfo[Publisher::kPubkey].toString(), Qt::CaseInsensitive) == 0))
                            {
                                continue;
                            }

                            const auto idx = std::find_if(_publishers.cbegin(), _publishers.cend(),
                                [publisherKey](const auto& publisher) -> bool {
                                    return !publisher[Publisher::kPubkey].toString().compare(publisherKey, Qt::CaseInsensitive);
                                }
                            );

                            QString publisherName = "";

                            if (idx != _publishers.cend())
                            {
                                publisherName = (*idx)[Publisher::kName].toString();
                            }

                            LOG_DEBUG() << "Parsing DApp from contract, guid - " << guid.toStdString() << ", publisher - " << publisherKey.toStdString();

                            // parse version
                            auto versionObj = item.value()[DApp::kVersion];

                            if (versionObj.empty() || !versionObj.is_object())
                            {
                                throw std::runtime_error("Invalid 'version' of the dapp");
                            }

                            auto majorObj = versionObj[DApp::kMajor];
                            auto minorObj = versionObj[DApp::kMinor];
                            auto releaseObj = versionObj[DApp::kRelease];
                            auto buildObj = versionObj[DApp::kBuild];
                            if (majorObj.empty() || !majorObj.is_number_unsigned() ||
                                minorObj.empty() || !minorObj.is_number_unsigned() ||
                                releaseObj.empty() || !releaseObj.is_number_unsigned() ||
                                buildObj.empty() || !buildObj.is_number_unsigned())
                            {
                                throw std::runtime_error("Invalid 'version' of the dapp");
                            }

                            QString version;
                            QTextStream textStream(&version);
                            textStream << majorObj.get<uint32_t>() << '.' << minorObj.get<uint32_t>() << '.'
                                << releaseObj.get<uint32_t>() << '.' << buildObj.get<uint32_t>();

                            QMap<QString, QVariant> app;
                            app.insert(DApp::kDescription, decodeStringField(item.value(), DApp::kDescription));
                            app.insert(DApp::kName, decodeStringField(item.value(), DApp::kName));
                            app.insert(DApp::kIpfsId, parseStringField(item.value(), DApp::kIpfsId));
                            app.insert(DApp::kUrl, "");
                            app.insert(DApp::kApiVersion, decodeStringField(item.value(), DApp::kApiVersion));
                            app.insert(DApp::kMinApiVersion, decodeStringField(item.value(), DApp::kMinApiVersion));
                            app.insert(DApp::kGuid, guid);
                            app.insert(DApp::kPublisherKey, publisherKey);
                            app.insert(DApp::kPublisherName, publisherName);
                            app.insert(DApp::kVersion, version);

                            Category category = static_cast<Category>(item.value()[DApp::kCategory].get<int>());
                            app.insert(DApp::kCategory, item.value()[DApp::kCategory].get<int>());
                            app.insert(DApp::kCategoryName, converToString(category));
                            app.insert(DApp::kCategoryColor, getCategoryColor(category));
                            app.insert(DApp::kIcon, decodeStringField(item.value(), DApp::kIcon));
                            app.insert(DApp::kSupported, isAppSupported(app));
                            app.insert(DApp::kNotInstalled, true);

                            result.push_back(app);
                        }
                        catch (std::runtime_error& err)
                        {
                            LOG_ERROR() << "Error while parsing app from contract" << ", " << err.what();
                        }
                    }

                    if (result != _shaderApps)
                    {
                        _shaderApps = result;
                        unpinDeletedDApps();

                        emit appsChanged();
                    }
                }
                catch (std::runtime_error& err)
                {
                    LOG_ERROR() << "Error while parsing app from contract" << ", " << err.what();
                }
            }
        );
    }

    void AppsViewModel::loadPublishers()
    {
        ContractArgs args(Actions::kViewPublishers);

        QPointer<AppsViewModel> guard(this);

        AppModel::getInstance().getWalletModel()->getAsync()->callShaderAndStartTx(AppSettings().getDappStorePath(), args.args(),
            [this, guard](const std::string& err, const std::string& output, const beam::wallet::TxID& id)
            {
                if (!guard)
                {
                    return;
                }

                if (!err.empty())
                {
                    LOG_ERROR() << "Failed to load publishers list" << ", " << err;
                    return;
                }

                try
                {
                    auto json = nlohmann::json::parse(output);

                    if (json.empty() || !json.is_object() || !json["publishers"].is_array())
                    {
                        throw std::runtime_error("Invalid response of the view_publishers method");
                    }

                    QList<QVariantMap> publishers;

                    for (auto& item : json["publishers"].items())
                    {
                        if (!item.value().is_object())
                        {
                            throw std::runtime_error("Invalid body of the publishers list " + item.key());
                        }

                        publishers.push_back(parsePublisherInfo(item.value()));
                    }

                    setPublishers(publishers);
                }
                catch (std::runtime_error& err)
                {
                    LOG_ERROR() << "Error while parsing publisher from contract" << ", " << err.what();
                }
            }
        );
    }

    void AppsViewModel::loadUserPublishers()
    {
        _userPublishersKeys = AppSettings().getDappStoreUserPublishers();
    }

    void AppsViewModel::loadMyPublisherInfo(bool hideTxIsSentDialog, bool showYouArePublsherDialog)
    {
        ContractArgs args(Actions::kMyPublisherInfo);
        QPointer<AppsViewModel> guard(this);

        AppModel::getInstance().getWalletModel()->getAsync()->callShaderAndStartTx(AppSettings().getDappStorePath(), args.args(),
            [this, guard, hideTxIsSentDialog, showYouArePublsherDialog](const std::string& err, const std::string& output, const beam::wallet::TxID& id)
            {
                if (!guard)
                {
                    return;
                }

                if (!err.empty())
                {
                    LOG_ERROR() << "Failed to load my publisher info" << ", " << err;
                    return;
                }
                else
                {
                    try
                    {
                        auto json = nlohmann::json::parse(output);

                        if (json.empty() || !json.is_object() || !json["my_publisher_info"].is_object())
                        {
                            throw std::runtime_error("Invalid response of the view_publishers method");
                        }

                        if (!json["my_publisher_info"].empty())
                        {
                            auto& info = json["my_publisher_info"];
                            QVariantMap tmp = parsePublisherInfo(info);

                            setPublisherInfo(tmp);
                        }
                    }
                    catch (std::runtime_error& err)
                    {
                        LOG_ERROR() << "Error while parsing publisher from contract" << ", " << err.what();
                    }
                }

                if (hideTxIsSentDialog)
                {
                    emit hideTxIsSent();
                }

                if (showYouArePublsherDialog)
                {
                    emit showYouArePublisher();
                }
            }
        );
    }

    void AppsViewModel::setPublishers(const QList<QVariantMap>& value)
    {
        if (value != _publishers)
        {
            _publishers = value;

            emit userPublishersChanged();
        }
    }

    QList<QVariantMap> AppsViewModel::getUserPublishers()
    {
        QList<QVariantMap> userPublishers;

        std::copy_if(_publishers.cbegin(), _publishers.cend(), std::back_inserter(userPublishers),
            [this](const auto& publisher) -> bool {
                return _userPublishersKeys.contains(publisher[Publisher::kPubkey].toString(), Qt::CaseInsensitive);
            }
        );

        return userPublishers;
    }

    QList<QVariantMap> AppsViewModel::getApps()
    {
        // Apps order: Dev APP, remote apps, *.dapp files, installed from shader, not installed from shader
        QList<QVariantMap> result = _devApps + _remoteApps;
        QList<QVariantMap> notInstalled, installed;

        for (const auto& app : _shaderApps)
        {
            const auto it = std::find_if(_localApps.cbegin(), _localApps.cend(),
                [guid = app[DApp::kGuid]](const auto& tmp) -> bool {
                    const auto appIt = tmp.find(DApp::kGuid);
                    if (appIt == tmp.cend())
                    {
                        return false;
                    }
                    return appIt->toString() == guid;
                }
            );

            if (it != _localApps.cend())
            {
                auto tmp = *it;
                if (compareDAppVersion(app[DApp::kVersion].toString(), tmp[DApp::kVersion].toString()) > 0)
                {
                    tmp.insert(DApp::kHasUpdate, true);
                }
                tmp.insert(DApp::kIpfsId, app[DApp::kIpfsId]);
                tmp.insert(DApp::kPublisherKey, app[DApp::kPublisherKey]);
                tmp.insert(DApp::kPublisherName, app[DApp::kPublisherName]);
                installed.push_back(tmp);
            }
            else
            {
                notInstalled.push_back(app);
            }
        }

        // installed from dapp file
        for (const auto& app : _localApps)
        {
            const auto it = std::find_if(installed.cbegin(), installed.cend(),
                [guid = app[DApp::kGuid]](const auto& tmp) -> bool {
                    const auto appIt = tmp.find(DApp::kGuid);
                    if (appIt == tmp.cend())
                    {
                        return false;
                    }
                    return appIt->toString() == guid;
                }
            );

            if (it == installed.cend())
            {
                result.push_back(app);
            }
        }

        // installed and not installed from dapp store 
        if (_publisherInfo.empty())
        {
            result += installed + notInstalled;
        }
        else
        {
            for (const auto& app : installed)
            {
                if (app[DApp::kPublisherKey] == _publisherInfo[Publisher::kPubkey])
                {
                    result.push_back(app);
                }
            }
            for (const auto& app : notInstalled)
            {
                if (app[DApp::kPublisherKey] == _publisherInfo[Publisher::kPubkey])
                {
                    result.push_back(app);
                }
            }

            for (const auto& app : installed)
            {
                if (app[DApp::kPublisherKey] != _publisherInfo[Publisher::kPubkey])
                {
                    result.push_back(app);
                }
            }
            for (const auto& app : notInstalled)
            {
                if (app[DApp::kPublisherKey] != _publisherInfo[Publisher::kPubkey])
                {
                    result.push_back(app);
                }
            }
        }

        return result;
    }

    bool AppsViewModel::isPublisher() const
    {
        return !_publisherInfo.empty();
    }

    QVariantMap AppsViewModel::getPublisherInfo() const
    {
        return _publisherInfo;
    }

    void AppsViewModel::setPublisherInfo(const QVariantMap& value)
    {
        if (value != _publisherInfo)
        {
            _publisherInfo = value;
            emit publisherInfoChanged();
            emit isPublisherChanged();
        }
    }

    QString AppsViewModel::addPublisherByKey(const QString& publisherKey)
    {
        // find publisher in _publishers by publicKey
        const auto it = std::find_if(_publishers.cbegin(), _publishers.cend(),
            [publisherKey] (const auto& publisher) -> bool {
                const auto publisherIt = publisher.find(Publisher::kPubkey);
                if (publisherIt == publisher.cend())
                {
                    assert(false);
                    return false;
                }
                return publisherIt->toString() == publisherKey;
            }
        );

        if (it == _publishers.cend())
        {
            return {};
        }

        if (!_userPublishersKeys.contains(publisherKey, Qt::CaseInsensitive))
        {
            _userPublishersKeys.append(publisherKey);
            AppSettings().setDappStoreUserPublishers(_userPublishersKeys);

            emit userPublishersChanged();
        }

        return (*it)[Publisher::kName].toString();
    }

    void AppsViewModel::removePublisherByKey(const QString& publisherKey)
    {
        if (_userPublishersKeys.removeOne(publisherKey))
        {
            AppSettings().setDappStoreUserPublishers(_userPublishersKeys);
            
            emit userPublishersChanged();
        }
    }

    void AppsViewModel::changePublisherInfo(const QVariantMap& publisherInfo, bool isCreating)
    {
        ContractArgs args(isCreating ? Actions::kAddPublisher : Actions::kUpdatePublisher);
        args.append(Publisher::kName, toHex(publisherInfo[Publisher::kName].toString()));
        args.append(Publisher::kShortTitle, toHex(publisherInfo[Publisher::kShortTitle].toString()));
        args.append(Publisher::kAboutMe, toHex(publisherInfo[Publisher::kAboutMe].toString()));
        args.append(Publisher::kWebsite, toHex(publisherInfo[Publisher::kWebsite].toString()));
        args.append(Publisher::kTwitter, toHex(publisherInfo[Publisher::kTwitter].toString()));
        args.append(Publisher::kLinkedin, toHex(publisherInfo[Publisher::kLinkedin].toString()));
        args.append(Publisher::kInstagramp, toHex(publisherInfo[Publisher::kInstagramp].toString()));
        args.append(Publisher::kTelegram, toHex(publisherInfo[Publisher::kTelegram].toString()));
        args.append(Publisher::kDiscord, toHex(publisherInfo[Publisher::kDiscord].toString()));

        QPointer<AppsViewModel> guard(this);

        AppModel::getInstance().getWalletModel()->getAsync()->callShader(AppSettings().getDappStorePath(), args.args(),
            [this, guard, isCreating](const std::string& err, const std::string& output, const beam::ByteBuffer& data)
            {
                if (!guard)
                {
                    return;
                }

                if (!err.empty())
                {
                    LOG_ERROR() << (isCreating ? "Failed to create a publisher" : "Failed to change a publisher info") 
                        << ", " << err;

                    isCreating ? emit publisherCreateFail() : emit publisherEditFail();
                    return;
                }

                handleShaderTxData(isCreating ? Action::CreatePublisher : Action::UpdatePublisher, data);
            }
        );
    }

    bool AppsViewModel::uninstallLocalApp(const QString& appid)
    {
        const auto it = std::find_if(_localApps.cbegin(), _localApps.cend(), [appid](const auto& props) -> bool {
            const auto ait = props.find(DApp::kAppid);
            if (ait == props.cend())
            {
                assert(false);
                return false;
            }
            return ait->toString() == appid;
        });

        if (it == _localApps.cend())
        {
            assert(false);
            return false;
        }

        const auto pathit = it->find(DApp::kFullPath);
        if (pathit == it->cend())
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
        bool result = dir.removeRecursively();
        if (result)
        {
            // refresh
            loadApps();
        }
        return result;
    }

    QString AppsViewModel::expandLocalUrl(const QString& folder, const std::string& url) const
    {
        QString result = QString::fromStdString(url);
        result.replace(kLocalapp, QString("http://") + _serverAddr + "/" + folder);
        return result;
    }

    QString AppsViewModel::expandLocalFile(const QString& folder, const std::string& url) const
    {
        auto path = QDir(AppSettings().getLocalAppsPath()).filePath(folder);
        auto result = QString::fromStdString(url);
        result.replace(kLocalapp, QString("file:///") + path);
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
                LOG_ERROR() << "Failed to launch local apps server: " << err.what();
            }
        }
    }

    QString AppsViewModel::chooseFile(const QString& title)
    {
        QFileDialog dialog(nullptr,
                           title,
                           "",
                           "BEAM DApp files (*.dapp)");

        dialog.setWindowModality(Qt::WindowModality::ApplicationModal);
        if (!dialog.exec())
        {
            return "";
        }
        return dialog.selectedFiles().value(0);
    }

    QVariantMap AppsViewModel::getDAppFileProperties(const QString& fname)
    {
        QFileInfo fileInfo(fname);

        QVariantMap properties;
        properties.insert(DApp::kName, fileInfo.fileName());
        properties.insert("size", fileInfo.size());

        return properties;
    }

    QVariantMap AppsViewModel::parseDAppFile(const QString& fname)
    {
        try
        {
            auto dappFilePath = removeFilePrefix(fname);
            QuaZip zip(dappFilePath);
            if (!zip.open(QuaZip::Mode::mdUnzip))
            {
                throw std::runtime_error("Failed to open the DApp file");
            }

            QVariantMap app;
            for (bool ok = zip.goToFirstFile(); ok; ok = zip.goToNextFile())
            {
                const auto zipFname = zip.getCurrentFileName();
                if (zipFname == kManifestFile)
                {
                    QuaZipFile mfile(zip.getZipName(), zipFname);
                    if (!mfile.open(QIODevice::ReadOnly))
                    {
                        throw std::runtime_error("Failed to read the DApp file");
                    }

                    QTextStream in(&mfile);
                    app = parseAppManifest(in, "", false);
                }
            }

            // TODO roman.strilets it's temporary solution
            for (bool ok = zip.goToFirstFile(); ok; ok = zip.goToNextFile())
            {
                const auto zipFname = zip.getCurrentFileName();
                if (zipFname == app[DApp::kIcon].value<QString>())
                {
                    QuaZipFile mfile(zip.getZipName(), zipFname);
                    if (!mfile.open(QIODevice::ReadOnly))
                    {
                        throw std::runtime_error("Failed to read the DApp file");
                    }
                    QTextStream in(&mfile);
                    const auto content = in.readAll();
                    app[DApp::kIcon] = "data:image/svg+xml;utf8," + content;
                }
            }

            checkDAppFieldSize(app);

            if (app[DApp::kGuid].value<QString>().isEmpty())
            {
                throw std::runtime_error("Invalid DApp file");
            }
            
            // add estimated release_date
            app.insert(DApp::kReleaseDate, QLocale(QLocale::English).toString(QDate::currentDate(), "dd MMM yyyy"));

            app.insert(DApp::kSupported, isAppSupported(app));

            // preload DApp file
            QFile dappFile(dappFilePath);

            if (!dappFile.open(QFile::ReadOnly))
            {
                throw std::runtime_error("Failed to read the DApp file");
            }

            auto buffer = dappFile.readAll();
            _loadedDAppBuffer = beam::ByteBuffer(buffer.cbegin(), buffer.cend());
            _loadedDApp = app;

            return app;
        }
        catch (std::runtime_error& err)
        {
            LOG_ERROR() << "Failed to parse DApp: " << err.what();
        }
        return {};
    }

    void AppsViewModel::publishDApp(bool isUpdating)
    {
#ifdef BEAM_IPFS_SUPPORT
        auto ipfs = AppModel::getInstance().getWalletModel()->getIPFS();
        QPointer<AppsViewModel> guard(this);

        if (!_loadedDApp.has_value() || !_loadedDAppBuffer.has_value())
        {
            assert(false);
            LOG_ERROR() << "Failed to publish DApp, empty buffers.";

            emit appPublishFail();
            return;
        }

        ipfs->AnyThread_add(std::move(*_loadedDAppBuffer), true, 0,
            [this, guard, app = std::move(*_loadedDApp), isUpdating](std::string&& ipfsID) mutable
            {
                if (!guard)
                {
                    return;
                }

                LOG_INFO() << "IPFS_ID: " << ipfsID;

                // save result to contract
                uploadAppToStore(std::move(app), ipfsID, isUpdating);
            },
            [this](std::string&& err) {
                LOG_ERROR() << "Failed to add to ipfs: " << err;
                emit appPublishFail();
            }
        );
#endif // BEAM_IPFS_SUPPORT
    }

    bool AppsViewModel::checkDAppNewVersion(const QVariantMap& currentDApp, const QVariantMap& newDApp)
    {
        if (currentDApp[DApp::kGuid].value<QString>() == newDApp[DApp::kGuid].value<QString>())
        {
            auto result = compareDAppVersion(newDApp[DApp::kVersion].value<QString>(), currentDApp[DApp::kVersion].value<QString>()) > 0;
            if (!result)
            {
                LOG_ERROR() << "checkDAppNewVersion: New DApp has the wrong version. The version of the new DApp must be larger than the current version.";
            }
            return result;
        }
        LOG_ERROR() << "checkDAppNewVersion: Guid doesn't match, it's different DApps.";
        return false;
    }

    void AppsViewModel::uploadAppToStore(QVariantMap&& app, const std::string& ipfsID, bool isUpdating)
    {
        QString guid = app[DApp::kGuid].value<QString>();
        QString appName = app[DApp::kName].value<QString>();
        QString description = app[DApp::kDescription].value<QString>();

        ContractArgs args(isUpdating ? Actions::kUpdateDapp : Actions::kAddDapp);
        args.append(DApp::kIpfsId, ipfsID);
        args.append(DApp::kName, toHex(appName));
        args.append(DApp::kId, guid.toStdString());
        args.append(DApp::kDescription, toHex(description));
        args.append(DApp::kApiVersion, toHex(app[DApp::kApiVersion].value<QString>()));
        args.append(DApp::kMinApiVersion, toHex(app[DApp::kMinApiVersion].value<QString>()));
        args.append(DApp::kCategory, app[DApp::kCategory].value<uint32_t>());
        args.append(DApp::kIcon, toHex(app[DApp::kIcon].value<QString>()));

        // parse version
        QStringList version = app[DApp::kVersion].value<QString>().split(".");

        for (; version.length() < kCountDAppVersionParts;)
        {
            version.append("0");
        }

        args.append(DApp::kMajor, version[0].toStdString());
        args.append(DApp::kMinor, version[1].toStdString());
        args.append(DApp::kRelease, version[2].toStdString());
        args.append(DApp::kBuild, version[3].toStdString());

        QPointer<AppsViewModel> guard(this);

        LOG_INFO() << "args: " << args.args();

        AppModel::getInstance().getWalletModel()->getAsync()->callShader(AppSettings().getDappStorePath(), args.args(),
            [this, guard](const std::string& err, const std::string& output, const beam::ByteBuffer& data)
            {
                if (!guard)
                {
                    return;
                }

                if (!err.empty())
                {
                    LOG_ERROR() << "Failed to publish app" << ", " << err;

                    emit appPublishFail();
                    return;
                }
                
                if (data.empty())
                {
                    LOG_ERROR() << "Failed to publish app" << ", " << output;

                    emit appPublishFail();
                    return;
                }
                handleShaderTxData(Action::UploadDApp, data);
            }
        );
    }

    void AppsViewModel::installApp(const QString& guid)
    {
#ifdef BEAM_IPFS_SUPPORT
        try
        {
            const auto app = getAppByGUID(guid);
            if (app.isEmpty())
            {
                LOG_ERROR() << "Failed to find Dapp by guid " << guid.toStdString();
                return;
            }

            const auto ipfsID = app[DApp::kIpfsId].toString();
            const auto appName = app[DApp::kName].toString();

            // get dapp binary data from ipfs
            QPointer<AppsViewModel> guard(this);
            auto ipfs = AppModel::getInstance().getWalletModel()->getIPFS();

            // TODO: check timeout value
            ipfs->AnyThread_get(ipfsID.toStdString(), 0,
                [this, guard, appName, guid](beam::ByteBuffer&& data) mutable
                {
                    if (!guard)
                    {
                        return;
                    }

                    try
                    {
                        // unpack & verify & install
                        LOG_DEBUG() << "Installing DApp " << appName.toStdString() << " from ipfs";

                        QByteArray qData;
                        std::copy(data.cbegin(), data.cend(), std::back_inserter(qData));

                        QBuffer buffer(&qData);
                        
                        checkManifestFile(&buffer, appName, guid);
                        installFromBuffer(&buffer, guid);

                        emit appInstallOK(appName);
                        loadApps();
                    }
                    catch (std::runtime_error& err)
                    {
                        LOG_ERROR() << "Failed to install DApp: " << err.what();
                        emit appInstallFail(appName);
                    }
                },
                [this, guard, appName](std::string&& err)
                {
                    LOG_ERROR() << "Failed to get app from ipfs: " << err;
                    emit appInstallFail(appName);
                }
                );
        }
        catch (const std::runtime_error& err)
        {
            assert(false);
            LOG_ERROR() << "Failed to get properties for " << guid.toStdString() << ", " << err.what();

            emit appInstallFail("");
            return;
        }
#endif // BEAM_IPFS_SUPPORT
    }

    void AppsViewModel::installFromBuffer(QIODevice* ioDevice, const QString& guid)
    {
        const auto appsPath = AppSettings().getLocalAppsPath();
        const auto appFolder = QDir(appsPath).filePath(guid);

        if (QDir(appFolder).exists())
        {
            if (!QDir(appFolder).removeRecursively())
            {
                throw std::runtime_error("Failed to prepare folder");
            }
        }

        QDir(appsPath).mkdir(guid);
        if (JlCompress::extractDir(ioDevice, appFolder).isEmpty())
        {
            throw std::runtime_error("DApp Installation failed");
        }
    }

    QString AppsViewModel::installFromFile(const QString& rawFname)
    {
        try
        {
            QString fname = removeFilePrefix(rawFname);

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
                if (zipFname == kManifestFile)
                {
                    QuaZipFile mfile(zip.getZipName(), zipFname);
                    if (!mfile.open(QIODevice::ReadOnly))
                    {
                        throw std::runtime_error("Failed to read the DApp file");
                    }

                    QTextStream in(&mfile);
                    const auto app = parseAppManifest(in, "");
                    guid = app[DApp::kGuid].value<QString>();
                    appName = app[DApp::kName].value<QString>();

                    if (!isAppSupported(app))
                    {
                        throw std::runtime_error("DApp is unsupported.");
                    }
                }
            }

            if (guid.isEmpty())
            {
                throw std::runtime_error("Invalid DApp file");
            }

            if (const auto app = getAppByGUID(guid); !app.isEmpty())
            {
                throw std::runtime_error("DApp with same guid already installed!");
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

            // refresh
            loadApps();

            return appName;
        }
        catch(std::exception& err)
        {
            LOG_ERROR() << "Failed to install DApp: " << err.what();
            return "";
        }
    }

    void AppsViewModel::handleShaderTxData(Action action, const beam::ByteBuffer& data)
    {
        try
        {
            beam::bvm2::ContractInvokeData contractInvokeData;

            if (!beam::wallet::fromByteBuffer(data, contractInvokeData))
            {
                throw std::runtime_error("Failed to parse invoke data");
            }

            const auto comment = beam::bvm2::getFullComment(contractInvokeData);
            const auto fee = beam::bvm2::getFullFee(contractInvokeData, AppModel::getInstance().getWalletModel()->getCurrentHeight());
            const auto fullSpend = beam::bvm2::getFullSpend(contractInvokeData);

            if (!fullSpend.empty())
            {
                throw std::runtime_error("Unexpected fullSpend amounts");
            }

            const auto assetsManager = AppModel::getInstance().getAssets();
            const auto feeRate = beamui::AmountToUIString(assetsManager->getRate(beam::Asset::s_BeamID));
            const auto rateUnit = assetsManager->getRateUnit();
            const auto tmp = QString::fromStdString(beam::to_hex(data.data(), data.size()));
            const bool isEnough = AppModel::getInstance().getWalletModel()->getAvailable(beam::Asset::s_BeamID) > beam::AmountBig::Type(fee);

            emit shaderTxData(static_cast<int>(action), tmp, QString::fromStdString(comment),
                beamui::AmountToUIString(fee), feeRate, rateUnit, isEnough);
        }
        catch (const std::runtime_error& err)
        {
            LOG_ERROR() << "Failed to handle shader TX data: " << err.what();

            showErrorDialog(action);
        }
    }

    void AppsViewModel::contractInfoApproved(int rawAction, const QString& data)
    {
        QPointer<AppsViewModel> guard(this);
        beam::ByteBuffer buffer = beam::from_hex(data.toStdString());

        AppModel::getInstance().getWalletModel()->getAsync()->processShaderTxData(std::move(buffer),
            [this, guard, rawAction](const std::string& err, const beam::wallet::TxID& id)
            {
                if (!guard)
                {
                    return;
                }

                Action action = static_cast<Action>(rawAction);

                if (!err.empty())
                {
                    LOG_ERROR() << "Failed to process shader TX: " << ", " << err;

                    showErrorDialog(action);

                    return;
                }

                if (_activeTx.find(id) != _activeTx.end())
                {
                    LOG_ERROR() << "There is already such a transaction id = " << id;
                    return;
                }

                _activeTx[id] = action;

                if (Action::CreatePublisher == action || Action::UpdatePublisher == action)
                {
                    emit showTxIsSent();
                }
            }
        );
    }

    void AppsViewModel::contractInfoRejected()
    {
    }

    void AppsViewModel::onTransactionsChanged(
        beam::wallet::ChangeAction changeAction,
        const std::vector<beam::wallet::TxDescription>& transactions)
    {
        for (auto& tx : transactions)
        {
            if (tx.GetTxID())
            {
                auto txId = *tx.GetTxID();
                if (_activeTx.find(txId) != _activeTx.end())
                {
                    LOG_DEBUG() << "onTransactionsChanged: changeAction = " << static_cast<int>(changeAction) << ", status = " << static_cast<int>(tx.m_status);

                    Action action = _activeTx[txId];

                    if (action == Action::CreatePublisher || action == Action::UpdatePublisher)
                    {
                        if (changeAction == beam::wallet::ChangeAction::Updated && tx.m_status == beam::wallet::TxStatus::Completed)
                        {
                            loadMyPublisherInfo(true, action == Action::CreatePublisher);
                            _activeTx.erase(txId);
                        }
                        else if ((changeAction == beam::wallet::ChangeAction::Updated || changeAction == beam::wallet::ChangeAction::Added)
                            && tx.m_status == beam::wallet::TxStatus::Failed)
                        {
                            emit hideTxIsSent();
                            _activeTx.erase(txId);
                            if (action == Action::CreatePublisher)
                            {
                                emit publisherCreateFail();
                            }
                            else
                            {
                                emit publisherEditFail();
                            }
                        }
                    }
                    if (action == Action::UploadDApp || action == Action::DeleteDApp)
                    {
                        if (changeAction == beam::wallet::ChangeAction::Added && tx.m_status != beam::wallet::TxStatus::Failed) {
                            std::string str{ tx.m_message.begin(), tx.m_message.end() };
                            auto comment = QString(str.c_str()).trimmed();

                            emit showDAppStoreTxPopup(comment, QString::fromStdString(std::to_string(txId)));
                        }

                        if (changeAction == beam::wallet::ChangeAction::Updated && tx.m_status == beam::wallet::TxStatus::Completed)
                        {
                            _activeTx.erase(txId);
                        }
                        else if ((changeAction == beam::wallet::ChangeAction::Updated || changeAction == beam::wallet::ChangeAction::Added)
                            && tx.m_status == beam::wallet::TxStatus::Failed)
                        {
                            (action == Action::UploadDApp) ? emit appPublishFail() : emit appRemoveFail();
                            _activeTx.erase(txId);
                        }
                    }
                }
            }
        }
    }

    void AppsViewModel::onUserPublishersChanged()
    {
        if (_shaderApps.empty())
            return;

        auto updater = [this](QList<QVariantMap>& apps)
        {
            for (auto& app : apps)
            {
                if (app.contains(DApp::kPublisherKey))
                {
                    auto publisherKey = app[DApp::kPublisherKey].value<QString>();
                    const auto idx = std::find_if(_publishers.cbegin(), _publishers.cend(),
                        [publisherKey](const auto& publisher) -> bool {
                            return !publisher[Publisher::kPubkey].toString().compare(publisherKey, Qt::CaseInsensitive);
                        }
                    );

                    if (idx != _publishers.cend())
                    {
                        app[DApp::kPublisherName] = (*idx)[Publisher::kName].toString();
                    }
                }
            }
        };

        updater(_localApps);
        updater(_shaderApps);

        emit appsChanged();
    }

    QList<QVariantMap> AppsViewModel::getPublisherDApps(const QString& publisherKey)
    {
        QList<QVariantMap> publisherApps;
        QList<QVariantMap> apps = getApps();

        std::copy_if(apps.cbegin(), apps.cend(), std::back_inserter(publisherApps),
            [publisherKey] (const auto& app) -> bool {
                const auto appFieldsIt = app.find(DApp::kPublisherKey);
                if (appFieldsIt == app.cend())
                {
                    return false;
                }
                // skip local installed own apps by publisher, that didn't be uploaded
                return appFieldsIt->toString() == publisherKey && app.contains(DApp::kIpfsId);
            }
        );

        return publisherApps;
    }

    QVariantMap AppsViewModel::getAppByGUID(const QString& guid)
    {
        QList<QVariantMap> apps = getApps();
        // find app in _apps by guid
        const auto it = std::find_if(apps.cbegin(), apps.cend(),
            [guid](const auto& app) -> bool {
                const auto appFieldsIt = app.find(DApp::kGuid);
                if (appFieldsIt == app.cend())
                {
                    // TODO: uncomment when all DApps will have new full list of required fields 
                    // assert(false);
                    return false;
                }
                return appFieldsIt->toString() == guid;
            });

        if (it == apps.cend())
        {
            return {};
        }
        return *it;
    }

    void AppsViewModel::removeDApp(const QString& guid)
    {
#ifdef BEAM_IPFS_SUPPORT
        try
        {
            const auto app = getAppByGUID(guid);
            if (app.isEmpty())
            {
                LOG_ERROR() << "Failed to find Dapp by guid " << guid.toStdString();
                return;
            }

            const auto ipfsID = app[DApp::kIpfsId].toString();

            if (!_ipfsIdsToUnpin.contains(ipfsID))
            {
                _ipfsIdsToUnpin.push_back(ipfsID);
            }

            deleteAppFromStore(guid);
        }
        catch (const std::runtime_error& err)
        {
            assert(false);
            LOG_ERROR() << "Failed to get properties for " << guid.toStdString() << ", " << err.what();
            emit appRemoveFail();
            return;
        }
#endif // BEAM_IPFS_SUPPORT
    }

    void AppsViewModel::deleteAppFromStore(const QString& guid)
    {
        ContractArgs args(Actions::kDeleteDapp);
        args.append(DApp::kId, guid.toStdString());

        QPointer<AppsViewModel> guard(this);
        AppModel::getInstance().getWalletModel()->getAsync()->callShader(AppSettings().getDappStorePath(), args.args(),
            [this, guard](const std::string& err, const std::string& output, const beam::ByteBuffer& data)
            {
                if (!guard)
                {
                    return;
                }

                if (!err.empty())
                {
                    LOG_ERROR() << "Failed to delete app" << ", " << err;
                    emit appRemoveFail();
                    return;
                }

                if (data.empty())
                {
                    LOG_ERROR() << "Failed to delete app" << ", " << output;
                    emit appRemoveFail();
                    return;
                }
                handleShaderTxData(Action::DeleteDApp, data);
            }
        );
    }

    void AppsViewModel::updateDApp(const QString& guid)
    {
#ifdef BEAM_IPFS_SUPPORT
        try
        {
            const auto app = getAppByGUID(guid);
            if (app.isEmpty())
            {
                LOG_ERROR() << "Failed to find Dapp by guid " << guid.toStdString();
                return;
            }

            const auto ipfsID = app[DApp::kIpfsId].toString();
            const auto appName = app[DApp::kName].toString();

            // get dapp binary data from ipfs
            QPointer<AppsViewModel> guard(this);
            auto ipfs = AppModel::getInstance().getWalletModel()->getIPFS();

            ipfs->AnyThread_get(ipfsID.toStdString(), 0,
                [this, guard, guid, appName](beam::ByteBuffer&& data) mutable
                {
                    if (!guard)
                    {
                        return;
                    }

                    try
                    {
                        // unpack & verify & install
                        LOG_DEBUG() << "Updating DApp " << appName.toStdString() << " from ipfs";

                        QByteArray qData;
                        std::copy(data.cbegin(), data.cend(), std::back_inserter(qData));

                        QBuffer buffer(&qData);

                        checkManifestFile(&buffer, appName, guid);

                        const auto appsPath = AppSettings().getLocalAppsPath();
                        auto appsDir = QDir(appsPath);
                        const auto appFolder = appsDir.filePath(guid);
                        const auto backupName = guid + "-backup";

                        if (appsDir.exists(guid))
                        {
                            if (!appsDir.rename(guid, backupName))
                            {
                                throw std::runtime_error("Failed to backup folder");
                            }
                        }

                        appsDir.mkdir(guid);
                        if (JlCompress::extractDir(&buffer, appFolder).isEmpty())
                        {
                            if (!QDir(appFolder).removeRecursively() || !appsDir.rename(backupName, guid))
                            {
                                throw std::runtime_error("Failed to restore folder");
                            }

                            throw std::runtime_error("DApp Installation failed");
                        }

                        if (!QDir(appsDir.filePath(backupName)).removeRecursively())
                        {
                            LOG_ERROR() << "Failed to remove backup folder - " << backupName.toStdString();
                        }

                        emit appInstallOK(appName);
                        loadApps();
                    }
                    catch (std::runtime_error& err)
                    {
                        LOG_ERROR() << "Failed to update DApp: " << err.what();
                        emit appInstallFail(appName);
                    }
                },
                [this, guard, appName](std::string&& err)
                {
                    LOG_ERROR() << "Failed to get app from ipfs: " << err;
                    emit appInstallFail(appName);
                }
            );
        }
        catch (const std::runtime_error& err)
        {
            LOG_ERROR() << "Failed to get properties for " << guid.toStdString() << ", " << err.what();
            return;
        }
#endif // BEAM_IPFS_SUPPORT
    }

    void AppsViewModel::onIPFSStatus(bool running, const QString& error, uint32_t peercnt)
    {
        bool isIPFSAvailable = running && peercnt;

        if (_isIPFSAvailable != isIPFSAvailable)
        {
            _isIPFSAvailable = isIPFSAvailable;
            emit isIPFSAvailableChanged();
        }
    }

    bool AppsViewModel::isIPFSAvailable() const
    {
        return _isIPFSAvailable;
    }

    void AppsViewModel::unpinDeletedDApps()
    {
#ifdef BEAM_IPFS_SUPPORT
        auto start = std::remove_if(_ipfsIdsToUnpin.begin(), _ipfsIdsToUnpin.end(),
            [this](const QString& ipfsId)
            {
                const auto idx = std::find_if(_shaderApps.cbegin(), _shaderApps.cend(),
                    [ipfsId](const auto& app)
                    {
                        return app[DApp::kIpfsId] == ipfsId;
                    });

                if (idx == _shaderApps.cend())
                {
                    // unpin dapp binary data from ipfs
                    QPointer<AppsViewModel> guard(this);
                    auto ipfs = AppModel::getInstance().getWalletModel()->getIPFS();

                    ipfs->AnyThread_unpin(ipfsId.toStdString(),
                        [guard, ipfsId]()
                        {
                            if (!guard)
                            {
                                return;
                            }
                            LOG_INFO() << "Successfully unpin app " << ipfsId.toStdString() << " from ipfs";
                        },
                        [ipfsId](std::string&& err)
                        {
                            LOG_ERROR() << "Failed to unpin app " << ipfsId.toStdString() << " from ipfs : " << err;
                        });
                    return true;
                }
                return false;
            });

        _ipfsIdsToUnpin.erase(start, _ipfsIdsToUnpin.end());
#endif // BEAM_IPFS_SUPPORT
    }

    void AppsViewModel::showErrorDialog(Action action)
    {
        if (action == Action::CreatePublisher)
        {
            emit publisherCreateFail();
        }
        if (action == Action::UpdatePublisher)
        {
            emit publisherEditFail();
        }
        if (action == Action::UploadDApp)
        {
            emit appPublishFail();
        }
        if (action == Action::DeleteDApp)
        {
            emit appRemoveFail();
        }
    }

    void AppsViewModel::checkManifestFile(QIODevice* ioDevice, const QString& expectedAppName, const QString& expectedGuid)
    {
        QuaZip zip(ioDevice);
        if (!zip.open(QuaZip::Mode::mdUnzip))
        {
            throw std::runtime_error("Failed to open the DApp archive");
        }

        bool isFound = false;
        for (bool ok = zip.goToFirstFile(); ok; ok = zip.goToNextFile())
        {
            const auto zipFname = zip.getCurrentFileName();
            if (zipFname == kManifestFile)
            {
                QuaZipFile mfile(&zip);
                if (!mfile.open(QIODevice::ReadOnly))
                {
                    throw std::runtime_error("Failed to read the DApp archive");
                }

                QTextStream in(&mfile);
                const auto app = parseAppManifest(in, "");
                if (expectedGuid != app[DApp::kGuid].value<QString>())
                {
                    throw std::runtime_error("Wrong guid");
                }
                if (expectedAppName != app[DApp::kName].value<QString>())
                {
                    throw std::runtime_error("Wrong name of app");
                }
                isFound = true;
            }
        }

        if (!isFound)
        {
            throw std::runtime_error("Maybe dapp file is broken");
        }
    }
}
