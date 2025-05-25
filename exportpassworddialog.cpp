#include "exportpassworddialog.h"
#include "ui_exportpassworddialog.h"

ExportPasswordDialog::ExportPasswordDialog(QWidget *parent,
                                           const QVector<PasswordManager*> &passwords)
    : QDialog(parent)
    , ui(new Ui::ExportPasswordDialog)
    , m_passwords(passwords)
{
    ui->setupUi(this);

    connect(ui->selectAllCheckBox, &QCheckBox::toggled, this, &ExportPasswordDialog::onSelectAllToggled);
    connect(ui->csvCheckBox, &QCheckBox::toggled, ui->csvFilenameLineEdit, &QLineEdit::setVisible);
    connect(ui->jsonCheckBox, &QCheckBox::toggled, ui->jsonFilenameLineEdit, &QLineEdit::setVisible);
    connect(ui->xmlCheckBox, &QCheckBox::toggled, ui->xmlFilenameLineEdit, &QLineEdit::setVisible);
    disconnect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    if (QPushButton *okButton = ui->buttonBox->button(QDialogButtonBox::Ok)) {
        okButton->setText("Export");
        connect(okButton, &QPushButton::clicked, this, &ExportPasswordDialog::onExportClicked);
    }

    ui->csvFilenameLineEdit->setVisible(false);
    ui->jsonFilenameLineEdit->setVisible(false);
    ui->xmlFilenameLineEdit->setVisible(false);

    for (const auto *p : std::as_const(m_passwords)) {
        QString label = QString("%1 | %2 | %3")
                            .arg(QString::number(p->id()),
                                 p->serviceName(),
                                 p->username());
        QListWidgetItem *item = new QListWidgetItem(label);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        ui->listWidget->addItem(item);
    }
}

ExportPasswordDialog::~ExportPasswordDialog() {
    delete ui;
}

void ExportPasswordDialog::onSelectAllToggled(bool checked) {
    for (int i = 0; i < ui->listWidget->count(); i++) {
        QListWidgetItem *item = ui->listWidget->item(i);
        item->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
    }
}

void ExportPasswordDialog::onExportClicked() {
    m_selectedPasswords.clear();
    for (int i = 0; i< ui->listWidget->count(); i++) {
        if (ui->listWidget->item(i)->checkState() == Qt::Checked) {
            m_selectedPasswords.append(m_passwords[i]);
        }
    }

    if (m_selectedPasswords.isEmpty()) {
        QMessageBox::warning(this, "No selected passwords", "Check at least 1 password");
        return;
    }

    if ( !(ui->csvCheckBox->isChecked() || ui->jsonCheckBox->isChecked() || ui->xmlCheckBox->isChecked()) ) {
        QMessageBox::warning(this, "No format", "Select at least 1 format");
        return;
    }

    bool fileNameCorrected =
        isValidFilename(ui->csvCheckBox, ui->csvFilenameLineEdit, "CSV") &&
        isValidFilename(ui->jsonCheckBox, ui->jsonFilenameLineEdit, "JSON") &&
        isValidFilename(ui->xmlCheckBox, ui->xmlFilenameLineEdit, "XML");

    if (!fileNameCorrected) {
        return;
    }

    m_dir = QFileDialog::getExistingDirectory(this, "Select destination folder");
    if (m_dir.isEmpty()) {
        return;
    }
    accept();
}

bool ExportPasswordDialog::isValidFilename(QCheckBox *checkBox, QLineEdit *lineEdit, const QString &formatName) {
    if (checkBox->isChecked() && lineEdit->text().isEmpty()) {
        QMessageBox::warning(this, "No filename", tr("Enter the name of the %1 file").arg(formatName));
        return false;
    }
    return true;
}

QVector<PasswordManager*> ExportPasswordDialog::selectPasswords() const { return m_selectedPasswords; }
bool ExportPasswordDialog::isCSVChecked() const { return ui->csvCheckBox->isChecked(); }
QString ExportPasswordDialog::csvFileName() const { return ui->csvFilenameLineEdit->text().trimmed(); }
bool ExportPasswordDialog::isJSONChecked() const { return ui->jsonCheckBox->isChecked(); }
QString ExportPasswordDialog::jsonFileName() const { return ui->jsonFilenameLineEdit->text().trimmed(); }
bool ExportPasswordDialog::isXMLChecked() const { return ui->xmlCheckBox->isChecked(); }
QString ExportPasswordDialog::xmlFileName() const { return ui->xmlFilenameLineEdit->text().trimmed(); }
QString ExportPasswordDialog::dir() const { return m_dir; }
