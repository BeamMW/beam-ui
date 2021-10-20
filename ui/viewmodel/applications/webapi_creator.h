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
#include "apps_api_ui.h"
#include "model/assets_list.h"

namespace beamui::applications
{
    class WebAPICreator
        : public QObject
    {
        Q_OBJECT

    public:
        explicit WebAPICreator(QObject *parent = nullptr);
        ~WebAPICreator() override;

        Q_INVOKABLE void createApi(const QString& verWant, const QString& verMin, const QString& appName, const QString& appUrl);
        Q_INVOKABLE void destroyApi();
        Q_INVOKABLE [[nodiscard]] bool apiSupported(const QString& apiVersion) const;
        Q_INVOKABLE [[nodiscard]] QString generateAppID(const QString& appName, const QString& appUrl);

    signals:
        void apiCreated(QObject* api);

    private:
        std::shared_ptr<AppsApiUI> _api;
    };
}
