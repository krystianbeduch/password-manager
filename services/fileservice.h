#ifndef FILESERVICE_H
#define FILESERVICE_H

#include "group.h"
#include "databasemanager.h"
#include "passwordmanager.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QSet>
#include <QString>
#include <QVector>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class FileService : QObject {
    Q_OBJECT;
public:
    static void exportToCSV(const QString &path, const QVector<PasswordManager*> &selected, const QHash<int, QString> &decrypted);
    static void exportToJSON(const QString &path, const QVector<PasswordManager*> &selected, const QHash<int, QString> &decrypted);
    static void exportToXML(const QString &path, const QVector<PasswordManager*> &selected, const QHash<int, QString> &decrypted);

    [[nodiscard]] static QVector<PasswordManager*> parseCSV(const QString &path, DatabaseManager *dbManager);
    [[nodiscard]] static QVector<PasswordManager*> parseJSON(const QString &path, DatabaseManager *dbManager);
    [[nodiscard]] static QVector<PasswordManager*> parseXML(const QString &path, DatabaseManager *dbManager);

    [[nodiscard]] static std::optional<QString> findFileInParentDirs(const QString &relativeFilePath, int maxUpLevels = 5);

private:
    static QHash<QString, Group> validGroupsMap;
    static bool isValidGroup(const Group &group, DatabaseManager *dbManager);
    static QString normalizeGroupName(const QString &name);
};

#endif // FILESERVICE_H
