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

#include "model/app_model.h"
#include "apps_api_client.h"

namespace beamui::applications {
    class WebAPI_Beam : public QObject
    {
    Q_OBJECT
    public:
        explicit WebAPI_Beam(QObject *parent = nullptr);

    private slots:
        // TODO: check that this is not exposed to JS
        void onGeneratedNewAddress(const beam::wallet::WalletAddress& walletAddr);
        void onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>&);

    //
    // Slots below are called by web
    //
    public slots:
       int test();
       void generatePermanentAddress(const QString& comment);

       QString sendBEAM(QString appTitle, QString address, double amount, double fee);
       void callWalletApi(const QString& request);

    //
    // Signals are received by web
    //
    signals:
        void permanentAddressGenerated(const QString& address);
        void callWalletApiResult(const QString& result);

    private:
        std::string  _addressLabel;

        typedef std::shared_ptr<AppsApiClient> ApiClientPtr;
        typedef std::weak_ptr<AppsApiClient> WeakApiClientPtr;
        ApiClientPtr _apiClient;
    };
}
