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
        _apiClient = std::make_shared<AppsApiClient>();
        connect(&getWallet(), &WalletModel::generatedNewAddress, this, &WebAPI_Beam::onGeneratedNewAddress);
        connect(&getWallet(), SIGNAL(addressesChanged(bool, const std::vector<beam::wallet::WalletAddress>&)), SLOT(onAddresses(bool, const std::vector<beam::wallet::WalletAddress>&)));
    }

    int WebAPI_Beam::test()
    {
        // only for test, always 42
        return 42;
    }

    void WebAPI_Beam::generatePermanentAddress(const QString& comment) {
        _addressLabel = comment.toStdString();
        getAsyncWallet().getAddresses(true);
    }

    void WebAPI_Beam::onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>& addrs)
    {
        assert(own);
        for(const auto& addr: addrs) {
            if (addr.m_label == _addressLabel) {
                // notify plugin
                auto saddr = std::to_string(addr.m_walletID);
                emit permanentAddressGenerated(QString(saddr.c_str()));
                return;
            }
        }
        // not found, make new
        getAsyncWallet().generateNewAddress();
    }

    void WebAPI_Beam::onGeneratedNewAddress(const WalletAddress& generatedAddr) {
        // save generated address
        WalletAddress newAddr = generatedAddr;
        newAddr.setLabel(_addressLabel);
        newAddr.m_duration = WalletAddress::AddressExpirationNever;
        getAsyncWallet().saveAddress(newAddr, true);
        // notify plugin
        auto addr = std::to_string(newAddr.m_walletID);
        emit permanentAddressGenerated(QString(addr.c_str()));
    }

    QString WebAPI_Beam::sendBEAM(QString appTitle, QString address, double damount, double dfee) {
        std::ostringstream ss;

        beam::Amount amount = static_cast<int>(damount);
        beam::Amount fee = static_cast<int>(dfee);

        ss << "<p style='font-size:15px'><b>" << appTitle.toStdString() << "</b> wants to send <b>" << amount / beam::Rules::Coin
           << " BEAM</b> to the following address:<br>" << address.toStdString()
           << "<br>Fee is <b>" << fee << " GROTH</b>"
           << "<p align='center'>Please confirm.</p></p>";

        QString message(ss.str().c_str());
        if (QMessageBox::StandardButton::Yes != QMessageBox::question(nullptr, "BEAM Applications", message)) {
            return "";
        }

        beam::wallet::WalletID receiver;
        receiver.FromHex(address.toStdString());

        auto& wallet = getWallet();
        const std::string comment = appTitle.toStdString() + " Deposit";
        wallet.getAsync()->sendMoney(receiver, comment, amount, fee);

        return "";
    }

    void WebAPI_Beam::callWalletApi(const QString& request)
    {
        WeakApiClientPtr wp = _apiClient;
        getAsyncWallet().makeIWTCall(
            [wp, request]() -> boost::any {
                if(auto sp = wp.lock())
                {
                    return sp->pluginApiRequest(request.toStdString());
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
                        emit callWalletApiResult(QString::fromStdString(apiResult));
                    }
                    catch (const boost::bad_any_cast &)
                    {
                        assert(false);
                    }
                }
                // this means that api is disconnected and destroyed already
                // this is not safe to use "this" here and actually nothing to do
            }
        );
    }
}
