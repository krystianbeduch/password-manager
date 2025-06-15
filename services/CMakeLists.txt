add_library(services
    fileservice.cpp
    fileservice.h
)

target_include_directories(services
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(services
    PUBLIC
        Qt6::Core
        Qt6::Widgets
        Qt6::Sql
        models
        database
)
