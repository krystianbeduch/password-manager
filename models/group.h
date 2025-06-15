#ifndef GROUP_H
#define GROUP_H

#include <QString>

class Group {
public:
    Group() = default;
    Group(int id, const QString &groupName);
    Group(const QString &groupName);
    Group(int id);
    ~Group() = default;

    [[nodiscard]] int id() const;
    [[nodiscard]] QString groupName() const;

    void setId(int id);
    void setGroupName(const QString &groupName);

private:
    int m_id;
    QString m_groupName;
};

#endif // GROUP_H
