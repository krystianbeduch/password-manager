#ifndef ENCRYPTIONUTILS_H
#define ENCRYPTIONUTILS_H

#include <QByteArray>
#include <QString>
#include <QDebug>
// #include <QCryptographicHash>
// #include <QRandomGenerator>
#include <sodium.h>


class EncryptionUtils {
public:
    // static QByteArray generateSalt(int length = 32);
    // static QByteArray hashPasswordArgon(const QString &password, const QByteArray &salt, int iterations = 100000);
    // bool verifyPassword(const QString &password, const QByteArray &salt, const QByteArray &hash);
    // static QByteArray hashPasswordSha512(const QString &password, const QByteArray &salt);

    EncryptionUtils();

    QByteArray generateSaltToEncrypt();
    bool generateKey(); // losowy klucz
    bool generateKeyFromPassword(const QString &password, const QByteArray &salt); // klucz na podstawie hasla logowania
    QByteArray encrypt(const QByteArray &plainText, QByteArray &nonceOut);
    QByteArray decrypt(const QByteArray &cipherText, const QByteArray &nonce);


    void setKey(const QByteArray &key);
    QByteArray getKey() const;

private:
    QByteArray m_key;
};

#endif // ENCRYPTIONUTILS_H
