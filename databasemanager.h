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


class DatabaseManager : public QObject {
    Q_OBJECT
public:
    DatabaseManager(const QString &host,
                    int port,
                    const QString &dbName,
                    const QString &username,
                    const QString &password);
    ~DatabaseManager();
    [[nodiscard]] bool connectDb();
    void disconnectDb();
    [[nodiscard]] bool insertSamplePasswordsData(EncryptionUtils *crypto);
    QVector<QVector<QVariant>> fetchAllPasswords();
    [[nodiscard]] bool addPassword(PasswordManager *newPassword, CryptoData &cryptoData);
    [[nodiscard]] bool editPassword(PasswordManager *password, CryptoData &cryptoData);
    [[nodiscard]] bool deletePasswordById(int id);
    [[nodiscard]] bool truncatePasswords();
    QMap<int, QString> fetchPasswordsToExport(QVector<PasswordManager*> passwords, EncryptionUtils *crypto);

    QString decryptPassword(int passwordId, EncryptionUtils *crypto);
    [[nodiscard]] bool savePositionsToDatabase(QVector<PasswordManager*> passwords);
    [[nodiscard]] bool addPasswordList(QHash<PasswordManager*, CryptoData> &passwords);
    [[nodiscard]] bool addMainPassword(CryptoData &cryptoData);
    CryptoData fetchMainPassword();

private:
    QString m_host;
    int m_port;
    QString m_dbName;
    QString m_username;
    QString m_password;
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
