// Copyright 2018 The Beam Team
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

#include "main_view.h"
#include "model/app_model.h"
#include <QApplication>
#include <QClipboard>
#include "qml_globals.h"
#include "applications/public.h"

namespace
{
    const int msInMinute = 60 * 1000;
    const int LockTimeouts[] =
    {
        0 * msInMinute,
        1 * msInMinute,
        5 * msInMinute,
        15 * msInMinute,
        30 * msInMinute,
        60 * msInMinute,
    };

    const int minResetPeriodInMs = 1000;
}

MainViewModel::MainViewModel()
    : m_settings{AppModel::getInstance().getSettings()}
    , m_timer(this)
{
    m_timer.setSingleShot(true);
    
    auto walletModelPtr = AppModel::getInstance().getWalletModel().get();

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(lockWallet()));
    connect(&m_settings, SIGNAL(lockTimeoutChanged()), this, SLOT(onLockTimeoutChanged()));
    connect(walletModelPtr, &WalletModel::walletStatusChanged, this, &MainViewModel::unsafeTxCountChanged);
    connect(walletModelPtr, SIGNAL(transactionsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::TxDescription>&)), SIGNAL(unsafeTxCountChanged()));
    connect(walletModelPtr, SIGNAL(notificationsChanged(beam::wallet::ChangeAction, const std::vector<beam::wallet::Notification>&)), SIGNAL(unreadNotificationsChanged()));
#if defined(BEAM_HW_WALLET)
    connect(walletModelPtr, SIGNAL(showTrezorMessage()), this, SIGNAL(showTrezorMessage()));
    connect(walletModelPtr, SIGNAL(hideTrezorMessage()), this, SIGNAL(hideTrezorMessage()));
    connect(walletModelPtr, SIGNAL(showTrezorError(const QString&)), this, SIGNAL(showTrezorError(const QString&)));
#endif
    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(onClipboardDataChanged()));

    onLockTimeoutChanged();
    m_settings.maxPrivacyLockTimeLimitInit();
    m_settings.minConfirmationsInit();
}

void MainViewModel::update(int page)
{
	// TODO: update page model or smth...
}

void MainViewModel::lockWallet()
{
    emit gotoStartScreen();
}

void MainViewModel::onLockTimeoutChanged()
{
    int index = m_settings.getLockTimeout();

    assert(static_cast<size_t>(index) < sizeof(LockTimeouts) / sizeof(LockTimeouts[0]));

    if (index > 0)
    {
        m_timer.start(LockTimeouts[index]);
    }
    else
    {
        m_timer.stop();
    }
}

void MainViewModel::onClipboardDataChanged()
{
    auto text = QApplication::clipboard()->text();
    if (QMLGlobals::isToken(text))
    {
        //% "Address copied to clipboard"
        emit clipboardChanged(qtTrId("notification-address-copied"));
    }
}

void MainViewModel::resetLockTimer()
{
    if (m_timer.isActive() && (m_timer.interval() - m_timer.remainingTime() > minResetPeriodInMs))
    {
        m_timer.start();
    }
}

int MainViewModel::getUnsafeTxCount() const
{
    return static_cast<int>(AppModel::getInstance().getWalletModel()->getUnsafeActiveTransactionsCount());
}

int MainViewModel::getUnreadNotifications() const
{
    return static_cast<int>(AppModel::getInstance().getWalletModel()->getUnreadNotificationsCount());
}

QString MainViewModel::getDaoCoreAppID() const
{
    const std::string appName = "BEAM DAO CORE";
    std::string appURL  = "";

    #if defined(BEAM_TESTNET)
    appURL = "https://apps-testnet.beam.mw/app/plugin-dao-core/index.html";
    #elif defined(BEAM_MAINNET)
    appURL = "https://apps.beam.mw/app/plugin-dao-core/index.html";
    #else
    appURL = "http://3.19.141.112:80/app/plugin-dao-core/index.html";
    #endif

    const auto appid = beamui::applications::GenerateAppID(appName, appURL);
    return QString::fromStdString(appid);
}

QString MainViewModel::getFaucetAppID() const
{
    const std::string appName = "BEAM Faucet";
    std::string appURL  = "";

    #if defined(BEAM_TESTNET)
    appURL = "https://apps-testnet.beam.mw/app/plugin-faucet/index.html";
    #elif defined(BEAM_MAINNET)
    appURL = "https://apps.beam.mw/app/plugin-faucet/index.html";
    #else
    appURL = "http://3.19.141.112:80/app/plugin-faucet/index.html";
    #endif

    const auto appid = beamui::applications::GenerateAppID(appName, appURL);
    return QString::fromStdString(appid);
}
