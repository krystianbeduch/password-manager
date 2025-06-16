add_library(services
    fileservice.h fileservice.cpp
)

target_include_directories(models
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(services
    PUBLIC
        models
        database
)
