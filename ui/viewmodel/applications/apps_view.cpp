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
    const uint8_t kCountDAppVersionParts = 4;
    const QString kBeamPublisherName = "Beam Development Limited";
    const QString kBeamPublisherKey = "";
    const QString kLocalapp = "localapp";

    namespace DApp
    {
        const char kName[] = "name";
        const char kDescription[] = "description";
        const char kIpfsId[] = "ipfs_id";
        const char kUrl[] = "url";
        const char kApiVersion[] = "api_version";
        const char kMinApiVersion[] = "min_api_version";
        const char kGuid[] = "guid";
        const char kPublisherKey[] = "publisher";
        const char kPublisherName[] = "publisherName";
        const char kCategory[] = "category";
        const char kCategoryName[] = "categoryName";
        const char kCategoryColor[] = "categoryColor";
        const char kSupported[] = "supported";
        const char kNotInstalled[] = "notInstalled";
        const char kIcon[] = "icon";
        const char kVersion[] = "version";

        const int kNameMaxSize = 30;
        const int kDescriptionMaxSize = 1024;
        const int kApiVersionMaxSize = 10;
        const int kIconMaxSize = 10240; // 10kb
    } // namespace DApp

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

        tmp["publisherKey"] = QString::fromStdString(info["pubkey"].get<std::string>());
        tmp["nickname"] = fromHex(info["name"].get<std::string>());
        tmp["shortTitle"] = fromHex(info["short_title"].get<std::string>());
        tmp["aboutMe"] = fromHex(info["about_me"].get<std::string>());
        tmp["website"] = fromHex(info["website"].get<std::string>());
        tmp["twitter"] = fromHex(info["twitter"].get<std::string>());
        tmp["linkedin"] = fromHex(info["linkedin"].get<std::string>());
        tmp["instagram"] = fromHex(info["instagram"].get<std::string>());
        tmp["telegram"] = fromHex(info["telegram"].get<std::string>());
        tmp["discord"] = fromHex(info["discord"].get<std::string>());

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
        return apiSupported(app.contains("api_version") ? app["api_version"].toString() : "current") ||
            apiSupported(app.contains("min_api_version") ? app["min_api_version"].toString() : "");
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

        const auto& icon = json["icon"];
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
                ipath = QString::fromStdString(json["icon"].get<std::string>());

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
            app.insert(DApp::kApiVersion, QString::fromStdString(av.get<std::string>()));
        }

        const auto& mav = json[DApp::kMinApiVersion];
        if (!mav.empty())
        {
            if (!mav.is_string())
            {
                throw std::runtime_error("Invalid min_api_version in the manifest file");
            }
            app.insert(DApp::kMinApiVersion, QString::fromStdString(mav.get<std::string>()));
        }

        const auto& v = json["version"];
        if (!v.empty())
        {
            if (!v.is_string())
            {
                throw std::runtime_error("Invalid version in the manifest file");
            }
            app.insert("version", QString::fromStdString(v.get<std::string>()));
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

        // TODO: Make guid is required field after fixing all DApps
        const auto& guidObj = json[DApp::kGuid];
        if (!guidObj.empty())
        {
            if (!guidObj.is_string())
            {
                throw std::runtime_error("Invalid 'guid' in the manifest file");
            }
            app.insert(DApp::kGuid, QString::fromStdString(guidObj.get<std::string>()));
        }

        app.insert("local", true);
        // TODO: check why we used surl instead of extended url - app["url"]
        const auto appid = beam::wallet::GenerateAppID(sname, app[DApp::kUrl].toString().toStdString());
        app.insert("appid", QString::fromStdString(appid));

        return app;
    }

    void AppsViewModel::loadApps()
    {
        // TODO: It mb worth loading in parallel and then putting it together
        loadLocalApps();
        loadDevApps();
        loadAppsFromServer();
        loadAppsFromStore();
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
                        auto name = parseStringField(item.value(), "name");
                        auto url = parseStringField(item.value(), "url");
                        const auto appid = beam::wallet::GenerateAppID(name.toStdString(), url.toStdString());

                        app.insert("appid", QString::fromStdString(appid));
                        app.insert(DApp::kDescription, parseStringField(item.value(), "description"));
                        app.insert(DApp::kName, name);
                        app.insert(DApp::kUrl, url);
                        app.insert(DApp::kIcon, parseStringField(item.value(), "icon"));
                        app.insert(DApp::kPublisherName, kBeamPublisherName);
                        app.insert("publisherKey", kBeamPublisherKey);
                        app.insert(DApp::kSupported, isAppSupported(app));
                        app.insert("isFromServer", true);

                        // TODO: check order of the DApps
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
        std::string args = "action=view_dapps,cid=";
        args += AppSettings().getDappStoreCID();

        QPointer<AppsViewModel> guard(this);

        AppModel::getInstance().getWalletModel()->getAsync()->callShaderAndStartTx(AppSettings().getDappStorePath(), std::move(args),
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
                            auto guid = parseStringField(item.value(), "id");
                            auto publisherKey = parseStringField(item.value(), "publisher");

                            // parse DApps only of the user publishers + own
                            if (!_userPublishersKeys.contains(publisherKey, Qt::CaseInsensitive) &&
                                !(isPublisher() && publisherKey.compare(_publisherInfo["publisherKey"].toString(), Qt::CaseInsensitive) == 0))
                            {
                                continue;
                            }

                            const auto idx = std::find_if(_publishers.cbegin(), _publishers.cend(),
                                [publisherKey](const auto& publisher) -> bool {
                                    return !publisher["publisherKey"].toString().compare(publisherKey, Qt::CaseInsensitive);
                                }
                            );

                            QString publisherName = "";

                            if (idx != _publishers.end())
                            {
                                publisherName = (*idx)["nickname"].toString();
                            }

                            LOG_DEBUG() << "Parsing DApp from contract, guid - " << guid.toStdString() << ", publisher - " << publisherKey.toStdString();

                            // parse version
                            auto versionObj = item.value()["version"];

                            if (versionObj.empty() || !versionObj.is_object())
                            {
                                throw std::runtime_error("Invalid 'version' of the dapp");
                            }

                            auto majorObj = versionObj["major"];
                            auto minorObj = versionObj["minor"];
                            auto releaseObj = versionObj["release"];
                            auto buildObj = versionObj["build"];
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
                            app.insert(DApp::kDescription, decodeStringField(item.value(), "description"));
                            app.insert(DApp::kName, decodeStringField(item.value(), "name"));
                            app.insert(DApp::kIpfsId, parseStringField(item.value(), "ipfs_id"));
                            // TODO: check if empty url is allowed for not installed app
                            app.insert(DApp::kUrl, "");
                            app.insert(DApp::kApiVersion, decodeStringField(item.value(), "api_ver"));
                            app.insert(DApp::kMinApiVersion, decodeStringField(item.value(), "min_api_ver"));
                            app.insert(DApp::kGuid, guid);
                            app.insert(DApp::kPublisherKey, publisherKey);
                            app.insert(DApp::kPublisherName, publisherName);
                            app.insert(DApp::kVersion, version);

                            Category category = static_cast<Category>(item.value()["category"].get<int>());
                            app.insert(DApp::kCategory, item.value()["category"].get<int>());
                            app.insert(DApp::kCategoryName, converToString(category));
                            app.insert(DApp::kCategoryColor, getCategoryColor(category));
                            app.insert(DApp::kIcon, decodeStringField(item.value(), "icon"));
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
        std::string args = "action=view_publishers,cid=";
        args += AppSettings().getDappStoreCID();

        QPointer<AppsViewModel> guard(this);

        AppModel::getInstance().getWalletModel()->getAsync()->callShaderAndStartTx(AppSettings().getDappStorePath(), std::move(args),
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
        std::string args = "action=my_publisher_info,cid=";
        args += AppSettings().getDappStoreCID();

        QPointer<AppsViewModel> guard(this);

        AppModel::getInstance().getWalletModel()->getAsync()->callShaderAndStartTx(AppSettings().getDappStorePath(), std::move(args),
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
                return _userPublishersKeys.contains(publisher["publisherKey"].toString(), Qt::CaseInsensitive);
            }
        );

        return userPublishers;
    }

    QList<QVariantMap> AppsViewModel::getApps()
    {
        QList<QVariantMap> result = _remoteApps + _devApps;
        QList<QVariantMap> notInstalled, installed, installedFromFile;

        for (const auto& app : _shaderApps)
        {
            const auto it = std::find_if(_localApps.begin(), _localApps.end(),
                [guid = app[DApp::kGuid]](const auto& tmp) -> bool {
                    const auto appIt = tmp.find(DApp::kGuid);
                    if (appIt == tmp.end())
                    {
                        return false;
                    }
                    return appIt->toString() == guid;
                }
            );

            if (it != _localApps.end())
            {
                auto tmp = *it;
                if (compareDAppVersion(app[DApp::kVersion].toString(), tmp[DApp::kVersion].toString()) > 0)
                {
                    tmp.insert("hasUpdate", true);
                }
                tmp.insert(DApp::kIpfsId, app[DApp::kIpfsId]);
                if (!tmp.contains(DApp::kPublisherKey))
                {
                    tmp.insert(DApp::kPublisherKey, app[DApp::kPublisherKey]);
                }
                if (!tmp.contains(DApp::kPublisherName))
                {
                    tmp.insert(DApp::kPublisherName, app[DApp::kPublisherName]);
                }
                installed.push_back(tmp);
            }
            else
            {
                notInstalled.push_back(app);
            }
        }

        for (const auto& app : _localApps)
        {
            const auto it = std::find_if(installed.begin(), installed.end(),
                [guid = app[DApp::kGuid]](const auto& tmp) -> bool {
                    const auto appIt = tmp.find(DApp::kGuid);
                    if (appIt == tmp.end())
                    {
                        return false;
                    }
                    return appIt->toString() == guid;
                }
            );

            if (it == installed.end())
            {
                installedFromFile.push_back(app);
            }
        }

        result += installedFromFile;

        if (_publisherInfo.empty())
        {
            result += installed + notInstalled;
        }
        else
        {
            for (const auto& app : installed)
            {
                if (app[DApp::kPublisherKey] == _publisherInfo["publisherKey"])
                {
                    result.push_back(app);
                }
            }
            for (const auto& app : notInstalled)
            {
                if (app[DApp::kPublisherKey] == _publisherInfo["publisherKey"])
                {
                    result.push_back(app);
                }
            }

            for (const auto& app : installed)
            {
                if (app[DApp::kPublisherKey] != _publisherInfo["publisherKey"])
                {
                    result.push_back(app);
                }
            }
            for (const auto& app : notInstalled)
            {
                if (app[DApp::kPublisherKey] != _publisherInfo["publisherKey"])
                {
                    result.push_back(app);
                }
            }
        }

        return result;
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
            devapp.insert("appid", appid);
            devapp.insert(DApp::kSupported, true);
            result.push_back(devapp);
        }

        if (_devApps != result)
        {
            _devApps = result;
            emit appsChanged();
        }
    }

    void AppsViewModel::loadLocalApps()
    {
        QList<QVariantMap> result;
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

                auto app = parseAppManifest(in, justFolder);
                app.insert("full_path", fullFolder);
                app.insert(DApp::kSupported, isAppSupported(app));

                result.push_back(app);
            }
            catch(std::runtime_error& err)
            {
                LOG_ERROR() << "Error while reading local app from " << mpath.toStdString() << ", " << err.what();
            }
        }

        _localApps = result;

        if (!_runApp)
            emit appsChanged();
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
                const auto publisherIt = publisher.find("publisherKey");
                if (publisherIt == publisher.end())
                {
                    assert(false);
                    return false;
                }
                return publisherIt->toString() == publisherKey;
            }
        );

        if (it == _publishers.end())
        {
            return {};
        }

        if (!_userPublishersKeys.contains(publisherKey, Qt::CaseInsensitive))
        {
            _userPublishersKeys.append(publisherKey);
            AppSettings().setDappStoreUserPublishers(_userPublishersKeys);

            emit userPublishersChanged();
        }

        return (*it)["nickname"].toString();
    }

    void AppsViewModel::removePublisherByKey(const QString& publisherKey)
    {
        if (_userPublishersKeys.removeOne(publisherKey))
        {
            AppSettings().setDappStoreUserPublishers(_userPublishersKeys);
            
            emit userPublishersChanged();
        }
    }

    void AppsViewModel::createPublisher(const QVariantMap& publisherInfo)
    {
        std::string args = "action=add_publisher,cid=";
        args += AppSettings().getDappStoreCID();
        args += ",name=" + toHex(publisherInfo["nickname"].toString());
        args += ",short_title=" + toHex(publisherInfo["shortTitle"].toString());
        args += ",about_me=" + toHex(publisherInfo["aboutMe"].toString());
        args += ",website=" + toHex(publisherInfo["website"].toString());
        args += ",twitter=" + toHex(publisherInfo["twitter"].toString());
        args += ",linkedin=" + toHex(publisherInfo["linkedin"].toString());
        args += ",instagram=" + toHex(publisherInfo["instagram"].toString());
        args += ",telegram=" + toHex(publisherInfo["telegram"].toString());
        args += ",discord=" + toHex(publisherInfo["discord"].toString());

        QPointer<AppsViewModel> guard(this);

        AppModel::getInstance().getWalletModel()->getAsync()->callShader(AppSettings().getDappStorePath(), std::move(args),
            [this, guard](const std::string& err, const std::string& output, const beam::ByteBuffer& data)
            {
                if (!guard)
                {
                    return;
                }

                if (!err.empty())
                {
                    LOG_ERROR() << "Failed to create a publisher" << ", " << err;
                    return;
                }

                handleShaderTxData(Action::CreatePublisher, data);
            }
        );
    }

    void AppsViewModel::changePublisherInfo(const QVariantMap& publisherInfo)
    {
        std::string args = "action=update_publisher,cid=";
        args += AppSettings().getDappStoreCID();
        args += ",name=" + toHex(publisherInfo["nickname"].toString());
        args += ",short_title=" + toHex(publisherInfo["shortTitle"].toString());
        args += ",about_me=" + toHex(publisherInfo["aboutMe"].toString());
        args += ",website=" + toHex(publisherInfo["website"].toString());
        args += ",twitter=" + toHex(publisherInfo["twitter"].toString());
        args += ",linkedin=" + toHex(publisherInfo["linkedin"].toString());
        args += ",instagram=" + toHex(publisherInfo["instagram"].toString());
        args += ",telegram=" + toHex(publisherInfo["telegram"].toString());
        args += ",discord=" + toHex(publisherInfo["discord"].toString());

        QPointer<AppsViewModel> guard(this);

        AppModel::getInstance().getWalletModel()->getAsync()->callShader(AppSettings().getDappStorePath(), std::move(args),
            [this, guard](const std::string& err, const std::string& output, const beam::ByteBuffer& data)
            {
                if (!guard)
                {
                    return;
                }

                if (!err.empty())
                {
                    LOG_ERROR() << "Failed to change a publisher info" << ", " << err;
                    return;
                }

                handleShaderTxData(Action::UpdatePublisher, data);
            }
        );
    }

    bool AppsViewModel::uninstallLocalApp(const QString& appid)
    {
        const auto it = std::find_if(_localApps.begin(), _localApps.end(), [appid](const auto& props) -> bool {
            const auto ait = props.find("appid");
            if (ait == props.end())
            {
                assert(false);
                return false;
            }
            return ait->toString() == appid;
        });

        if (it == _localApps.end())
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
                if (zipFname == "manifest.json")
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
            app.insert("release_date", QLocale(QLocale::English).toString(QDate::currentDate(), "dd MMM yyyy"));

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
    }

    bool AppsViewModel::checkDAppNewVersion(const QVariantMap& currentDApp, const QVariantMap& newDApp)
    {
        if (currentDApp[DApp::kGuid].value<QString>() == currentDApp[DApp::kGuid].value<QString>())
        {
            return compareDAppVersion(newDApp["version"].value<QString>(), currentDApp["version"].value<QString>()) > 0;
        }
        return false;
    }

    void AppsViewModel::uploadAppToStore(QVariantMap&& app, const std::string& ipfsID, bool isUpdating)
    {
        QString guid = app[DApp::kGuid].value<QString>();
        QString appName = app[DApp::kName].value<QString>();
        QString description = app[DApp::kDescription].value<QString>();

        std::stringstream argsStream;
        argsStream << (isUpdating ? "action=update_dapp," : "action=add_dapp,");
        argsStream << "cid=" << AppSettings().getDappStoreCID().c_str();
        argsStream << ",ipfs_id=" << ipfsID;
        argsStream << ",name=" << toHex(appName);
        argsStream << ",id=" << guid.toStdString();
        argsStream << ",description=" << toHex(description);
        argsStream << ",api_ver=" << toHex(app[DApp::kApiVersion].value<QString>());
        argsStream << ",min_api_ver=" << toHex(app[DApp::kMinApiVersion].value<QString>());
        argsStream << ",category=" << app[DApp::kCategory].value<uint32_t>();
        argsStream << ",icon=" << toHex(app[DApp::kIcon].value<QString>());

        // parse version
        QStringList version = app["version"].value<QString>().split(".");

        for (; version.length() < kCountDAppVersionParts;)
        {
            version.append("0");
        }

        argsStream << ",major=" << version[0].toStdString();
        argsStream << ",minor=" << version[1].toStdString();
        argsStream << ",release=" << version[2].toStdString();
        argsStream << ",build=" << version[3].toStdString();

        QPointer<AppsViewModel> guard(this);

        LOG_INFO() << "args: " << argsStream.str();

        AppModel::getInstance().getWalletModel()->getAsync()->callShader(AppSettings().getDappStorePath(), argsStream.str(),
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
                        
                        // TODO: does we need add additional verification of the DApp file?

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
                if (zipFname == "manifest.json")
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

            // TODO roman.strilets maybe need to process error
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

                    return;
                }

                if (_activeTx.find(id) != _activeTx.end())
                {
                    // TODO roman.strilets ????
                    return;
                }

                _activeTx[id] = action;

                if (Action::CreatePublisher == action || Action::UpdatePublisher == action)
                {
                    emit showTxIsSent();
                }
                // TODO: check TX status
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
                    LOG_INFO() << "onTransactionsChanged: changeAction = " << static_cast<int>(changeAction) << ", status = " << static_cast<int>(tx.m_status);

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
                    if (action == Action::UploadDApp)
                    {
                        if (changeAction == beam::wallet::ChangeAction::Updated && tx.m_status == beam::wallet::TxStatus::Completed)
                        {
                            _activeTx.erase(txId);
                        }
                        else if ((changeAction == beam::wallet::ChangeAction::Updated || changeAction == beam::wallet::ChangeAction::Added)
                            && tx.m_status == beam::wallet::TxStatus::Failed)
                        {
                            emit appPublishFail();
                            _activeTx.erase(txId);
                        }
                    }
                    if (action == Action::DeleteDApp)
                    {
                        if (changeAction == beam::wallet::ChangeAction::Updated && tx.m_status == beam::wallet::TxStatus::Completed)
                        {
                            _activeTx.erase(txId);
                        }
                        else if ((changeAction == beam::wallet::ChangeAction::Updated || changeAction == beam::wallet::ChangeAction::Added)
                            && tx.m_status == beam::wallet::TxStatus::Failed)
                        {
                            emit appRemoveFail();
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
                            return !publisher["publisherKey"].toString().compare(publisherKey, Qt::CaseInsensitive);
                        }
                    );

                    QString publisherName = "";

                    if (idx != _publishers.end())
                    {
                        app[DApp::kPublisherName] = (*idx)["nickname"].toString();
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
                if (appFieldsIt == app.end())
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
                if (appFieldsIt == app.end())
                {
                    // TODO: uncomment when all DApps will have new full list of required fields 
                    // assert(false);
                    return false;
                }
                return appFieldsIt->toString() == guid;
            });

        if (it == apps.end())
        {
            return {};
        }
        return *it;
    }

    void AppsViewModel::removeDApp(const QString& guid)
    {
        // TODO: change the order of operations to: first remove from contract -> unpin from IPFS
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

            // unpin dapp binary data from ipfs
            QPointer<AppsViewModel> guard(this);
            auto ipfs = AppModel::getInstance().getWalletModel()->getIPFS();

            ipfs->AnyThread_unpin(ipfsID.toStdString(),
                [this, guard, appName, guid, ipfsID]() mutable
                {
                    if (!guard)
                    {
                        return;
                    }
                    LOG_INFO() << "Successfully unpin app" << appName.toStdString() << "(" << ipfsID.toStdString() << ") from ipfs : ";
                    deleteAppFromStore(guid);
                },
                [this, appName, ipfsID](std::string&& err)
                {
                    LOG_ERROR() << "Failed to unpin app" << appName.toStdString() << "(" << ipfsID.toStdString() << ") from ipfs : " << err;
                    emit appRemoveFail();
                }
            );
        }
        catch (const std::runtime_error& err)
        {
            assert(false);
            LOG_ERROR() << "Failed to get properties for " << guid.toStdString() << ", " << err.what();
            emit appRemoveFail();
            return;
        }
    }

    void AppsViewModel::deleteAppFromStore(const QString& guid)
    {
        std::stringstream argsStream;
        argsStream << "action=delete_dapp,";
        argsStream << "cid=" << AppSettings().getDappStoreCID().c_str();
        argsStream << ",id=" << guid.toStdString();

        QPointer<AppsViewModel> guard(this);
        AppModel::getInstance().getWalletModel()->getAsync()->callShader(AppSettings().getDappStorePath(), argsStream.str(),
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

                        // TODO: does we need add additional verification of the DApp file?

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
    }
}
