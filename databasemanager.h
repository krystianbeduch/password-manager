#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "passwordmanager.h"
#include "encryptionutils.h"
#include "cryptodata.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <QHash>
#include <QVariant>
#include <QByteArray>
#include <QDateTime>
#include <QTimeZone>


class DatabaseManager {
public:
    DatabaseManager(const QString &host,
                    int port,
                    const QString &dbName,
                    const QString &username,
                    const QString &password);
    ~DatabaseManager();
    bool connect();
    void disconnect();
    void insertSamplePasswordsData(EncryptionUtils *crypto);
    QVector<QVector<QVariant>> fetchAllPasswords();
    bool addPassword(PasswordManager *newPassword, CryptoData &cryptoData);
    bool editPassword(PasswordManager *password, CryptoData &cryptoData);
    bool deletePasswordById(int id);
    bool truncatePasswords();
    QMap<int, QString> fetchPasswordsToExport(QVector<PasswordManager*> passwords, EncryptionUtils *crypto);

    QString decryptPassword(int passwordId, EncryptionUtils *crypto);
    bool savePositionsToDatabase(QVector<PasswordManager*> passwords);
    bool addPasswordList(QHash<PasswordManager*, CryptoData> &passwords);

private:
    QString m_host;
    int m_port;
    QString m_dbName;
    QString m_username;
    QString m_password;
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
