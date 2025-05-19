#ifndef FILESERVICE_H
#define FILESERVICE_H

#include "passwordmanager.h"
#include <QString>
#include <QDebug>
#include <QFile>
#include <QVector>
#include <QSet>
#include <QByteArray>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

class FileService {
public:

    static void exportToCSV(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted);
    static void exportToJSON(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted);
    static void exportToXML(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted);

    static QVector<PasswordManager*> parseCSV(const QString &path);
    static QVector<PasswordManager*> parseJSON(const QString &path);
    static QVector<PasswordManager*> parseXML(const QString &path);

private:
    static bool isValidGroup(const QString &group);
};

#endif // FILESERVICE_H
