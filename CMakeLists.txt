cmake_minimum_required(VERSION 3.19)
project(password_manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
# set(OPENSSL_INCLUDE_DIR "${OPENSSL_ROOT_DIR}/include")
# set(OPENSSL_LIBRARIES "${OPENSSL_ROOT_DIR}/lib")

# include_directories("C:/Program Files/OpenSSL-Win64/include")
# link_directories("C:/Program Files/OpenSSL-Win64/lib")

# find_package(OpenSSL REQUIRED)

find_path(LIBSODIUM_INCLUDE_DIR sodium.h PATHS "C:/libsodium/include")
find_library(LIBSODIUM_LIBRARY sodium PATHS "C:/libsodium/lib")
if(NOT LIBSODIUM_INCLUDE_DIR OR NOT LIBSODIUM_LIBRARY)
    message(FATAL_ERROR "libsodium not found")
endif()

include_directories(${LIBSODIUM_INCLUDE_DIR})

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Sql)
# find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(password_manager
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    passwordformdialog.h passwordformdialog.cpp
    passwordformdialog.h passwordformdialog.cpp
    passwordformdialog.h passwordformdialog.cpp passwordformdialog.ui
    selectpassworddialog.h selectpassworddialog.cpp selectpassworddialog.ui

    PasswordManager.h
    passwordmanager.h passwordmanager.cpp
    databasemanager.h databasemanager.cpp
    passwordmode.h
    exportpassworddialog.h exportpassworddialog.cpp exportpassworddialog.ui
    EncryptionUtils.cpp
    encryptionutils.h encryptionutils.cpp
    fileservice.h fileservice.cpp
    cryptodata.h
    logindialog.h logindialog.cpp logindialog.ui

)

qt_add_translations(
    TARGETS password_manager
    TS_FILES password_manager_pl_PL.ts
)

target_link_libraries(password_manager
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Sql
        ${LIBSODIUM_LIBRARY}
        # Qt6::QPSQLDriverPlugin
        # OpenSSL::SSL
        # OpenSSL::Crypto
        # libssl libcrypto
)

include(GNUInstallDirs)

install(TARGETS password_manager
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET password_manager
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})


# target_link_libraries(password_manager libssl libcrypto)

