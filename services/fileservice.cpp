#include "fileservice.h"

void FileService::exportToCSV(const QString &path, const QVector<PasswordManager*> &selected, const QHash<int, QString> &decrypted) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Cannot open CSV file to writing"));
        return;
    }

    QTextStream out(&file);
    out << "ID;Service name;Username;Password;Group;Addition date\n";
    for (const auto *p : selected) {
        if (decrypted.contains(p->id())) {
            out << QString("%1;%2;%3;%4;%5;%6\n")
            .arg(QString::number(p->id()),
                 p->serviceName(),
                 p->username(),
                 decrypted[p->id()],
                 p->group(),
                 p->formattedDate());
        }
    }
    file.close();
}

void FileService::exportToJSON(const QString &path, const QVector<PasswordManager*> &selected, const QHash<int, QString> &decrypted) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Cannot open JSON file to writing"));
        return;
    }

    QJsonArray jsonArray;
    for (const auto *p : selected) {
        if (decrypted.contains(p->id())) {
            QJsonObject jsonObject;
            jsonObject["id"] = p->id();
            jsonObject["service_name"] = p->serviceName();
            jsonObject["username"] = p->username();
            jsonObject["password"] = decrypted[p->id()];
            jsonObject["group"] = p->group();
            jsonObject["addition_date"] = p->formattedDate();
            jsonArray.append(jsonObject);
        }
    }
    QJsonDocument jsonDoc(jsonArray);
    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();
}

void FileService::exportToXML(const QString &path, const QVector<PasswordManager*> &selected, const QHash<int, QString> &decrypted) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Cannot open XML file to writing"));
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("Passwords");

    for (const auto *p : selected) {
        if (decrypted.contains(p->id())) {
            writer.writeStartElement("password_data");
            writer.writeAttribute("id", QString::number(p->id()));
            writer.writeTextElement("service_name", p->serviceName());
            writer.writeTextElement("username", p->username());
            writer.writeTextElement("password", decrypted[p->id()]);
            writer.writeTextElement("group", p->group());
            writer.writeTextElement("addition_date", p->formattedDate());
            writer.writeEndElement();
        }
    }

    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
}

QVector<PasswordManager*> FileService::parseCSV(const QString &path, DatabaseManager *dbManager) {
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Failed to open CSV file"));
        return {};
    }

    QTextStream in(&file);
    in.readLine();
    QVector<PasswordManager*> passwords;

    while(!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QStringList parts = line.split(";");
        if (parts.size() >= 3 && isValidGroup(parts[4], dbManager)) {
            passwords.append(new PasswordManager(parts[1], parts[2], parts[3], parts[4]));
        }
        else {
            QMessageBox::warning(nullptr, "Error", "Incorrect CSV file format. Expected format:\n"
                                                   "ID;Serice name;Username;Password;Group;Additional date (not required)\n\n"
                                                   "The group must be created in advance");
            return passwords;
        }
    }
    file.close();
    return passwords;
}

