#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QString>
#include <QMessageBox>
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
    bool isNewProfileRequested() const;

private slots:
    void onNewProfileCheckboxToggled(bool checked);
    void onAccept();

private:
    Ui::LoginDialog *ui;
    bool m_newProfileRequested;
};

#endif // LOGINDIALOG_H
