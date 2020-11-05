#include <QObject>
#include <QMessageBox>
#include <sstream>
#include "webapi_beam.h"
#include "utility/logger.h"

namespace beamui::applications {
    using namespace beam::wallet;

    namespace {
        WalletModel& getWallet() {
            return *AppModel::getInstance().getWallet();
        }

        IWalletModelAsync& getAsyncWallet() {
            return *getWallet().getAsync();
        }
    }

    WebAPI_Beam::WebAPI_Beam(QObject *parent)
            : QObject(parent)
    {
        LOG_INFO() << "WebAPI_Beam (BEAM channel object) created";
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

        auto& wallet = *AppModel::getInstance().getWallet();
        const std::string comment = appTitle.toStdString() + " Deposit";
        wallet.getAsync()->sendMoney(receiver, comment, amount, fee);

        return "";
    }
}
