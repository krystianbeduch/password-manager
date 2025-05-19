#include "fileservice.h"

void FileService::exportToCSV(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error", "Cannot open CSV file to writing");
        return;
    }

    QTextStream out(&file);
    out << "ID;Service name;Username;Password;Group;Addition date\n";
    for (const PasswordManager *p : selected) {
        if (decrypted.contains(p->getId())) {
            out << QString("%1;%2;%3;%4;%5;%6;\n")
            .arg(QString::number(p->getId()),
                 p->getServiceName(),
                 p->getUsername(),
                 decrypted[p->getId()],
                 p->getGroup(),
                 p->getFormattedDate());
        }
    }
    file.close();
}

void FileService::exportToJSON(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error", "Cannot open JSON file to writing");
        return;
    }

    QJsonArray jsonArray;
    for (PasswordManager *p : selected) {
        if (decrypted.contains(p->getId())) {
            QJsonObject jsonObject;
            jsonObject["id"] = p->getId();
            jsonObject["service_name"] = p->getServiceName();
            jsonObject["username"] = p->getUsername();
            jsonObject["password"] = decrypted[p->getId()];
            jsonObject["group"] = p->getGroup();
            jsonObject["addition_date"] = p->getFormattedDate();
            jsonArray.append(jsonObject);
        }
    }
    QJsonDocument jsonDoc(jsonArray);
    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();
}

void FileService::exportToXML(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error", "Cannot open XML file to writing");
        return;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("Passwords");

    for (PasswordManager *p : selected) {
        if (decrypted.contains(p->getId())) {
            writer.writeStartElement("password_data");
            writer.writeAttribute("id", QString::number(p->getId()));
            writer.writeTextElement("service_name", p->getServiceName());
            writer.writeTextElement("username", p->getUsername());
            writer.writeTextElement("password", decrypted[p->getId()]);
            writer.writeTextElement("group", p->getGroup());
            writer.writeTextElement("addition_date", p->getFormattedDate());
            writer.writeEndElement();
        }
    }

    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();
}

QVector<PasswordManager*> FileService::parseCSV(const QString &path) {
    QVector<PasswordManager*> passwords;
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error", "Failed to open CSV file");
        return QVector<PasswordManager*>();
    }

    QTextStream in(&file);
    in.readLine();

    while(!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QStringList parts = line.split(";");
        if (parts.size() >= 3 && isValidGroup(parts[4])) {
            passwords.append(new PasswordManager(parts[1], parts[2], parts[3], parts[4]));
        }
        else {
            QMessageBox::warning(nullptr, "Error", "Incorrect CSV file format. Expected format:\n"
                                                   "ID;Serice name;Username;Password;Group;Additional date (not required)\n\n"
                                                   "The group is: 'Work', 'Personal', 'Banking' or 'Email'");
            return passwords;
        }
    }
    file.close();
    return passwords;
}

QVector<PasswordManager*> FileService::parseJSON(const QString &path) {
    QVector<PasswordManager*> passwords;
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error", "Failed to open JSON file");
        return QVector<PasswordManager*>();
    }

    QByteArray data = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    if (!jsonDoc.isArray()) {
        QMessageBox::warning(nullptr, "Error", "Incorrect JSON format");
        return QVector<PasswordManager*>();
    }

    QJsonArray jsonArray = jsonDoc.array();
    for (const QJsonValue &jsonVal : jsonArray) {
        if (!jsonVal.isObject()) {
            QMessageBox::warning(nullptr, "Error", "Incorrect JSON format");
            return QVector<PasswordManager*>();
        }
        QJsonObject jsonObj = jsonVal.toObject();
        if (!jsonObj.contains("service_name") ||
            !jsonObj.contains("username") ||
            !jsonObj.contains("password") ||
            !jsonObj.contains("group")) {
            QMessageBox::warning(nullptr, "Error", "Incorrect JSON format. Required format:\n"
                                                   "[\n"
                                                   "\t    {\n"
                                                   "\t\t        \"service_name\": \"service1\",\n"
                                                   "\t\t        \"username\": \"user1\",\n"
                                                   "\t\t        \"password\": \"password1\",\n"
                                                   "\t\t        \"group\": \"Work|Personal|Banking|Email\"\n"
                                                   "\t    },\n"
                                                   "\t    {\n"
                                                   "\t\t        \"service_name\": \"service2\"\n"
                                                   "\t\t        \"username\": \"user2\"\n"
                                                   "\t\t        \"password\": \"password2\"\n"
                                                   "\t\t        \"group\": \"Work|Personal|Banking|Email\"\n"
                                                   "\t    }\n"
                                                   "]");
            return QVector<PasswordManager*>();
        }
        QString serviceName = jsonObj.value("service_name").toString();
        QString username = jsonObj.value("username").toString();
        QString password = jsonObj.value("password").toString();
        QString group = jsonObj.value("group").toString();
        if (!isValidGroup(group)) {
            QMessageBox::warning(nullptr, "Error", "Incorrect group\n"
                                                   "The group is: 'Work', 'Personal', 'Banking' or 'Email'");
            return QVector<PasswordManager*>();
        }
        passwords.append(new PasswordManager(serviceName, username, password, group));
    }
    file.close();
    return passwords;
}

QVector<PasswordManager*> FileService::parseXML(const QString &path) {
    QVector<PasswordManager*> passwords;
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error", "Failed to open XML file");
        return QVector<PasswordManager*>();
    }

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
                        if (isValidGroup(group)) {
                            newPassword->setGroup(group);
                        }
                        else {
                            QMessageBox::warning(nullptr, "Error", "Incorrect group\n"
                                                                   "The group is: 'Work', 'Personal', 'Banking' or 'Email'");
                            delete newPassword;
                            return QVector<PasswordManager*>();
                        }
                    }
                } // if StartElement
            } // while End Password
            if (newPassword->getServiceName().isEmpty() ||
                newPassword->getUsername().isEmpty() ||
                newPassword->getPassword().isEmpty() ||
                newPassword->getGroup().isEmpty())
            {
                QMessageBox::warning(nullptr, "Error", "Incorrect XML format. Required format:\n"
                                                       "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                                       "<Passwords>\n"
                                                       "  <password_data>\n"
                                                       "    <service_name>service1</service_name>\n"
                                                       "    <username>user1</username>\n"
                                                       "    <password>testo1</password>\n"
                                                       "    <group>Work|Personal|Banking|Email</group>\n"
                                                       "  </password_data>\n"
                                                       "</Passwords>");
                delete newPassword;
                return QVector<PasswordManager*>();
            }
            passwords.append(newPassword);
        } // if StartElement Password
    } // while EndDocument

    file.close();
    if (xml.hasError()) {
        QMessageBox::warning(nullptr, "Error", "XML error: " + xml.errorString());
    }
    return passwords;
}

bool FileService::isValidGroup(const QString &group) {
    static const QSet<QString> validGroups = {"Work", "Personal", "Banking", "Email"};
    return validGroups.contains(group);
}
