add_library(services
    fileservice.cpp
    fileservice.h
)

target_link_libraries(services Qt6::Core Qt6::Widgets)
