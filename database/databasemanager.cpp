#include "databasemanager.h"

DatabaseManager::DatabaseManager(const QString &host,
                                 int port,
                                 const QString &dbName,
                                 const QString &username,
                                 const QString &password)
    : QObject()
    , m_host(host)
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
    disconnectDb();
}

bool DatabaseManager::connectDb() {
    if (m_db.open()) {
        return true;
    }

    if (!m_db.open()) {
        QMessageBox::critical(nullptr, tr("Database Connection Error"), tr("Failed to connect to database: %1").arg(m_db.lastError().text()));
        return false;
    }
    qDebug() << tr("Connected to database:") << m_db.databaseName();
    return true;
}

void DatabaseManager::disconnectDb() {
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << tr("Connection to the database has been closed");
    }
}

bool DatabaseManager::insertSamplePasswordsData(EncryptionUtils *crypto) {
    const QVector<PasswordManager*> passwords = {
        new PasswordManager("Gmail", "user1@gmail.com", "password123", "Email"),
        new PasswordManager("Facebook", "user2@o2.pl", "mypass456", "Personal"),
        new PasswordManager("Microsoft Team", "user@outlook.com", "securePassword", "Work")
    };

    QHash<PasswordManager*, CryptoData> cryptoMap;
    for (auto *p : passwords) {
        const std::optional<CryptoData> cryptoDataOpt = crypto->prepareCryptoData(QString::fromUtf8(crypto->mainPassword()), p->password());
        if (!cryptoDataOpt.has_value()) {
            qDebug() << tr("Error during generation of cryptographic data");
            qDeleteAll(passwords);
            return false;
        }
        cryptoMap.insert(p, cryptoDataOpt.value());
    }

    if (addPasswordList(cryptoMap)) {
        qDeleteAll(passwords);
        return true;
    }
    qDeleteAll(passwords);
    return false;
}

QVector<QVector<QVariant>> DatabaseManager::fetchAllPasswords() {
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return {};
    }

    QVector<QVector<QVariant>> data;
    QSqlQuery query(m_db);    
    query.prepare(R"(
        SELECT
            id, service_name, username, group_name, addition_date, position
        FROM
            public.passwords
        ORDER BY position ASC
    )");

    if (!query.exec()) {
        qDebug() << tr("Error fetching all passwords: %1").arg(query.lastError().text());
        disconnectDb();
        return {};
    }

    while (query.next()) {
        QVector<QVariant> row;
        for (int i = 0; i < 6; ++i) {
            row.append(query.value(i));
        }
        data.append(row);
    }
    disconnectDb();
    return data;
}

QHash<int, QString> DatabaseManager::fetchPasswordsToExport(const QVector<PasswordManager*> &passwords, EncryptionUtils *crypto) {
    if (passwords.isEmpty()) {
        qWarning() << tr("Password list is empty");
        return {};
    }

    if (!crypto) {
        qWarning() << tr("Invalid EncryptionUtils instance");
        return {};
    }


    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return {};
    }

    QHash<int, QString> decryptedMap;
    QStringList idList;
    for (int i = 0; i < passwords.size(); ++i) {
        idList += QString::number(passwords[i]->id());
    }

    QSqlQuery query(m_db);
    QString queryStr = QString(R"(
        SELECT
            password_id, encrypted_password, nonce, salt
        FROM
            public.encrypted_passwords
        WHERE
            password_id IN (%1)
    )").arg(idList.join(","));

    if (!query.exec(queryStr)) {
        qDebug() << tr("Failed to fetch encrypted passwords: %1").arg(query.lastError().text());
        disconnectDb();
        return {};
    }

    while (query.next()) {
        const int id = query.value("password_id").toInt();
        const QByteArray encrypted = query.value("encrypted_password").toByteArray();
        const QByteArray salt = query.value("salt").toByteArray();
        const QByteArray nonce = query.value("nonce").toByteArray();

        if (!crypto->generateKeyFromPassword(QString::fromUtf8(crypto->mainPassword()), salt)) {
            qWarning() << tr("Failed to generate a key for the ID record: %1").arg(id);
            continue;
        }

        const QString decryptedPassword = crypto->decrypt(encrypted, nonce);
        decryptedMap.insert(id, decryptedPassword);
    }
    disconnectDb();
    return decryptedMap;
}

