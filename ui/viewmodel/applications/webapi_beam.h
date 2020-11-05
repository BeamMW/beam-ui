#pragma once

#include "model/app_model.h"

namespace beamui::applications {
    class WebAPI_Beam : public QObject
    {
    Q_OBJECT
    public:
        explicit WebAPI_Beam(QObject *parent = nullptr);

    public slots:
       int test();
       void generatePermanentAddress(const QString& comment);
       QString sendBEAM(QString appTitle, QString address, double amount, double fee);

    private slots:
        // TODO: check that this is not exposed to JS
        void onGeneratedNewAddress(const beam::wallet::WalletAddress& walletAddr);
        void onAddresses(bool own, const std::vector<beam::wallet::WalletAddress>&);

    signals:
        void permanentAddressGenerated(const QString& address);

    private:
        std::string _addressLabel;
    };
}
