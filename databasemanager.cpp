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
    QString queryStr = QString(R"(
        SELECT
            p.id, p.service_name, p.username, p.group_name, p.addition_date
        FROM
            public.passwords AS p
        ORDER BY p.id ASC
    )");



    if (!query.exec(queryStr)) {
        qDebug() << "SELECT error:" << query.lastError().text();
        disconnect();
        return data;
    }
    // int columnCount = query.record().count();

    while (query.next()) {
        QVector<QVariant> row;
        for (int i = 0; i < 5; ++i) {
            row.append(query.value(i));
        }
        data.append(row);
    }
    disconnect();
    return data;
}

QMap<int, QString> DatabaseManager::fetchPasswordsToExport(QVector<PasswordManager*> passwords, EncryptionUtils *crypto) {
    QMap<int, QString> decryptedMap;

    if (passwords.isEmpty()) {
        return decryptedMap;
    }

    if (!connect()) {
        qDebug() << "Database not open!";
        return decryptedMap;
    }

    QString idList;
    for (int i = 0; i < passwords.size(); ++i) {
        idList += QString::number(passwords[i]->getId());
        if (i < passwords.size() - 1) {
            idList += ",";
        }
    }

    QSqlQuery query(m_db);
    QString queryStr = QString(R"(
        SELECT
            password_id, encrypted_password, nonce, salt
        FROM
            public.encrypted_passwords
        WHERE
            password_id IN (%1)
    )").arg(idList);

    if (!query.exec(queryStr)) {
        qDebug() << "Failed to fetch encrypted passwords:" << query.lastError().text();
        disconnect();
        return decryptedMap;
    }

    while (query.next()) {
        int id = query.value("password_id").toInt();
        QByteArray encrypted = query.value("encrypted_password").toByteArray();
        QByteArray salt = query.value("salt").toByteArray();
        QByteArray nonce = query.value("nonce").toByteArray();

        if (!crypto->generateKeyFromPassword("1234", salt)) {
            qWarning() << "Failed to generate a key for the ID record:" << id;
            continue;
        }

        QString decryptedPassword = crypto->decrypt(encrypted, nonce);
        decryptedMap[id] = decryptedPassword;
        // qDebug() << decryptedPassword;
    }
    disconnect();
    return decryptedMap;
}

bool DatabaseManager::addPassword(PasswordManager *newPassword, const QByteArray &encryptedPassword,
                                  const QByteArray &nonce, const QByteArray &salt) {
    if (!connect()) {
        qWarning() << "Failed to connect to database.";
        return false;
    }

    {
        QSqlQuery query(m_db);
        query.prepare(R"(
            INSERT INTO public.passwords (service_name, username, group_name)
            VALUES (:service, :username, :group)
            RETURNING id, addition_date
        )");

        query.bindValue(":service", newPassword->getServiceName());
        query.bindValue(":username", newPassword->getUsername());
        query.bindValue(":group", newPassword->getGroup());

        if (query.exec() && query.next()) {
            newPassword->setId(query.value("id").toInt());
            QDateTime dateTime = query.value("addition_date").toDateTime();
            dateTime.setTimeZone(QTimeZone("Europe/Warsaw"));
            newPassword->setAdditionalDate(dateTime);
            qDebug() << "Password added with ID:" << newPassword->getId();
        }
        else {
            qDebug() << "Failed to insert password:" << query.lastError().text();
            disconnect();
            return false;
        }
    }

    {
        QSqlQuery query(m_db);
        query.prepare(R"(
            INSERT INTO public.encrypted_passwords (password_id, encrypted_password, nonce, salt)
            VALUES (:password_id, :encrypted_password, :nonce, :salt)
        )");

        query.bindValue(":password_id", newPassword->getId());
        query.bindValue(":encrypted_password", encryptedPassword);
        query.bindValue(":nonce", nonce);
        query.bindValue(":salt", salt);

        if (query.exec()) {
            qDebug() << "Password hash added for password with ID:" << newPassword->getId();
        }
        else {
            qDebug() << "Failed to insert password:" << query.lastError().text();
            disconnect();
            return false;
        }
    }

    disconnect();
    return true;
}

bool DatabaseManager::editPassword(PasswordManager *password) {
    if (!connect() || !password) {
        return false;
    }

    // QByteArray salt = EncryptionUtils::generateSalt();
    // QByteArray hash = EncryptionUtils::hashPassword(password)

    QSqlQuery query(m_db);
    query.prepare(R"(
        UPDATE public.passwords
        SET service_name = :service,
            username = :username,
            group_name = :group
        WHERE id = :id
    )");

    query.bindValue(":service", password->getServiceName());
    query.bindValue(":username", password->getUsername());
    // query.bindValue(":password", password->getPassword());
    query.bindValue(":group", password->getGroup());
    query.bindValue(":id", password->getId());

    // qDebug() << password->getServiceName() << " " << password->getUsername() << " " << password->getPassword() << " " << password->getGroup() << " " << password->getId();

    bool success = false;
    if (query.exec()) {
        qDebug() << "Password upadated";
        success = true;
    }
    else {
        qDebug() << "Failed to update password:" << query.lastError().text();
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

bool DatabaseManager::truncatePasswords() {
    if (!connect()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare("TRUNCATE public.passwords RESTART IDENTITY CASCADE;");
    // RESTART IDENTITY - restart licznika SERIAL (id) do 1
    // CASCADE - usuwanie rekordow powiazanych FK

    bool success = false;
    if (query.exec()) {
        qDebug() << "Truncated passwords table successfully";
        success = true;
    }
    else {
        qDebug() << "Failed to truncate passwords table:" << query.lastError().text();
        QMessageBox::critical(nullptr, "Database Connection Error", "Failed to truncate passwords table: " + m_db.lastError().text());
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

void DatabaseManager::insertSamplePasswordsData(EncryptionUtils *crypto) {
    QVector<PasswordManager*> passwords = {
        new PasswordManager("Gmail", "user1@gmail.com", "password123", "Email"),
        new PasswordManager("Facebook", "user2@o2.pl", "mypass456", "Personal"),
        new PasswordManager("Microsoft Team", "user@outlook.com", "securePassword", "Work")
    };

    for (const auto &p : passwords) {
        QByteArray salt = crypto->generateSaltToEncrypt();
        if (!crypto->generateKeyFromPassword("1234", salt)) {
            qWarning() << "Key derivation failed!";
            return;
        }

        QByteArray nonce;
        QByteArray encryptedPassword = crypto->encrypt(p->getPassword().toUtf8(), nonce);
        addPassword(p, encryptedPassword, nonce, salt);
        // qDebug() << encrypted;
    }
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



