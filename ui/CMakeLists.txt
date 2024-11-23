cmake_minimum_required(VERSION 3.13)

set (TARGET_NAME "beam-wallet-ui")

set(APP_NAME "Beam Wallet${BEAM_DISPLAY_SUFFIX}")
if(LINUX)
    set (OUTPUT_NAME "BeamWallet${BEAM_SUFFIX_UPPER_CASE}")
else()
    set (OUTPUT_NAME "Beam Wallet${BEAM_DISPLAY_SUFFIX}")
endif()
set (CMAKE_PREFIX_PATH $ENV{QT5_ROOT_DIR})

find_package(
    QT NAMES Qt6 Qt5
    QUIET COMPONENTS Core
)
if (NOT QT_FOUND)
    set(QT_VERSION_MAJOR 5)
endif()

set(BEAM_QT_MAJOR_VERSION ${QT_VERSION_MAJOR} CACHE STRING "Qt version to use (5 or 6), defaults to ${QT_VERSION_MAJOR}")

set(BEAM_QT_PREFIX Qt${BEAM_QT_MAJOR_VERSION})

function(BEAM_TRANSLATIONS_UPDATE_TS supported_langs out_ts_files)
    find_package(${BEAM_QT_PREFIX}LinguistTools)
    if (${BEAM_QT_PREFIX}LinguistTools_FOUND)
        if(LINUX)
            execute_process(COMMAND awk -F= "/^NAME/{print $2}" /etc/os-release 
                            OUTPUT_VARIABLE DISTR_NAME
                            OUTPUT_STRIP_TRAILING_WHITESPACE)
        endif()
        
    foreach(SUPPORTED_LANG ${supported_langs})
        set(TS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/i18n/${SUPPORTED_LANG}.ts")
        list(APPEND TS_FILES "${TS_FILE}")
        message("update: ${TS_FILE}")
        if (${DISTR_NAME} MATCHES "Fedora")
            execute_process(COMMAND lupdate-qt${BEAM_QT_MAJOR_VERSION} -recursive -locations none -no-obsolete ${CMAKE_CURRENT_SOURCE_DIR} -ts "${TS_FILE}")
        else()
            execute_process(COMMAND lupdate -recursive -locations none -no-obsolete ${CMAKE_CURRENT_SOURCE_DIR} -ts "${TS_FILE}")
        endif()
    endforeach()
    set(${out_ts_files} ${TS_FILES} PARENT_SCOPE)
    else()
        message("${BEAM_QT_PREFIX}LinguistTools not found, please check your Qt${BEAM_QT_MAJOR_VERSION} instalation")
    endif()
endfunction()

function(BEAM_TRANSLATIONS_COMPILE_QM ts_files out_qm_files)
    find_package(${BEAM_QT_PREFIX}LinguistTools)
    if (${BEAM_QT_PREFIX}LinguistTools_FOUND)
        if(LINUX)
            execute_process(COMMAND awk -F= "/^NAME/{print $2}" /etc/os-release
                            OUTPUT_VARIABLE DISTR_NAME
                            OUTPUT_STRIP_TRAILING_WHITESPACE)
        endif()
    foreach(TS_FILE ${ts_files})
        message("compile: ${TS_FILE}")
        get_filename_component(TS_FILE_WE ${TS_FILE} NAME_WE)
        set(QM_FILE_NAME "${TS_FILE_WE}.qm")
        list(APPEND QM_FILES "${QM_FILE_NAME}")
        if (${DISTR_NAME} MATCHES "Fedora")
            execute_process(COMMAND lrelease-qt${BEAM_QT_MAJOR_VERSION} -idbased "${TS_FILE}" -qm "${CMAKE_CURRENT_SOURCE_DIR}/${QM_FILE_NAME}")
        else()
            execute_process(COMMAND lrelease -idbased "${TS_FILE}" -qm "${CMAKE_CURRENT_SOURCE_DIR}/${QM_FILE_NAME}")
        endif()
        message("compiled translation: ${CMAKE_CURRENT_SOURCE_DIR}/${QM_FILE_NAME}")
    endforeach()
    set(${out_qm_files} ${QM_FILES} PARENT_SCOPE)
    else()
        message("${BEAM_QT_PREFIX}LinguistTools not found, please check your Qt${BEAM_QT_MAJOR_VERSION} instalation")
    endif()
endfunction()

find_package(${BEAM_QT_PREFIX} COMPONENTS Qml Quick Svg WebEngine WebEngineWidgets REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    else()
        set(SYSTEM_TYPE WIN32)
    endif()

    set_source_files_properties(ui.cpp PROPERTIES COMPILE_FLAGS "/wd4127") # QtCore/qvarlengtharray.h(274): warning C4127: conditional expression is constant
    set_source_files_properties(viewmodel/wallet/tx_object_list.cpp PROPERTIES COMPILE_FLAGS "/wd4127")
elseif(APPLE)
    set(SYSTEM_TYPE MACOSX_BUNDLE)
endif()

list(APPEND SUPPORTED_LANGS "be_BY")
list(APPEND SUPPORTED_LANGS "cs_CZ")
list(APPEND SUPPORTED_LANGS "de_DE")
list(APPEND SUPPORTED_LANGS "en_US")
list(APPEND SUPPORTED_LANGS "es_ES")
list(APPEND SUPPORTED_LANGS "fi_FI")
list(APPEND SUPPORTED_LANGS "fr_FR")
list(APPEND SUPPORTED_LANGS "id_ID")
list(APPEND SUPPORTED_LANGS "it_IT")
list(APPEND SUPPORTED_LANGS "ja_JP")
list(APPEND SUPPORTED_LANGS "ko_KR")
list(APPEND SUPPORTED_LANGS "nl_NL")
list(APPEND SUPPORTED_LANGS "pt_BR")
list(APPEND SUPPORTED_LANGS "ru_RU")
list(APPEND SUPPORTED_LANGS "sr_SR")
list(APPEND SUPPORTED_LANGS "sv_SE")
list(APPEND SUPPORTED_LANGS "th_TH")
list(APPEND SUPPORTED_LANGS "tr_TR")
list(APPEND SUPPORTED_LANGS "uk_UA")
list(APPEND SUPPORTED_LANGS "vi_VI")
list(APPEND SUPPORTED_LANGS "zh_CN")

set(UI_SRC
    ui.cpp

    viewmodel/applications/apps_server.h
    viewmodel/applications/apps_server.cpp
    viewmodel/applications/apps_view.h
    viewmodel/applications/apps_view.cpp
    viewmodel/applications/apps_api_ui.h
    viewmodel/applications/apps_api_ui.cpp
    viewmodel/applications/public.cpp
    viewmodel/applications/public.h
    viewmodel/applications/publishers_view.h
    viewmodel/applications/publishers_view.cpp
    viewmodel/helpers/list_model.h
    viewmodel/helpers/sortfilterproxymodel.h
    viewmodel/helpers/sortfilterproxymodel.cpp
    viewmodel/helpers/token_bootstrap_manager.h
    viewmodel/helpers/token_bootstrap_manager.cpp
    viewmodel/helpers/seed_validation_helper.h
    viewmodel/helpers/seed_validation_helper.cpp
    viewmodel/wallet/tx_object.cpp
    viewmodel/wallet/tx_object_list.cpp
    viewmodel/wallet/wallet_view.cpp
    viewmodel/wallet/tx_table.cpp
    viewmodel/atomic_swap/swap_utils.cpp
    viewmodel/atomic_swap/swap_eth_settings_item.cpp
    viewmodel/atomic_swap/seed_phrase_item.cpp
    viewmodel/atomic_swap/swap_offer_item.cpp
    viewmodel/atomic_swap/swap_offers_list.cpp
    viewmodel/atomic_swap/swap_settings_item.cpp
    viewmodel/atomic_swap/swap_tx_object.cpp
    viewmodel/atomic_swap/swap_tx_object_list.cpp
    viewmodel/atomic_swap/swap_offers_view.cpp
    viewmodel/atomic_swap/swap_token_item.cpp
    viewmodel/start_view.h
    viewmodel/start_view.cpp
    viewmodel/loading_view.h
    viewmodel/loading_view.cpp
    viewmodel/utxo/utxo_item.h
    viewmodel/utxo/utxo_item.cpp
    viewmodel/utxo/utxo_item_list.h
    viewmodel/utxo/utxo_item_list.cpp
    viewmodel/utxo/utxo_view.h
    viewmodel/utxo/utxo_view.cpp
    viewmodel/utxo/utxo_view_status.h
    viewmodel/utxo/utxo_view_type.h
    viewmodel/notifications/app_notification_helper.cpp
    viewmodel/notifications/notification_item.cpp
    viewmodel/notifications/notifications_list.cpp
    viewmodel/notifications/notifications_view.cpp
    viewmodel/notifications/notifications_settings.cpp
    viewmodel/notifications/push_notification_manager.cpp
        model/exchange_rates_manager.cpp
    viewmodel/main_view.h
    viewmodel/main_view.cpp
    viewmodel/help_view.h
    viewmodel/help_view.cpp
    viewmodel/settings_view.h
    viewmodel/settings_view.cpp
    viewmodel/address_item.h
    viewmodel/address_item.cpp
    viewmodel/address_book_view.h
    viewmodel/address_book_view.cpp
    viewmodel/asset_swap_accept_view.h
    viewmodel/asset_swap_accept_view.cpp
    viewmodel/asset_swap_create_view.h
    viewmodel/asset_swap_create_view.cpp
    viewmodel/fee_helpers.h
    viewmodel/fee_helpers.cpp
    viewmodel/ui_helpers.h
    viewmodel/ui_helpers.cpp
    viewmodel/messages_view.h
    viewmodel/messages_view.cpp
    viewmodel/statusbar_view.h
    viewmodel/statusbar_view.cpp
    viewmodel/theme.h
    viewmodel/theme.cpp
    viewmodel/receive_view.h
    viewmodel/receive_view.cpp
    viewmodel/send_view.h
    viewmodel/send_view.cpp
    viewmodel/settings_helpers.cpp
    viewmodel/payment_item.h
    viewmodel/payment_item.cpp
    viewmodel/qml_globals.h
    viewmodel/qml_globals.cpp
    viewmodel/receive_swap_view.h
    viewmodel/receive_swap_view.cpp
    viewmodel/currencies.h
    viewmodel/currencies.cpp
    viewmodel/send_swap_view.h
    viewmodel/send_swap_view.cpp
    viewmodel/el_seed_validator.h
    viewmodel/el_seed_validator.cpp
    viewmodel/wallet/token_item.h
    viewmodel/wallet/token_item.cpp
    viewmodel/wallet/assets_view.h
    viewmodel/wallet/assets_view.cpp
        model/assets_list.h
        model/assets_list.cpp
        model/asset_object.h
        model/asset_object.cpp
        model/assets_manager.h
        model/assets_manager.cpp
    viewmodel/applications/webapi_creator.cpp
    viewmodel/window_event_filter.h
    viewmodel/window_event_filter.cpp
    viewmodel/dex/dex_orders_model.h
    viewmodel/dex/dex_orders_model.cpp
    viewmodel/dex/dex_orders_list.h
    viewmodel/dex/dex_orders_list.cpp
    viewmodel/messenger_address_add.h
    viewmodel/messenger_address_add.cpp
    viewmodel/messenger_chat.h
    viewmodel/messenger_chat.cpp
    viewmodel/messenger_chat_list.h
    viewmodel/messenger_chat_list.cpp

    model/wallet_model.h
    model/wallet_model.cpp
    model/app_model.h
    model/app_model.cpp
    model/keyboard.h
    model/keyboard.cpp
    model/settings.h
    model/settings.cpp
    model/messages.h
    model/messages.cpp
    model/node_model.h
    model/node_model.cpp
    model/qr.h
    model/qr.cpp
    model/helpers.h
    model/translator.cpp
    model/translator.h
    model/swap_coin_client_model.cpp
    model/swap_coin_client_model.h
    model/swap_eth_client_model.cpp
    model/swap_eth_client_model.h
)

beam_translations_update_ts("${SUPPORTED_LANGS}" TS_FILES)
beam_translations_compile_qm("${TS_FILES}" QM_FILES)
   
set(TRANSLATIONS_QRC_FILE_NAME "translations.qrc")
set(TRANSLATIONS_QRC_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${TRANSLATIONS_QRC_FILE_NAME}")

file(WRITE "${TRANSLATIONS_QRC_FILE}" "<RCC>\n\t<qresource prefix=\"/translations\">\n")
foreach (QM_FILE ${QM_FILES})
    file(APPEND "${TRANSLATIONS_QRC_FILE}" "\t\t<file>${QM_FILE}</file>\n")
endforeach ()
file(APPEND "${TRANSLATIONS_QRC_FILE}" "\t</qresource>\n</RCC>")


find_package(Qt5QuickCompiler)
if(Qt5QuickCompiler_FOUND)
    qtquick_compiler_add_resources(QT_RESOURCES view/qml.qrc ${TRANSLATIONS_QRC_FILE})
else()
    qt5_add_resources(QT_RESOURCES view/qml.qrc ${TRANSLATIONS_QRC_FILE})
endif()

if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

add_definitions(-DAPP_NAME=\"${APP_NAME}\")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/icons/icon.ico
          ${CMAKE_CURRENT_SOURCE_DIR}/icons/beam.png
          ${CMAKE_CURRENT_SOURCE_DIR}/icons/beam.icns
     DESTINATION  ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE DAPPS_LIST ${CMAKE_CURRENT_SOURCE_DIR}/apps *.dapp)

string(REPLACE "." "," PROJECT_VERSION_VERSIONINFO "${PROJECT_VERSION}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/beam.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/beam.rc)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/beam.desktop.in ${CMAKE_CURRENT_SOURCE_DIR}/beam.desktop)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
if (APPLE)
    find_library(CARBON_LIBRARY Carbon)
    if(NOT CARBON_LIBRARY)
        message(FATAL_ERROR "Carbon framework not found")
    endif()
endif()
if(BEAM_USE_STATIC AND LINUX)
    set(CMAKE_C_FLAGS "-no-pie ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-no-pie ${CMAKE_CXX_FLAGS}")
endif()
if (APPLE)
    add_executable(${TARGET_NAME} ${SYSTEM_TYPE} ${UI_SRC} ${QT_RESOURCES} beam.rc "${CMAKE_CURRENT_SOURCE_DIR}/beam.icns" "${CMAKE_CURRENT_SOURCE_DIR}/dapps_store_app.wasm" ${DAPPS_LIST})
else()
    add_executable(${TARGET_NAME} ${SYSTEM_TYPE} ${UI_SRC} ${QT_RESOURCES} beam.rc "${CMAKE_CURRENT_SOURCE_DIR}/beam.icns")
endif()
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})

if(BEAM_HW_WALLET)
    target_compile_definitions(${TARGET_NAME} PRIVATE BEAM_HW_WALLET)
endif()

configure_file("${PROJECT_SOURCE_DIR}/version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/version.h")

add_definitions(-DBEAM_LIB_VERSION="${BEAM_VERSION}")
add_definitions(-DBEAM_CLIENT_VERSION="${PROJECT_VERSION}")

#qrcode library
target_link_libraries(${TARGET_NAME} qrcode cli)

# QHttpEngine library
target_link_libraries(${TARGET_NAME} qhttpengine)

if (LINUX)
    target_link_libraries(${TARGET_NAME} X11)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if (APPLE)
    target_link_libraries(${TARGET_NAME} ${CARBON_LIBRARY})
endif()

target_link_libraries(${TARGET_NAME} QuaZip::QuaZip)

#if (BEAM_WALLET_WITH_NODE)
    add_definitions(-DBEAM_WALLET_WITH_NODE)
    target_link_libraries(${TARGET_NAME}
            node 
            external_pow
    )
#endif()

target_link_libraries(${TARGET_NAME}
        beam
        wallet_client
        wallet_api
        mnemonic 
        ${BEAM_QT_PREFIX}::Qml
        ${BEAM_QT_PREFIX}::Quick 
        ${BEAM_QT_PREFIX}::Svg
        ${BEAM_QT_PREFIX}::WebEngine
        ${BEAM_QT_PREFIX}::WebEngineWidgets
)

if (BEAM_SIGN_PACKAGE AND WIN32)
    add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${SIGNTOOL_EXECUTABLE} sign ${SIGNTOOL_OPTIONS} "${CMAKE_CURRENT_BINARY_DIR}\\${OUTPUT_NAME}.exe"
    )
endif()

function(set_folder_resources_properties SOURCE_PATH DESTINATION PATTERN)
    file(GLOB_RECURSE _LIST CONFIGURE_DEPENDS ${SOURCE_PATH}/${PATTERN})
    foreach (RESOURCE ${_LIST})
        get_filename_component(_PARENT ${RESOURCE} DIRECTORY)
        if (${_PARENT} STREQUAL ${SOURCE_PATH})
            set(_DESTINATION ${DESTINATION})
        else ()
            file(RELATIVE_PATH _DESTINATION ${SOURCE_PATH} ${_PARENT})
            set(_DESTINATION ${DESTINATION}/${_DESTINATION})
        endif ()
        message("${RESOURCE}->${_DESTINATION}")
        set_source_files_properties(${RESOURCE} PROPERTIES MACOSX_PACKAGE_LOCATION ${_DESTINATION})
    endforeach (RESOURCE)
endfunction()

if(LINUX)
    install(TARGETS ${TARGET_NAME} DESTINATION bin)
    install(FILES dapps_store_app.wasm DESTINATION bin)
    install(DIRECTORY apps DESTINATION bin)

    SET(BEAM_DESKTOP_DIR     "${CMAKE_INSTALL_PREFIX}/share/applications/")
    SET(BEAM_PIXMAPS_DIR     "${CMAKE_INSTALL_PREFIX}/share/icons/")

    install (FILES beam.desktop DESTINATION ${BEAM_DESKTOP_DIR})
    install (FILES beam.png DESTINATION ${BEAM_PIXMAPS_DIR})
elseif(APPLE)
    set(MACOSX_BUNDLE_NAME "Beam Wallet${BEAM_DISPLAY_SUFFIX}")
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/beam.icns" PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/dapps_store_app.wasm" PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
    set_folder_resources_properties(${CMAKE_CURRENT_SOURCE_DIR}/apps Resources/apps *.dapp)
    set_target_properties(${TARGET_NAME} PROPERTIES 
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist")

    if (BEAM_SIGN_IDENTITY)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND macdeployqt
            "${CMAKE_CURRENT_BINARY_DIR}/Beam Wallet${BEAM_DISPLAY_SUFFIX}.app"
            -always-overwrite
            -qmldir=${CMAKE_CURRENT_SOURCE_DIR}
            -sign-for-notarization="${BEAM_SIGN_IDENTITY}"
            COMMENT "Deploying Qt..."
        )

        if(BEAM_SIGN_MACOS_BINARY)
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND codesign 
                --force
                --deep
                --verify
                --verbose
                --display
                -s "${BEAM_SIGN_IDENTITY}"
                -o runtime
                --timestamp
                #--entitlements ${CMAKE_CURRENT_SOURCE_DIR}/QtWebEngineProcess.entitlements
                "${CMAKE_CURRENT_BINARY_DIR}/Beam Wallet${BEAM_DISPLAY_SUFFIX}.app"
                COMMENT "Signing Beam Wallet${BEAM_DISPLAY_SUFFIX}.app..."
            )
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND codesign 
                --force
                --verify
                --verbose
                --display
                -s "${BEAM_SIGN_IDENTITY}"
                -o runtime
                --timestamp
                --entitlements ${CMAKE_CURRENT_SOURCE_DIR}/QtWebEngineProcess.entitlements
                "${CMAKE_CURRENT_BINARY_DIR}/Beam Wallet${BEAM_DISPLAY_SUFFIX}.app/Contents/Frameworks/QtWebEngineCore.framework/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess"
                COMMENT "Signing QtWebEngineProcess..."
            )
        endif()
    endif()
    install(TARGETS ${TARGET_NAME} DESTINATION .)
