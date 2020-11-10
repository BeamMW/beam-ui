// Copyright 2019 The Beam Team
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
#include "qml_globals.h"
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>
#include "version.h"
#include "model/app_model.h"
#include "wallet/core/common.h"
#include "ui_helpers.h"
#include "wallet/client/extensions/offers_board/swap_offer_token.h"
#include "wallet/transactions/swaps/bridges/bitcoin/bitcoin_side.h"
#include "wallet/transactions/swaps/bridges/litecoin/litecoin_side.h"
#include "wallet/transactions/swaps/bridges/qtum/qtum_side.h"
#include "wallet/transactions/swaps/bridges/bitcoin_cash/bitcoin_cash_side.h"
#include "wallet/transactions/swaps/bridges/bitcoin_sv/bitcoin_sv_side.h"
#include "wallet/transactions/swaps/bridges/dash/dash_side.h"
#include "wallet/transactions/swaps/bridges/dogecoin/dogecoin_side.h"
#include "wallet/transactions/swaps/utils.h"
#include "utility/string_helpers.h"

#include <boost/algorithm/string.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include "3rdparty/libbitcoin/include/bitcoin/bitcoin/formats/base_10.hpp"

using boost::multiprecision::cpp_dec_float_50;

namespace
{
    constexpr uint8_t kBTCDecimalPlaces = libbitcoin::btc_decimal_places;
    constexpr uint8_t kUSDDecimalPlaces = 2;

    template <char C>
    bool char_is(const char c)
    {
        return c == C;
    }

    QString roundWithPrecision(const QString& number, uint8_t precision)
    {
        //TODO rounding precision
        const char delimeter = '.';
        auto parts = string_helpers::split(number.toStdString(), delimeter);

        std::string result;
        std::ostringstream oss;
        if (parts.size() == 2)
        {    
            cpp_dec_float_50 afterPoint("0." + parts[1]);

            std::ostringstream afterPointOss;
            afterPointOss.precision(precision);
            afterPointOss << std::fixed << afterPoint;

            auto afterPointParts = string_helpers::split(afterPointOss.str(), delimeter);
            oss << parts[0] << delimeter << (afterPointParts.size() > 1 ? afterPointParts[1] : "0");
            result = oss.str();
            boost::algorithm::trim_right_if(result, char_is<'0'>);
            boost::algorithm::trim_right_if(result, char_is<'.'>);
        }
        else
        {
            oss << parts[0];
            result = oss.str();
        }

        return QString::fromStdString(result);
    }

    QString multiplyWithPrecision(const QString& first, const QString& second, uint8_t precision)
    {
        cpp_dec_float_50 dec_first(first.toStdString().c_str());
        cpp_dec_float_50 dec_second(second.toStdString().c_str());

        cpp_dec_float_50 product = dec_first * dec_second;

        std::ostringstream oss;
        oss.precision(std::numeric_limits<cpp_dec_float_50>::digits10);
        oss << std::fixed << product;

        QString result = QString::fromStdString(oss.str());
        return roundWithPrecision(result, precision);
    }

    beamui::Currencies convertUiCurrencyToCurrencies(WalletCurrency::Currency currency)
    {
        switch (currency)
        {
            case Currency::CurrBeam:
                return beamui::Currencies::Beam;

            case Currency::CurrBitcoin:
                return beamui::Currencies::Bitcoin;

            case Currency::CurrLitecoin:
                return beamui::Currencies::Litecoin;

            case Currency::CurrQtum:
                return beamui::Currencies::Qtum;

            case Currency::CurrBitcoinCash:
                return beamui::Currencies::BitcoinCash;

            case Currency::CurrBitcoinSV:
                return beamui::Currencies::BitcoinSV;

            case Currency::CurrDash:
                return beamui::Currencies::Dash;

            case Currency::CurrDogecoin:
                return beamui::Currencies::Dogecoin;

            case Currency::CurrEthereum:
                return beamui::Currencies::Ethereum;

            case Currency::CurrDai:
                return beamui::Currencies::Dai;

            case Currency::CurrTether:
                return beamui::Currencies::Tether;

            case Currency::CurrWrappedBTC:
                return beamui::Currencies::WrappedBTC;

            default:
                return beamui::Currencies::Unknown;
        }
    }
}

