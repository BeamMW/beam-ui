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
#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include "currencies.h"

class QMLGlobals : public QObject
{
    Q_OBJECT
public:
    QMLGlobals(QQmlEngine&);

    Q_INVOKABLE static void showMessage(const QString& message);
    Q_INVOKABLE static void logInfo(const QString& message);
    Q_INVOKABLE static void copyToClipboard(const QString& text);
    Q_INVOKABLE QString version();
    Q_INVOKABLE static bool isToken(const QString& text);
    Q_INVOKABLE static bool isSwapToken(const QString& text);
    Q_INVOKABLE static QString getLocaleName();
    Q_INVOKABLE static int  maxCommentLength();
    Q_INVOKABLE static bool needPasswordToSpend();
    Q_INVOKABLE static bool isFork3();
    Q_INVOKABLE static bool isPasswordValid(const QString& value);
    Q_INVOKABLE static void fatal(const QString& message);

    // new currency utils
    Q_PROPERTY(QString beamUnit     READ getBeamUnit    CONSTANT)
    Q_PROPERTY(QString beamFeeUnit  READ getBeamFeeUnit CONSTANT)

    [[nodiscard]] QString getBeamUnit() const;
    [[nodiscard]] QString getBeamFeeUnit() const;

    // Currency utils
    // TODO maybe to need use beam::Amount instead of int
    Q_INVOKABLE static QString calcWithdrawTxFee(OldWalletCurrency::OldCurrency currency, unsigned int feeRate);
    Q_INVOKABLE static QString calcFeeInSecondCurrency(unsigned int fee, const QString& exchangeRate, const QString& secondCurrencyUnitName);
    Q_INVOKABLE static QString calcAmountInSecondCurrency(const QString& amount, const QString& exchangeRate, const QString& secondCurrUnitName);

    Q_INVOKABLE static QString roundUp(QString amount);
    Q_INVOKABLE static QString getCurrencyUnitName(OldWalletCurrency::OldCurrency);
    Q_INVOKABLE static QString getCurrencyName(OldWalletCurrency::OldCurrency);
    Q_INVOKABLE static QString getFeeRateLabel(OldWalletCurrency::OldCurrency);
    Q_INVOKABLE static QString getCurrencySubunitFromLabel(const QString& currLabel);
    Q_INVOKABLE static uint    getCurrencyDecimals(OldWalletCurrency::OldCurrency);
    
    Q_INVOKABLE static QString getMinimalFee(OldWalletCurrency::OldCurrency, bool isShielded);
    Q_INVOKABLE static QString getMaximumFee(OldWalletCurrency::OldCurrency);
    Q_INVOKABLE static QString getRecommendedFee(OldWalletCurrency::OldCurrency);
    Q_INVOKABLE static QString getDefaultFee(OldWalletCurrency::OldCurrency);

    // Swap & other currencies utils
    Q_INVOKABLE static bool canSwap();
    Q_INVOKABLE static bool haveSwapClient(OldWalletCurrency::OldCurrency);
    Q_INVOKABLE static QString rawTxParametrsToTokenStr(const QVariant& variantTxParams);
    Q_INVOKABLE static bool canReceive(OldWalletCurrency::OldCurrency currency);
    Q_INVOKABLE static QString divideWithPrecision(const QString& dividend, const QString& divider, uint precision);
    Q_INVOKABLE static QString multiplyWithPrecision(const QString& first, const QString& second, uint precision);
    Q_INVOKABLE static QString roundWithPrecision(const QString& number, uint precision);

private:
    QQmlEngine& _engine;
};
