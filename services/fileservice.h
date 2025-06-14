#ifndef FILESERVICE_H
#define FILESERVICE_H

#include "../models/passwordmanager.h"
#include "../database/databasemanager.h"
#include <QString>
#include <QDebug>
#include <QFile>
#include <QVector>
#include <QSet>
#include <QHash>
#include <QByteArray>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

class FileService : QObject {
    Q_OBJECT;
public:
    static void exportToCSV(const QString &path, const QVector<PasswordManager*> &selected, const QHash<int, QString> &decrypted);
    static void exportToJSON(const QString &path, const QVector<PasswordManager*> &selected, const QHash<int, QString> &decrypted);
    static void exportToXML(const QString &path, const QVector<PasswordManager*> &selected, const QHash<int, QString> &decrypted);

    [[nodiscard]] static QVector<PasswordManager*> parseCSV(const QString &path, DatabaseManager *dbManager);
    [[nodiscard]] static QVector<PasswordManager*> parseJSON(const QString &path, DatabaseManager *dbManager);
    [[nodiscard]] static QVector<PasswordManager*> parseXML(const QString &path, DatabaseManager *dbManager);

private:
    static bool isValidGroup(const QString &group, DatabaseManager *dbManager);
};

#endif // FILESERVICE_H
