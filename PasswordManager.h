#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QString>
#include <QDateTime>

class PasswordManager {
public:
    PasswordManager() = default;
    PasswordManager(int id,
                    const QString &serviceName,
                    const QString &username,
                    const QString &group,
                    const QDateTime &additionalDate);

    PasswordManager(int id,
                    const QString &serviceName,
                    const QString &username,
                    const QString &group,
                    const QDateTime &additionalDate,
                    int position);

    PasswordManager(const QString &serviceName,
                    const QString &username,
                    const QString &group,
                    const QDateTime &additionalDate);
    PasswordManager(const QString &serviceName,
                    const QString &username,
                    const QString &group);
    PasswordManager(const QString &serviceName,
                    const QString &username,
                    const QString &password,
                    const QString &group);
    ~PasswordManager() = default;

    [[nodiscard]] int id() const;
    [[nodiscard]] QString serviceName() const;
    [[nodiscard]] QString username() const;
    [[nodiscard]] QString password() const;
    [[nodiscard]] QDateTime additionalDate() const;
    [[nodiscard]] QString group() const;
    [[nodiscard]] int position() const;
    [[nodiscard]] QString formattedDate() const;

    void setId(int id);
    void setServiceName(const QString &serviceName);
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setGroup(const QString &group);
    void setAdditionalDate(const QDateTime &additionalDate);
    void setPostition(int position);

private:
    int m_id;
    QString m_serviceName;
    QString m_username;
    QString m_password;
    QString m_group;
    QDateTime m_additionalDate;
    int m_position;
};

#endif // PASSWORDMANAGER_H
