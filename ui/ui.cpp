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

#include <QApplication>
#include <QtQuick>
#include <QQmlApplicationEngine>

#include <QInputDialog>
#include <QMessageBox>
// uncomment for QML profiling
//#include <QQmlDebuggingEnabler>
//QQmlDebuggingEnabler enabler;
#include <qqmlcontext.h>
#include "viewmodel/start_view.h"
#include "viewmodel/loading_view.h"
#include "viewmodel/main_view.h"
#include "viewmodel/utxo/utxo_view.h"
#include "viewmodel/utxo/utxo_view_status.h"
#include "viewmodel/utxo/utxo_view_type.h"
#include "viewmodel/atomic_swap/swap_offers_view.h"
#include "viewmodel/atomic_swap/swap_token_item.h"
#include "viewmodel/address_book_view.h"
#include "viewmodel/wallet/wallet_view.h"
#include "viewmodel/wallet/token_item.h"
#include "viewmodel/wallet/assets_view.h"
#include "viewmodel/wallet/tx_table.h"
#include "viewmodel/help_view.h"
#include "viewmodel/settings_view.h"
#include "viewmodel/messages_view.h"
#include "viewmodel/statusbar_view.h"
#include "viewmodel/theme.h"
#include "viewmodel/receive_view.h"
#include "viewmodel/receive_swap_view.h"
#include "viewmodel/send_view.h"
#include "viewmodel/send_swap_view.h"
#include "viewmodel/el_seed_validator.h"
#include "viewmodel/currencies.h"
#include "model/app_model.h"
#include "viewmodel/qml_globals.h"
#include "viewmodel/helpers/sortfilterproxymodel.h"
#include "viewmodel/helpers/token_bootstrap_manager.h"
#include "viewmodel/helpers/seed_validation_helper.h"
#include "viewmodel/notifications/notifications_view.h"
#include "viewmodel/notifications/push_notification_manager.h"
#include "viewmodel/notifications/exchange_rates_manager.h"
#include "wallet/core/wallet_db.h"
#include "utility/log_rotation.h"
#include "core/ecc_native.h"
#include "utility/cli/options.h"
#include "version.h"
#include "utility/helpers.h"
#include "model/translator.h"
#include "viewmodel/applications/public.h"
#include "model/qr.h"
#include "viewmodel/dex/dex_view.h"

#if defined(BEAM_USE_STATIC_QT)

#if defined Q_OS_WIN
Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#elif defined Q_OS_MACOS
Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin)
#elif defined Q_OS_LINUX
Q_IMPORT_PLUGIN(QtQmlPlugin)
Q_IMPORT_PLUGIN(QXcbIntegrationPlugin)
Q_IMPORT_PLUGIN(QXcbGlxIntegrationPlugin)
#endif

