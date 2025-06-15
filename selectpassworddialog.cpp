#include "selectpassworddialog.h"
#include "ui_selectpassworddialog.h"

SelectPasswordDialog::SelectPasswordDialog(QWidget *parent,
                                           const QVector<PasswordManager*> passwordList,
                                           PasswordMode mode)
    : QDialog(parent)
    , ui(new Ui::SelectPasswordDialog)
{
    ui->setupUi(this);

    for (PasswordManager *p : passwordList) {
        QString entry = QString("%1 | %2 | %3")
                            .arg(QString::number(p->id()), p->serviceName(), p->username());
        ui->comboBox->addItem(entry);
    }

    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (mode == PasswordMode::EditMode){
        setWindowTitle(tr("Edit Password"));
        ui->label->setText(tr("Select the password you want to edit"));
        okButton->setText(tr("Edit"));
    }
    else if (mode == PasswordMode::DeleteMode) {
        setWindowTitle(tr("Delete Password"));
        ui->label->setText(tr("Select the password you want to delete"));
        okButton->setText(tr("Delete"));
    }
}


SelectPasswordDialog::SelectPasswordDialog(QWidget *parent,
                                           const QList<Group> groups,
                                           PasswordMode mode)
    : QDialog(parent)
    , ui(new Ui::SelectPasswordDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Delete Group"));
    ui->label->setText(tr("Select the group you want to delete"));

    for (auto const &g : groups) {
        ui->comboBox->addItem(g.groupName());
    }
    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setText(tr("Delete"));
}


SelectPasswordDialog::~SelectPasswordDialog() {
    delete ui;
}

int SelectPasswordDialog::selectedIndex() const { return ui->comboBox->currentIndex(); }
QString SelectPasswordDialog::selectedText() const { return ui->comboBox->currentText(); }