QMLGlobals::QMLGlobals(QQmlEngine& engine)
    : _engine(engine)
{
}

void QMLGlobals::showMessage(const QString& message)
{
    QMessageBox::information(nullptr, "BeamWalletUI", message);
}

void QMLGlobals::copyToClipboard(const QString& text)
{
    QApplication::clipboard()->setText(text);
}


QString QMLGlobals::version()
{
    return QString::fromStdString(PROJECT_VERSION);
}

bool QMLGlobals::isTAValid(const QString& text)
{
    return QMLGlobals::isTransactionToken(text) || QMLGlobals::isAddress(text);
}

bool QMLGlobals::isAddress(const QString& text)
{
    return beam::wallet::CheckReceiverAddress(text.toStdString());
}

bool QMLGlobals::isTransactionToken(const QString& text)
{
    if (text.isEmpty()) return false;
    
    auto params = beam::wallet::ParseParameters(text.toStdString());
    return params && params->GetParameter<beam::wallet::TxType>(beam::wallet::TxParameterID::TransactionType);
}

bool QMLGlobals::isSwapToken(const QString& text)
{
    return beam::wallet::SwapOfferToken::isValid(text.toStdString());
}

QString QMLGlobals::getLocaleName()
{
    const auto& settings = AppModel::getInstance().getSettings();
    return settings.getLocale();
}

int QMLGlobals::maxCommentLength()
{
    return 1024;
}

bool QMLGlobals::isFeeOK(uint32_t fee, Currency currency, bool isShielded)
{
    switch (currency)
    {
    case Currency::CurrBeam: return fee >= minFeeBeam(isShielded);
    case Currency::CurrBitcoin:  return true;
    case Currency::CurrLitecoin:  return true;
    case Currency::CurrQtum: return true;
    case Currency::CurrBitcoinCash: return true;
    case Currency::CurrBitcoinSV: return true;
    case Currency::CurrDash: return true;
    case Currency::CurrDogecoin: return true;
    case Currency::CurrEthereum: return true;
    case Currency::CurrDai: return true;
    case Currency::CurrTether: return true;
    case Currency::CurrWrappedBTC: return true;
    default:
        return false;
    }
}

uint32_t QMLGlobals::minFeeBeam(bool isShielded)
{
    return isShielded ? beam::wallet::kShieldedTxMinFeeInGroth : beam::wallet::kMinFeeInGroth;
}

bool QMLGlobals::needPasswordToSpend()
{
    return AppModel::getInstance().getSettings().isPasswordReqiredToSpendMoney();
}

bool QMLGlobals::isPasswordValid(const QString& value)
{
    beam::SecString secretPass = value.toStdString();
    return AppModel::getInstance().checkWalletPassword(secretPass);
}

int QMLGlobals::getMinFeeOrRate(Currency currency)
{
    switch (currency) {
        case Currency::CurrBeam: return minFeeBeam();
        default: return 0;
    }
}

beam::wallet::AtomicSwapCoin QMLGlobals::convertCurrencyToSwapCoin(Currency currency)
{
    switch (currency)
    {
        case Currency::CurrBitcoin:
            return beam::wallet::AtomicSwapCoin::Bitcoin;
        case Currency::CurrLitecoin:
            return beam::wallet::AtomicSwapCoin::Litecoin;
        case Currency::CurrQtum:
            return beam::wallet::AtomicSwapCoin::Qtum;
        case Currency::CurrBitcoinCash:
            return beam::wallet::AtomicSwapCoin::Bitcoin_Cash;
        case Currency::CurrBitcoinSV:
            return beam::wallet::AtomicSwapCoin::Bitcoin_SV;
        case Currency::CurrDash:
            return beam::wallet::AtomicSwapCoin::Dash;
        case Currency::CurrDogecoin:
            return beam::wallet::AtomicSwapCoin::Dogecoin;
        case Currency::CurrEthereum:
            return beam::wallet::AtomicSwapCoin::Ethereum;
        case Currency::CurrDai:
            return beam::wallet::AtomicSwapCoin::Dai;
        case Currency::CurrTether:
            return beam::wallet::AtomicSwapCoin::Tether;
        case Currency::CurrWrappedBTC:
            return beam::wallet::AtomicSwapCoin::WBTC;
        default:
            return beam::wallet::AtomicSwapCoin::Unknown;
    }
}