Q_IMPORT_PLUGIN(QtQuick2Plugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
Q_IMPORT_PLUGIN(QtQuickControls1Plugin)
Q_IMPORT_PLUGIN(QtQuickControls2Plugin)
Q_IMPORT_PLUGIN(QtGraphicalEffectsPlugin)
Q_IMPORT_PLUGIN(QtGraphicalEffectsPrivatePlugin)
Q_IMPORT_PLUGIN(QSvgPlugin)
Q_IMPORT_PLUGIN(QtQuickLayoutsPlugin)
Q_IMPORT_PLUGIN(QtQuickTemplates2Plugin)


#endif

#ifdef Q_OS_MACOS
#include <sys/types.h>
#include <sys/sysctl.h>
#endif // Q_OS_MACOS


using namespace beam;
using namespace std;
using namespace ECC;

int main (int argc, char* argv[])
{
    wallet::g_AssetsEnabled = true;

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    block_sigpipe();
    QApplication app(argc, argv);
    QApplication::setApplicationName(QMLGlobals::getAppName());
    QApplication::setWindowIcon(QIcon(Theme::iconPath()));
    QDir appDataDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));

    try
    {
        auto [options, visibleOptions] = createOptionsDescription(GENERAL_OPTIONS | UI_OPTIONS | WALLET_OPTIONS, WalletSettings::WalletCfg);
        visibleOptions;// unused
        po::variables_map vm;

        try
        {
#ifdef Q_OS_MACOS // on Big Sur we have broken current dir, let's restore it
            QDir t = app.applicationDirPath();
            if (t.dirName() == "MacOS" && t.cdUp() && t.dirName() == "Contents" && t.cdUp())
            {
                t.cdUp(); // Go up to the bundle parent directory
                QDir::setCurrent(t.absolutePath());
            }

            // workaround for https://github.com/BeamMW/beam-ui/issues/623
            auto isAppleTranslated = []() 
            {
                int    ret = 0;
                size_t size = sizeof(ret);
                if (sysctlbyname("sysctl.proc_translated", &ret, &size, NULL, 0) == -1)
                {
                    if (errno == ENOENT)
                        return 0;
                    return -1;
                }
                return ret;
            };
            if (isAppleTranslated() == 1)
            {
                LOG_INFO() << "You are on apple M1 chipset running an Intel application, forcing NativeTextRendering";
                QQuickWindow::setTextRenderType(QQuickWindow::TextRenderType::NativeTextRendering);
            }
#endif
            vm = getOptions(argc, argv, options, true);
        }
        catch (const po::error& e)
        {
            QMessageBox msgBox;
            msgBox.setText(e.what());
            msgBox.exec();
            return -1;
        }

        if (vm.count(cli::VERSION))
        {
            QMessageBox msgBox;
            msgBox.setText(PROJECT_VERSION.c_str());
            msgBox.exec();
            return 0;
        }

        if (vm.count(cli::GIT_COMMIT_HASH))
        {
            QMessageBox msgBox;
            msgBox.setText(GIT_COMMIT_HASH.c_str());
            msgBox.exec();
            return 0;
        }

        if (vm.count(cli::APPDATA_PATH))
        {
            const auto newPath = QString::fromStdString(vm[cli::APPDATA_PATH].as<string>());
            appDataDir.setPath(newPath);
        }

        int logLevel = getLogLevel(cli::LOG_LEVEL, vm, LOG_LEVEL_DEBUG);
        int fileLogLevel = getLogLevel(cli::FILE_LOG_LEVEL, vm, LOG_LEVEL_DEBUG);

        beam::Crash::InstallHandler(appDataDir.filePath(QMLGlobals::getAppName()).toStdString().c_str());

#define LOG_FILES_PREFIX "beam_ui_"

        const auto logFilesPath = appDataDir.filePath(WalletSettings::LogsFolder).toStdString();
        auto logger = beam::Logger::create(logLevel, logLevel, fileLogLevel, LOG_FILES_PREFIX, logFilesPath);

        unsigned logCleanupPeriod = vm[cli::LOG_CLEANUP_DAYS].as<uint32_t>() * 24 * 3600;

        clean_old_logfiles(logFilesPath, LOG_FILES_PREFIX, logCleanupPeriod);

        try
        {
            Rules::get().UpdateChecksum();
            LOG_INFO() << "Beam Wallet UI " << PROJECT_VERSION << " (" << BRANCH_NAME << ")";
            LOG_INFO() << "Beam Core " << BEAM_VERSION << " (" << BEAM_BRANCH_NAME << ")";
            LOG_INFO() << "Rules signature: " << Rules::get().get_SignatureStr();
            LOG_INFO() << "AppData folder: " << appDataDir.absolutePath().toStdString();

            // AppModel Model MUST BE created before the UI engine and destroyed after.
            // AppModel serves the UI and UI should be able to access AppModel at any time
            // even while being destroyed. Do not move engine above AppModel
            WalletSettings settings(appDataDir);
            LOG_INFO() << "WalletDB: " << settings.getWalletStorage();

            AppModel appModel(settings);
            QQmlApplicationEngine engine;
            Translator translator(settings, engine);
            
            if (settings.getNodeAddress().isEmpty())
            {
                if (vm.count(cli::NODE_ADDR))
                {
                    string nodeAddr = vm[cli::NODE_ADDR].as<string>();
                    settings.setNodeAddress(nodeAddr.c_str());
                }
            }

            qmlRegisterSingletonType<Theme>(
                    "Beam.Wallet", 1, 0, "Theme",
                    [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
                        Q_UNUSED(engine)
                        Q_UNUSED(scriptEngine)
                        return new Theme;
                    });

            qmlRegisterUncreatableType<OldWalletCurrency>("Beam.Wallet", 1, 0, "OldWalletCurrency", "You cannot create an instance of the Enums.");
            qRegisterMetaType<OldWalletCurrency::OldCurrency>("OldWalletCurrency::OldCurrency");

            qmlRegisterSingletonType<QMLGlobals>(
                    "Beam.Wallet", 1, 0, "BeamGlobals",
                    [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
                        Q_UNUSED(engine)
                        Q_UNUSED(scriptEngine)
                        return new QMLGlobals(*engine);
                    });

            qRegisterMetaType<beam::Asset::ID>("beam::Asset::ID");
            qRegisterMetaType<std::vector<beam::wallet::VerificationInfo>>("std::vector<beam::wallet::VerificationInfo>");
            qRegisterMetaType<beam::wallet::WalletAsset>("beam::wallet::WalletAsset");
            qmlRegisterType<StartViewModel>("Beam.Wallet", 1, 0, "StartViewModel");
            qmlRegisterType<LoadingViewModel>("Beam.Wallet", 1, 0, "LoadingViewModel");
            qmlRegisterType<MainViewModel>("Beam.Wallet", 1, 0, "MainViewModel");
            qmlRegisterType<WalletViewModel>("Beam.Wallet", 1, 0, "WalletViewModel");
            qmlRegisterType<TxTableViewModel>("Beam.Wallet", 1, 0, "TxTableViewModel");
            qmlRegisterType<AssetsViewModel>("Beam.Wallet", 1, 0, "AssetsViewModel");
            qmlRegisterUncreatableType<UtxoViewStatus>("Beam.Wallet", 1, 0, "UtxoStatus", "Not creatable as it is an enum type.");
            qmlRegisterUncreatableType<UtxoViewType>("Beam.Wallet", 1, 0, "UtxoType", "Not creatable as it is an enum type.");
            qmlRegisterType<UtxoViewModel>("Beam.Wallet", 1, 0, "UtxoViewModel");
            qmlRegisterType<SettingsViewModel>("Beam.Wallet", 1, 0, "SettingsViewModel");
            qmlRegisterType<AddressBookViewModel>("Beam.Wallet", 1, 0, "AddressBookViewModel");
            qmlRegisterType<SwapOffersViewModel>("Beam.Wallet", 1, 0, "SwapOffersViewModel");
            qmlRegisterType<NotificationsViewModel>("Beam.Wallet", 1, 0, "NotificationsViewModel");
            qmlRegisterType<HelpViewModel>("Beam.Wallet", 1, 0, "HelpViewModel");
            qmlRegisterType<MessagesViewModel>("Beam.Wallet", 1, 0, "MessagesViewModel");
            qmlRegisterType<StatusbarViewModel>("Beam.Wallet", 1, 0, "StatusbarViewModel");
            qmlRegisterType<ReceiveViewModel>("Beam.Wallet", 1, 0, "ReceiveViewModel");
            qmlRegisterType<ReceiveSwapViewModel>("Beam.Wallet", 1, 0, "ReceiveSwapViewModel");
            qmlRegisterType<SendViewModel>("Beam.Wallet", 1, 0, "SendViewModel");
            qmlRegisterType<SendSwapViewModel>("Beam.Wallet", 1, 0, "SendSwapViewModel");
            qmlRegisterType<ELSeedValidator>("Beam.Wallet", 1, 0, "ELSeedValidator");
            qmlRegisterType<AddressItem>("Beam.Wallet", 1, 0, "AddressItem");
            qmlRegisterType<ContactItem>("Beam.Wallet", 1, 0, "ContactItem");
            qmlRegisterType<UtxoItem>("Beam.Wallet", 1, 0, "UtxoItem");
            qmlRegisterType<PaymentInfoItem>("Beam.Wallet", 1, 0, "PaymentInfoItem");
            qmlRegisterType<WalletDBPathItem>("Beam.Wallet", 1, 0, "WalletDBPathItem");
            qmlRegisterType<SwapOfferItem>("Beam.Wallet", 1, 0, "SwapOfferItem");
            qmlRegisterType<SwapOffersList>("Beam.Wallet", 1, 0, "SwapOffersList");
            qmlRegisterType<SwapTokenInfoItem>("Beam.Wallet", 1, 0, "SwapTokenInfoItem");
            qmlRegisterType<SwapTxObjectList>("Beam.Wallet", 1, 0, "SwapTxObjectList");
            qmlRegisterType<TxObjectList>("Beam.Wallet", 1, 0, "TxObjectList");
            qmlRegisterType<AssetsList>("Beam.Wallet", 1, 0, "AssetsList");
            qmlRegisterType<TokenInfoItem>("Beam.Wallet", 1, 0, "TokenInfoItem");
            qmlRegisterType<SwapCoinClientWrapper>("Beam.Wallet", 1, 0, "SwapCoinClientWrapper");
            qmlRegisterType<TokenBootstrapManager>("Beam.Wallet", 1, 0, "TokenBootstrapManager");
            qmlRegisterType<PushNotificationManager>("Beam.Wallet", 1, 0, "PushNotificationManager");
            qmlRegisterType<ExchangeRatesManager>("Beam.Wallet", 1, 0, "ExchangeRatesManager");
            qmlRegisterType<SortFilterProxyModel>("Beam.Wallet", 1, 0, "SortFilterProxyModel");
            qmlRegisterType<SeedValidationHelper>("Beam.Wallet", 1, 0, "SeedValidationHelper");
            qmlRegisterType<QR>("Beam.Wallet", 1, 0, "QR");
            qmlRegisterType<beamui::dex::DexView>("Beam.Wallet", 1, 0, "DexViewModel");
            beamui::applications::RegisterQMLTypes();

            engine.load(QUrl("qrc:/root.qml"));
            if (engine.rootObjects().count() < 1)
            {
                LOG_ERROR() << "Problem with QT";
                return -1;
            }

            auto topLevel = engine.rootObjects().value(0);
            auto window = qobject_cast<QQuickWindow*>(topLevel);

            if (!window)
            {
                LOG_ERROR() << "Problem with QT";
                return -1;
            }

            window->setFlag(Qt::WindowFullscreenButtonHint);
            window->show();

            return QApplication::exec();
        }
        catch (const po::error& e)
        {
            LOG_ERROR() << e.what();
            return -1;
        }
    }
    catch (const std::exception& e)
    {
        QMessageBox msgBox;
        msgBox.setText(e.what());
        msgBox.exec();
        return -1;
    }
}
