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
#include "wallet/transactions/swaps/utils.h"
#include "utility/string_helpers.h"

#include <boost/algorithm/string.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include "3rdparty/libbitcoin/include/bitcoin/bitcoin/formats/base_10.hpp"

#include "fee_helpers.h"

using boost::multiprecision::cpp_dec_float_50;
using boost::multiprecision::cpp_int;

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

    beamui::Currencies convertUiCurrencyToCurrencies(OldWalletCurrency::OldCurrency currency)
    {
        switch (currency)
        {
            case OldWalletCurrency::OldCurrency::CurrBeam:
                return beamui::Currencies::Beam;

            case OldWalletCurrency::OldCurrency::CurrBitcoin:
                return beamui::Currencies::Bitcoin;

            case OldWalletCurrency::OldCurrency::CurrLitecoin:
                return beamui::Currencies::Litecoin;

            case OldWalletCurrency::OldCurrency::CurrQtum:
                return beamui::Currencies::Qtum;
#if defined(BITCOIN_CASH_SUPPORT)
            case OldWalletCurrency::OldCurrency::CurrBitcoinCash:
                return beamui::Currencies::BitcoinCash;
#endif // BITCOIN_CASH_SUPPORT
            case OldWalletCurrency::OldCurrency::CurrDash:
                return beamui::Currencies::Dash;

            case OldWalletCurrency::OldCurrency::CurrDogecoin:
                return beamui::Currencies::Dogecoin;

            case OldWalletCurrency::OldCurrency::CurrEthereum:
                return beamui::Currencies::Ethereum;

            case OldWalletCurrency::OldCurrency::CurrDai:
                return beamui::Currencies::Dai;

            case OldWalletCurrency::OldCurrency::CurrUsdt:
                return beamui::Currencies::Usdt;

            case OldWalletCurrency::OldCurrency::CurrWrappedBTC:
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

bool QMLGlobals::isToken(const QString& text)
{
    return beam::wallet::CheckReceiverAddress(text.toStdString());
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

bool QMLGlobals::needPasswordToSpend()
{
    return AppModel::getInstance().getSettings().isPasswordReqiredToSpendMoney();
}

bool QMLGlobals::isPasswordValid(const QString& value)
{
    beam::SecString secretPass = value.toStdString();
    return AppModel::getInstance().checkWalletPassword(secretPass);
}

QString QMLGlobals::calcWithdrawTxFee(OldWalletCurrency::OldCurrency currency, unsigned int feeRate)
{
    return ::calcWithdrawTxFee(currency, feeRate);
}

QString QMLGlobals::calcFeeInSecondCurrency(unsigned int fee, const QString& exchangeRate, const QString& secondCurrencyUnitName)
{
    QString feeInOriginalCurrency = beamui::AmountToUIString(fee);
    return calcAmountInSecondCurrency(feeInOriginalCurrency, exchangeRate, secondCurrencyUnitName);
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

QString QMLGlobals::roundUp(QString amount)
{
    if (amount.isEmpty() || amount == "0") {
        return amount;
    }

    const auto point = amount.indexOf('.');
    if (point == -1)
    {
        if (amount.length() < 4)
        {
            // cannot be divide by 1000
            return amount;
        }

        static const std::array<char, 3> postfixes = {'K', 'M', 'B'};
        char postfix = postfixes[0];

        for (size_t idx = 0; idx < postfixes.size(); ++idx)
        {
            if (amount[amount.length() - 1] == postfixes[idx])
            {
                if (idx == postfixes.size() - 1)
                {
                    // already rounded to max possible postfix
                    return amount;
                }
                amount = amount.left(amount.length() - 1);
                postfix = postfixes[idx + 1];
                break;
            }
        }

        const cpp_int original(amount.toStdString().c_str());
        const cpp_int rounded = original / 1000 + (original % 1000 > 0 ? 1 : 0);
        return QString(rounded.str().c_str()) + postfix;
    }
    else
    {
        const auto targetDecimals = amount.length() - point - 2;
        const cpp_dec_float_50 scale = pow(cpp_dec_float_50(10), targetDecimals);

        const cpp_dec_float_50 original(amount.toStdString().c_str());
        const cpp_dec_float_50 rounded = ceil(original * scale) / scale;

        auto roundedStr = rounded.str(targetDecimals, std::ios_base::fixed);
        boost::algorithm::trim_right_if(roundedStr, char_is<'0'>);
        boost::algorithm::trim_right_if(roundedStr, char_is<'.'>);

        return QString(roundedStr.c_str());
    }
}

bool QMLGlobals::canSwap()
{
    return haveSwapClient(OldWalletCurrency::OldCurrency::CurrBitcoin) || haveSwapClient(OldWalletCurrency::OldCurrency::CurrLitecoin) || haveSwapClient(OldWalletCurrency::OldCurrency::CurrQtum)
#if defined(BITCOIN_CASH_SUPPORT)
        || haveSwapClient(OldWalletCurrency::OldCurrency::CurrBitcoinCash)
#endif // BITCOIN_CASH_SUPPORT
        || haveSwapClient(OldWalletCurrency::OldCurrency::CurrDash) || haveSwapClient(OldWalletCurrency::OldCurrency::CurrDogecoin) || haveSwapClient(OldWalletCurrency::OldCurrency::CurrEthereum);
}

bool QMLGlobals::haveSwapClient(OldWalletCurrency::OldCurrency currency)
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

bool QMLGlobals::canReceive(OldWalletCurrency::OldCurrency currency)
{
    if (OldWalletCurrency::OldCurrency::CurrBeam == currency)
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

QString QMLGlobals::getBeamUnit() const
{
    return beamui::getCurrencyUnitName(beamui::Currencies::Beam);
}

QString QMLGlobals::getBeamFeeUnit() const
{
    return beamui::getFeeRateLabel(beamui::Currencies::Beam);
}

QString QMLGlobals::getCurrencyUnitName(OldWalletCurrency::OldCurrency currency)
{
    beamui::Currencies currencyCommon = convertUiCurrencyToCurrencies(currency);
    return beamui::getCurrencyUnitName(currencyCommon);
}

QString QMLGlobals::getCurrencyName(OldWalletCurrency::OldCurrency currency)
{
    switch(currency)
    {
    case OldWalletCurrency::OldCurrency::CurrBeam:
    {
        //% "BEAM"
        return qtTrId("general-beam");
    }
    case OldWalletCurrency::OldCurrency::CurrBitcoin:
    {
        //% "Bitcoin"
        return qtTrId("general-bitcoin");
    }
    case OldWalletCurrency::OldCurrency::CurrLitecoin:
    {
        //% "Litecoin"
        return qtTrId("general-litecoin");
    }
    case OldWalletCurrency::OldCurrency::CurrQtum:
    {
        //% "QTUM"
        return qtTrId("general-qtum");
    }
    case OldWalletCurrency::OldCurrency::CurrDogecoin:
    {
        //% "Dogecoin"
        return qtTrId("general-dogecoin");
    }
#if defined(BITCOIN_CASH_SUPPORT)
    case OldCurrency::CurrBitcoinCash:
    {
        //% "Bitcoin Cash"
        return qtTrId("general-bitcoin-cash");
    }
#endif // BITCOIN_CASH_SUPPORT
    case OldWalletCurrency::OldCurrency::CurrDash:
    {
        //% "DASH"
        return qtTrId("general-dash");
    }
    case OldWalletCurrency::OldCurrency::CurrEthereum:
    case OldWalletCurrency::OldCurrency::CurrDai:
    case OldWalletCurrency::OldCurrency::CurrUsdt:
    case OldWalletCurrency::OldCurrency::CurrWrappedBTC:
    {
        return qtTrId("general-ethereum").toUpper();
    }
    default:
    {
        assert(false && "unexpected swap coin!");
        return QString();
    }
    }
}

QString QMLGlobals::getFeeRateLabel(OldWalletCurrency::OldCurrency currency)
{
    beamui::Currencies currencyCommon = convertUiCurrencyToCurrencies(currency);
    return beamui::getFeeRateLabel(currencyCommon);
}

QString QMLGlobals::getCurrencySubunitFromLabel(const QString& currLabel)
{
    return beamui::getCurrencySubunitFromLabel(currLabel);
}

uint QMLGlobals::getCurrencyDecimals(OldWalletCurrency::OldCurrency currency)
{
    beamui::Currencies currencyCommon = convertUiCurrencyToCurrencies(currency);
    return beamui::getCurrencyDecimals(currencyCommon);
}

QString QMLGlobals::getMinimalFee(OldWalletCurrency::OldCurrency currency, bool isShielded)
{
    return QString::fromStdString(std::to_string(minimalFee(currency, isShielded)));
}

QString QMLGlobals::getMaximumFee(OldWalletCurrency::OldCurrency currency)
{
    return QString::fromStdString(std::to_string(maximumFee(currency)));
}

QString QMLGlobals::getRecommendedFee(OldWalletCurrency::OldCurrency currency)
{
    if (OldWalletCurrency::OldCurrency::CurrBeam == currency)
    {
        return QString::fromStdString(std::to_string(0));
    }

    if (isEthereumBased(currency))
    {
        return QString::fromStdString(std::to_string(AppModel::getInstance().getSwapEthClient()->getGasPrice()));
    }

    auto swapCoin = convertCurrencyToSwapCoin(currency);
    return QString::fromStdString(std::to_string(AppModel::getInstance().getSwapCoinClient(swapCoin)->getEstimatedFeeRate()));
}

QString QMLGlobals::getDefaultFee(OldWalletCurrency::OldCurrency currency)
{
    if (OldWalletCurrency::OldCurrency::CurrBeam == currency)
    {
        return QString::fromStdString(std::to_string(minFeeBeam()));
    }

    if (isEthereumBased(currency))
    {
        return QString::fromStdString(std::to_string(AppModel::getInstance().getSwapEthClient()->getGasPrice()));
    }

    auto swapCoin = convertCurrencyToSwapCoin(currency);
    return QString::fromStdString(std::to_string(AppModel::getInstance().getSwapCoinClient(swapCoin)->getEstimatedFeeRate()));
}

QString QMLGlobals::divideWithPrecision(const QString& dividend, const QString& divider, uint precision)
{
    cpp_dec_float_50 dec_dividend(dividend.toStdString().c_str());
    cpp_dec_float_50 dec_divider(divider.toStdString().c_str());

    cpp_dec_float_50 quotient = dec_dividend / dec_divider;

    std::ostringstream oss;
    oss.precision(std::numeric_limits<cpp_dec_float_50>::digits10);
    oss << std::fixed << quotient;

    QString result = QString::fromStdString(oss.str());
    return roundWithPrecision(result, static_cast<uint8_t>(precision));
}

QString QMLGlobals::multiplyWithPrecision(const QString& first, const QString& second, uint precision)
{
    cpp_dec_float_50 dec_first(first.toStdString().c_str());
    cpp_dec_float_50 dec_second(second.toStdString().c_str());

    cpp_dec_float_50 product = dec_first * dec_second;

    std::ostringstream oss;
    oss.precision(std::numeric_limits<cpp_dec_float_50>::digits10);
    oss << std::fixed << product;

    QString result = QString::fromStdString(oss.str());
    return roundWithPrecision(result, static_cast<uint8_t>(precision));
}