bool DatabaseManager::addPassword(PasswordManager *newPassword, CryptoData &cryptoData) {
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return false;
    }

    if (!newPassword) {
        qWarning() << tr("Invalid new password instance");
        return false;
    }

    if (cryptoData.cipher.isEmpty() || cryptoData.salt.isEmpty() || cryptoData.nonce.isEmpty()) {
        qWarning() << tr("Incomplete CryptoData – aborting insert");
        return false;
    }

    QSqlDatabase::database().transaction();
    {
        QSqlQuery query(m_db);
        query.prepare(R"(
            WITH next_pos AS (
                SELECT COALESCE(MAX(position), 0) + 1 AS new_position FROM passwords
            )
            INSERT INTO public.passwords
                (service_name, username, group_name, position)
            SELECT :service, :username, :group_name, new_position FROM next_pos
            RETURNING id, addition_date;
        )");

        query.bindValue(":service", newPassword->serviceName());
        query.bindValue(":username", newPassword->username());
        query.bindValue(":group_name", newPassword->group());

        if (query.exec() && query.next()) {
            newPassword->setId(query.value("id").toInt());
            QDateTime dateTime = query.value("addition_date").toDateTime();
            dateTime.setTimeZone(QTimeZone("Europe/Warsaw"));
            newPassword->setAdditionalDate(dateTime);
            qDebug() << tr("Password added with ID: %1").arg(newPassword->id());
        }
        else {
            qDebug() << tr("Failed to insert password: %1").arg(query.lastError().text());
            QSqlDatabase::database().rollback();
            disconnectDb();
            return false;
        }
    }

    {
        QSqlQuery query(m_db);
        query.prepare(R"(
            INSERT INTO
                public.encrypted_passwords
                (password_id, encrypted_password, nonce, salt)
            VALUES
                (:password_id, :encrypted_password, :nonce, :salt)
        )");

        query.bindValue(":password_id", newPassword->id());
        query.bindValue(":encrypted_password", cryptoData.cipher);
        query.bindValue(":nonce", cryptoData.nonce);
        query.bindValue(":salt", cryptoData.salt);

        if (query.exec()) {
            qDebug() << tr("Password hash added for password with ID: %1").arg(newPassword->id());
        }
        else {
            qDebug() << tr("Failed to insert password: %1").arg(query.lastError().text());
            QSqlDatabase::database().rollback();
            disconnectDb();
            return false;
        }
    }

    QSqlDatabase::database().commit();
    disconnectDb();
    return true;
}

bool DatabaseManager::editPassword(PasswordManager *password, CryptoData &cryptoData) {
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return false;
    }

    if (!password) {
        qWarning() << tr("Invalid password instance");
        return false;
    }

    if (cryptoData.cipher.isEmpty() || cryptoData.salt.isEmpty() || cryptoData.nonce.isEmpty()) {
        qWarning() << tr("Incomplete CryptoData – aborting insert");
        return false;
    }


    QSqlDatabase::database().transaction();
    {
        QSqlQuery query(m_db);
        query.prepare(R"(
            UPDATE public.passwords
            SET service_name = :service,
                username = :username,
                group_name = :group
            WHERE id = :id
        )");

        query.bindValue(":service", password->serviceName());
        query.bindValue(":username", password->username());
        query.bindValue(":group", password->group());
        query.bindValue(":id", password->id());

        if (query.exec()) {
            qDebug() << tr("Password upadated");
        }
        else {
            qDebug() << tr("Failed to update password: %1").arg(query.lastError().text());
            QSqlDatabase::database().rollback();
            disconnectDb();
            return false;
        }
    }

    {
        QSqlQuery query(m_db);
        query.prepare(R"(
            UPDATE public.encrypted_passwords
            SET encrypted_password = :encrypted_password,
                nonce = :nonce,
                salt = :salt
            WHERE password_id = :password_id;
        )");

        query.bindValue(":encrypted_password", cryptoData.cipher);
        query.bindValue(":nonce", cryptoData.nonce);
        query.bindValue(":salt", cryptoData.salt);
        query.bindValue(":password_id", password->id());

        if (query.exec()) {
            qDebug() << tr("Encrypted password edited for password with ID: %1").arg(password->id());
        }
        else {
            qDebug() << tr("Failed to edit encrypted password: %1").arg(query.lastError().text());
            QSqlDatabase::database().rollback();
            disconnectDb();
            return false;
        }
    }

    QSqlDatabase::database().commit();
    disconnectDb();
    return true;
}

bool DatabaseManager::deletePasswordById(int id) {
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return false;
    }

    QSqlDatabase::database().transaction();
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM public.passwords WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            qDebug() << tr("Deleted password with ID: %1").arg(id);
        }
        else {
            QMessageBox::warning(nullptr, tr("No record"), tr("No record found with ID: %1").arg(id));
            QSqlDatabase::database().rollback();
            disconnectDb();
            return false;
        }
    }
    else {
        qDebug() << tr("Failed to delete password: %1").arg(query.lastError().text());
        QSqlDatabase::database().rollback();
        disconnectDb();
        return false;
    }

    QSqlDatabase::database().commit();
    disconnectDb();
    return true;
}

