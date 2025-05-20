#ifndef SELECTPASSWORDDIALOG_H
#define SELECTPASSWORDDIALOG_H

#include "passwordmanager.h"
#include "passwordmode.h"
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
    int getSelectedIndex() const;

private:
    Ui::SelectPasswordDialog *ui;
};

#endif // SELECTPASSWORDDIALOG_H
