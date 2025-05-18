#include "exportservice.h"

void ExportService::exportToCSV(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted) {
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

void ExportService::exportToJSON(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted) {
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

void ExportService::exportToXML(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted) {
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
            writer.writeStartElement("password");
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