bool DatabaseManager::truncatePasswords() {
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return false;
    }

    QSqlDatabase::database().transaction();
    QSqlQuery query(m_db);
    query.prepare("TRUNCATE public.passwords RESTART IDENTITY CASCADE;");
    // RESTART IDENTITY - restart licznika SERIAL (id) do 1
    // CASCADE - usuwanie rekordow powiazanych FK

    if (query.exec()) {
        qDebug() << tr("Truncated passwords table successfully");
    }
    else {
        QMessageBox::critical(nullptr, tr("Database Connection Error"), tr("Failed to truncate passwords table: %1").arg(m_db.lastError().text()));
        QSqlDatabase::database().rollback();
        disconnectDb();
        return false;
    }

    QSqlDatabase::database().commit();
    disconnectDb();
    return true;
}

QString DatabaseManager::decryptPassword(int passwordId, EncryptionUtils *crypto) {
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return {};
    }

    if(!crypto) {
        qWarning() << tr("Invalid EncryptionUtils instance");
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT
            encrypted_password, nonce, salt
        FROM public.encrypted_passwords
        WHERE password_id = :password_id;
    )");
    query.bindValue(":password_id", passwordId);

    if (!query.exec()) {
        qDebug() << tr("Failed to fetch password data: %1").arg(query.lastError().text());
        disconnectDb();
        return {};
    }

    QString decryptedPassword;
    while (query.next()) {
        const QByteArray encryptedPassword = query.value("encrypted_password").toByteArray();
        const QByteArray nonce = query.value("nonce").toByteArray();
        const QByteArray salt = query.value("salt").toByteArray();

        if (!crypto->generateKeyFromPassword(QString::fromUtf8(crypto->mainPassword()), salt)) {
            qWarning() << tr("Failed to generate a key for the ID record: %1").arg(passwordId);
            continue;
        }
        decryptedPassword = crypto->decrypt(encryptedPassword, nonce);
        qDebug() << tr("Decrypted password with ID: %1").arg(passwordId);
    }
    disconnectDb();
    return decryptedPassword;
}

bool DatabaseManager::savePositionsToDatabase(QVector<PasswordManager*> passwords) {    
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return false;
    }

    if (passwords.isEmpty()) {
        qWarning() << tr("Invalid password instance");
        return false;
    }

    QSqlDatabase::database().transaction();
    QString updateQuery = "UPDATE public.passwords SET position = CASE id ";
    for (int i = 0; i < passwords.size(); i++) {
        updateQuery += QString("WHEN %1 THEN %2 ")
                           .arg(passwords[i]->id()).arg(i);
    }

    updateQuery += "END WHERE id IN (";
    for (int i = 0; i < passwords.size(); ++i) {
        updateQuery += QString::number(passwords[i]->id());
        if (i != passwords.size() - 1) {
            updateQuery += ", ";
        }
    }
    updateQuery += ");";

    QSqlQuery query(m_db);
    if (query.exec(updateQuery)) {
        qDebug() << tr("Passwords order saved");
    }
    else {
        qDebug() << tr("Failed to update passwords order: %1").arg(query.lastError().text());
        QSqlDatabase::database().rollback();
        disconnectDb();
        return false;
    }

    QSqlDatabase::database().commit();
    disconnectDb();
    return true;
}

bool DatabaseManager::addPasswordList(QHash<PasswordManager*, CryptoData> &passwords) {
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return false;
    }

    if (passwords.isEmpty()) {
        qWarning() << tr("Password list is empty");
        return false;
    }

    QSqlDatabase::database().transaction();
    for (auto it = passwords.begin(); it != passwords.end(); it++) {
        PasswordManager* newPassword = it.key();
        const CryptoData cryptoData = it.value();

        if (!newPassword) {
            qDebug() << tr("Null password entry");
            return false;
        }

        {
            QSqlQuery query(m_db);
            query.prepare(R"(
                WITH next_pos AS (
                    SELECT COALESCE(MAX(position), 0) + 1 AS new_position FROM passwords
                )
                INSERT INTO public.passwords
                    (service_name, username, group_name, position)
                SELECT :service, :username, :group_name, new_position FROM next_pos
                RETURNING id, addition_date;
            )");

            query.bindValue(":service", newPassword->serviceName());
            query.bindValue(":username", newPassword->username());
            query.bindValue(":group_name", newPassword->group());

            if (query.exec() && query.next()) {
                newPassword->setId(query.value("id").toInt());
                QDateTime dateTime = query.value("addition_date").toDateTime();
                dateTime.setTimeZone(QTimeZone("Europe/Warsaw"));
                newPassword->setAdditionalDate(dateTime);
                qDebug() << tr("Password added with ID: %1").arg(newPassword->id());
            }
            else {
                qDebug() << tr("Failed to insert password: %1").arg(query.lastError().text());
                QSqlDatabase::database().rollback();
                disconnectDb();
                return false;
            }
        }

        {
            QSqlQuery query(m_db);
            query.prepare(R"(
                INSERT INTO public.encrypted_passwords
                    (password_id, encrypted_password, nonce, salt)
                VALUES
                    (:password_id, :encrypted_password, :nonce, :salt)
            )");

            query.bindValue(":password_id", newPassword->id());
            query.bindValue(":encrypted_password", cryptoData.cipher);
            query.bindValue(":nonce", cryptoData.nonce);
            query.bindValue(":salt", cryptoData.salt);

            if (query.exec()) {
                qDebug() << tr("Password hash added for password with ID: %1").arg(newPassword->id());
            }
            else {
                qDebug() << tr("Failed to insert password: %1").arg(query.lastError().text());
                disconnectDb();
                return false;
            }
        }
    }

    QSqlDatabase::database().commit();
    disconnectDb();
    return true;
}

