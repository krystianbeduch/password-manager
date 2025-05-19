#ifndef SELECTPASSWORDDIALOG_H
#define SELECTPASSWORDDIALOG_H

#include <QDialog>
#include <QVector>
#include <QMessageBox>
#include <QPushButton>
#include "passwordmanager.h"
#include "passwordMode.h"

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
    // PasswordMode m_mode;
};

#endif // SELECTPASSWORDDIALOG_H
