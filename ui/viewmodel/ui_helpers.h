#pragma once
#include <QObject>
#include <QQmlListProperty>
#include "wallet/core/common.h"
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
#include "wallet/transactions/swaps/common.h"
#endif  // BEAM_ATOMIC_SWAP_SUPPORT
#include "wallet/client/extensions/news_channels/interface.h"
#include <type_traits>

Q_DECLARE_METATYPE(beam::wallet::TxID)
Q_DECLARE_METATYPE(beam::wallet::TxParameters)
Q_DECLARE_METATYPE(ECC::uintBig)

namespace beamui
{
    template<typename, typename = std::void_t<>>
    struct QmlListPropertyHasNewerConstructor : std::false_type
    {};

    template<typename T>
    struct QmlListPropertyHasNewerConstructor<T, std::void_t<decltype(T(nullptr, nullptr))>> : std::true_type
    {};

    template<typename T>
    QQmlListProperty<T> CreateQmlListProperty(QObject* obj, QList<T*>& list)
    {
        if constexpr (QmlListPropertyHasNewerConstructor<QQmlListProperty<T>>::value)
        {
            return QQmlListProperty<T>(obj, &list);
        }
        else
        {
            return QQmlListProperty<T>(obj, list);
        }
    }

    // UI labels all for Currencies elements

#define CURRENCY_MAP(macro) \
    /*    name         label            short label      subunit     fee unit   decimal places*/ \
    macro(Beam,        "BEAM",          "BEAM",          "GROTH",    "GROTH",     8) \
    macro(Bitcoin,     "Bitcoin",       "BTC",           "satoshi",  "sat/kB",    8) \
    macro(Litecoin,    "Litecoin",      "LTC",           "photon",   "ph/kB",     8) \
    macro(Qtum,        "QTUM",          "QTUM",          "qsatoshi", "qsat/kB",   8) \
    macro(Dogecoin,    "Dogecoin",      "DOGE",          "satoshi",  "sat/kB",    8) \
    macro(Dash,        "Dash",          "DASH",          "duff",     "duff/kB",   8) \
    macro(Usd,         "USD",           "USD",           "cent",     "",          2) \
    macro(Unknown,     "",              "",              "",         "",          0)

    // TODO roman.strilets deleted from CURRENCY_MAP
    //macro(BitcoinCash, "Bitcoin Cash",  "BCH",           "satoshi",  "sat/kB",    8)
    enum class Currencies
    {
#define MACRO(name, label, slabel, subUnit, feeLabel, dec) name,
        CURRENCY_MAP(MACRO)
#undef MACRO
    };

    QString toString(Currencies currency);
    std::string toStdString(Currencies currency);

    QString getCurrencyLabel(Currencies);
    QString getCurrencyLabel(beam::wallet::ExchangeRate::Currency);
    QString getFeeRateLabel(Currencies);
    QString getCurrencySubunitLabel(Currencies);
    QString getCurrencySubunitFromLabel(const QString& currLabel);

    /// Convert amount to ui string with "." as a separator. With the default @coinType, no currency label added.
    QString AmountToUIString(const beam::Amount& value, Currencies coinType = Currencies::Unknown);
    QString AmountInGrothToUIString(const beam::Amount& value);

    /// expects ui string with a "." as a separator
    beam::Amount UIStringToAmount(const QString& value);

    Currencies convertExchangeRateCurrencyToUiCurrency(beam::wallet::ExchangeRate::Currency);
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
    Currencies convertSwapCoinToCurrency(beam::wallet::AtomicSwapCoin coin);
#endif

    QString toString(const beam::wallet::WalletID&);
    QString toString(const beam::wallet::PeerID&);
    QString toString(const beam::Merkle::Hash&);
    QString toString(const beam::Timestamp& ts);

    class Filter
    {
    public:
        Filter(size_t size = 12);
        void addSample(double value);
        double getAverage() const;
        double getMedian() const;
    private:
        std::vector<double> _samples;
        size_t _index;
        bool _is_poor;
    };
    QDateTime CalculateExpiresTime(beam::Timestamp currentHeightTime, beam::Height currentHeight, beam::Height expiresHeight);
    QString getEstimateTimeStr(int estimate);
    QString convertBeamHeightDiffToTime(int32_t dt);

    beam::Version getCurrentLibVersion();
    quint32 getCurrentUIRevision();

}  // namespace beamui