bool DatabaseManager::addMainPassword(CryptoData &cryptoData) {
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return false;
    }

    if (cryptoData.cipher.isEmpty() || cryptoData.salt.isEmpty() || cryptoData.nonce.isEmpty()) {
        qWarning() << tr("Incomplete CryptoData – aborting insert");
        return false;
    }

    QSqlDatabase::database().transaction();
    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO public.login_data
            (encrypted_main_password, salt, nonce)
        VALUES
            (:encrypted_main_password, :salt, :nonce);
    )");

    query.bindValue(":encrypted_main_password", cryptoData.cipher);
    query.bindValue(":salt", cryptoData.salt);
    query.bindValue(":nonce", cryptoData.nonce);

    if (query.exec()) {
        qDebug() << tr("Main password inserted");
        QSqlDatabase::database().commit();
        disconnectDb();
        return true;
    }
    else {
        qDebug() << tr("Failed to insert main password: %1").arg(query.lastError().text());
        QSqlDatabase::database().rollback();
        disconnectDb();
        return false;
    }
}

CryptoData DatabaseManager::fetchMainPassword() {
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return {};
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT
            encrypted_main_password, salt, nonce
        FROM public.login_data
        ORDER BY id DESC
        LIMIT 1;
    )");

    if (!query.exec()) {
        qDebug() << tr("Failed to fetch main password data: %1").arg(query.lastError().text());
        disconnectDb();
        return {};
    }

    CryptoData cryptoData;
    while (query.next()) {
        cryptoData.cipher = query.value("encrypted_main_password").toByteArray();
        cryptoData.salt = query.value("salt").toByteArray();
        cryptoData.nonce = query.value("nonce").toByteArray();
    }
    disconnectDb();
    return cryptoData;
}

QStringList DatabaseManager::fetchGroupNames() {
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return {};
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT
            group_name
        FROM public.groups
        ORDER BY group_name ASC;
    )");

    if (!query.exec()) {
        qDebug() << tr("Failed to fetch group names: %1").arg(query.lastError().text());
        disconnectDb();
        return {};
    }

    QStringList groupList;
    while (query.next()) {
        groupList.append(query.value(0).toString());
    }
    disconnectDb();
    return groupList;
}

bool DatabaseManager::addGroup(const QString &groupName) {
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return false;
    }

    if (groupName.isEmpty()) {
        qWarning() << tr("Empty group name");
        return false;
    }

    QSqlDatabase::database().transaction();
    QSqlQuery query(m_db);
    query.prepare(R"(
        INSERT INTO public.groups
            (group_name)
        VALUES
            (:group_name);
    )");

    query.bindValue(":group_name", groupName);

    if (query.exec()) {
        qDebug() << tr("Group inserted");
        QSqlDatabase::database().commit();
        disconnectDb();
        return true;
    }
    else {
        qDebug() << tr("Failed to insert group: %1").arg(query.lastError().text());
        QSqlDatabase::database().rollback();
        disconnectDb();
        return false;
    }
}

bool DatabaseManager::deleteGroup(const QString &groupName) {
    if (!connectDb()) {
        qWarning() << tr("Failed to connect to database");
        return false;
    }

    QSqlDatabase::database().transaction();
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM public.groups WHERE group_name = :group_name");
    query.bindValue(":group_name", groupName);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            qDebug() << tr("Deleted group: %1").arg(groupName);
        }
        else {
            QMessageBox::warning(nullptr, tr("No record"), tr("No record found with name: %1").arg(groupName));
            QSqlDatabase::database().rollback();
            disconnectDb();
            return false;
        }
    }
    else {
        qDebug() << tr("Failed to delete group: %1").arg(query.lastError().text());
        QSqlDatabase::database().rollback();
        disconnectDb();
        return false;
    }

    QSqlDatabase::database().commit();
    disconnectDb();
    return true;
}
