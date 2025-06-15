add_library(database
    databasemanager.cpp
    databasemanager.h
)

target_include_directories(database
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/models
        ${CMAKE_SOURCE_DIR}/encryption
)

target_link_libraries(database
    PUBLIC
        Qt6::Sql
        Qt6::Widgets
        models
        encryption
)
