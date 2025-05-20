#include "encryptionutils.h"

EncryptionUtils::EncryptionUtils() {
    if (sodium_init() < 0) {
        qFatal("libsodium init failed");
    }
}

// QByteArray EncryptionUtils::generateSalt(int length) {
//     QByteArray salt(length, 0);
//     randombytes_buf(salt.data(), length);
//     return salt;
// }

// QByteArray EncryptionUtils::hashPasswordArgon(const QString &password, const QByteArray &salt) {
//     const unsigned long long opslimit = 4;
//     const size_t memlimit = 1 << 16;
//     const int hashLength = 32;
//     QByteArray hash(hashLength, 0);

//     int result = crypto_pwhash(
//         reinterpret_cast<unsigned char*>(hash.data()), hashLength,
//         password.toUtf8().constData(), password.toUtf8().size(),
//         reinterpret_cast<const unsigned char*>(salt.constData()),
//         opslimit, memlimit,
//         crypto_pwhash_ALG_ARGON2ID13
//     );

//     if (result != 0) {
//         qDebug() << "Hashing failed";
//         return QByteArray();
//     }

//     return hash;
// }

// QByteArray EncryptionUtils::hashPasswordSha512(const QString &password, const QByteArray &salt, int iterations) {
//     QByteArray result = password.toUtf8() + salt;
//     for (int i = 0; i < iterations; i++)     {
//         result = QCryptographicHash::hash(result, QCryptographicHash::Sha512);
//         result += salt;
//     }
//     return result.toHex();
// }

// bool EncryptionUtils::verifyPassword(const QString &password, const QByteArray &salt, const QByteArray &hash) {
//     QByteArray calculatedHash = hashPassword(password, salt);
//     return calculatedHash == hash;
// }

QByteArray EncryptionUtils::generateSaltToEncrypt() {
    QByteArray salt(crypto_pwhash_SALTBYTES, 0);
    randombytes_buf(salt.data(), salt.size());
    return salt;
}

bool EncryptionUtils::generateKey() {
    m_key.resize(crypto_aead_xchacha20poly1305_ietf_KEYBYTES);
    randombytes_buf(m_key.data(), m_key.size());
    return true;
}

bool EncryptionUtils::generateKeyFromPassword(const QString &password, const QByteArray &salt) {
    if (salt.size() != crypto_pwhash_SALTBYTES) {
        qWarning() << "Invalid salt size!";
        return false;
    }

    m_key.resize(crypto_aead_xchacha20poly1305_ietf_KEYBYTES);

    if (crypto_pwhash(
            reinterpret_cast<unsigned char *>(m_key.data()), m_key.size(),
            password.toUtf8().constData(), password.toUtf8().size(),
            reinterpret_cast<const unsigned char *>(salt.constData()),
            crypto_pwhash_OPSLIMIT_MODERATE,
            crypto_pwhash_MEMLIMIT_MODERATE,
            crypto_pwhash_ALG_DEFAULT) != 0) {
        qWarning() << "Key derivation failed!";
        return false;
    }
    return true;
}

QByteArray EncryptionUtils::encrypt(const QByteArray &plainText, QByteArray &nonceOut) {
    nonceOut.resize(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
    randombytes_buf(nonceOut.data(), nonceOut.size());

    QByteArray cipherText;
    cipherText.resize(plainText.size() + crypto_aead_xchacha20poly1305_ietf_ABYTES);

    unsigned long long cipherLen;
    crypto_aead_xchacha20poly1305_ietf_encrypt(
        reinterpret_cast<unsigned char *>(cipherText.data()), &cipherLen,
        reinterpret_cast<const unsigned char *>(plainText.data()), plainText.size(),
        nullptr, 0,
        nullptr,
        reinterpret_cast<const unsigned char *>(nonceOut.constData()),
        reinterpret_cast<const unsigned char *>(m_key.constData())
        );

    cipherText.resize(cipherLen);
    return cipherText;
}

QByteArray EncryptionUtils::decrypt(const QByteArray &cipherText, const QByteArray &nonce) {
    QByteArray decrypted;
    decrypted.resize(cipherText.size() - crypto_aead_xchacha20poly1305_ietf_ABYTES);

    unsigned long long decryptedLen;
    if (crypto_aead_xchacha20poly1305_ietf_decrypt(
            reinterpret_cast<unsigned char *>(decrypted.data()), &decryptedLen,
            nullptr,
            reinterpret_cast<const unsigned char *>(cipherText.constData()), cipherText.size(),
            nullptr, 0,
            reinterpret_cast<const unsigned char *>(nonce.constData()),
            reinterpret_cast<const unsigned char *>(m_key.constData())
            ) != 0) {
        qWarning() << "Decryption failed!";
        return {};
    }

    decrypted.resize(decryptedLen);
    return decrypted;
}

std::optional<CryptoData> EncryptionUtils::prepareCryptoData(const QString &mainPassword, const QString &passwordToEncrypt) {
    CryptoData data;
    data.salt = generateSaltToEncrypt();

    if (!generateKeyFromPassword(mainPassword, data.salt)) {
        qWarning() << "Key derivation failed!";
        return std::nullopt;
    }

    QByteArray nonce;
    data.cipher = encrypt(passwordToEncrypt.toUtf8(), nonce);
    data.nonce = nonce;
    return data;
}

bool EncryptionUtils::verifyMainPassword(const QString &userPassword, const CryptoData &cryptoData) {
    if (!generateKeyFromPassword(userPassword, cryptoData.salt)) {
        qWarning() << "Key derivation failed!";
        return false;
    }

    QByteArray decryptedMainPassword = decrypt(cryptoData.cipher, cryptoData.nonce);
    if (userPassword.toUtf8() == decryptedMainPassword) {
        m_mainPassword = decryptedMainPassword;
        return true;
    }
    else {
        return false;
    }
}

void EncryptionUtils::setKey(const QByteArray &key) { m_key = key; }
QByteArray EncryptionUtils::getKey() const { return m_key; }
void EncryptionUtils::setMainPassword(const QByteArray &mainPassword) { m_mainPassword = mainPassword; }
QByteArray EncryptionUtils::getMainPassword() const { return m_mainPassword; }
