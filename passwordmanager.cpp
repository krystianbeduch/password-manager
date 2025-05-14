#include "passwordmanager.h"

PasswordManager::PasswordManager(const QString &serviceName,
                                 const QString &username,
                                 const QString &password,
                                 const QString &additionalDate,
                                 const QString &group,
                                 int id)
    : serviceName(serviceName)
    , username(username)
    , password(password)
    , additionalDate(additionalDate)
    , group(group)
    , id(id)
{}

int PasswordManager::getId() const { return id; }
QString PasswordManager::getServiceName() const { return serviceName; }
QString PasswordManager::getUsername() const { return username; }
QString PasswordManager::getPassword() const { return password; }
QString PasswordManager::getAdditionalDate() const { return additionalDate; }
QString PasswordManager::getGroup() const { return group; }

void PasswordManager::setId(int value) { id = value; }
void PasswordManager::setServiceName(const QString &value) { serviceName = value; }
void PasswordManager::setUsername(const QString &value) { username = value; }
void PasswordManager::setPassword(const QString &value) { password = value; }
void PasswordManager::setAdditionalDate(const QString &value) { additionalDate = value; }
void PasswordManager::setGroup(const QString &value) { group = value; }
