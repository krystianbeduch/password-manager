#include "passwordmanager.h"

PasswordManager::PasswordManager(int id,
                                 const QString &serviceName,
                                 const QString &username,
                                 const QString &group,
                                 const QDateTime &additionalDate)
    : m_id(id)
    , m_serviceName(serviceName)
    , m_username(username)
    , m_group(group)
    , m_additionalDate(additionalDate) {}

PasswordManager::PasswordManager(const QString &serviceName,
                                 const QString &username,
                                 const QString &group,
                                 const QDateTime &additionalDate)
    : m_serviceName(serviceName)
    , m_username(username)    
    , m_group(group)
    , m_additionalDate(additionalDate) {}

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
QString PasswordManager::getGroup() const { return m_group; }
QDateTime PasswordManager::getAdditionalDate() const { return m_additionalDate; }
int PasswordManager::getPosition() const { return m_position; }

void PasswordManager::setId(int id) { m_id = id; }
void PasswordManager::setServiceName(const QString &serviceName) { m_serviceName = serviceName; }
void PasswordManager::setUsername(const QString &username) { m_username = username; }
void PasswordManager::setPassword(const QString &password) { m_password = password; }
void PasswordManager::setGroup(const QString &group) { m_group = group; }
void PasswordManager::setAdditionalDate(const QDateTime &additionalDate) { m_additionalDate = additionalDate; }
void PasswordManager::setPostition(int position) { m_position = position; }
QString PasswordManager::getFormattedDate() const { return m_additionalDate.toLocalTime().toString("dd.MM.yyyy HH:mm"); }
