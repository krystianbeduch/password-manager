add_library(encryption
    encryptionutils.h encryptionutils.cpp
    cryptodata.h
)

target_include_directories(encryption
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(encryption
    PUBLIC
        Qt6::Core
        ${LIBSODIUM_LIBRARY}
)
