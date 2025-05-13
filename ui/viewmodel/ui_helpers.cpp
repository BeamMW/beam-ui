#include "ui_helpers.h"
#include <QDateTime>
#include <QLocale>
#include <QTextStream>
#include "3rdparty/libbitcoin/include/bitcoin/bitcoin/formats/base_10.hpp"
#include "version.h"
#include "core/common.h"
#include "wallet/transactions/swaps/bridges/ethereum/common.h"
#include "utility/logger.h"
#include "utility/string_helpers.h"

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

     QString AmountBigToUIString(const beam::AmountBig::Number& value)
     {
        beam::wallet::PrintableAmount print(value, true);

        auto str = string_helpers::trimCommas(std::to_string(print));
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
        datetime.setSecsSinceEpoch(ts);
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
            expiresTime.setSecsSinceEpoch(currentHeightTime + (expiresHeight - currentHeight) * 60);
        }
        else
        {
            expiresTime.setSecsSinceEpoch(currentHeightTime - (currentHeight - expiresHeight) * 60);
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

    QString convertBeamHeightDiffToTime(int64_t dh)
    {
        if (dh <= 0)
        {
            return "";
        }
        const int32_t minute_s = 60;
        const int32_t quantum_s = 5 * minute_s;
        int64_t time_ms = dh * beam::Rules().DA.Target_ms;
        int time_s = static_cast<int>(time_ms / 1000);
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

    QString getReasonString(beam::wallet::TxFailureReason reason)
    {
    // clang doesn't allow to make 'auto reasons' so for the moment assertions below are a bit pointles
    // let's wait until they fix template arg deduction and restore it back
        static const std::array<QString, beam::wallet::TxFailureReason::Count> reasons = {
            //% "Unexpected reason, please send wallet logs to Beam support" 
            qtTrId("tx-failure-undefined"),
            //% "Transaction cancelled"
            qtTrId("tx-failure-cancelled"),
            //% "Receiver signature in not valid, please send wallet logs to Beam support"
            qtTrId("tx-failure-receiver-signature-invalid"),
            //% "Failed to register transaction with the blockchain, see node logs for details"
            qtTrId("tx-failure-not-registered-in-blockchain"),
            //% "Transaction is not valid, please send wallet logs to Beam support"
            qtTrId("tx-failure-not-valid"),
            //% "Invalid kernel proof provided"
            qtTrId("tx-failure-kernel-invalid"),
            //% "Failed to send Transaction parameters"
            qtTrId("tx-failure-parameters-not-sended"),
            //% "Not enough inputs to process the transaction"
            qtTrId("tx-failure-no-inputs"),
            //% "Address is expired"
            qtTrId("tx-failure-addr-expired"),
            //% "Failed to get transaction parameters"
            qtTrId("tx-failure-parameters-not-readed"),
            //% "Transaction timed out"
            qtTrId("tx-failure-time-out"),
            //% "Payment not signed by the receiver, please send wallet logs to Beam support"
            qtTrId("tx-failure-not-signed-by-receiver"),
            //% "Kernel maximum height is too high"
            qtTrId("tx-failure-max-height-to-high"),
            //% "Transaction has invalid state"
            qtTrId("tx-failure-invalid-state"),
            //% "Subtransaction has failed"
            qtTrId("tx-failure-subtx-failed"),
            //% "Contract's amount is not valid"
            qtTrId("tx-failure-invalid-contract-amount"),
            //% "Side chain has invalid contract"
            qtTrId("tx-failure-invalid-sidechain-contract"),
            //% "Side chain bridge has internal error"
            qtTrId("tx-failure-sidechain-internal-error"),
            //% "Side chain bridge has network error"
            qtTrId("tx-failure-sidechain-network-error"),
            //% "Side chain bridge has response format error"
            qtTrId("tx-failure-invalid-sidechain-response-format"),
            //% "Invalid credentials of Side chain"
            qtTrId("tx-failure-invalid-side-chain-credentials"),
            //% "Not enough time to finish btc lock transaction"
            qtTrId("tx-failure-not-enough-time-btc-lock"),
            //% "Failed to create multi-signature"
            qtTrId("tx-failure-create-multisig"),
            //% "Fee is too small"
            qtTrId("tx-failure-fee-too-small"),
            //% "Fee is too large"
            qtTrId("tx-failure-fee-too-large"),
            //% "Kernel's min height is unacceptable"
            qtTrId("tx-failure-kernel-min-height"),
            //% "Not a loopback transaction"
            qtTrId("tx-failure-loopback"),
            //% "Key keeper is not initialized"
            qtTrId("tx-failure-key-keeper-no-initialized"),
            //% "No valid asset id/asset owner id"
            qtTrId("tx-failure-invalid-asset-id"),
            //% "No asset info or asset info is not valid"
            qtTrId("tx-failure-asset-invalid-info"),
            //% "No asset metadata or asset metadata is not valid"
            qtTrId("tx-failure-asset-invalid-metadata"),
            //% "Invalid asset id"
            qtTrId("tx-failure-asset-invalid-id"),
            //% "Failed to receive asset confirmation"
            qtTrId("tx-failure-asset-confirmation"),
            //% "Asset is still in use (issued amount > 0)"
            qtTrId("tx-failure-asset-in-use"),
            //% "Asset is still locked"
            qtTrId("tx-failure-asset-locked"),
            //% "Asset registration fee is too small"
            qtTrId("tx-failure-asset-small-fee"),
            //% "Cannot issue/consume more than MAX_INT64 asset groth in one transaction"
            qtTrId("tx-failure-invalid-asset-amount"),
            //% "Some mandatory data for payment proof is missing"
            qtTrId("tx-failure-invalid-data-for-payment-proof"),
            //%  "Master key is needed for this transaction, but unavailable"
            qtTrId("tx-failure-there-is-no-master-key"),
            //% "Key keeper malfunctioned"
            qtTrId("tx-failure-keeper-malfunctioned"),
            //% "Aborted by the user"
            qtTrId("tx-failure-aborted-by-user"),
            //% "Asset has been already registered"
            qtTrId("tx-failure-asset-exists"),
            //% "Invalid asset owner id"
            qtTrId("tx-failure-asset-invalid-owner-id"),
            //% "Asset transactions are disabled in the wallet"
            qtTrId("tx-failure-assets-disabled"),
            //% "No voucher, no address to receive it"
            qtTrId("tx-failure-no-vouchers"),
            //% "Asset transactions are not available until fork2"
            qtTrId("tx-failure-assets-fork2"),
            //% "Key keeper out of slots"
            qtTrId("tx-failure-out-of-slots"),
            //% "Cannot extract shielded coin, fee is too big."
            qtTrId("tx-failure-shielded-coin-fee"),
            //% "Asset transactions are disabled in the receiver wallet"
            qtTrId("tx-failure-assets-disabled-receiver"),
            //% "Asset transactions are disabled in blockchain configuration"
            qtTrId("tx-failure-assets-disabled-blockchain"),
            //% "Peer wallet's signature required"
            qtTrId("tx-failure-identity-required"),
            //% "The sender cannot get vouchers for max privacy transaction"
            qtTrId("tx-failure-cannot-get-vouchers")
        };
        // ensure QString
        static_assert(std::is_same<decltype(reasons)::value_type, QString>::value);
        // ensure that we have all reasons, otherwise it would be runtime crash
        static_assert(std::tuple_size<decltype(reasons)>::value == static_cast<size_t>(beam::wallet::TxFailureReason::Count));

        assert(reasons.size() > static_cast<size_t>(reason));
        if (static_cast<size_t>(reason) >= reasons.size())
        {
            BEAM_LOG_WARNING()  << "Unknown failure reason code " << reason << ". Defaulting to 0";
            reason = beam::wallet::TxFailureReason::Unknown;
        }

        return reasons[reason];
    }

}  // namespace beamui
