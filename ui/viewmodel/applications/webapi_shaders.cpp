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
#include "webapi_shaders.h"

namespace beamui::applications
{
    WebAPI_Shaders::WebAPI_Shaders(IConsentHandler& consentHandler, const std::string &appid)
        : _consentHandler(consentHandler)
    {
        _realShaders = AppModel::getInstance().getWalletModel()->getAppsShaders(appid);
        _asyncWallet = AppModel::getInstance().getWalletModel()->getAsync();
    }

    void WebAPI_Shaders::CompileAppShader(const std::vector<uint8_t> &shader)
    {
        assert(_realShaders != nullptr);
        return _realShaders->CompileAppShader(shader);
    }

    void WebAPI_Shaders::CallShaderAndStartTx(const std::string &args, unsigned method, DoneAllHandler doneHandler)
    {
        //
        // This is reactor thread
        //
        assert(_realShaders != nullptr);

        std::weak_ptr<bool> wguard = _guard;
        _realShaders->CallShader(args, method, [this, doneHandler, wguard](
                boost::optional<beam::ByteBuffer> data,
                boost::optional<std::string> output,
                boost::optional<std::string> error)
        {
            if (error || !data.is_initialized())
            {
                return doneHandler(boost::none, std::move(output), std::move(error));
            }

            if (wguard.lock())
            {
                if (_approveData)
                {
                    // this should never happen, unexpected contract calls should be guarded by API
                    // if you're here something really really bad is going on
                    throw std::runtime_error("unexpected approve contract call");
                }

                 AppModel::getInstance().getWalletModel()->getCurrentHeight();

                _approveData = std::make_unique<ApproveData>(*data, output, doneHandler);
                _consentHandler.AnyThread_getContractConsent(*data);
            }
            else
            {
                LOG_WARNING() << "API destroyed before shader response received.";
                return;
            }
        });
    }

    void WebAPI_Shaders::AnyThread_contractApproved()
    {
        std::weak_ptr<bool> wguard = _guard;
        _asyncWallet->makeIWTCall([this, wguard] () -> boost::any {
            if (!wguard.lock())
            {
                LOG_WARNING() << "Wallet shaders destroyed while waiting for consent (y)";
                assert(false);
                return boost::none;
            }

            if (!_approveData)
            {
                // this should never happen
                // if you're here something really really bad is going on
                throw std::runtime_error("approve contract call but contract data is empty");
            }

            auto data = std::move(_approveData);
            _realShaders->ProcessTxData(data->data, [data] (
                    boost::optional<beam::wallet::TxID> txid,
                    boost::optional<std::string> error)
            {
                return data->doneHandler(std::move(txid), data->output, std::move(error));
            });

            return boost::none;
        }, [](const boost::any&){
            int a = 0;
            a++;
        });
    }

    void WebAPI_Shaders::AnyThread_contractRejected(bool byUser, const std::string& error)
    {
        std::weak_ptr<bool> wguard = _guard;
        _asyncWallet->makeIWTCall([this, wguard, byUser, error] () -> boost::any {
            if (!wguard.lock())
            {
                LOG_WARNING() << "Wallet shaders destroyed while waiting for consent (n)";
                assert(false);
                return boost::none;
            }

            if (!_approveData)
            {
                // this should never happen
                // if you're here something really really bad is going on
                throw std::runtime_error("reject contract call but contract data is empty");
            }

            auto data = std::move(_approveData);
            data->doneHandler(boost::none, data->output, byUser ? std::string("rejected by user") : error);
            return boost::none;

        }, [](const boost::any&){
        });
    }

    void WebAPI_Shaders::CallShader(const std::string &args, unsigned method, DoneCallHandler doneHandler)
    {
        throw std::runtime_error("CallShader should be never called in apps API");
    }

    void WebAPI_Shaders::ProcessTxData(const beam::ByteBuffer &data, DoneTxHandler doneHandler)
    {
        throw std::runtime_error("ProcessTxData should be never called in apps API");
    }

    bool WebAPI_Shaders::IsDone() const
    {
        assert(_realShaders != nullptr);
        return _realShaders->IsDone();
    }

    void WebAPI_Shaders::SetCurrentApp(const std::string &appid)
    {
        assert(_realShaders != nullptr);
        return _realShaders->SetCurrentApp(appid);
    }

    void WebAPI_Shaders::ReleaseCurrentApp(const std::string &appid)
    {
        assert(_realShaders != nullptr);
        return _realShaders->ReleaseCurrentApp(appid);
    }
}
