#include "deletepassworddialog.h"
#include "ui_deletepassworddialog.h"

DeletePasswordDialog::DeletePasswordDialog(QWidget *parent, const QVector<PasswordManager*> passwordList)
    : QDialog(parent)
    , ui(new Ui::DeletePasswordDialog) {
    ui->setupUi(this);

    // Docelowo pobor z bazy danych

    for (PasswordManager *p : passwordList) {
        QString entry = QString("%1 | %2 | %3").arg(p->getId()).arg(p->getServiceName()).arg(p->getUsername());
        ui->comboBox->addItem(entry);
    }

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [=]() {
        if (ui->comboBox->currentIndex() == -1) {
            QMessageBox::warning(this, "No Selection", "Please select a password to delete");
            return;
        }

        const QString itemText = ui->comboBox->currentText();
        QMessageBox::StandardButton confirm = QMessageBox::question(
            this, "Confirm Deletion",
            QString("Do you want to delete this password?\n\n%1").arg(itemText),
            QMessageBox::Yes | QMessageBox::No
        );

        if (confirm == QMessageBox::Yes) {
            accept();
        }
    });

    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);



}

DeletePasswordDialog::~DeletePasswordDialog() {
    delete ui;
}

int DeletePasswordDialog::getSelectedIndex() const {
    return ui->comboBox->currentIndex();
}
