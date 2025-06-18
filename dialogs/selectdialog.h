#ifndef SELECTDIALOG_H
#define SELECTDIALOG_H

#include "group.h"
#include "passwordmanager.h"
#include "passwordmode.h"

#include <QDialog>
#include <QList>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QVector>

namespace Ui {
class SelectDialog;
}

class SelectDialog : public QDialog {
    Q_OBJECT

public:
    SelectDialog(QWidget *parent = nullptr,
                         const QVector<PasswordManager*> passwordList = {},
                         PasswordMode mode = PasswordMode::EditMode);
    SelectDialog(QWidget *parent,
                         const QVector<Group> groups,
                         PasswordMode mode);
    ~SelectDialog();
    [[nodiscard]] int selectedIndex() const;
    [[nodiscard]] QString selectedText() const;
    [[nodiscard]] Group selectedGroup() const;

private:
    Ui::SelectDialog *ui;
};

#endif // SELECTDIALOG_H
