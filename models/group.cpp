#include "group.h"

Group::Group(int id, const QString &groupName)
    : m_id(id), m_groupName(groupName) {}


Group::Group(const QString &groupName)
    : m_groupName(groupName) {}

Group::Group(int id)
    : m_id(id) {}

int Group::id() const { return m_id; }
QString Group::groupName() const { return m_groupName; }

void Group::setId(int id) { m_id = id; }
void Group::setGroupName(const QString &groupName) { m_groupName = groupName; }
