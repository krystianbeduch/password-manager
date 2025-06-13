add_library(models
    passwordmanager.cpp
    passwordmanager.h
    passwordmode.h
)

target_link_libraries(models Qt6::Core)