QVector<PasswordManager*> FileService::parseJSON(const QString &path, DatabaseManager *dbManager) {
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Failed to open JSON file"));
        return {};
    }

    QByteArray data = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (!jsonDoc.isArray()) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Incorrect JSON format"));
        return {};
    }

    QVector<PasswordManager*> passwords;
    QJsonArray jsonArray = jsonDoc.array();
    for (const auto &jsonVal : std::as_const(jsonArray)) {
        if (!jsonVal.isObject()) {
            QMessageBox::warning(nullptr, tr("Error"), tr("Incorrect JSON format"));
            return {};
        }
        QJsonObject jsonObj = jsonVal.toObject();
        if (!jsonObj.contains("service_name") ||
            !jsonObj.contains("username") ||
            !jsonObj.contains("password") ||
            !jsonObj.contains("group")) {
            QMessageBox::warning(nullptr, tr("Error"), tr("Incorrect JSON format. Required format:\n"
                                                   "[\n"
                                                   "\t    {\n"
                                                   "\t\t        \"service_name\": \"service1\",\n"
                                                   "\t\t        \"username\": \"user1\",\n"
                                                   "\t\t        \"password\": \"password1\",\n"
                                                   "\t\t        \"group\": \"group1\"\n"
                                                   "\t    },\n"
                                                   "\t    {\n"
                                                   "\t\t        \"service_name\": \"service2\",\n"
                                                   "\t\t        \"username\": \"user2\",\n"
                                                   "\t\t        \"password\": \"password2\",\n"
                                                   "\t\t        \"group\": \"group2\"\n"
                                                   "\t    }\n"
                                                   "]"));
            return {};
        }
        QString serviceName = jsonObj.value("service_name").toString();
        QString username = jsonObj.value("username").toString();
        QString password = jsonObj.value("password").toString();
        QString group = jsonObj.value("group").toString();
        if (!isValidGroup(group, dbManager)) {
            QMessageBox::warning(nullptr, tr("Error"), tr("Incorrect group\n"
                                                   "The group must be created in advance"));
            return {};
        }
        passwords.append(new PasswordManager(serviceName, username, password, group));
    }
    file.close();
    return passwords;
}

QVector<PasswordManager*> FileService::parseXML(const QString &path, DatabaseManager *dbManager) {
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, tr("Error"), tr("Failed to open XML file"));
        return {};
    }

    QVector<PasswordManager*> passwords;
    QXmlStreamReader xml(&file);

    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "Passwords") && !xml.hasError()) {
        xml.readNext();

        if (xml.tokenType() == QXmlStreamReader::StartElement && xml.name() == "password_data") {
            PasswordManager *newPassword = new PasswordManager();
            while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "password_data")) {
                xml.readNext();

                if (xml.tokenType() == QXmlStreamReader::StartElement) {
                    if (xml.name() == "service_name") {
                        newPassword->setServiceName(xml.readElementText());
                    }
                    else if (xml.name() == "username") {
                        newPassword->setUsername(xml.readElementText());
                    }
                    else if (xml.name() == "password") {
                        newPassword->setPassword(xml.readElementText());
                    }
                    else if (xml.name() == "group") {
                        QString group = xml.readElementText();
                        if (isValidGroup(group, dbManager)) {
                            newPassword->setGroup(group);
                        }
                        else {
                            QMessageBox::warning(nullptr, tr("Error"), tr("Incorrect group\n"
                                                                   "The group must be created in advance"));
                            delete newPassword;
                            return {};
                        }
                    }
                } // if StartElement
            } // while End Password
            if (newPassword->serviceName().isEmpty() ||
                newPassword->username().isEmpty() ||
                newPassword->password().isEmpty() ||
                newPassword->group().isEmpty())
            {
                QMessageBox::warning(nullptr, tr("Error"), tr("Incorrect XML format. Required format:\n"
                                                       "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                       "<Passwords>\n"
                                                       "  <password_data>\n"
                                                       "    <service_name>service1</service_name>\n"
                                                       "    <username>user1</username>\n"
                                                       "    <password>password1</password>\n"
                                                       "    <group>group1</group>\n"
                                                       "  </password_data>\n"
                                                       "</Passwords>"));
                delete newPassword;
                return {};
            }
            passwords.append(newPassword);
        } // if StartElement Password
    } // while EndDocument

    file.close();
    if (xml.hasError()) {
        QMessageBox::warning(nullptr, tr("Error"), tr("XML error: %1").arg(xml.errorString()));
    }
    return passwords;
}

bool FileService::isValidGroup(const QString &group, DatabaseManager *dbManager) {
    static QSet<QString> validGroupsCache;
    static bool initialized = false;
    if (!initialized) {
        qDebug() << "sas";
        QStringList groups = dbManager->fetchGroupNames();
        validGroupsCache = QSet<QString>(groups.begin(), groups.end());
        initialized = true;
    }

    return validGroupsCache.contains(group);
}
