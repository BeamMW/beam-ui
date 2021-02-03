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
        _apiClient = std::make_shared<AppsApiClient>(*static_cast<AppsApiClient::IHandler*>(this));
    }

    void WebAPI_Beam::callWalletApi(const QString& request)
    {
        WeakApiClientPtr wp = _apiClient;
        getAsyncWallet().makeIWTCall(
            [wp, request]() -> boost::any {
                if(auto sp = wp.lock())
                {
                    return sp->executeAPIRequest(request.toStdString());
                }
                // this means that api is disconnected and destroyed already
                // well, okay, nothing to do then
                return std::string();
            },
            [this, wp] (boost::any res) {
                if (auto sp = wp.lock())
                {
                    // it is safe to use "this" pointer here
                    try
                    {
                        auto apiResult = boost::any_cast<std::string>(res);
                        if (!apiResult.empty())
                        {
                            emit callWalletApiResult(QString::fromStdString(apiResult));
                        }
                    }
                    catch (const boost::bad_any_cast &)
                    {
                        // THIS SHOULD NEVER HAPPEN AND MEANS THAT THERE IS A CODING
                        // MISTAKE IN the executeAPIRequest method
                        assert(false);
                    }
                }
                // this means that api is disconnected and destroyed already
                // it is not safe to use "this" here and actually nothing to do
            }
        );
    }

    void WebAPI_Beam::onInvokeContract(const beam::wallet::JsonRpcId& id, const InvokeContract& data)
    {
        WeakApiClientPtr wp = _apiClient;
        getAsyncWallet().callShader(data.contract, data.args, [msgid = id, wp,  this] (const std::string& shaderError, const std::string& shaderResult, const TxID& txid)
        {
            if (auto sp = wp.lock())
            {
                nlohmann::json jsonRes;

                if (shaderError.empty())
                {
                    InvokeContract::Response result;
                    result.output = shaderResult;
                    result.txid = TxIDToString(txid);
                    sp->getAppsApiResponse(msgid, result, jsonRes);
                    sp->sendMessage(jsonRes);
                }
                else
                {
                    sp->sendError(msgid, ApiError::InternalErrorJsonRpc, shaderError);
                }
            }
            // this means that api is disconnected and destroyed already
            // this is not safe to use "this" here and actually nothing to do
        });
    }

    int WebAPI_Beam::test()
    {
        // only for test, always 42
        return 42;
    }
}
