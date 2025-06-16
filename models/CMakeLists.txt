add_library(models
    passwordmode.h
    passwordmanager.h passwordmanager.cpp
    group.h group.cpp
)

target_include_directories(models
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(models
    PUBLIC Qt6::Core
)
