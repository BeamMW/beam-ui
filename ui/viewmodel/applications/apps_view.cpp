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

namespace beamui::applications {
    AppsViewModel::AppsViewModel()
    {
        LOG_INFO() << "AppsViewModel created";

        auto& settings = AppModel::getInstance().getSettings();
        auto defaultProfile = QWebEngineProfile::defaultProfile();
        defaultProfile->setCachePath(settings.getAppsCachePath());
        defaultProfile->setPersistentStoragePath(settings.getAppsStoragePath());
    }

    AppsViewModel::~AppsViewModel()
    {
        LOG_INFO() << "AppsViewModel destroyed";
    }

    void AppsViewModel::onCompleted(QObject *webView)
    {
        assert(webView != nullptr);
    }

    QString AppsViewModel::getDevAppUrl() const
    {
        auto& settings = AppModel::getInstance().getSettings();
        return settings.getDevBeamAppUrl();
    }

    QString AppsViewModel::getDevAppName() const
    {
        auto& settings = AppModel::getInstance().getSettings();
        return settings.getDevBeamAppName();
    }

    QString AppsViewModel::getAppsUrl() const
    {
        auto& settings = AppModel::getInstance().getSettings();
        return settings.getAppsUrl();
    }
}