else()
    if(WIN32)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
            COMMAND windeployqt --release --no-virtualkeyboard --no-angle --dir "${CMAKE_CURRENT_BINARY_DIR}/windeployqt" --qmldir "${CMAKE_CURRENT_SOURCE_DIR}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/$<TARGET_FILE_NAME:${TARGET_NAME}>"
            COMMENT "Deploying Qt..."
        )
        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/windeployqt/ DESTINATION .)
            
        set(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_SKIP TRUE) # skip default behaviour
        include(InstallRequiredSystemLibraries) #dymanic CRT
        install(PROGRAMS ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS} DESTINATION .)
        
        install(FILES ${OPENSSL_ROOT_DIR}/bin/libcrypto-1_1-x64.dll DESTINATION .)
        install(FILES ${OPENSSL_ROOT_DIR}/bin/libssl-1_1-x64.dll DESTINATION .)
        install(FILES beam-wallet.cfg DESTINATION .)
        install(FILES dapps_store_app.wasm DESTINATION .)
        install(DIRECTORY apps DESTINATION .)
        install(TARGETS ${TARGET_NAME} DESTINATION .)
    endif()

    # Signing the installer
    #add_custom_target(signing-installer)
    #
    #message(BEAM_INSTALLER_NAME = ${BEAM_INSTALLER_NAME})
    #add_custom_command(
    #    TARGET signing-installer
    #    COMMAND ${SIGNTOOL_EXECUTABLE} sign ${SIGNTOOL_OPTIONS} ${CMAKE_CURRENT_BINARY_DIR}\\${BEAM_INSTALLER_NAME}
    #  )
endif()

if (BEAM_IPFS_SUPPORT AND WIN32)
    # TODO: This if hacky AF and beacuse we place everything in different folders.
    get_target_property(IPFS_DLL asio-ipfs IPFS_DLL)
    add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${IPFS_DLL} ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

if(NOT APPLE)
    add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/dapps_store_app.wasm ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()

add_custom_command(
    TARGET ${TARGET_NAME} POST_BUILD
    COMMENT "Copying apps..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/apps ${CMAKE_CURRENT_BINARY_DIR}/apps
)