bool QMLGlobals::isEthereumBased(Currency currency)
{
    switch (currency)
    {
    case Currency::CurrEthereum: return true;
    case Currency::CurrDai: return true;
    case Currency::CurrTether: return true;
    case Currency::CurrWrappedBTC: return true;
    default:
        return false;
    }
}

Currency QMLGlobals::convertSwapCoinToCurrency(beam::wallet::AtomicSwapCoin swapCoin)
{
    switch (swapCoin)
    {
        case beam::wallet::AtomicSwapCoin::Bitcoin:
            return Currency::CurrBitcoin;
        case beam::wallet::AtomicSwapCoin::Litecoin:
            return Currency::CurrLitecoin;
        case beam::wallet::AtomicSwapCoin::Qtum:
            return Currency::CurrQtum;
        case beam::wallet::AtomicSwapCoin::Bitcoin_Cash:
            return Currency::CurrBitcoinCash;
        case beam::wallet::AtomicSwapCoin::Bitcoin_SV:
            return Currency::CurrBitcoinSV;
        case beam::wallet::AtomicSwapCoin::Dash:
            return Currency::CurrDash;
        case beam::wallet::AtomicSwapCoin::Dogecoin:
            return Currency::CurrDogecoin;
        case beam::wallet::AtomicSwapCoin::Ethereum:
            return Currency::CurrEthereum;
        case beam::wallet::AtomicSwapCoin::Dai:
            return Currency::CurrDai;
        case beam::wallet::AtomicSwapCoin::Tether:
            return Currency::CurrTether;
        case beam::wallet::AtomicSwapCoin::WBTC:
            return Currency::CurrWrappedBTC;
        default:
            return Currency::CurrEnd;
    }
}

QString QMLGlobals::calcTotalFee(Currency currency, unsigned int feeRate)
{
    switch (currency) {
        case Currency::CurrBeam: {
            return QString::fromStdString(std::to_string(feeRate));
        }
        case Currency::CurrBitcoin: {
            auto total = beam::wallet::BitcoinSide::CalcTotalFee(feeRate);
            return QString::fromStdString(std::to_string(total)) + " sat";
        }
        case Currency::CurrLitecoin: {
            auto total = beam::wallet::LitecoinSide::CalcTotalFee(feeRate);
            return QString::fromStdString(std::to_string(total)) + " ph";
        }
        case Currency::CurrQtum: {
            auto total = beam::wallet::QtumSide::CalcTotalFee(feeRate);
            return QString::fromStdString(std::to_string(total)) + " qsat";
        }
        case Currency::CurrBitcoinCash: {
            auto total = beam::wallet::BitcoinCashSide::CalcTotalFee(feeRate);
            return QString::fromStdString(std::to_string(total)) + " sat";
        }
        case Currency::CurrBitcoinSV: {
            auto total = beam::wallet::BitcoinSVSide::CalcTotalFee(feeRate);
            return QString::fromStdString(std::to_string(total)) + " sat";
        }
        case Currency::CurrDash: {
            auto total = beam::wallet::DashSide::CalcTotalFee(feeRate);
            return QString::fromStdString(std::to_string(total)) + " duff";
        }
        case Currency::CurrDogecoin: {
            auto total = beam::wallet::DogecoinSide::CalcTotalFee(feeRate);
            return QString::fromStdString(std::to_string(total)) + " sat";
        }
        case Currency::CurrEthereum:
        case Currency::CurrDai:
        case Currency::CurrTether:
        case Currency::CurrWrappedBTC: {
            return QString();
        }
        default: {
            return QString();
        }
    }
}

