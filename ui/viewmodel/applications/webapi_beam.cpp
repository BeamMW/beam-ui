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
#include <sstream>
#include "webapi_beam.h"
#include "utility/logger.h"
#include "model/app_model.h"

namespace beamui::applications {
    using namespace beam::wallet;

    namespace {
        WalletModel& getWallet() {
            return *AppModel::getInstance().getWalletModel();
        }

        IWalletModelAsync& getAsyncWallet() {
            return *getWallet().getAsync();
        }
    }

    WebAPI_Beam::WebAPI_Beam(QObject *parent)
        : QObject(parent)
    {
    }

    QString WebAPI_Beam::chooseApi(const QString& version)
    {
        auto newVer = IWalletApi::SApiVer2NApiVer(version.toStdString());
        if (_currApiVersion == newVer)
        {
            return QString();
        }

        if (!IWalletApi::ValidateAPIVersion(version.toStdString()))
        {
            //% "Unsupported API version requested: %1"
            return qtTrId("apps-bad-api-version").arg(version);
        }

        IWalletApi::InitData data;

        data.contracts = AppModel::getInstance().getWalletModel()->getAppsShaders();
        data.swaps     = nullptr;
        data.wallet    = AppModel::getInstance().getWalletModel()->getWallet();
        data.walletDB  = AppModel::getInstance().getWalletDB();

        _walletAPI = IWalletApi::CreateInstance(version.toStdString(), *this, data);
        _currApiVersion = newVer;

        return QString();
    }

    void WebAPI_Beam::callWalletApi(const QString& request)
    {
        LOG_INFO () << "WebAPP API: " << request.toStdString();

        IWalletApi::WeakPtr wp = _walletAPI;
        getAsyncWallet().makeIWTCall(
            [wp, request]() -> boost::any {
                if(auto sp = wp.lock())
                {
                    std::string stdreq = request.toStdString();
                    sp->executeAPIRequest(stdreq.c_str(), stdreq.size());
                    return boost::none;
                }
                // this means that api is disconnected and destroyed already
                // well, okay, nothing to do then
                return std::string();
            },
            [] (boost::any) {
            }
        );
    }

    void WebAPI_Beam::sendAPIResponse(const json& result)
    {
        auto str = result.dump();
        if (!str.empty())
        {
            emit callWalletApiResult(QString::fromStdString(str));
        }
    }

    int WebAPI_Beam::test()
    {
        // only for test, always 42
        return 42;
    }
}
