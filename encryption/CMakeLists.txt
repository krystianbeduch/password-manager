add_library(encryption
    encryptionutils.cpp
    encryptionutils.h
    cryptodata.h
)

# target_include_directories(encryption
#     PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
# )

target_link_libraries(encryption Qt6::Core ${LIBSODIUM_LIBRARY})
