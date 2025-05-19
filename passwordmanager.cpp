#include "passwordmanager.h"

PasswordManager::PasswordManager(const QString &serviceName,
                                 const QString &username,
                                 // const QString &password,
                                 const QDateTime &additionalDate,
                                 const QString &group,
                                 int id)
    : m_serviceName(serviceName)
    , m_username(username)
    // , m_password(password)
    , m_additionalDate(additionalDate)
    , m_group(group)
    , m_id(id) {}

PasswordManager::PasswordManager(const QString &serviceName,
                                 const QString &username,
                                 // const QString &password,
                                 const QDateTime &additionalDate,
                                 const QString &group)
    : m_serviceName(serviceName)
    , m_username(username)
    // , m_password(password)
    , m_additionalDate(additionalDate)
    , m_group(group) {}

PasswordManager::PasswordManager(const QString &serviceName,
                                 const QString &username,
                                 const QString &group)
    : m_serviceName(serviceName)
    , m_username(username)
    , m_group(group) {}

PasswordManager::PasswordManager(const QString &serviceName,
                                 const QString &username,
                                 const QString &password,
                                 const QString &group)
    : m_serviceName(serviceName)
    , m_username(username)
    , m_password(password)
    , m_group(group) {}


int PasswordManager::getId() const { return m_id; }
QString PasswordManager::getServiceName() const { return m_serviceName; }
QString PasswordManager::getUsername() const { return m_username; }
QString PasswordManager::getPassword() const { return m_password; }
QDateTime PasswordManager::getAdditionalDate() const { return m_additionalDate; }
QString PasswordManager::getGroup() const { return m_group; }
int PasswordManager::getPosition() const { return m_position; }


void PasswordManager::setId(int id) { m_id = id; }
void PasswordManager::setServiceName(const QString &serviceName) { m_serviceName = serviceName; }
void PasswordManager::setUsername(const QString &username) { m_username = username; }
void PasswordManager::setPassword(const QString &password) { m_password = password; }
void PasswordManager::setAdditionalDate(const QDateTime &additionalDate) { m_additionalDate = additionalDate; }
void PasswordManager::setGroup(const QString &group) { m_group = group; }
void PasswordManager::setPostition(int position) { m_position = position; }

QString PasswordManager::getFormattedDate() const {
    return m_additionalDate.toLocalTime().toString("dd.MM.yyyy HH:mm");
}
