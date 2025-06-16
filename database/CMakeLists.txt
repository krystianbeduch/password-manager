add_library(database
    databasemanager.h databasemanager.cpp
)

target_include_directories(database
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(database
    PUBLIC
        Qt6::Sql
        Qt6::Widgets
        models
        encryption
        services
)
