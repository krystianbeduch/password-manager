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

    void setId(int value);
    void setServiceName(const QString &value);
    void setUsername(const QString &value);
    void setPassword(const QString &value);
    void setAdditionalDate(const QString &value);
    void setGroup(const QString &value);

private:
    int id;
    QString serviceName;
    QString username;
    QString password;
    QString additionalDate;
    QString group;
};

#endif // PASSWORDMANAGER_H
