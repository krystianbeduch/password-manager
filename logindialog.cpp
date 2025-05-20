#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog) {
    ui->setupUi(this);

    const QRegularExpression regex("^[a-zA-Z0-9!@#$%^&*()_+=\\-\\[\\]{};:'\"\\\\|,.<>/?]*$");
    QValidator *validator = new QRegularExpressionValidator(regex, this);
    ui->lineEdit->setValidator(validator);
}

LoginDialog::~LoginDialog() {
    delete ui;
}

QString LoginDialog::getPassword() const { return ui->lineEdit->text().trimmed(); }
