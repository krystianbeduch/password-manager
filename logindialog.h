#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QString>
#include <QByteArray>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    QString password() const;

private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