QString QMLGlobals::calcFeeInSecondCurrency(int fee, const QString& exchangeRate, const QString& secondCurrencyLabel)
{
    QString feeInOriginalCurrency = beamui::AmountToUIString(fee);
    return calcAmountInSecondCurrency(feeInOriginalCurrency, exchangeRate, secondCurrencyLabel);
}

QString QMLGlobals::calcAmountInSecondCurrency(const QString& amount, const QString& exchangeRate, const QString& secondCurrLabel)
{
    if (exchangeRate.isEmpty() || exchangeRate == "0")
    {
        return "";
    }
    else
    {
#define MACRO(name, label, slabel, subunit, feeLabel, dec) \
        if (slabel == secondCurrLabel) \
        { \
            return multiplyWithPrecision(amount, exchangeRate, dec); \
        } 
        CURRENCY_MAP(MACRO)
#undef MACRO
    }
    return "";
}

bool QMLGlobals::canSwap()
{
    return haveSwapClient(Currency::CurrBitcoin) || haveSwapClient(Currency::CurrLitecoin) || haveSwapClient(Currency::CurrQtum)
        || haveSwapClient(Currency::CurrBitcoinCash) || haveSwapClient(Currency::CurrBitcoinSV) || haveSwapClient(Currency::CurrDash)
        || haveSwapClient(Currency::CurrDogecoin) || haveSwapClient(Currency::CurrEthereum);
}

bool QMLGlobals::haveSwapClient(Currency currency)
{
    auto swapCoin = convertCurrencyToSwapCoin(currency);
    if (isEthereumBased(currency))
    {
        return AppModel::getInstance().getSwapEthClient()->GetSettings().IsActivated();
    }
    return AppModel::getInstance().getSwapCoinClient(swapCoin)->GetSettings().IsActivated();
}

QString QMLGlobals::rawTxParametrsToTokenStr(const QVariant& variantTxParams)
{
    if (!variantTxParams.isNull() && variantTxParams.isValid())
    {
        auto txParameters = variantTxParams.value<beam::wallet::TxParameters>();
        return QString::fromStdString(std::to_string(txParameters));
    }
    return "";
}

bool QMLGlobals::canReceive(Currency currency)
{
    if (Currency::CurrBeam == currency)
    {
        return true;
    }

    auto swapCoin = convertCurrencyToSwapCoin(currency);
    if (isEthereumBased(currency))
    {
        auto client = AppModel::getInstance().getSwapEthClient();
        return client->GetSettings().IsActivated() && client->getStatus() == beam::ethereum::Client::Status::Connected;
    }
    auto client = AppModel::getInstance().getSwapCoinClient(swapCoin);
    return client->GetSettings().IsActivated() && client->getStatus() == beam::bitcoin::Client::Status::Connected;
}

QString QMLGlobals::getCurrencyLabel(Currency currency)
{
    beamui::Currencies currencyCommon = convertUiCurrencyToCurrencies(currency);
    return beamui::getCurrencyLabel(currencyCommon);
}

