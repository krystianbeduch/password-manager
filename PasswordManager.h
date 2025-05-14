#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QString>

class PasswordManager {
public:
    PasswordManager() = default;
    PasswordManager(const QString &serviceName,
                    const QString &username,
                    const QString &password,
                    const QString &additionalDate,
                    const QString &group,
                    int id);
    ~PasswordManager() = default;

    int getId() const;
    QString getServiceName() const;
    QString getUsername() const;
    QString getPassword() const;
    QString getAdditionalDate() const;
    QString getGroup() const;

    void setId(int id);
    void setServiceName(const QString &serviceName);
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setAdditionalDate(const QString &additionalDate);
    void setGroup(const QString &group);

private:
    int m_id;
    QString m_serviceName;
    QString m_username;
    QString m_password;
    QString m_additionalDate;
    QString m_group;
};

#endif // PASSWORDMANAGER_H
