#ifndef SELECTPASSWORDDIALOG_H
#define SELECTPASSWORDDIALOG_H

#include "models/passwordmanager.h"
#include "models/passwordmode.h"
#include <QDialog>
#include <QVector>
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
    ~SelectPasswordDialog();
    [[nodiscard]] int selectedIndex() const;

private:
    Ui::SelectPasswordDialog *ui;
};

#endif // SELECTPASSWORDDIALOG_H
