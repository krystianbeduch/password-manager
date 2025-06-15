#include "selectpassworddialog.h"
#include "ui_selectpassworddialog.h"

SelectPasswordDialog::SelectPasswordDialog(QWidget *parent,
                                           const QVector<PasswordManager*> passwordList,
                                           PasswordMode mode)
    : QDialog(parent)
    , ui(new Ui::SelectPasswordDialog)
{
    ui->setupUi(this);

    for (const auto *p : passwordList) {
        QString entry = QString("%1 | %2 | %3")
                            .arg(QString::number(p->id()), p->serviceName(), p->username());
        ui->comboBox->addItem(entry);
    }

    if (QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok)) {
        switch (mode) {
            case PasswordMode::EditMode:
                setWindowTitle(tr("Edit Password"));
                ui->label->setText(tr("Select the password you want to edit"));
                okButton->setText(tr("Edit"));
                break;
            case PasswordMode::DeleteMode:
                setWindowTitle(tr("Delete Password"));
                ui->label->setText(tr("Select the password you want to delete"));
                okButton->setText(tr("Delete"));
                break;
            default: return;
        }
    }
}


SelectPasswordDialog::SelectPasswordDialog(QWidget *parent,
                                           const QList<Group> groups,
                                           PasswordMode mode)
    : QDialog(parent)
    , ui(new Ui::SelectPasswordDialog)
{
    ui->setupUi(this);

    for (const auto &g : groups) {
        ui->comboBox->addItem(g.groupName(), g.id());
    }

    if (QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok)) {
        switch (mode) {
            case PasswordMode::GroupDeleteMode:
                setWindowTitle(tr("Delete Group"));
                ui->label->setText(tr("Select the group you want to delete"));
                okButton->setText(tr("Delete"));
                break;
            case PasswordMode::GroupEditMode:
                setWindowTitle(tr("Edit Group"));
                ui->label->setText(tr("Select the group you want to edit"));
                okButton->setText(tr("Edit"));
            default: return;
        }
    }
}


SelectPasswordDialog::~SelectPasswordDialog() {
    delete ui;
}

int SelectPasswordDialog::selectedIndex() const { return ui->comboBox->currentIndex(); }
QString SelectPasswordDialog::selectedText() const { return ui->comboBox->currentText(); }
Group SelectPasswordDialog::selectedGroup() const {
    QString groupName = ui->comboBox->currentText();
    int groupId = ui->comboBox->currentData().toInt();
    return Group(groupId, groupName);
}

