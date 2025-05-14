#ifndef PASSWORDDATA_H
#define PASSWORDDATA_H

#include <QString>

struct PasswordData {
    int id;
    QString serviceName;
    QString username;
    QString password;
    QString additionDate;
    QString group;
};

#endif // PASSWORDDATA_H
