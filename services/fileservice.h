#ifndef FILESERVICE_H
#define FILESERVICE_H

#include "passwordmanager.h"
#include "databasemanager.h"
#include "group.h"
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
    static QHash<QString, Group> validGroupsMap;
    static bool groupsInitialized;
    static void initializeGroups(DatabaseManager *dbManager);
    static bool isValidGroup(const Group &group, DatabaseManager *dbManager);
    static QString normalizeGroupName(const QString &name);
};

#endif // FILESERVICE_H
