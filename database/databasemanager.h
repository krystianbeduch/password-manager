#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "cryptodata.h"
#include "encryptionutils.h"
#include "passwordmanager.h"

#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QHash>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QTimeZone>
#include <QVariant>
#include <QVector>

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
    [[nodiscard]] bool createTableIfNotExists();
    [[nodiscard]] bool insertSamplePasswordsData(EncryptionUtils *crypto);
    [[nodiscard]] QVector<QVector<QVariant>> fetchAllPasswords();
    [[nodiscard]] bool addPassword(PasswordManager *newPassword, CryptoData &cryptoData);
    [[nodiscard]] bool editPassword(PasswordManager *password, CryptoData &cryptoData);
    [[nodiscard]] bool deletePasswordById(int id);
    [[nodiscard]] bool truncatePasswords();
    [[nodiscard]] QHash<int, QString> fetchPasswordsToExport(const QVector<PasswordManager*> &passwords, EncryptionUtils *crypto);
    [[nodiscard]] QString decryptPassword(int passwordId, EncryptionUtils *crypto);
    [[nodiscard]] bool savePositionsToDatabase(QVector<PasswordManager*> passwords);
    [[nodiscard]] bool addPasswordList(QHash<PasswordManager*, CryptoData> &passwords);
    [[nodiscard]] bool addMainPassword(CryptoData &cryptoData);
    [[nodiscard]] CryptoData fetchMainPassword();
    [[nodiscard]] QVector<Group> fetchGroups();
    [[nodiscard]] bool addGroup(const QString &groupName);
    [[nodiscard]] bool deleteGroup(int groupId);
    [[nodiscard]] bool updateGroup(int groupId, const QString &newName);
    [[nodiscard]] bool hasPasswordsInGroup(int groupId);

private:
    QString m_host;
    int m_port;
    QString m_dbName;
    QString m_username;
    QString m_password;
    QSqlDatabase m_db;
    static constexpr auto DB_QUERY_PATH = "database/db_query.sql";
};

#endif // DATABASEMANAGER_H
