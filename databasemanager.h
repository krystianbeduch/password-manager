#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <QVariant>

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
    // bool isConnected() const;
    QVector<QVector<QVariant>> fetchAllPasswords();
    bool addPassword(const QString &service, const QString &username, const QString &password, const QString &group, const QDateTime &date, int &insertedId);
    bool deletePasswordById(int id);

    // QSqlQuery executeQuery(const QString &queryStr);

    QString createPasswordsTable();
    void insertSamplePasswordsData();
    QString createUsersTable();
    void createTable(const QString &queryStr, const QString &tableName);



private:
    QString m_host;
    int m_port;
    QString m_dbName;
    QString m_username;
    QString m_password;
    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H
