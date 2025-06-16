#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , m_newProfileRequested(false)
{
    ui->setupUi(this);

    const QRegularExpression regex("^[a-zA-Z0-9!@#$%^&*()_+=\\-\\[\\]{};:'\"\\\\|,.<>/?]*$");
    QValidator *validator = new QRegularExpressionValidator(regex, this);
    ui->passwordLineEdit->setValidator(validator);
    ui->password2LineEdit->setValidator(validator);
    ui->password2LineEdit->setVisible(false);
    this->adjustSize();

    connect(ui->newProfileCheckBox, &QCheckBox::toggled, this, &LoginDialog::onNewProfileCheckboxToggled);
    disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &LoginDialog::onAccept);
    onNewProfileCheckboxToggled(false);
}

LoginDialog::~LoginDialog() {
    delete ui;
}

void LoginDialog::onNewProfileCheckboxToggled(bool isChecked) {
    ui->password2LineEdit->setVisible(isChecked);
    m_newProfileRequested = isChecked;
    this->adjustSize();
}


void LoginDialog::onAccept() {
    QString password = ui->passwordLineEdit->text().trimmed();
    if (password.isEmpty()) {
        QMessageBox::warning(this, tr("Invalid input"), tr("Password cannot be empty."));
        return;
    }

    if (m_newProfileRequested) {
        QString pass2 = ui->password2LineEdit->text().trimmed();
        if (password != pass2) {
            QMessageBox::warning(this, tr("Password mismatch"), tr("Passwords do not match."));
            return;
        }

        QMessageBox::StandardButton confirm = QMessageBox::question(this,
                                                                    tr("Create new profile"),
                                                                    tr("Creating a new profile will DELETE all existing data.\n\n"
                                                                       "Do you want to continue?"),
                                                                    QMessageBox::Yes | QMessageBox::No
        );

        if (confirm != QMessageBox::Yes) {
            return;
        }
    }
    accept();
}

QString LoginDialog::password() const { return ui->passwordLineEdit->text().trimmed(); }
bool LoginDialog::isNewProfileRequested() const { return m_newProfileRequested; }
