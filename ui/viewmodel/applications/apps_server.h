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

#include <QHttpServer>
#include <QTcpServer>
#include <QDir>
#include <QFile>
#include <QMimeDatabase>

namespace beamui::applications
{
    class AppsServer
    {
    public:
        AppsServer(const QString& serveFrom, uint32_t port);
        ~AppsServer();

    private:
        QDir _documentRoot;
        QString _canonicalRoot;  // resolved once at construction for path-traversal checks
        QMimeDatabase _mimeDb;
        std::unique_ptr<QHttpServer> _server;
        QTcpServer* _tcpServer = nullptr;  // owned by _server after bind()
    };
}
