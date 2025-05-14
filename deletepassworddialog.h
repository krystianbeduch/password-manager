#ifndef DELETEPASSWORDDIALOG_H
#define DELETEPASSWORDDIALOG_H

#include <QDialog>
#include <QVector>
#include <QMessageBox>
#include "passwordmanager.h"

namespace Ui {
class DeletePasswordDialog;
}

class DeletePasswordDialog : public QDialog {
    Q_OBJECT

public:
    DeletePasswordDialog(QWidget *parent = nullptr, const QVector<PasswordManager*> passwords = {});
    ~DeletePasswordDialog();
    int getSelectedIndex() const;

private:
    Ui::DeletePasswordDialog *ui;

};

#endif // DELETEPASSWORDDIALOG_H
