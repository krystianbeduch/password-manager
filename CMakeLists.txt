cmake_minimum_required(VERSION 3.19)
project(password_manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ======================
# libsodium configuration
# ======================

find_path(LIBSODIUM_INCLUDE_DIR sodium.h PATHS "C:/libsodium/include")
find_library(LIBSODIUM_LIBRARY sodium PATHS "C:/libsodium/lib")
if(NOT LIBSODIUM_INCLUDE_DIR OR NOT LIBSODIUM_LIBRARY)
    message(FATAL_ERROR "libsodium not found")
endif()

include_directories(${LIBSODIUM_INCLUDE_DIR})

# ======================
# Qt configuration
# ======================

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Sql)
qt_standard_project_setup()

# ======================
# Source directories
# ======================

add_subdirectory(models)
add_subdirectory(database)
add_subdirectory(encryption)
# add_subdirectory(dialogs)
add_subdirectory(services)
# add_subdirectory(ui)

# ======================
# Executable
# ======================

qt_add_executable(password_manager
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    passwordformdialog.h passwordformdialog.cpp passwordformdialog.ui
    selectpassworddialog.h selectpassworddialog.cpp selectpassworddialog.ui
    exportpassworddialog.h exportpassworddialog.cpp exportpassworddialog.ui
    logindialog.h logindialog.cpp logindialog.ui
    resources.qrc
)

qt_add_translations(
    TARGETS password_manager
    TS_FILES password_manager_pl_PL.ts
)

qt_add_resources(${PROJECT_NAME} "resources"
    PREFIX "/"
    FILES
        icons/app_icon.ico
)

if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/app_icon.rc")
    target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
endif()

target_link_libraries(password_manager
    PRIVATE
        Qt::Core
        Qt::Widgets
        models
        encryption
        database
        # dialogs

        services
        # ui
)

target_link_directories(password_manager
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(password_manager
    PUBLIC
        ${CMAKE_SOURCE_DIR}/models
        ${CMAKE_SOURCE_DIR}/encryption
        ${CMAKE_SOURCE_DIR}/database
        ${CMAKE_SOURCE_DIR}/services
        ${CMAKE_CURRENT_SOURCE_DIR}
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
