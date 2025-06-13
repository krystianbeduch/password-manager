# qt_wrap_ui(dialogs_UIS
#     exportpassworddialog.ui
#     logindialog.ui
#     passwordformdialog.ui
#     selectpassworddialog.ui
# )

add_library(dialogs
    exportpassworddialog.cpp exportpassworddialog.h exportpassworddialog.ui
    logindialog.cpp logindialog.h logindialog.ui
    passwordformdialog.cpp passwordformdialog.h passwordformdialog.ui
    selectpassworddialog.cpp selectpassworddialog.h selectpassworddialog.ui
    # ${dialogs_UIS}
)
target_link_libraries(dialogs Qt::Widgets)
