qt_wrap_ui(ui_UIS mainwindow.ui)

add_library(ui
    mainwindow.cpp
    mainwindow.h
    ${ui_UIS}
)
target_link_libraries(ui Qt::Widgets)

target_include_directories(ui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/database
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/encryption
)
