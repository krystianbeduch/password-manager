add_library(encryption
    encryptionutils.cpp
    encryptionutils.h
    cryptodata.h
)
target_link_libraries(encryption Qt6::Core ${LIBSODIUM_LIBRARY})
