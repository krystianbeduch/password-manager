#ifndef ENCRYPTIONUTILS_H
#define ENCRYPTIONUTILS_H

#include <QByteArray>
#include <QString>
#include <QDebug>
#include <sodium.h>
#include "cryptodata.h"

class EncryptionUtils {
public:
    EncryptionUtils();
    [[nodiscard]] QByteArray decrypt(const QByteArray &cipherText, const QByteArray &nonce);
    [[nodiscard]] QByteArray encrypt(const QByteArray &plainText, QByteArray &nonceOut);
    [[nodiscard]] bool generateKeyFromPassword(const QString &password, const QByteArray &salt); // klucz na podstawie hasla logowania
    [[nodiscard]] QByteArray generateSaltToEncrypt();
    [[nodiscard]] std::optional<CryptoData> prepareCryptoData(const QString &mainPassword, const QString &passwordToEncrypt);
    [[nodiscard]] bool verifyMainPassword(const QString &userPassword, const CryptoData &cryptoData);

    void setKey(const QByteArray &key);
    QByteArray key() const;
    void setMainPassword(const QByteArray &mainPassword);
    QByteArray mainPassword() const;

private:
    QByteArray m_key;
    QByteArray m_mainPassword;
};

#endif // ENCRYPTIONUTILS_H
