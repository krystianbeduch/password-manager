#include "passwordformdialog.h"
#include "selectpassworddialog.h"

PasswordFormDialog::PasswordFormDialog(QWidget *parent,
                                       PasswordMode mode,
                                       DatabaseManager *dbManager)
    : QDialog(parent)
    , ui(new Ui::PasswordFormDialog)
    , m_mode(mode)
    , m_dbManager(dbManager)
{
    ui->setupUi(this);
    initUI();
    connectSignals();
}

PasswordFormDialog::PasswordFormDialog(QWidget *parent,
                                       PasswordManager *password,
                                       PasswordMode mode,
                                       DatabaseManager *dbManager)
    : QDialog(parent)
    , ui(new Ui::PasswordFormDialog)
    , m_mode(mode)
    , m_dbManager(dbManager)
{
    ui->setupUi(this);
    initUI();

    if (password) {
        ui->serviceNameLineEdit->setText(password->serviceName());
        ui->usernameLineEdit->setText(password->username());
        ui->groupComboBox->setCurrentText(password->group().groupName());
    }

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
        ui->headerLabel->setText(tr("Add new password"));
    }
    else if (m_mode == PasswordMode::EditMode) {
        ui->headerLabel->setText(tr("Edit password"));
    }

    m_groupNames = m_dbManager->fetchGroups();
    for (auto const &group : m_groupNames) {
        ui->groupComboBox->addItem(group.groupName(), group.id());
    }

    // ui->groupComboBox->setCurrentText(newGroup);
    // m_groupNames = m_dbManager->fetchGroups();
    // ui->groupComboBox->addItems(m_groupNames);
}

void PasswordFormDialog::connectSignals() {
    if (QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok)) {
        disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);

        if (m_mode == PasswordMode::AddMode) {
            setWindowTitle(tr("Add password"));
            ui->headerLabel->setText(tr("Add new password"));
            okButton->setText(tr("Add"));
        }
        else if (m_mode == PasswordMode::EditMode) {
            setWindowTitle(tr("Edit password"));
            ui->headerLabel->setText(tr("Edit selected password"));
            okButton->setText(tr("Edit"));
        }

        connect(okButton, &QPushButton::clicked, this, &PasswordFormDialog::onButtonClicked);
        connect(ui->generatePasswordButton, &QPushButton::clicked, this, &PasswordFormDialog::onGeneratePasswordClicked);
        connect(ui->showHidePasswordCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
            ui->passwordLineEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
            ui->showHidePasswordCheckBox->setText(checked ? tr("Hide password") : tr("Show password"));
        });
        connect(ui->addGroupButton, &QPushButton::clicked, this, &PasswordFormDialog::onAddGroupButtonClicked);
        connect(ui->deleteGroupButton, &QPushButton::clicked, this, &PasswordFormDialog::onDeleteGroupButtonClicked);
    }
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
        QMessageBox::warning(this, tr("Input Error"), tr("All fields are required"));
        return;
    }
    accept();
}

void PasswordFormDialog::onGeneratePasswordClicked() {
    QString generatedPassword = generateRandomPassword();
    ui->passwordLineEdit->setText(generatedPassword);
    ui->passwordLineEdit->setEchoMode(QLineEdit::Normal);
    ui->showHidePasswordCheckBox->setText(tr("Hide password"));
    ui->showHidePasswordCheckBox->setCheckState(Qt::Checked);
}

void PasswordFormDialog::onAddGroupButtonClicked() {
    bool ok;
    QString newGroup = QInputDialog::getText(this, tr("New group"),
                                             tr("Enter group name:"),
                                             QLineEdit::Normal, "", &ok);
    if (ok && !newGroup.trimmed().isEmpty()) {
        newGroup = newGroup.trimmed();

        bool exists = std::any_of(m_groupNames.begin(), m_groupNames.end(), [&newGroup](const Group &group) {
            return group.groupName().compare(newGroup, Qt::CaseInsensitive) == 0;
        });

        if (exists) {
            QMessageBox::information(this, tr("Group exists"), tr("Group with this name already exists"));
            return;
        }

        if (!m_dbManager->addGroup(newGroup)) {
            QMessageBox::critical(this, tr("Error"), tr("Failed to add new group"));
            return;
        }

        ui->groupComboBox->clear();
        m_groupNames = m_dbManager->fetchGroups();
        for (auto const &group : m_groupNames) {
            ui->groupComboBox->addItem(group.groupName(), group.id());
        }

        ui->groupComboBox->setCurrentText(newGroup);
    }
}

void PasswordFormDialog::onDeleteGroupButtonClicked() {
    SelectPasswordDialog dialog(this, m_groupNames);
    if (dialog.exec() == QDialog::Accepted && m_dbManager->deleteGroup(dialog.selectedText())) {
        QMessageBox::information(this, tr("Group deleted"), tr("Group %1 has been removed").arg(dialog.selectedText()));
        m_groupNames = m_dbManager->fetchGroups();
        ui->groupComboBox->removeItem(dialog.selectedIndex());
    }
}

QString PasswordFormDialog::serviceName() const { return ui->serviceNameLineEdit->text().trimmed(); }
QString PasswordFormDialog::username() const { return ui->usernameLineEdit->text().trimmed(); }
QString PasswordFormDialog::password() const { return ui->passwordLineEdit->text().trimmed(); }
Group PasswordFormDialog::group() const {
    QString groupName = ui->groupComboBox->currentText();
    int groupId = ui->groupComboBox->currentData().toInt();
    return Group(groupId, groupName);
}
