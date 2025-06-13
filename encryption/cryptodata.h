#ifndef CRYPTODATA_H
#define CRYPTODATA_H

#include <QByteArray>

struct CryptoData {
    QByteArray cipher;
    QByteArray salt;
    QByteArray nonce;
};

#endif // CRYPTODATA_H