QString QMLGlobals::getCurrencyName(Currency currency)
{
    switch(currency)
    {
    case Currency::CurrBeam:
    {
        //% "BEAM"
        return qtTrId("general-beam");
    }
    case Currency::CurrBitcoin:
    {
        //% "Bitcoin"
        return qtTrId("general-bitcoin");
    }
    case Currency::CurrLitecoin:
    {
        //% "Litecoin"
        return qtTrId("general-litecoin");
    }
    case Currency::CurrQtum:
    {
        //% "QTUM"
        return qtTrId("general-qtum");
    }
    case Currency::CurrDogecoin:
    {
        //% "Dogecoin"
        return qtTrId("general-dogecoin");
    }
    case Currency::CurrBitcoinCash:
    {
        //% "Bitcoin Cash"
        return qtTrId("general-bitcoin-cash");
    }
    case Currency::CurrBitcoinSV:
    {
        //% "Bitcoin SV"
        return qtTrId("general-bitcoin-sv");
    }
    case Currency::CurrDash:
    {
        //% "DASH"
        return qtTrId("general-dash");
    }
    case Currency::CurrEthereum:
    case Currency::CurrDai:
    case Currency::CurrTether:
    case Currency::CurrWrappedBTC:
    {
        //% "ETHEREUM"
        return qtTrId("general-ethereum");
    }
    default:
    {
        assert(false && "unexpected swap coin!");
        return QString();
    }
    }
}

QString QMLGlobals::getFeeRateLabel(Currency currency)
{
    beamui::Currencies currencyCommon = convertUiCurrencyToCurrencies(currency);
    return beamui::getFeeRateLabel(currencyCommon);
}

QString QMLGlobals::getCurrencySubunitFromLabel(const QString& currLabel)
{
    return beamui::getCurrencySubunitFromLabel(currLabel);
}

unsigned int QMLGlobals::getMinimalFee(Currency currency, bool isShielded)
{
    if (Currency::CurrBeam == currency)
    {
        return minFeeBeam(isShielded);
    }

    if (isEthereumBased(currency))
    {
        // TODO roman.strilets
        return 0;
    }
    auto swapCoin = convertCurrencyToSwapCoin(currency);
    return AppModel::getInstance().getSwapCoinClient(swapCoin)->GetSettings().GetMinFeeRate();
}

unsigned int QMLGlobals::getRecommendedFee(Currency currency)
{
    if (Currency::CurrBeam == currency)
    {
        // TODO roman.strilets need to investigate
        return 0;
    }

    if (isEthereumBased(currency))
    {
        return AppModel::getInstance().getSwapEthClient()->getGasPrice();
    }

    auto swapCoin = convertCurrencyToSwapCoin(currency);
    return AppModel::getInstance().getSwapCoinClient(swapCoin)->getEstimatedFeeRate();
}

unsigned int QMLGlobals::getDefaultFee(Currency currency)
{
    if (Currency::CurrBeam == currency)
    {
        return minFeeBeam();
    }

    if (isEthereumBased(currency))
    {
        return AppModel::getInstance().getSwapEthClient()->getGasPrice();
    }

    auto swapCoin = convertCurrencyToSwapCoin(currency);
    return AppModel::getInstance().getSwapCoinClient(swapCoin)->getEstimatedFeeRate();
}

bool QMLGlobals::isSwapFeeOK(unsigned int amount, unsigned int fee, Currency currency)
{
    if (Currency::CurrBeam == currency)
    {
        return amount > fee && fee >= QMLGlobals::minFeeBeam();
    }

    // TODO roman.strilets maybe need to process exception?
    return beam::wallet::IsSwapAmountValid(convertCurrencyToSwapCoin(currency), amount, fee);
}

QString QMLGlobals::divideWithPrecision8(const QString& dividend, const QString& divider)
{
    cpp_dec_float_50 dec_dividend(dividend.toStdString().c_str());
    cpp_dec_float_50 dec_divider(divider.toStdString().c_str());

    cpp_dec_float_50 quotient = dec_dividend / dec_divider;

    std::ostringstream oss;
    oss.precision(std::numeric_limits<cpp_dec_float_50>::digits10);
    oss << std::fixed << quotient;

    QString result = QString::fromStdString(oss.str());
    return QMLGlobals::roundWithPrecision8(result);
}

QString QMLGlobals::multiplyWithPrecision8(const QString& first, const QString& second)
{
    return multiplyWithPrecision(first, second, kBTCDecimalPlaces);
}

QString QMLGlobals::roundWithPrecision8(const QString& number)
{
    return roundWithPrecision(number, kBTCDecimalPlaces);
}
