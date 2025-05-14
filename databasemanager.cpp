#include "databasemanager.h"

DatabaseManager::DatabaseManager(const QString &host,
                                 int port,
                                 const QString &dbName,
                                 const QString &username,
                                 const QString &password)
    : m_host(host)
    , m_port(port)
    , m_dbName(dbName)
    , m_username(username)
    , m_password(password)
{
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName(m_host);
    m_db.setPort(m_port);
    m_db.setDatabaseName(m_dbName);
    m_db.setUserName(m_username);
    m_db.setPassword(m_password);
}

DatabaseManager::~DatabaseManager() {
    disconnect();
}


bool DatabaseManager::connect() {
    if (!m_db.isOpen()) {
        if (!m_db.open()) {
            QMessageBox::critical(nullptr, "Database Connection Error", "Failed to connect to database: " + m_db.lastError().text());
            return false;
        }
    }
    qDebug() << "Connected to database:" << m_db.databaseName();
    return true;
}

void DatabaseManager::disconnect() {
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "Connection to the database has been closed";
    }
}

QVector<QVector<QVariant>> DatabaseManager::fetchAllPasswords() {
    QVector<QVector<QVariant>> data;
    if (!connect()) {
        qDebug() << "Database not open!";
        return data;
    }

    QSqlQuery query(m_db);
    QString queryStr = "SELECT id, service_name, username, password, group_name, addition_date FROM public.passwords";

    if (!query.exec(queryStr)) {
        qDebug() << "SELECT error:" << query.lastError().text();
        disconnect();
        return data;
    }

    while (query.next()) {
        QVector<QVariant> row;
        for (int i = 0; i < 6; ++i) {
            row.append(query.value(i));
        }
        data.append(row);
    }
    disconnect();
    return data;
}

bool DatabaseManager::addPassword(const QString &service, const QString &username, const QString &password,
                                  const QString &group, const QDateTime &date, int &insertedId) {
    if (!connect()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO public.passwords (service_name, username, password, group_name, addition_date)
        VALUES (:service, :username, :password, :group, :date)
        RETURNING id
    )");

    query.bindValue(":service", service);
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    query.bindValue(":group", group);
    query.bindValue(":date", date);

    bool success = false;

    if (query.exec()) {
        if (query.next()) {
            insertedId = query.value(0).toInt();
            qDebug() << "Password added with ID:" << insertedId;
            success = true;
        }
        else {
            qDebug() << "Query executed but no ID returned.";
        }
    }
    else {
        qDebug() << "Failed to insert password:" << query.lastError().text();
    }

    disconnect();
    return success;
}

bool DatabaseManager::deletePasswordById(int id) {
    if (!connect()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM public.passwords WHERE id = :id");
    query.bindValue(":id", id);

    bool success = false;

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            qDebug() << "Deleted password with ID:" << id;
            success = true;
        }
        else {
            qDebug() << "No record found with ID:" << id;
        }
    }
    else {
        qDebug() << "Failed to delete password:" << query.lastError().text();
    }

    disconnect();
    return success;
}



QString DatabaseManager::createPasswordsTable() {
    return R"(
        CREATE TABLE IF NOT EXISTS passwords (
            id SERIAL PRIMARY KEY,
            service_name VARCHAR(255) NOT NULL,
            username VARCHAR(255) NOT NULL,
            password TEXT NOT NULL,
            group_name VARCHAR(255),
            addition_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )";
}

void DatabaseManager::insertSamplePasswordsData() {
    QSqlQuery query;
    QString insertQuery = R"(
        INSERT INTO passwords (service_name, username, password, group_name)
        VALUES
        ('Gmail', 'user1gmail.com', 'password123', 'Email'),
        ('Facebook', 'user2@o2.pl', 'mypass456', 'Personal'),
        ('Microsoft Team', 'user@outlook.com', 'securePassword', 'Work')
    )";

    if (!query.exec(insertQuery)) {
        qDebug() << "Error inserting sample records:" << query.lastError().text();
        return;
    }

    qDebug() << "Sample records successfully inserted.";
}

QString DatabaseManager::createUsersTable() {
    return R"(
        CREATE TABLE IF NOT EXISTS users (
            id SERIAL PRIMARY KEY,
            username VARCHAR(255) UNIQUE NOT NULL,
            password TEXT NOT NULL,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );
    )";
}

void DatabaseManager::createTable(const QString &queryStr, const QString &tableName) {
    QSqlQuery query;

    if (!query.exec(queryStr)) {
        qDebug() << "Table creation error: " << query.lastError().text();
        return;
    }
    qDebug() << QString("Table '%1%' successfully created").arg(tableName);
}



