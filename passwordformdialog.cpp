#include "passwordformdialog.h"

PasswordFormDialog::PasswordFormDialog(QWidget *parent,
                                       PasswordMode mode)
    : QDialog(parent)
    , ui(new Ui::PasswordFormDialog)
    , m_mode(mode)
{
    ui->setupUi(this);
    initUI();
    connectSignals();
}

PasswordFormDialog::PasswordFormDialog(QWidget *parent,
                                    const QString &serviceName,
                                    const QString &username,
                                    const QString &password,
                                    const QString &group,
                                    PasswordMode mode)
    : QDialog(parent)
    , ui(new Ui::PasswordFormDialog)
    , m_mode(mode)
{
    ui->setupUi(this);
    initUI();

    ui->serviceNameLineEdit->setText(serviceName);
    ui->usernameLineEdit->setText(username);
    ui->passwordLineEdit->setText(password);
    ui->groupComboBox->setCurrentText(group);

    connectSignals();
}

PasswordFormDialog::PasswordFormDialog(QWidget *parent,
                                       PasswordManager *password,
                                       PasswordMode mode)
    : QDialog(parent)
    , ui(new Ui::PasswordFormDialog)
    , m_mode(mode)
{
    ui->setupUi(this);
    initUI();

    ui->serviceNameLineEdit->setText(password->getServiceName());
    ui->usernameLineEdit->setText(password->getUsername());
    ui->groupComboBox->setCurrentText(password->getGroup());

    connectSignals();
}

PasswordFormDialog::~PasswordFormDialog() {
    delete ui;
}

void PasswordFormDialog::initUI() {
    const QRegularExpression regex("^[a-zA-Z0-9!@#$%^&*()_+=\\-\\[\\]{};:'\"\\\\|,.<>/?]*$");
    QValidator *validator = new QRegularExpressionValidator(regex, this);
    ui->passwordLineEdit->setValidator(validator);
    if (m_mode == PasswordMode::AddMode) {
        ui->headerLabel->setText("Add new password");
    }
    else if (m_mode == PasswordMode::EditMode) {
        ui->headerLabel->setText("Edit password");
    }
}

void PasswordFormDialog::connectSignals() {
    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

    if (m_mode == PasswordMode::AddMode) {
        setWindowTitle("Add password");
        ui->headerLabel->setText("Add new password");
        okButton->setText("Add");
    }
    else if (m_mode == PasswordMode::EditMode) {
        setWindowTitle("Edit password");
        ui->headerLabel->setText("Edit selected password");
        okButton->setText("Edit");
    }

    connect(okButton, &QPushButton::clicked, this, &PasswordFormDialog::onButtonClicked);
    connect(ui->generatePasswordButton, &QPushButton::clicked, this, &PasswordFormDialog::onGeneratePasswordClicked);
    connect(ui->showHidePasswordCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        ui->passwordLineEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
        ui->showHidePasswordCheckBox->setText(checked ? "Hide password" : "Show password");
    });
}

QString PasswordFormDialog::generateRandomPassword(int length) {
    const QString chars = "abcdefghijklmnopqrstuvwxyz"
                          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                          "0123456789"
                          "!@#$%^&*()_+-=[]{}|;:',.<>?/";
    QString randomPassword;
    for (int i = 0; i < length; i++) {
        int index = QRandomGenerator::global()->bounded(chars.length());
        randomPassword.append(chars.at(index));
    }
    return randomPassword;
}

void PasswordFormDialog::onButtonClicked() {
    if (ui->serviceNameLineEdit->text().trimmed().isEmpty() ||
        ui->usernameLineEdit->text().trimmed().isEmpty() ||
        ui->passwordLineEdit->text().trimmed().isEmpty()
        ) {
        QMessageBox::warning(this, "Input Error", "All fields are required");
        return;
    }
    accept();
}

void PasswordFormDialog::onGeneratePasswordClicked() {
    QString generatedPassword = generateRandomPassword();
    ui->passwordLineEdit->setText(generatedPassword);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
    ui->showHidePasswordCheckBox->setText("Hide password");
    ui->showHidePasswordCheckBox->setCheckState(Qt::Checked);
}

QString PasswordFormDialog::getServiceName() const { return ui->serviceNameLineEdit->text().trimmed(); }
QString PasswordFormDialog::getUsername() const { return ui->usernameLineEdit->text().trimmed(); }
QString PasswordFormDialog::getPassword() const { return ui->passwordLineEdit->text().trimmed(); }
QString PasswordFormDialog::getGroup() const { return ui->groupComboBox->currentText(); }
