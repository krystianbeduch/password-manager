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
    for (PasswordManager *p : passwords) {
        const std::optional<CryptoData> cryptoDataOpt = crypto->prepareCryptoData(QString::fromUtf8(crypto->getMainPassword()), p->getPassword());
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
    QVector<QVector<QVariant>> data;
    if (!connectDb()) {
        qDebug() << "Database not open!";
        return QVector<QVector<QVariant>>();
    }

    QSqlQuery query(m_db);    
    query.prepare(R"(
        SELECT
            p.id, p.service_name, p.username, p.group_name, p.addition_date, p.position
        FROM
            public.passwords AS p
        ORDER BY p.position ASC
    )");

    if (!query.exec()) {
        qDebug() << "Error fetching all passwords:" << query.lastError().text();
        disconnectDb();
        return QVector<QVector<QVariant>>();
    }

    while (query.next()) {
        QVector<QVariant> row;
        for (int i = 0; i < 5; ++i) {
            row.append(query.value(i));
        }
        data.append(row);
    }
    disconnectDb();
    return data;
}

QMap<int, QString> DatabaseManager::fetchPasswordsToExport(QVector<PasswordManager*> passwords, EncryptionUtils *crypto) {
    QMap<int, QString> decryptedMap;

    if (passwords.isEmpty()) {
        return QMap<int, QString>();
    }

    if (!connectDb()) {
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
        disconnectDb();
        return QMap<int, QString>();
    }

    while (query.next()) {
        int id = query.value("password_id").toInt();
        QByteArray encrypted = query.value("encrypted_password").toByteArray();
        QByteArray salt = query.value("salt").toByteArray();
        QByteArray nonce = query.value("nonce").toByteArray();

        if (!crypto->generateKeyFromPassword(QString::fromUtf8(crypto->getMainPassword()), salt)) {
            qWarning() << "Failed to generate a key for the ID record:" << id;
            continue;
        }

        QString decryptedPassword = crypto->decrypt(encrypted, nonce);
        decryptedMap[id] = decryptedPassword;
    }
    disconnectDb();
    return decryptedMap;
}

