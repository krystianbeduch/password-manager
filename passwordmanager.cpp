#include "passwordmanager.h"

PasswordManager::PasswordManager(const QString &serviceName,
                                 const QString &username,
                                 const QString &password,
                                 const QString &additionalDate,
                                 const QString &group,
                                 int id)
    : m_serviceName(serviceName)
    , m_username(username)
    , m_password(password)
    , m_additionalDate(additionalDate)
    , m_group(group)
    , m_id(id) {}

int PasswordManager::getId() const { return m_id; }
QString PasswordManager::getServiceName() const { return m_serviceName; }
QString PasswordManager::getUsername() const { return m_username; }
QString PasswordManager::getPassword() const { return m_password; }
QString PasswordManager::getAdditionalDate() const { return m_additionalDate; }
QString PasswordManager::getGroup() const { return m_group; }

void PasswordManager::setId(int id) { m_id = id; }
void PasswordManager::setServiceName(const QString &serviceName) { m_serviceName = serviceName; }
void PasswordManager::setUsername(const QString &username) { m_username = username; }
void PasswordManager::setPassword(const QString &password) { m_password = password; }
void PasswordManager::setAdditionalDate(const QString &additionalDate) { m_additionalDate = additionalDate; }
void PasswordManager::setGroup(const QString &group) { m_group = group; }
