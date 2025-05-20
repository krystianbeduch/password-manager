#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QString>
#include <QDateTime>

class PasswordManager {
public:
    PasswordManager() = default;
    PasswordManager(const QString &serviceName,
                    const QString &username,
                    const QDateTime &additionalDate,
                    const QString &group,
                    int id);
    PasswordManager(const QString &serviceName,
                    const QString &username,
                    const QDateTime &additionalDate,
                    const QString &group);
    PasswordManager(const QString &serviceName,
                    const QString &username,
                    const QString &group);

    PasswordManager(const QString &serviceName,
                    const QString &username,
                    const QString &password,
                    const QString &group);

    ~PasswordManager() = default;

    int getId() const;
    QString getServiceName() const;
    QString getUsername() const;
    QString getPassword() const;
    QDateTime getAdditionalDate() const;
    QString getGroup() const;
    int getPosition() const;

    void setId(int id);
    void setServiceName(const QString &serviceName);
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setAdditionalDate(const QDateTime &additionalDate);
    void setGroup(const QString &group);
    void setPostition(int position);
    QString getFormattedDate() const;

private:
    int m_id;
    QString m_serviceName;
    QString m_username;
    QString m_password; // TYLKO DO TESTÓW
    QDateTime m_additionalDate;
    QString m_group;
    int m_position;
};

#endif // PASSWORDMANAGER_H
