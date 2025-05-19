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
#include <QVariant>
#include <QByteArray>
#include <QDateTime>
#include <QTimeZone>
#include <QMap>

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
    QVector<QVector<QVariant>> fetchAllPasswords();
    bool addPassword(PasswordManager *newPassword,
                     const QByteArray &encryptedPassword,
                     const QByteArray &nonce,
                     const QByteArray &salt);
    // bool editPassword(PasswordManager *password);
    bool editPassword(PasswordManager *password,
                     const QByteArray &encryptedPassword,
                     const QByteArray &nonce,
                     const QByteArray &salt);

    bool deletePasswordById(int id);
    bool truncatePasswords();
    QMap<int, QString> fetchPasswordsToExport(QVector<PasswordManager*> passwords, EncryptionUtils *crypto);

    QString createPasswordsTable();
    void insertSamplePasswordsData(EncryptionUtils *crypto);
    QString createUsersTable();
    void createTable(const QString &queryStr, const QString &tableName);

    QString decryptPassword(int passwordId, EncryptionUtils *crypto);
    bool savePositionsToDatabase(QVector<PasswordManager*> passwords);

    bool importPasswords(QMap<PasswordManager*, CryptoData> &passwords);

private:
    QString m_host;
    int m_port;
    QString m_dbName;
    QString m_username;
    QString m_password;
    QSqlDatabase m_db;


};

#endif // DATABASEMANAGER_H
