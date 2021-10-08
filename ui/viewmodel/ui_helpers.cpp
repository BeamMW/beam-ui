#include "ui_helpers.h"
#include <QDateTime>
#include <QLocale>
#include <QTextStream>
#include "3rdparty/libbitcoin/include/bitcoin/bitcoin/formats/base_10.hpp"
#include "version.h"
#include "core/common.h"

#include "wallet/transactions/swaps/bridges/ethereum/common.h"

using namespace std;
using namespace beam;

namespace beamui
{
    QString toString(const beam::wallet::WalletID& walletID)
    {
        if (walletID != Zero)
        {
            auto id = std::to_string(walletID);
            return QString::fromStdString(id);
        }
        return "";
    }

    QString toString(const beam::PeerID& peerID)
    {
        if (peerID != Zero)
        {
            auto id = std::to_string(peerID);
            return QString::fromStdString(id);
        }
        return "";
    }

    QString toString(const beam::Merkle::Hash& walletID)
    {
        auto id = std::to_string(walletID);
        return QString::fromStdString(id);
    }
    
    QString getCurrencyUnitName(Currencies currency)
    {
        switch (currency)
        {
#define MACRO(name, label, slabel, subunit, feeLabel, dec) \
        case Currencies::name: \
            return QString(slabel); 
        CURRENCY_MAP(MACRO)
#undef MACRO
        default:
            return QString();
        }
    }

    QString getCurrencyUnitName(const beam::wallet::Currency& currency)
    {
        return getCurrencyUnitName(convertExchangeRateCurrencyToUiCurrency(currency));
    }

    QString getFeeRateLabel(Currencies currency)
    {
        switch (currency)
        {
#define MACRO(name, label, slabel, subunit, feeLabel, dec) \
        case Currencies::name: \
            return QString(feeLabel); 
        CURRENCY_MAP(MACRO)
#undef MACRO
        default:
            return "";
        }
    }

    QString getCurrencySubunitLabel(Currencies currency)
    {
        switch (currency)
        {
#define MACRO(name, label, slabel, subunit, feeLabel, dec) \
        case Currencies::name: \
            return QString(subunit); 
        CURRENCY_MAP(MACRO)
#undef MACRO
        default:
            return QString();
        }
    }

    QString getCurrencySubunitFromLabel(const QString& currLabel)
    {
#define MACRO(name, label, slabel, subunite, feeLabel, dec) \
        if (currLabel == slabel) \
        { \
            return subunite; \
        } 
        CURRENCY_MAP(MACRO)
#undef MACRO
        return QString();
    }

    // TODO(alex.starun): find better solution / mb use CURRENCY_MAP
    uint8_t getCurrencyDecimals(Currencies currency)
    {
        switch (currency)
        {
        case Currencies::Beam:
            static uint8_t beamDecimals = static_cast<uint8_t>(std::log10(Rules::Coin));
            return beamDecimals;
        case Currencies::Ethereum:
        case Currencies::Dai:
            return 9;
        case Currencies::Usdt:
            return 6;
        case Currencies::WrappedBTC:
            return libbitcoin::btc_decimal_places;
        default:
            return libbitcoin::btc_decimal_places;
        }
    }

    int beamDecimals()
    {
         static auto beamDecimals = static_cast<uint8_t>(std::log10(Rules::Coin));
         return beamDecimals;
    }

    QString AmountToUIString(const Amount& value, const QString& unitName, uint8_t decimalPlaces)
    {
        const auto samount = libbitcoin::encode_base10(value, decimalPlaces ? decimalPlaces : beamDecimals());
        return QString::fromStdString(samount) + (unitName.isEmpty() ? "" : " " + unitName);
    }

     QString AmountBigToUIString(const beam::AmountBig::Type& value)
     {
        beam::wallet::PrintableAmount print(value, true);

        std::ostringstream ss;
        ss << print;

        const auto str = ss.str();
        return QString::fromStdString(str);
     }

    /**
     *  Convert amount value to printable format.
     *  @value      Value in coin quants (satoshi, groth and s.o.).
     *              Unsigned integer with the fixed decimal point.
     *              Decimal point position depends on @coinType.
     *  @coinType   Specify coint type.
     */
    QString AmountToUIString(const Amount& value, Currencies coinType, bool currencyLabel)
    {
        return AmountToUIString(value, currencyLabel ? getCurrencyUnitName(coinType) : "", getCurrencyDecimals(coinType));
    }

    QString AmountInGrothToUIString(const beam::Amount& value)
    {
        //% "GROTH"
        return QString("%1 %2").arg(value).arg(qtTrId("general-groth"));
    }

