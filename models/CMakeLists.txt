add_library(models
    passwordmanager.cpp
    passwordmanager.h
    passwordmode.h
    group.cpp
    group.h
)

# target_include_directories(models
#     PUBLIC
#         ${CMAKE_CURRENT_SOURCE_DIR}
# )
target_link_libraries(models Qt6::Core)