bool DatabaseManager::addPassword(PasswordManager *newPassword, CryptoData &cryptoData) {
    if (!connectDb() || !newPassword) {
        qWarning() << "Failed to connect to database.";
        return false;
    }

    QSqlDatabase::database().transaction();
    {
        QSqlQuery query(m_db);
        query.prepare(R"(
            WITH next_pos AS (
                SELECT COALESCE(MAX(position), 0) + 1 AS new_position FROM passwords
            )
            INSERT INTO passwords
                (service_name, username, group_name, position)
            SELECT :service, :username, :group_name, new_position FROM next_pos
            RETURNING id, addition_date;
        )");

        query.bindValue(":service", newPassword->getServiceName());
        query.bindValue(":username", newPassword->getUsername());
        query.bindValue(":group_name", newPassword->getGroup());

        if (query.exec() && query.next()) {
            newPassword->setId(query.value("id").toInt());
            QDateTime dateTime = query.value("addition_date").toDateTime();
            dateTime.setTimeZone(QTimeZone("Europe/Warsaw"));
            newPassword->setAdditionalDate(dateTime);
            qDebug() << "Password added with ID:" << newPassword->getId();
        }
        else {
            qDebug() << "Failed to insert password:" << query.lastError().text();
            QSqlDatabase::database().rollback();
            disconnectDb();
            return false;
        }
    }

    {
        QSqlQuery query(m_db);
        query.prepare(R"(
            INSERT INTO
                public.encrypted_passwords (password_id, encrypted_password, nonce, salt)
            VALUES
                (:password_id, :encrypted_password, :nonce, :salt)
        )");

        query.bindValue(":password_id", newPassword->getId());
        query.bindValue(":encrypted_password", cryptoData.cipher);
        query.bindValue(":nonce", cryptoData.nonce);
        query.bindValue(":salt", cryptoData.salt);

        if (query.exec()) {
            qDebug() << "Password hash added for password with ID:" << newPassword->getId();
        }
        else {
            qDebug() << "Failed to insert password:" << query.lastError().text();
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
    if (!connectDb() || !password) {
        return false;
    }
        // qDebug() << password->getServiceName() << " " << password->getUsername() << " " << password->getPassword() << " " << password->getGroup() << " " << password->getId();
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

        query.bindValue(":service", password->getServiceName());
        query.bindValue(":username", password->getUsername());
        query.bindValue(":group", password->getGroup());
        query.bindValue(":id", password->getId());

        if (query.exec()) {
            qDebug() << "Password upadated";
        }
        else {
            qDebug() << "Failed to update password:" << query.lastError().text();
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
        query.bindValue(":password_id", password->getId());

        if (query.exec()) {
            qDebug() << "Encrypted password edited for password with ID:" << password->getId();
        }
        else {
            qDebug() << "Failed to edit encrypted password:" << query.lastError().text();
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
        return false;
    }

    QSqlDatabase::database().transaction();
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM public.passwords WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            qDebug() << "Deleted password with ID:" << id;
        }
        else {
            QMessageBox::warning(nullptr, "No record", "No record found with ID: " + QString::number(id));
            QSqlDatabase::database().rollback();
            disconnectDb();
            return false;
        }
    }
    else {
        qDebug() << "Failed to delete password:" << query.lastError().text();
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
        return false;
    }

    QSqlDatabase::database().transaction();
    QSqlQuery query(m_db);
    query.prepare("TRUNCATE public.passwords RESTART IDENTITY CASCADE;");
    // RESTART IDENTITY - restart licznika SERIAL (id) do 1
    // CASCADE - usuwanie rekordow powiazanych FK

    if (query.exec()) {
        qDebug() << "Truncated passwords table successfully";
    }
    else {
        QMessageBox::critical(nullptr, "Database Connection Error", "Failed to truncate passwords table: " + m_db.lastError().text());
        QSqlDatabase::database().rollback();
        disconnectDb();
        return false;
    }

    QSqlDatabase::database().commit();
    disconnectDb();
    return true;
}

QString DatabaseManager::decryptPassword(int passwordId, EncryptionUtils *crypto) {
    QString decryptedPassword;
    if (!connectDb()) {
        return QString();
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
        qDebug() << "Failed to fetch password data:" << query.lastError().text();
        disconnectDb();
        return decryptedPassword;
    }

    while (query.next()) {
        QByteArray encryptedPassword = query.value("encrypted_password").toByteArray();
        QByteArray nonce = query.value("nonce").toByteArray();
        QByteArray salt = query.value("salt").toByteArray();

        if (!crypto->generateKeyFromPassword(QString::fromUtf8(crypto->getMainPassword()), salt)) {
            qWarning() << "Failed to generate a key for the ID record:" << passwordId;
            continue;
        }
        decryptedPassword = crypto->decrypt(encryptedPassword, nonce);
        qDebug() << "Decrypted password with ID: " << passwordId;
    }
    disconnectDb();
    return decryptedPassword;
}


bool DatabaseManager::savePositionsToDatabase(QVector<PasswordManager*> passwords) {
    if (!connectDb() || passwords.isEmpty()) {
        return false;
    }

    QSqlDatabase::database().transaction();
    QSqlQuery query(m_db);
    QString updateQuery = "UPDATE public.passwords SET position = CASE id ";
    for (int i = 0; i < passwords.size(); i++) {
        updateQuery += QString("WHEN %1 THEN %2 ")
                           .arg(passwords[i]->getId()).arg(i);
    }
    updateQuery += "END WHERE id IN (";
    for (int i = 0; i < passwords.size(); ++i) {
        updateQuery += QString::number(passwords[i]->getId());
        if (i != passwords.size() - 1) {
            updateQuery += ", ";
        }
    }
    updateQuery += ");";

    if (query.exec(updateQuery)) {
        qDebug() << "Passwords order saved";
    }
    else {
        qDebug() << "Failed to update passwords order:" << query.lastError().text();
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
        return false;
    }

    QSqlDatabase::database().transaction();
    for (auto it = passwords.begin(); it != passwords.end(); it++) {
        PasswordManager* newPassword = it.key();
        CryptoData cryptoData = it.value();

        qDebug() << newPassword->getGroup();

        if (!newPassword) {
            qDebug() << "Null password entry";
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

            query.bindValue(":service", newPassword->getServiceName());
            query.bindValue(":username", newPassword->getUsername());
            query.bindValue(":group_name", newPassword->getGroup());

            if (query.exec() && query.next()) {
                newPassword->setId(query.value("id").toInt());
                QDateTime dateTime = query.value("addition_date").toDateTime();
                dateTime.setTimeZone(QTimeZone("Europe/Warsaw"));
                newPassword->setAdditionalDate(dateTime);
                qDebug() << "Password added with ID:" << newPassword->getId();
            }
            else {
                qDebug() << "Failed to insert password:" << query.lastError().text();
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

            query.bindValue(":password_id", newPassword->getId());
            query.bindValue(":encrypted_password", cryptoData.cipher);
            query.bindValue(":nonce", cryptoData.nonce);
            query.bindValue(":salt", cryptoData.salt);

            if (query.exec()) {
                qDebug() << "Password hash added for password with ID:" << newPassword->getId();
            }
            else {
                qDebug() << "Failed to insert password:" << query.lastError().text();
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
    // qDebug() << cryptoData.cipher;
    // qDebug() << cryptoData.salt;
    // qDebug() << cryptoData.nonce;
    if (!connectDb()) {
        qDebug() << "js";
        return false;
    }
    qDebug() << cryptoData.cipher;
    qDebug() << cryptoData.salt;
    qDebug() << cryptoData.nonce;

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
        qDebug() << "Main password inserted";
        QSqlDatabase::database().commit();
        disconnectDb();
        return true;
    }
    else {
        qDebug() << "Failed to insert main password:" << query.lastError().text();
        QSqlDatabase::database().rollback();
        disconnectDb();
        return false;
    }
}

CryptoData DatabaseManager::fetchMainPassword() {
    CryptoData cryptoData;
    if (!connectDb()) {
        return cryptoData;
    }

    QSqlQuery query(m_db);
    query.prepare(R"(
        SELECT
            encrypted_main_password, salt, nonce
        FROM public.login_data
        LIMIT 1;
    )");

    if (!query.exec()) {
        qDebug() << "Failed to fetch main password data:" << query.lastError().text();
        disconnectDb();
        return cryptoData;
    }

    while (query.next()) {
        cryptoData.cipher = query.value("encrypted_main_password").toByteArray();
        cryptoData.salt = query.value("salt").toByteArray();
        cryptoData.nonce = query.value("nonce").toByteArray();
    }
    disconnectDb();
    return cryptoData;
}