    beam::Amount UIStringToAmount(const QString& value, Currencies currency)
    {
        beam::Amount amount = 0;
        libbitcoin::decode_base10(amount, value.toStdString(), getCurrencyDecimals(currency), true);
        return amount;
    }

    QString toString(const QDateTime& dt)
    {
        return dt.toString(QLocale().dateTimeFormat(QLocale::ShortFormat));
    }

    QString toString(const beam::Timestamp& ts)
    {
        QDateTime datetime;
        datetime.setTime_t(ts);
        return toString(datetime);
    }

#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    Currencies convertSwapCoinToCurrency(wallet::AtomicSwapCoin coin)
    {
        switch (coin)
        {
        case wallet::AtomicSwapCoin::Bitcoin:
            return beamui::Currencies::Bitcoin;
        case wallet::AtomicSwapCoin::Litecoin:
            return beamui::Currencies::Litecoin;
        case wallet::AtomicSwapCoin::Qtum:
            return beamui::Currencies::Qtum;
#if defined(BITCOIN_CASH_SUPPORT)
        case wallet::AtomicSwapCoin::Bitcoin_Cash:
            return beamui::Currencies::BitcoinCash;
#endif // BITCOIN_CASH_SUPPORT
        case wallet::AtomicSwapCoin::Dash:
            return beamui::Currencies::Dash;
        case wallet::AtomicSwapCoin::Dogecoin:
            return beamui::Currencies::Dogecoin;
        case wallet::AtomicSwapCoin::Ethereum:
            return beamui::Currencies::Ethereum;
        case wallet::AtomicSwapCoin::Dai:
            return beamui::Currencies::Dai;
        case wallet::AtomicSwapCoin::WBTC:
            return beamui::Currencies::WrappedBTC;
        case wallet::AtomicSwapCoin::Usdt:
            return beamui::Currencies::Usdt;
        case wallet::AtomicSwapCoin::Unknown:
        default:
            return beamui::Currencies::Unknown;
        }
    }

    beam::wallet::AtomicSwapCoin convertCurrenciesToSwapCoin(Currencies currency)
    {
        switch (currency)
        {
        case beamui::Currencies::Bitcoin:
            return beam::wallet::AtomicSwapCoin::Bitcoin;
        case beamui::Currencies::Litecoin:
            return beam::wallet::AtomicSwapCoin::Litecoin;
        case beamui::Currencies::Qtum:
            return beam::wallet::AtomicSwapCoin::Qtum;
#if defined(BITCOIN_CASH_SUPPORT)
        case beamui::Currencies::BitcoinCash:
            return beam::wallet::AtomicSwapCoin::Bitcoin_Cash;
#endif // BITCOIN_CASH_SUPPORT
        case beamui::Currencies::Dogecoin:
            return beam::wallet::AtomicSwapCoin::Dogecoin;
        case beamui::Currencies::Dash:
            return beam::wallet::AtomicSwapCoin::Dash;
        case beamui::Currencies::Ethereum:
            return beam::wallet::AtomicSwapCoin::Ethereum;
        case beamui::Currencies::Dai:
            return beam::wallet::AtomicSwapCoin::Dai;
        case beamui::Currencies::Usdt:
            return beam::wallet::AtomicSwapCoin::Usdt;
        case beamui::Currencies::WrappedBTC:
            return beam::wallet::AtomicSwapCoin::WBTC;
        default:
            return beam::wallet::AtomicSwapCoin::Unknown;
        }
    }
#endif  // BEAM_ATOMIC_SWAP_SUPPORT

    Currencies convertExchangeRateCurrencyToUiCurrency(const beam::wallet::Currency& currency)
    {
        if (currency == wallet::Currency::BEAM())
        {
            return beamui::Currencies::Beam;
        }

        if (currency == wallet::Currency::BTC())
        {
            return beamui::Currencies::Bitcoin;
        }

        if (currency == wallet::Currency::LTC())
        {
            return beamui::Currencies::Litecoin;
        }

        if (currency == wallet::Currency::LTC())
        {
            return beamui::Currencies::Litecoin;
        }
        if (currency == wallet::Currency::QTUM())
        {
            return beamui::Currencies::Qtum;
        }

        if (currency == wallet::Currency::USD())
        {
            return beamui::Currencies::Usd;
        }

        if (currency == wallet::Currency::DOGE())
        {
            return beamui::Currencies::Dogecoin;
        }

        if (currency == wallet::Currency::DASH())
        {
            return beamui::Currencies::Dash;
        }

        if (currency == wallet::Currency::DASH())
        {
            return beamui::Currencies::Dash;
        }

        if (currency == wallet::Currency::ETH())
        {
            return beamui::Currencies::Ethereum;
        }

        if (currency == wallet::Currency::DAI())
        {
            return beamui::Currencies::Dai;
        }

        if (currency == wallet::Currency::USDT())
        {
            return beamui::Currencies::Usdt;
        }

        if (currency == wallet::Currency::WBTC())
        {
            return beamui::Currencies::WrappedBTC;
        }

        return beamui::Currencies::Unknown;
    }

