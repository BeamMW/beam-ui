#pragma once
#include <QObject>
#include "wallet/core/common.h"
#ifdef BEAM_ATOMIC_SWAP_SUPPORT
#include "wallet/transactions/swaps/common.h"
#endif  // BEAM_ATOMIC_SWAP_SUPPORT
#include "wallet/client/extensions/news_channels/interface.h"

Q_DECLARE_METATYPE(beam::wallet::TxID)
Q_DECLARE_METATYPE(beam::wallet::TxParameters)
Q_DECLARE_METATYPE(ECC::uintBig)

namespace beamui
{
    // UI labels all for Currencies elements

#define CURRENCY_MAP(MACRO) \
    /*    name           label      subunit     fee unit     decimal places*/ \
    MACRO(Beam,         "BEAM",     "GROTH",    "GROTH",     8) \
    MACRO(Bitcoin,      "BTC",      "satoshi",  "sat/kB",    8) \
    MACRO(Litecoin,     "LTC",      "photon",   "ph/kB",     8) \
    MACRO(Qtum,         "QTUM",     "qsatoshi", "qsat/kB",   8) \
    MACRO(Usd,          "USD",      "cent",     "",          2) \
    MACRO(Unknown,      "",         "",         "",          0)

    enum class Currencies
    {
#define MACRO(name, label, subLabel, feeLabel, dec) name,
        CURRENCY_MAP(MACRO)
#undef MACRO
    };

    QString toString(Currencies currency);
    std::string toStdString(Currencies currency);

    QString getCurrencyLabel(Currencies);
    QString getCurrencyLabel(beam::wallet::ExchangeRate::Currency);
    QString getFeeRateLabel(Currencies);
    QString getCurrencySubunitLabel(Currencies);

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
