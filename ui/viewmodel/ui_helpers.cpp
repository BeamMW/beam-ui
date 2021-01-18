#include "ui_helpers.h"
#include <QDateTime>
#include <QLocale>
#include <QTextStream>
#include "3rdparty/libbitcoin/include/bitcoin/bitcoin/formats/base_10.hpp"
#include "version.h"

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

    QString toString(const beam::wallet::PeerID& peerID)
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
    
    QString getCurrencyLabel(Currencies currency)
    {
        switch (currency)
        {
#define MACRO(name, label, slabel, subunit, feeLabel, dec) \
        case Currencies::name: \
            return QString(slabel); 
        CURRENCY_MAP(MACRO)
#undef MACRO
        default:
            return "";
        }
    }

    QString getCurrencyLabel(beam::wallet::ExchangeRate::Currency currency)
    {
        return getCurrencyLabel(convertExchangeRateCurrencyToUiCurrency(currency));
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
            return "";
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
        return "";
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

    /**
     *  Convert amount value to printable format.
     *  @value      Value in coin quants (satoshi, groth and s.o.). 
     *              Unsigned integer with the fixed decimal point.
     *              Decimal point position depends on @coinType.
     *  @coinType   Specify coint type.
     */
    QString AmountToUIString(const Amount& value, Currencies coinType, bool currencyLabel)
    {
        std::string amountString = libbitcoin::encode_base10(value, getCurrencyDecimals(coinType));
        QString amount = QString::fromStdString(amountString);
        QString coinLabel = getCurrencyLabel(coinType);

        if (coinLabel.isEmpty() || !currencyLabel)
        {
            return amount;
        }
        else
        {
            return amount + " " + coinLabel;
        }
    }

    QString AmountInGrothToUIString(const beam::Amount& value)
    {
        //% "GROTH"
        return QString("%1 %2").arg(value).arg(qtTrId("general-groth"));
    }

    beam::Amount UIStringToAmount(const QString& value, Currencies currency)
    {
        beam::Amount amount = 0;
        libbitcoin::decode_base10(amount, value.toStdString(), getCurrencyDecimals(currency));
        return amount;
    }

    QString toString(const beam::Timestamp& ts)
    {
        QDateTime datetime;
        datetime.setTime_t(ts);

        return datetime.toString(Qt::SystemLocaleShortDate);
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

    Currencies convertExchangeRateCurrencyToUiCurrency(beam::wallet::ExchangeRate::Currency currency)
    {
        switch (currency)
        {
        case wallet::ExchangeRate::Currency::Beam:
            return beamui::Currencies::Beam;
        case wallet::ExchangeRate::Currency::Bitcoin:
            return beamui::Currencies::Bitcoin;
        case wallet::ExchangeRate::Currency::Litecoin:
            return beamui::Currencies::Litecoin;
        case wallet::ExchangeRate::Currency::Qtum:
            return beamui::Currencies::Qtum;
        case wallet::ExchangeRate::Currency::Usd:
            return beamui::Currencies::Usd;
        case wallet::ExchangeRate::Currency::Dogecoin:
            return beamui::Currencies::Dogecoin;
        case wallet::ExchangeRate::Currency::Dash:
            return beamui::Currencies::Dash;
        case wallet::ExchangeRate::Currency::Ethereum:
            return beamui::Currencies::Ethereum;
        case wallet::ExchangeRate::Currency::Dai:
            return beamui::Currencies::Dai;
        case wallet::ExchangeRate::Currency::Usdt:
            return beamui::Currencies::Usdt;
        case wallet::ExchangeRate::Currency::WBTC:
            return beamui::Currencies::WrappedBTC;
        case wallet::ExchangeRate::Currency::Unknown:
        default:
            return beamui::Currencies::Unknown;
        }
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
        auto writeTime = [&ss](const auto& value, const auto& units)
        { 
            ss << value << " " << units;
        };
        if (estimate >= kSecondsInHour)
        {
            value = estimate / kSecondsInHour;
            //% "h"
            units = qtTrId("loading-view-estimate-hours");
            writeTime(value, units);

            estimate %= kSecondsInHour;
            value = estimate / kSecondsInMinute;

            estimate %= kSecondsInMinute;
            if (estimate)
            {
                ++value;
            }

            if (value >= 1)
            {
                //% "min"
                units = qtTrId("loading-view-estimate-minutes");
                ss << " ";
                writeTime(value, units);
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
            units = qtTrId("loading-view-estimate-minutes");
        }
        else if (estimate > kSecondsInMinute)
        {
            value = estimate / kSecondsInMinute;
            units = qtTrId("loading-view-estimate-minutes");
            writeTime(value, units);
            value = estimate - kSecondsInMinute;
            //% "sec"
            units = qtTrId("loading-view-estimate-seconds");
            ss << " ";
            writeTime(value, units);
            return res;
        }
        else
        {
            value = estimate > 0 ? estimate : 1;
            units = qtTrId("loading-view-estimate-seconds");
        }
        writeTime(value, units);
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

}  // namespace beamui