    QDateTime CalculateExpiresTime(beam::Timestamp currentHeightTime, beam::Height currentHeight, beam::Height expiresHeight)
    {
        QDateTime expiresTime = QDateTime();

        if (currentHeight <= expiresHeight)
        {
            expiresTime.setTime_t(currentHeightTime + (expiresHeight - currentHeight) * 60);
        }
        else
        {
            expiresTime.setTime_t(currentHeightTime - (currentHeight - expiresHeight) * 60);
        }
        
        return expiresTime;
    }

    QString getEstimateTimeStr(int estimate)
    {
        const int kSecondsInMinute = 60;
        const int kSecondsInHour = 60 * kSecondsInMinute;
        int value = 0;
        QString res;
        QTextStream ss(&res);
        QString units;

        if (estimate >= kSecondsInHour)
        {
            value = estimate / kSecondsInHour;
            //% "%n hour(s)"
            ss << qtTrId("loading-view-estimate-hours", value);

            estimate %= kSecondsInHour;
            value = estimate / kSecondsInMinute;

            estimate %= kSecondsInMinute;
            if (estimate)
            {
                ++value;
            }

            if (value >= 1)
            {
                //% "%n minute(s)"
                ss << " " << qtTrId("loading-view-estimate-minutes", value);
            }

            return res;
        }
        else if (estimate > 100)
        {
            value = estimate / kSecondsInMinute;
            estimate %= kSecondsInMinute;
            if (estimate)
            {
                ++value;
            }
            ss << qtTrId("loading-view-estimate-minutes", value);
            
        }
        else if (estimate > kSecondsInMinute)
        {
            value = estimate / kSecondsInMinute;
            ss << qtTrId("loading-view-estimate-minutes", value);
            value = estimate - kSecondsInMinute;
            //% "%n second(s)"
            ss << " " << qtTrId("loading-view-estimate-seconds", value);
            return res;
        }
        else
        {
            //% "less than a minute"
            res = qtTrId("loading-view-less-than-minute");
            return res;
        }
        return res;
    }

    QString toString(Currencies currency)
    {
        switch(currency)
        {
            case Currencies::Beam: return "beam";
            case Currencies::Bitcoin: return "btc";
            case Currencies::Litecoin: return "ltc";
            case Currencies::Qtum: return "qtum";
#if defined(BITCOIN_CASH_SUPPORT)
            case Currencies::BitcoinCash: return "bch";
#endif // BITCOIN_CASH_SUPPORT
            case Currencies::Dash: return "dash";
            case Currencies::Dogecoin: return "doge";
            case Currencies::Usd: return "usd";
            case Currencies::Ethereum: return "eth";
            case Currencies::Dai: return "dai";
            case Currencies::Usdt: return "usdt";
            case Currencies::WrappedBTC: return "wbtc";
            default: return "unknown";
        }
    }

    std::string toStdString(Currencies currency)
    {
        return toString(currency).toStdString();
    }

    QString convertBeamHeightDiffToTime(int32_t dt)
    {
        if (dt <= 0)
        {
            return "";
        }
        const int32_t minute_s = 60;
        const int32_t quantum_s = 5 * minute_s;
        int32_t time_s = dt * beam::Rules().DA.Target_s;
        time_s = (time_s + (quantum_s >> 1)) / quantum_s;
        time_s *= quantum_s;
        return beamui::getEstimateTimeStr(time_s);
    }

    beam::Version getCurrentLibVersion()
    {
        beam::Version ver;
        return ver.from_string(BEAM_VERSION) ? ver : beam::Version();
    }

    quint32 getCurrentUIRevision()
    {
        return VERSION_REVISION;
    }

    QString GetTokenTypeUIString(const std::string& token, bool choiceOffline)
    {
        using namespace beam::wallet;
        const auto type = GetAddressType(token);

        if (type == TxAddressType::Offline && choiceOffline)
        {
            //% "Offline"
            return qtTrId("tx-address-offline");
        }

        if (type == TxAddressType::PublicOffline)
        {
            //% "Public offline"
            return qtTrId("tx-address-public-offline");
        }

        if (type == TxAddressType::MaxPrivacy)
        {
            //% "Maximum anonymity"
            return qtTrId("tx-max-privacy");
        }

        //% "Online"
        return qtTrId("tx-regular");
    }

}  // namespace beamui
