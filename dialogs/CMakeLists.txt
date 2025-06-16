qt_add_library(dialogs
    passwordformdialog.h passwordformdialog.cpp passwordformdialog.ui
    selectdialog.h selectdialog.cpp selectdialog.ui
    exportpassworddialog.h exportpassworddialog.cpp exportpassworddialog.ui
    logindialog.h logindialog.cpp logindialog.ui
)

target_include_directories(dialogs
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(dialogs
    PUBLIC
        models
        database
)
