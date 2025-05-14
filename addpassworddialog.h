#ifndef ADDPASSWORDDIALOG_H
#define ADDPASSWORDDIALOG_H

#include "ui_addpassworddialog.h"
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpressionValidator>

QT_BEGIN_NAMESPACE
namespace Ui {
class AddPasswordDialog;
}
QT_END_NAMESPACE

class AddPasswordDialog : public QDialog {
    Q_OBJECT
public:
    explicit AddPasswordDialog(QWidget *parent = nullptr);
     ~AddPasswordDialog();

    QString getServiceName() const;
    QString getUsername() const;
    QString getPassword() const;
    QString getGroup() const;

private:
    Ui::AddPasswordDialog *ui;

};

#endif // ADDPASSWORDDIALOG_H
