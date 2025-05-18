#ifndef ADDPASSWORDDIALOG_H
#define ADDPASSWORDDIALOG_H

#include "ui_passwordformdialog.h"
#include "PasswordMode.h"
#include "encryptionutils.h"
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

QT_BEGIN_NAMESPACE
namespace Ui {
class PasswordFormDialog;
}
QT_END_NAMESPACE

class PasswordFormDialog : public QDialog {
    Q_OBJECT
public:
    PasswordFormDialog(QWidget *parent = nullptr,
                       PasswordMode mode = PasswordMode::AddMode);
    PasswordFormDialog(QWidget *parent,
                       const QString &serviceName,
                       const QString &username,
                       const QString &password,
                       const QString &group,
                       PasswordMode mode);
     ~PasswordFormDialog();

    QString getServiceName() const;
    QString getUsername() const;
    QString getPassword() const;
    QString getGroup() const;

private slots:
    void onButtonClicked();

private:
    Ui::PasswordFormDialog *ui;    
    PasswordMode m_mode;

    void initUI();
    void connectSignals();
};

#endif // ADDPASSWORDDIALOG_H
