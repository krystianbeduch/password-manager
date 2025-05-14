cmake_minimum_required(VERSION 3.19)
project(password_manager LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Sql)
# find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(password_manager
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    addpassworddialog.h addpassworddialog.cpp
    addpassworddialog.h addpassworddialog.cpp
    addpassworddialog.h addpassworddialog.cpp addpassworddialog.ui
    deletepassworddialog.h deletepassworddialog.cpp deletepassworddialog.ui

    PasswordManager.h
    passwordmanager.h passwordmanager.cpp
    databasemanager.h databasemanager.cpp
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
        # Qt6::QPSQLDriverPlugin
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
