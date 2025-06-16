#include "passwordformdialog.h"
#include "selectpassworddialog.h"
#include "ui_passwordformdialog.h"

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
    for (const auto &group : std::as_const(m_groupNames)) {
        ui->groupComboBox->addItem(group.groupName(), group.id());
    }
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
        connect(ui->editGroupButton, &QPushButton::clicked, this, &PasswordFormDialog::onEditGroupButtonClicked);
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

    if (!ok || newGroup.trimmed().isEmpty()) {
        return;
    }

    newGroup = newGroup.trimmed();
    newGroup = newGroup.left(1).toUpper() + newGroup.mid(1).toLower();

    bool exists = std::any_of(m_groupNames.begin(), m_groupNames.end(), [&newGroup](const Group &group) {
        return group.groupName().compare(newGroup, Qt::CaseInsensitive) == 0;
    });

    if (exists) {
        QMessageBox::information(this, tr("Group exists"), tr("A group with this name already exists"));
        return;
    }

    if (!m_dbManager->addGroup(newGroup)) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to add new group"));
        return;
    }
    QMessageBox::information(this, tr("Group added"), tr("Group '%1' has been added").arg(newGroup));

    ui->groupComboBox->clear();
    m_groupNames = m_dbManager->fetchGroups();
    for (const auto &group : std::as_const(m_groupNames)) {
        ui->groupComboBox->addItem(group.groupName(), group.id());
    }

    ui->groupComboBox->setCurrentText(newGroup);
}

void PasswordFormDialog::onDeleteGroupButtonClicked() {
    SelectPasswordDialog dialog(this, m_groupNames, PasswordMode::GroupDeleteMode);
    if (dialog.exec() == QDialog::Accepted) {
        const Group &group = dialog.selectedGroup();
        if (m_dbManager->hasPasswordsInGroup(group.id())) {
            QMessageBox::warning(this,
                                 tr("Cannot delete group"),
                                 tr("Group '%1' contains passwords and cannot be deleted.").arg(group.groupName()));
            return;
        }

        if (m_dbManager->deleteGroup(group.id())) {
            QMessageBox::information(this, tr("Group deleted"), tr("Group '%1' has been removed").arg(group.groupName()));
            m_groupNames = m_dbManager->fetchGroups();
            ui->groupComboBox->removeItem(dialog.selectedIndex());
        }
    }
}

void PasswordFormDialog::onEditGroupButtonClicked() {
    SelectPasswordDialog dialog(this, m_groupNames, PasswordMode::GroupEditMode);
    if (dialog.exec() == QDialog::Accepted) {
        const Group &oldGroup = dialog.selectedGroup();
        qDebug() << oldGroup.id();
        QString currentName = oldGroup.groupName();
        bool ok;
        QString newName = QInputDialog::getText(this,
                                                tr("Edit group"),
                                                tr("Enter new group name:"),
                                                QLineEdit::Normal, currentName, &ok);

        if (!ok || newName.trimmed().isEmpty()) {
            return;
        }

        newName = newName.trimmed();
        newName = newName.left(1).toUpper() + newName.mid(1).toLower();

        if (newName.compare(currentName, Qt::CaseSensitive) == 0) {
            QMessageBox::warning(this, tr("No changes"), tr("The name has not changed"));
            return;
        }

        bool exists = std::any_of(m_groupNames.begin(), m_groupNames.end(), [&newName](const Group &group) {
            return group.groupName().compare(newName, Qt::CaseInsensitive) == 0;
        });

        if (exists) {
            QMessageBox::information(this, tr("Group exists"), tr("A group with this name already exists"));
            return;
        }

        if (!m_dbManager->updateGroup(oldGroup.id(), newName)) {
            QMessageBox::critical(this, tr("Error"), tr("Failed to update group"));
            return;
        }

        QMessageBox::information(this, tr("Group edited"), tr("Group has been edited: '%1' -> '%2'")
                                                               .arg(oldGroup.groupName(), newName));

        ui->groupComboBox->clear();
        m_groupNames = m_dbManager->fetchGroups();
        for (const auto &group : std::as_const(m_groupNames)) {
            ui->groupComboBox->addItem(group.groupName(), group.id());
        }

        ui->groupComboBox->setCurrentText(newName);
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
