#ifndef SELECTPASSWORDDIALOG_H
#define SELECTPASSWORDDIALOG_H

#include "group.h"
#include "passwordmanager.h"
#include "passwordmode.h"

#include <QDialog>
#include <QList>
#include <QMessageBox>
#include <QPushButton>
#include <QVector>

namespace Ui {
class SelectPasswordDialog;
}

class SelectPasswordDialog : public QDialog {
    Q_OBJECT

public:
    SelectPasswordDialog(QWidget *parent = nullptr,
                         const QVector<PasswordManager*> passwordList = {},
                         PasswordMode mode = PasswordMode::EditMode);
    SelectPasswordDialog(QWidget *parent,
                         const QList<Group> groups,
                         PasswordMode mode);
    ~SelectPasswordDialog();
    [[nodiscard]] int selectedIndex() const;
    [[nodiscard]] QString selectedText() const;
    [[nodiscard]] Group selectedGroup() const;

private:
    Ui::SelectPasswordDialog *ui;
};

#endif // SELECTPASSWORDDIALOG_H
