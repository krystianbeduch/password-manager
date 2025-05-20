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
                            .arg(QString::number(p->getId()), p->getServiceName(), p->getUsername());
        ui->comboBox->addItem(entry);
    }

    QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
    if (mode == PasswordMode::EditMode){
        setWindowTitle("Edit Password");
        ui->label->setText("Select the password you want to edit");
        okButton->setText("Edit");
    }
    else if (mode == PasswordMode::DeleteMode) {
        setWindowTitle("Delete Password");
        ui->label->setText("Select the password you want to delete");
        okButton->setText("Delete");
    }
}

SelectPasswordDialog::~SelectPasswordDialog() {
    delete ui;
}

int SelectPasswordDialog::getSelectedIndex() const { return ui->comboBox->currentIndex(); }
