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
    {
        _server = std::make_unique<QHttpServer>();

        // Use setMissingHandler as a catch-all to serve static files for any path
        _server->setMissingHandler(_server.get(),
            [this](const QHttpServerRequest &request, QHttpServerResponder &responder) {
                QString path = QUrl::fromPercentEncoding(request.url().path().toUtf8());
                if (path.startsWith('/'))
                    path = path.mid(1);

                QString absolutePath = _documentRoot.absoluteFilePath(path);

                // Prevent path traversal outside document root
                if (!absolutePath.startsWith(_documentRoot.absolutePath())) {
                    responder.write(QHttpServerResponder::StatusCode::Forbidden);
                    return;
                }

                QFileInfo fileInfo(absolutePath);
                if (!fileInfo.exists() || fileInfo.isDir()) {
                    responder.write(QHttpServerResponder::StatusCode::NotFound);
                    return;
                }

                QFile file(absolutePath);
                if (!file.open(QIODevice::ReadOnly)) {
                    responder.write(QHttpServerResponder::StatusCode::Forbidden);
                    return;
                }

                QByteArray content = file.readAll();
                QByteArray mimeType = _mimeDb.mimeTypeForFile(absolutePath).name().toUtf8();
                responder.write(content, mimeType);
            });

        _tcpServer = new QTcpServer();
        if (!_tcpServer->listen(QHostAddress::LocalHost, port))
        {
            delete _tcpServer;
            _tcpServer = nullptr;
            throw std::runtime_error("failed to listen");
        }
        _server->bind(_tcpServer);
    }

    AppsServer::~AppsServer()
    {
        _server.reset();
    }
}
