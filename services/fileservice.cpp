#include "fileservice.h"

QHash<QString, Group> FileService::validGroupsMap;

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
                 p->group().groupName(),
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
            jsonObject["group"] = p->group().groupName();
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
            writer.writeTextElement("group", p->group().groupName());
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
        if (parts.size() >= 5) {
            Group group(parts[4]);
            if (isValidGroup(group, dbManager)) {
                QString normalized = normalizeGroupName(group.groupName());;
                Group resolvedGroup = validGroupsMap.value(normalized);
                passwords.append(new PasswordManager(parts[1], parts[2], parts[3], resolvedGroup));
            }
            else {
                QMessageBox::warning(nullptr, tr("Error"), tr("Group '%1' is not valid\n"
                                                              "The group must be created in advance").arg(parts[4]));
                return {};
            }
        }
        else {
            QMessageBox::warning(nullptr, tr("Error"), tr("Incorrect CSV file format. Expected format:\n"
                                                          "ID;Serice name;Username;Password;Group;Additional date (not required)\n\n"
                                                          "The group must be created in advance"));
            return {};
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
        // QString serviceName = jsonObj.value("service_name").toString();
        // QString username = jsonObj.value("username").toString();
        // QString password = jsonObj.value("password").toString();
        QString groupName = jsonObj.value("group").toString();
        Group group(groupName);
        // Group group = jsonObj.value("group").toString();

        if (!isValidGroup(group, dbManager)) {
            QMessageBox::warning(nullptr, tr("Error"), tr("Group '%1' is not valid\n"
                                                          "The group must be created in advance").arg(groupName));

            // QMessageBox::warning(nullptr, tr("Error"), tr("Incorrect group\n"
                                                          // "The group must be created in advance"));
            return {};
        }
        QString normalized = normalizeGroupName(group.groupName());
        Group resolvedGroup = validGroupsMap.value(normalized);
        passwords.append(new PasswordManager(
            jsonObj.value("service_name").toString(),
            jsonObj.value("username").toString(),
            jsonObj.value("password").toString(),
            resolvedGroup
        ));
        // passwords.append(new PasswordManager(serviceName, username, password, group));
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
            QString groupName;
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
                        groupName = xml.readElementText();
                        // if (isValidGroup(group, dbManager)) {
                        //     newPassword->setGroup(group);
                        // }
                        // else {
                        //     QMessageBox::warning(nullptr, tr("Error"), tr("Group '%1' is not valid\n"
                        //                                                   "The group must be created in advance").arg(groupName));
                        //     delete newPassword;
                        //     return {};
                        // }
                    }
                } // if StartElement
            } // while End Password

            Group group(groupName);
            if (!isValidGroup(group, dbManager)) {
                QMessageBox::warning(nullptr, tr("Error"), tr("Group '%1' is not valid\n"
                                                              "The group must be created in advance").arg(groupName));
                delete newPassword;
                return {};
            }


            QString normalized = group.groupName().left(1).toUpper() + group.groupName().mid(1).toLower();
            newPassword->setGroup(validGroupsMap.value(normalized));


            if (newPassword->serviceName().isEmpty() ||
                newPassword->username().isEmpty() ||
                newPassword->password().isEmpty() ||
                newPassword->group().groupName().isEmpty())
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

void FileService::initializeGroups(DatabaseManager *dbManager) {
    // if ()

}

bool FileService::isValidGroup(const Group &group, DatabaseManager *dbManager) {
    // static QSet<QString> validGroupsCache;
    // static QHash<QString, Group> validGroupsMap;
    static bool initialized = false;

    if (!initialized) {
        qDebug() << "Initializing group";
        const QList<Group> groups = dbManager->fetchGroups();
        for (const auto &g : groups) {
            QString normalizedName = normalizeGroupName(g.groupName());
            validGroupsMap.insert(normalizedName, g);
        }
        initialized = true;
    }
    QString inputNormalized = normalizeGroupName(group.groupName());
    return validGroupsMap.contains(inputNormalized);
}

QString FileService::normalizeGroupName(const QString &name) {
    return name.left(1).toUpper() + name.mid(1).toLower();
}
