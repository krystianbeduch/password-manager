#ifndef EXPORTSERVICE_H
#define EXPORTSERVICE_H

#include <QString>
#include <QFile>
#include <QVector>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QXmlStreamWriter>
#include "passwordmanager.h"

class ExportService {
public:

    static void exportToCSV(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted);
    static void exportToJSON(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted);
    static void exportToXML(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted);
};

#endif // EXPORTSERVICE_H
