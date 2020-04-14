cmake_minimum_required(VERSION 3.13)

cmake_policy(SET CMP0074 NEW)

if (BEAM_SIGN_PACKAGE AND WIN32)
  #  add_custom_target(signing ALL DEPENDS beam-wallet-ui COMMENT "Signing Package...")

    find_program(SIGNTOOL_EXECUTABLE signtool.exe)
    message("SignTool: ${SIGNTOOL_EXECUTABLE}" )

    set(WINDOWS_IDENTITIY_PFX_FILE "THE_PFX_FILE_IS_MISSING_HERE!" CACHE STRING "PFX file, which represents the identity of the developer.")
    set(WINDOWS_IDENTITIY_PFX_PASSWORD "" CACHE STRING "Password, which unlocks the PFX file (optional)")
    set(BEAM_SIGN_SUBJECT_NAME "" CACHE STRING "Beam subjet")
    set(SIGNTOOL_OPTIONS /tr http://timestamp.digicert.com /td sha256 /fd sha256 /n "${BEAM_SIGN_SUBJECT_NAME}")
    
    #set(SIGNTOOL_OPTIONS /fd SHA256 /a /f ${WINDOWS_IDENTITIY_PFX_FILE} /tr http://timestamp.digicert.com  )
    
    #if(NOT ${WINDOWS_IDENTITIY_PFX_PASSWORD} EQUAL "")
    #    set(SIGNTOOL_OPTIONS ${SIGNTOOL_OPTIONS} /p ${WINDOWS_IDENTITIY_PFX_PASSWORD})
    #else()
    #    message(FATAL_ERROR "You can't sign your executables without passing your password here!")
    #endif()

    #if(EXISTS ${WINDOWS_IDENTITIY_PFX_FILE})
    #    message(STATUS "Signing executables with: " ${WINDOWS_IDENTITIY_PFX_FILE})
    #    if(${WINDOWS_IDENTITIY_PFX_PASSWORD})
    #        message(WARNING "USE WITH CAUTION: Password for the PFX file has been set!")
    #    endif()
    #else()
    #    message(WARNING "Could not find the PFX file. Can not sign the executables!")
    #endif()
endif()

find_package(Git)
if(Git_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} status
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        ERROR_VARIABLE RESULT_STRING
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    string(LENGTH "${RESULT_STRING}" LENGTH_RESULT_STRING)

    if(${LENGTH_RESULT_STRING} EQUAL 0)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} log -1 --format=%H
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE GIT_COMMIT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list HEAD --count
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE VERSION_REVISION
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        set(APPVEYOR_REPO_BRANCH_EXISTS $ENV{APPVEYOR_REPO_BRANCH})
        set(TRAVIS_BRANCH_EXISTS $ENV{TRAVIS_BRANCH})

        if(NOT DEFINED BRANCH_NAME)
            execute_process(
                COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE BRANCH_NAME
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        endif()

    endif()
else()
    set(BRANCH_NAME "")
endif()

# should be before project
set(BEAM_WALLET_CLIENT_LIBRARY TRUE)
set(BEAM_TESTS_ENABLED FALSE)
add_subdirectory(beam)

message("BRANCH_NAME: ${BRANCH_NAME}")

project(beam-ui VERSION ${BEAM_VERSION}.${VERSION_REVISION})
set(VERSION_MAJOR ${BEAM_VERSION_MAJOR})
set(VERSION_MINOR ${BEAM_VERSION_MINOR})

message("BEAM_UI_VERSION: ${PROJECT_VERSION}")
message("BEAM_TARGET_SUFFIX: ${BEAM_TARGET_SUFFIX}")


file(WRITE ${PROJECT_SOURCE_DIR}/beam_version.gen "${PROJECT_VERSION}")

if(UNIX AND NOT APPLE AND NOT ANDROID)
    set(LINUX TRUE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4 /WX" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4100") # unreferenced formal parameter
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4324") # '__blake2b_state': structure was padded due to alignment specifier
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2")

    SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /INCREMENTAL:NO")
    SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /OPT:REF /INCREMENTAL:NO")
    SET(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} /OPT:REF /INCREMENTAL:NO")

    #target_compile_definitions(beam INTERFACE -D_WIN32_WINNT=0x0600)
    #add_definitions(-D_WIN32_WINNT=0x0600)

    if(BEAM_USE_STATIC)
       # target_compile_options(beam INTERFACE $<IF:$<CONFIG:Debug>,/MTd, /MT>)
       # target_link_options(beam INTERFACE $<IF:$<NOT:$<CONFIG:Debug>>, /OPT:REF /INCREMENTAL:NO, >)
        # use static runtime
        foreach(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
            flag_var CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)

            if(${flag_var} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
            endif(${flag_var} MATCHES "/MD")
        endforeach(flag_var)
    endif()
else()
    if(NOT ANDROID AND NOT IOS)
        if(ARCH_NATIVE)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=core2")
        endif()
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pthread")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb3")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

    # disable some warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual") # appears alot in our macro expansions
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-const-variable") # so what?
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function") # mostly in 3rd-party libs
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-value") # proto.h
endif()

add_subdirectory(3rdparty/qrcode)
add_subdirectory(3rdparty/quazip)

add_subdirectory(ui)

