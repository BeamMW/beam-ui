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
#include "apps_server.h"

#include <stdexcept>
#include <QFileInfo>
#include <QUrl>

namespace beamui::applications
{
    AppsServer::AppsServer(const QString& serveFrom, uint32_t port)
        : _documentRoot(serveFrom)
        , _canonicalRoot(_documentRoot.canonicalPath())
    {
        if (_canonicalRoot.isEmpty())
            throw std::runtime_error("document root does not exist");

        _server = std::make_unique<QHttpServer>();

        // Use setMissingHandler as a catch-all to serve static files for any path
        _server->setMissingHandler(_server.get(),
            [this](const QHttpServerRequest &request, QHttpServerResponder &responder) {
                QString path = QUrl::fromPercentEncoding(request.url().path().toUtf8());
                if (path.startsWith('/'))
                    path = path.mid(1);

                QString absolutePath = _documentRoot.absoluteFilePath(path);

                // Resolve symlinks and ".." to a canonical path for safe comparison
                QString canonical = QFileInfo(absolutePath).canonicalFilePath();
                if (canonical.isEmpty() || !canonical.startsWith(_canonicalRoot)) {
                    responder.write(QHttpServerResponder::StatusCode::Forbidden);
                    return;
                }

                QFileInfo fileInfo(canonical);
                if (fileInfo.isDir()) {
                    responder.write(QHttpServerResponder::StatusCode::NotFound);
                    return;
                }

                // Stream via QIODevice — avoids loading entire file into memory
                auto *file = new QFile(canonical);
                if (!file->open(QIODevice::ReadOnly)) {
                    delete file;
                    responder.write(QHttpServerResponder::StatusCode::Forbidden);
                    return;
                }

                QByteArray mimeType = _mimeDb.mimeTypeForFile(canonical).name().toUtf8();
                // Note: This function takes the ownership of data.
                responder.write(file, mimeType);
            });

        _tcpServer = new QTcpServer();
        if (!_tcpServer->listen(QHostAddress::LocalHost, port))
        {
            delete _tcpServer;
            _tcpServer = nullptr;
            throw std::runtime_error("failed to listen");
        }
        if (!_server->bind(_tcpServer))
        {
            // listen() succeeded but bind() failed; _tcpServer was not reparented
            delete _tcpServer;
            _tcpServer = nullptr;
            throw std::runtime_error("failed to bind");
        }
    }

    AppsServer::~AppsServer()
    {
        _server.reset();
    }
}
