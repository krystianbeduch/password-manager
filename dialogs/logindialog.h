#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDebug>
#include <QDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QString>

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
