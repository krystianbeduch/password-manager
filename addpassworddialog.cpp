#include "addpassworddialog.h"

AddPasswordDialog::AddPasswordDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddPasswordDialog)
{
    ui->setupUi(this);

    QRegularExpression regex("^[a-zA-Z0-9!@#$%^&*()_+=\\-\\[\\]{};:'\"\\\\|,.<>/?]*$");
    QValidator *validator = new QRegularExpressionValidator(regex, this);
    ui->passwordLineEdit->setValidator(validator);

    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(okButton, &QPushButton::clicked, this, [this]() {
        if (ui->serviceNameLineEdit->text().trimmed().isEmpty() ||
            ui->usernameLineEdit->text().trimmed().isEmpty() ||
            ui->passwordLineEdit->text().trimmed().isEmpty()
        ) {
            QMessageBox::warning(this, "Input Error", "All fields are required");
            return;
        }
        accept();
    });

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}


AddPasswordDialog::~AddPasswordDialog() {
    delete ui;
}


QString AddPasswordDialog::getServiceName() const {
    return ui->serviceNameLineEdit->text().trimmed();
}

QString AddPasswordDialog::getUsername() const {
    return ui->usernameLineEdit->text().trimmed();
}

QString AddPasswordDialog::getPassword() const {
    return ui->passwordLineEdit->text().trimmed();
}
QString AddPasswordDialog::getGroup() const {
    return ui->groupComboBox->currentText();
}
