#include "ui_helpers.h"
#include <QDateTime>
#include <QLocale>
#include <QTextStream>
#include <numeric>
#include "3rdparty/libbitcoin/include/bitcoin/bitcoin/formats/base_10.hpp"
#include "version.h"

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
            return "";
        }
    }

    QString getCurrencyUnitName(beam::wallet::ExchangeRate::Currency currency)
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

    /**
     *  Convert amount value to printable format.
     *  @value      Value in coin quants (satoshi, groth and s.o.).
     *              Unsigned integer with the fixed decimal point.
     *              Decimal point position depends on @coinType.
     *  @coinType   Specify coint type.
     */
    QString AmountToUIString(const Amount& value, Currencies coinType)
    {
        const auto decimals = (coinType == Currencies::Usd || coinType == Currencies::Beam) ? beamDecimals() : libbitcoin::btc_decimal_places;
        return AmountToUIString(value, getCurrencyUnitName(coinType), decimals);
    }

    QString AmountInGrothToUIString(const beam::Amount& value)
    {
        //% "GROTH"
        return QString("%1 %2").arg(value).arg(qtTrId("general-groth"));
    }

    beam::Amount UIStringToAmount(const QString& value)
    {
        beam::Amount amount = 0;
        libbitcoin::btc_to_satoshi(amount, value.toStdString());
        return amount;
    }

    QString toString(const beam::Timestamp& ts)
    {
        QDateTime datetime;
        datetime.setTime_t(ts);

        return datetime.toString(QLocale().dateTimeFormat(QLocale::ShortFormat));
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
        case wallet::AtomicSwapCoin::Bitcoin_Cash:
            return beamui::Currencies::BitcoinCash;
        case wallet::AtomicSwapCoin::Bitcoin_SV:
            return beamui::Currencies::BitcoinSV;
        case wallet::AtomicSwapCoin::Dash:
            return beamui::Currencies::Dash;
        case wallet::AtomicSwapCoin::Dogecoin:
            return beamui::Currencies::Dogecoin;
        case wallet::AtomicSwapCoin::Unknown:
        default:
            return beamui::Currencies::Unknown;
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
        case wallet::ExchangeRate::Currency::Unknown:
        default:
            return beamui::Currencies::Unknown;
        }
    }

    Filter::Filter(size_t size)
        : _samples(size, 0.0)
        , _index{0}
        , _is_poor{true}
    {
    }
    
    void Filter::addSample(double value)
    {
        _samples[_index] = value;
        _index = (_index + 1) % _samples.size();
        if (_is_poor)
        {
            _is_poor = _index + 1 < _samples.size();
        }
    }

    double Filter::getAverage() const
    {
        double sum = accumulate(_samples.begin(), _samples.end(), 0.0);
        return sum / (_is_poor ? _index : _samples.size());
    }

    double Filter::getMedian() const
    {
        vector<double> temp(_samples.begin(), _samples.end());
        size_t medianPos = (_is_poor ? _index : temp.size()) / 2;
        nth_element(temp.begin(),
                    temp.begin() + medianPos,
                    _is_poor ? temp.begin() + _index : temp.end());
        return temp[medianPos];
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
            case Currencies::BitcoinCash: return "bch";
            case Currencies::BitcoinSV: return "bsv";
            case Currencies::Dash: return "dash";
            case Currencies::Dogecoin: return "doge";
            case Currencies::Usd: return "usd";
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
