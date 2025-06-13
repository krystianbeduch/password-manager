add_library(database
    databasemanager.cpp
    databasemanager.h
)
target_link_libraries(database Qt6::Sql Qt6::Widgets)
