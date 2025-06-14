#ifndef SELECTPASSWORDDIALOG_H
#define SELECTPASSWORDDIALOG_H

#include "models/passwordmanager.h"
#include "models/passwordmode.h"
#include <QDialog>
#include <QVector>
#include <QStringList>
#include <QMessageBox>
#include <QPushButton>

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
                         const QStringList groupsName,
                         PasswordMode mode = PasswordMode::GroupMode);
    ~SelectPasswordDialog();
    [[nodiscard]] int selectedIndex() const;
    [[nodiscard]] QString selectedText() const;

private:
    Ui::SelectPasswordDialog *ui;
};

#endif // SELECTPASSWORDDIALOG_H
