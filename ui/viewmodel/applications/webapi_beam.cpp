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
#include <QQmlEngine>
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

    WebAPI_Beam::WebAPI_Beam(const std::string& version, const std::string& appid)
        : QObject(nullptr)
    {
        IWalletApi::InitData data;

        data.contracts = AppModel::getInstance().getWalletModel()->getAppsShaders();
        data.swaps     = nullptr;
        data.wallet    = AppModel::getInstance().getWalletModel()->getWallet();
        data.walletDB  = AppModel::getInstance().getWalletDB();
        data.appid     = appid;

        _walletAPI = IWalletApi::CreateInstance(version, *this, data);
    }

    WebAPI_Beam::~WebAPI_Beam()
    {
    }

    void WebAPI_Beam::callWalletApi(const QString& request)
    {
        //
        // THIS IS THE UI THREAD
        //
        LOG_INFO () << "WebAPP API call: " << request.toStdString();

        const auto stdreq = request.toStdString();
        if (auto pres = _walletAPI->parseAPIRequest(stdreq.c_str(), stdreq.size()); pres)
        {
            if (pres->minfo.handlesApps)
            {
                //if (pres->acinfo.method == "tx_create")
                //{
                //    return;
                //}

                //if (pres->acinfo.method == "invoke_contract")
                //{
                //    return;
                //}

                if (pres->minfo.fee > 0 || !pres->minfo.spend.empty())
                {
                    LOG_INFO() << "Application called method " << pres->acinfo.method << " that spends funds, but user consent is not handled";
                    assert(false);

                    const auto error = _walletAPI->fromError(stdreq, ApiError::NotAllowedError, std::string());
                    emit callWalletApiResult(QString::fromStdString(error));

                    return;
                }

                return callWalletApiImp(stdreq);
            }

            LOG_INFO() << "Application request call of the not allowed method: " << pres->acinfo.method;
            const auto error = _walletAPI->fromError(stdreq, ApiError::NotAllowedError, std::string());
            emit callWalletApiResult(QString::fromStdString(error));
            return;
        }
        else
        {
            // parse failed, just log error and return. Error response is already sent back
            LOG_ERROR() << "WebAPP API parse failed: " << request.toStdString();
            return;
        }
    }

    void WebAPI_Beam::callWalletApiImp(const std::string& request)
    {
        //
        // this can be any thread, for now the UI thread only
        //
        IWalletApi::WeakPtr wp = _walletAPI;
        getAsyncWallet().makeIWTCall(
            [wp, request]() -> boost::any {
                if(auto sp = wp.lock())
                {
                    sp->executeAPIRequest(request.c_str(), request.size());
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

    WebAPICreator::WebAPICreator(QObject *parent)
        : QObject(parent)
    {
    }

    WebAPICreator::~WebAPICreator()
    {
    }

    void WebAPICreator::createApi(const QString &version, const QString &appName, const QString &appUrl)
    {
        const auto stdver = version.toStdString();
        if (!IWalletApi::ValidateAPIVersion(stdver))
        {
            //% "Unsupported API version requested: %1"
            const auto error = qtTrId("apps-bad-api-version").arg(version);
            return qmlEngine(this)->throwError(error);
        }

        ECC::Hash::Value hv;
        ECC::Hash::Processor() << appName.toStdString() << appUrl.toStdString() >> hv;
        const auto appid = hv.str();

        _api = std::make_unique<WebAPI_Beam>(stdver, appid);
        QQmlEngine::setObjectOwnership(_api.get(), QQmlEngine::CppOwnership);
        emit apiCreated(_api.get());
    }
}
