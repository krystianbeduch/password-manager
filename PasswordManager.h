#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QString>
#include <QDateTime>

class PasswordManager {
public:
    PasswordManager() = default;
    PasswordManager(const QString &serviceName,
                    const QString &username,
                    // const QString &password,
                    const QDateTime &additionalDate,
                    const QString &group,
                    int id);
    PasswordManager(const QString &serviceName,
                    const QString &username,
                    // const QString &password,
                    const QDateTime &additionalDate,
                    const QString &group);
    PasswordManager(const QString &serviceName,
                    const QString &username,
                    const QString &group);
    PasswordManager(const QString &serviceName, // TESTOWY KONSTRUKTOR - domyślnie zostawić 3-parametrowy
                    const QString &username,
                    const QString &password, // TYLKO DO TESTÓW
                    const QString &group);

    ~PasswordManager() = default;

    int getId() const;
    QString getServiceName() const;
    QString getUsername() const;
    QString getPassword() const; // TYLKO DO TESTÓW
    QDateTime getAdditionalDate() const;
    QString getGroup() const;    

    void setId(int id);
    void setServiceName(const QString &serviceName);
    void setUsername(const QString &username);
    void setPassword(const QString &password); // TYLKO DO TESTÓW
    void setAdditionalDate(const QDateTime &additionalDate);
    void setGroup(const QString &group);

    QString getFormattedDate() const;

private:
    int m_id;
    QString m_serviceName;
    QString m_username;
    QString m_password; // TYLKO DO TESTÓW
    QDateTime m_additionalDate;
    QString m_group;
};

#endif // PASSWORDMANAGER_H
