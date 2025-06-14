add_library(models
    passwordmanager.cpp
    passwordmanager.h
    passwordmode.h
    group.h group.cpp
)

target_link_libraries(models Qt6::Core)

