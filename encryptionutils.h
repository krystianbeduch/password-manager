#ifndef ENCRYPTIONUTILS_H
#define ENCRYPTIONUTILS_H

#include <QByteArray>
#include <QString>
#include <QDebug>
#include "cryptodata.h"
#include <sodium.h>

class EncryptionUtils {
public:
    // static QByteArray generateSaltToHash(int length = 32);
    // static QByteArray hashPasswordArgon(const QString &password, const QByteArray &salt, int iterations = 100000);

    // static QByteArray hashPasswordSha512(const QString &password, const QByteArray &salt);

    EncryptionUtils();
    QByteArray decrypt(const QByteArray &cipherText, const QByteArray &nonce);
    QByteArray encrypt(const QByteArray &plainText, QByteArray &nonceOut);
    bool generateKey(); // losowy klucz
    bool generateKeyFromPassword(const QString &password, const QByteArray &salt); // klucz na podstawie hasla logowania
    QByteArray generateSaltToEncrypt();
    std::optional<CryptoData> prepareCryptoData(const QString &mainPassword, const QString &passwordToEncrypt);
    bool verifyMainPassword(const QString &userPassword, const CryptoData &cryptoData);

    void setKey(const QByteArray &key);
    QByteArray getKey() const;
    void setMainPassword(const QByteArray &mainPassword);
    QByteArray getMainPassword() const;

private:
    QByteArray m_key;
    QByteArray m_mainPassword;
};

#endif // ENCRYPTIONUTILS_H
