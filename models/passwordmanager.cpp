#include "passwordmanager.h"

PasswordManager::PasswordManager(int id,
                                 const QString &serviceName,
                                 const QString &username,
                                 const Group &group,
                                 const QDateTime &additionalDate)
    : m_id(id)
    , m_serviceName(serviceName)
    , m_username(username)
    , m_group(group)
    , m_additionalDate(additionalDate) {}


PasswordManager::PasswordManager(int id,
                                 const QString &serviceName,
                                 const QString &username,
                                 const Group &group,
                                 const QDateTime &additionalDate,
                                 int position)
    : m_id(id)
    , m_serviceName(serviceName)
    , m_username(username)
    , m_group(group)
    , m_additionalDate(additionalDate)
    , m_position(position) {}

PasswordManager::PasswordManager(const QString &serviceName,
                                 const QString &username,
                                 const Group &group,
                                 const QDateTime &additionalDate)
    : m_serviceName(serviceName)
    , m_username(username)    
    , m_group(group)
    , m_additionalDate(additionalDate) {}

PasswordManager::PasswordManager(const QString &serviceName,
                                 const QString &username,
                                 const Group &group)
    : m_serviceName(serviceName)
    , m_username(username)
    , m_group(group) {}

PasswordManager::PasswordManager(const QString &serviceName,
                                 const QString &username,
                                 const QString &password,
                                 const Group &group)
    : m_serviceName(serviceName)
    , m_username(username)
    , m_password(password)
    , m_group(group) {}

int PasswordManager::id() const { return m_id; }
QString PasswordManager::serviceName() const { return m_serviceName; }
QString PasswordManager::username() const { return m_username; }
QString PasswordManager::password() const { return m_password; }
Group PasswordManager::group() const { return m_group; }
QDateTime PasswordManager::additionalDate() const { return m_additionalDate; }
int PasswordManager::position() const { return m_position; }

void PasswordManager::setId(int id) { m_id = id; }
void PasswordManager::setServiceName(const QString &serviceName) { m_serviceName = serviceName; }
void PasswordManager::setUsername(const QString &username) { m_username = username; }
void PasswordManager::setPassword(const QString &password) { m_password = password; }
void PasswordManager::setGroup(const Group &group) { m_group = group; }
void PasswordManager::setAdditionalDate(const QDateTime &additionalDate) { m_additionalDate = additionalDate; }
void PasswordManager::setPosition(int position) { m_position = position; }
QString PasswordManager::formattedDate() const { return m_additionalDate.toLocalTime().toString("dd.MM.yyyy HH:mm"); }
