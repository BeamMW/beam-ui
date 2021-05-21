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

    WebAPI_Beam::WebAPI_Beam(IConsentHandler& handler, IShadersManager::Ptr shaders, const std::string& version, const std::string& appid, const std::string& appname)
        : QObject(nullptr)
        , _consentHandler(handler)
        , _appId(appid)
        , _appName(appname)
    {
        IWalletApi::InitData data;

        data.contracts = std::move(shaders);
        data.swaps     = nullptr;
        data.wallet    = AppModel::getInstance().getWalletModel()->getWallet();
        data.walletDB  = AppModel::getInstance().getWalletDB();
        data.appId     = _appId;
        data.appName   = _appName;

        _walletAPI = IWalletApi::CreateInstance(version, *this, data);
        LOG_INFO () << "WebAPI_Beam created for " << appname << ", " << appid;
    }

    WebAPI_Beam::~WebAPI_Beam()
    {
        AppModel::getInstance().getWalletModel()->releaseAppsShaders(_appId);
    }

    void WebAPI_Beam::callWalletApi(const QString& request)
    {
        //
        // THIS IS THE UI THREAD
        //
        LOG_INFO () << "WebAPP API call for " << _appName << ", " << _appId << "): " << request.toStdString();

        IWalletApi::WeakPtr wp = _walletAPI;
        getAsyncWallet().makeIWTCall(
            [wp, this, request]() -> boost::any {
                if (!wp.lock())
                {
                    // this means that api is disconnected and destroyed already, this is normal
                    return boost::none;
                }

                const auto stdreq = request.toStdString();
                if (auto pres = _walletAPI->parseAPIRequest(stdreq.c_str(), stdreq.size()); pres)
                {
                    const auto& acinfo = pres->acinfo;
                    if (acinfo.appsAllowed)
                    {
                        if (acinfo.method == "tx_send")
                        {
                            _consentHandler.AnyThread_getSendConsent(stdreq, *pres);
                            return boost::none;
                        }

                        if (acinfo.method == "process_invoke_data")
                        {
                            _consentHandler.AnyThread_getContractInfoConsent(stdreq, *pres);
                            return boost::none;
                        }

                        if (pres->minfo.fee > 0 || !pres->minfo.spend.empty())
                        {
                            LOG_INFO() << "Application called method " << acinfo.method << " that spends funds, but user consent is not handled";
                            assert(false);

                            const auto error = _walletAPI->fromError(stdreq, ApiError::NotAllowedError, std::string());
                            emit callWalletApiResult(QString::fromStdString(error));

                            return boost::none;
                        }

                        _walletAPI->executeAPIRequest(stdreq.c_str(), stdreq.size());
                        return boost::none;
                    }

                    LOG_INFO() << "Application requested call of the not allowed method: " << pres->acinfo.method;
                    AnyThread_sendError(stdreq, ApiError::NotAllowedError, std::string());
                    return boost::none;
                }
                else
                {
                    // parse failed, just log error and return. Error response is already sent back
                    LOG_ERROR() << "WebAPP API parse failed: " << request.toStdString();
                    return boost::none;
                }
            },
            [] (const boost::any&) {
            }
        );
    }

    void WebAPI_Beam::AnyThread_callWalletApiImp(const std::string& request)
    {
        //
        // Do not assume thread here
        // Should be safe to call from any thread
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
            [] (const boost::any&) {
            }
        );
    }

    void WebAPI_Beam::sendAPIResponse(const json& result)
    {
        //
        // This is reactor thread
        //
        AnyThread_sendAPIResponse(result);
    }

    void WebAPI_Beam::AnyThread_sendAPIResponse(const beam::wallet::json& result)
    {
        //
        // Do not assume thread here
        // Should be safe to call from any thread
        //
        auto str = result.dump();
        if (!str.empty())
        {
            emit callWalletApiResult(QString::fromStdString(str));
        }
    }

    void WebAPI_Beam::AnyThread_sendError(const std::string& request, beam::wallet::ApiError err, const std::string& message)
    {
        //
        // Do not assume thread here
        // Should be safe to call from any thread
        //
        const auto error = _walletAPI->fromError(request, err, message);
        emit callWalletApiResult(QString::fromStdString(error));
    }

    int WebAPI_Beam::test()
    {
        // only for test, always 42
        return 42;
    }

    void WebAPI_Beam::AnyThread_sendApproved(const std::string& request)
    {
        //
        // Do not assume thread here
        // Should be safe to call from any thread
        //
        AnyThread_callWalletApiImp(request);
    }

    void WebAPI_Beam::AnyThread_sendRejected(const std::string& request, ApiError code, const std::string& message)
    {
        //
        // Do not assume thread here
        // Should be safe to call from any thread
        //
        AnyThread_sendError(request, code, message);
    }

    void WebAPI_Beam::AnyThread_contractInfoApproved(const std::string& request)
    {
        //
        // Do not assume thread here
        // Should be safe to call from any thread
        //
        AnyThread_callWalletApiImp(request);
    }

    void WebAPI_Beam::AnyThread_contractInfoRejected(const std::string& request, ApiError code, const std::string& message)
    {
        //
        // Do not assume thread here
        // Should be safe to call from any thread
        //
        AnyThread_sendError(request, code, message);
    }
}
