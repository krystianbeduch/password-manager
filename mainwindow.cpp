#include "databasemanager.h"
#include "passwordformdialog.h"
#include "selectpassworddialog.h"
#include "exportpassworddialog.h"
#include "fileservice.h"
#include "cryptodata.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_crypto(new EncryptionUtils)
    , m_statusLabel(new QLabel(this))
{
    ui->setupUi(this);
    ui->statusbar->addPermanentWidget(m_statusLabel);

    // ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    // ui->tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    // ui->tableWidget->setTextElideMode(Qt::ElideNone);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    // ui->tableWidget->setSelectionMode(QAbstractItemView::ContiguousSelection);
    // ui->tableWidget->setTextElideMode(Qt::ElideNone);
    // ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    // ui->tableWidget->setColumnWidth(0, 25);
    // ui->tableWidget->setColumnWidth(0, 150);
    // ui->tableWidget->setColumnWidth(1, 150);
    // ui->tableWidget->setColumnWidth(2, 150);
    // ui->tableWidget->setColumnWidth(4, 100);
    // ui->tableWidget->setColumnWidth(6, 150);

    connect(ui->actionAddPassword, &QAction::triggered, this, &MainWindow::addPassword);
    connect(ui->actionDeletePassword, &QAction::triggered, this, &MainWindow::selectPasswordToDelete);
    connect(ui->actionEditPassword, &QAction::triggered, this, &MainWindow::seletePasswordToEdit);
    connect(ui->actionExportPasswords, &QAction::triggered, this, &MainWindow::selectPasswordToExport);
    connect(ui->actionImportPasswords, &QAction::triggered, this, &MainWindow::importPasswords);
    connect(ui->actionDeleteAllPasswords, &QAction::triggered, this, &MainWindow::deleteAllPasswords);
    connect(ui->actionMovePasswordUp, &QAction::triggered, this, &MainWindow::moveSelectedRowUp);
    connect(ui->actionMovePasswordDown, &QAction::triggered, this, &MainWindow::moveSelectedRowDown);
    connect(ui->actionMovePasswordBeginning, &QAction::triggered, this, &MainWindow::moveSelectedRowToTop);
    connect(ui->actionMovePasswordEnd, &QAction::triggered, this, &MainWindow::moveSelectedRowToDown);
    connect(ui->actionSavePasswordPosition, &QAction::triggered, this, &MainWindow::savePositions);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionAboutAuthor, &QAction::triggered, this, &MainWindow::showAboutAuthor);
    connect(ui->actionAboutTechnologies, &QAction::triggered, this, &MainWindow::showAboutTechnologies);

    if (loadDatabaseConfig("config.json")) {
        m_dbManager = new DatabaseManager(m_host, m_port, m_dbName, m_username, m_password);
        // m_dbManager->insertSamplePasswordsData(m_crypto);
        loadPasswordsToTable();
    }
    else {
        QMessageBox::critical(this, "Configuration Error", "Failed to load database configuration");
    }
}

MainWindow::~MainWindow() {
    delete ui;
    delete m_dbManager;
    delete m_crypto;
}

bool MainWindow::loadDatabaseConfig(const QString &configFilePath) {
    QFile configFile(configFilePath);
    if (!configFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Could not open config file";
        return false;
    }

    QByteArray configData = configFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(configData);
    if (doc.isNull()) {
        qDebug() << "Failed to parse config file";
        return false;
    }

    QJsonObject jsonObject = doc.object();
    if (!jsonObject.contains("database") || !jsonObject["database"].isObject()) {
        qDebug() << "Invalid config file: missing 'database' object";
        return false;
    }

    QJsonObject dbConfig = jsonObject["database"].toObject();
    m_host = dbConfig["host"].toString();
    m_port = dbConfig["port"].toInt();
    m_dbName = dbConfig["db_name"].toString();
    m_username = dbConfig["username"].toString();
    m_password = dbConfig["password"].toString();
    return true;
}

void MainWindow::loadPasswordsToTable() {
    QVector<QVector<QVariant>> records = m_dbManager->fetchAllPasswords();
    for (const QVector<QVariant> &row : records) {
        int id = row[0].toInt();
        QString service = row[1].toString();
        QString user = row[2].toString();
        QString group = row[3].toString();
        QDateTime date = row[4].toDateTime();

        PasswordManager *password = new PasswordManager(service, user, date, group, id);
        m_passwordList.append(password);
    }
    updatePasswordTable();
}

void MainWindow::updatePasswordTable() {
    ui->tableWidget->setRowCount(m_passwordList.size());
    for (int i = 0; i < m_passwordList.size(); i++) {
        const PasswordManager *password = m_passwordList[i];
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(password->getId())));

        QLineEdit *serviceEdit = new QLineEdit(password->getServiceName());
        serviceEdit->setReadOnly(true);
        serviceEdit->setFrame(false);
        serviceEdit->setStyleSheet("background: transparent;");
        serviceEdit->setCursorPosition(0);
        ui->tableWidget->setCellWidget(i, 1, serviceEdit);

        QLineEdit *usernameEdit = new QLineEdit(password->getUsername());
        usernameEdit->setReadOnly(true);
        usernameEdit->setFrame(false);
        usernameEdit->setStyleSheet("background: transparent;");
        usernameEdit->setCursorPosition(0);
        ui->tableWidget->setCellWidget(i, 2, usernameEdit);

        QLineEdit *passwordEdit = new QLineEdit();
        passwordEdit->setEchoMode(QLineEdit::Password);
        passwordEdit->setReadOnly(true);
        passwordEdit->setFrame(false);
        passwordEdit->setStyleSheet("background: transparent;");
        passwordEdit->setText(generateDotStringForPasswordLineEdit(passwordEdit));
        ui->tableWidget->setCellWidget(i, 3, passwordEdit);

        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(password->getFormattedDate()));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(password->getGroup()));

        QWidget *actionWidget = new QWidget();
        QPushButton *editButton = new QPushButton("Edit");
        QPushButton *deleteButton = new QPushButton("Delete");
        QPushButton *showButton = new QPushButton("Show");
        editButton->setStyleSheet("padding: 4px; background-color: #007BFF;");
        deleteButton->setStyleSheet("padding: 4px; background-color: #dc3545;");
        showButton->setStyleSheet("padding: 4px; background-color: #28a745");

        QHBoxLayout *layout = new QHBoxLayout(actionWidget);
        layout->addWidget(showButton);
        layout->addWidget(editButton);
        layout->addWidget(deleteButton);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(5);
        actionWidget->setLayout(layout);

        ui->tableWidget->setCellWidget(i, 6, actionWidget);

        editButton->setProperty("index", i);
        deleteButton->setProperty("index", i);
        showButton->setProperty("index", i);
        showButton->setProperty("isPasswordVisible", false);

        connect(editButton, &QPushButton::clicked, this, [this, editButton]() {
            int index = editButton->property("index").toInt();
            editPassword(index);
        });

        connect(deleteButton, &QPushButton::clicked, this, [this, deleteButton]() {
            int index = deleteButton->property("index").toInt();
            deletePassword(index);
        });

        connect(showButton, &QPushButton::clicked, this, [this, showButton]() {
            showPassword(showButton);
        });
    }
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);    // ID
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);             // Service
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);             // Username
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);             // Password
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);    // Date
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);    // Group
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);    // Buttons

    ui->tableWidget->setDragEnabled(true);
    ui->tableWidget->setAcceptDrops(true);
    ui->tableWidget->setDropIndicatorShown(true);
    ui->tableWidget->setDragDropMode(QAbstractItemView::InternalMove);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    QSize tableSize = ui->tableWidget->sizeHint();
    setMinimumSize(tableSize.width() + 50, tableSize.height() + 100);




}

void MainWindow::addPassword() {
    PasswordFormDialog dialog(this, PasswordMode::AddMode);
    if (dialog.exec() == QDialog::Accepted) {
        QString serviceName = dialog.getServiceName();
        QString username = dialog.getUsername();
        QString password = dialog.getPassword();
        QString group = dialog.getGroup();

        QByteArray salt = m_crypto->generateSaltToEncrypt();
        if (!m_crypto->generateKeyFromPassword("1234", salt)) {
            qWarning() << "Key derivation failed!";
            return;
        }

        QByteArray nonce;
        QByteArray encryptedPassword = m_crypto->encrypt(password.toUtf8(), nonce);
        PasswordManager *newPassword = new PasswordManager(serviceName, username, group);
        if (!m_dbManager->addPassword(newPassword, encryptedPassword, nonce, salt)) {
            qDebug() << "Failed to add a password to the database!";
            return;
        }
        m_passwordList.append(newPassword);
        updatePasswordTable();
        QMessageBox::information(this, "Password added", "The password has been added to the manager");
    }
}


void MainWindow::handlePasswordSelection(PasswordMode mode) {
    if (m_passwordList.isEmpty()) {
        QMessageBox::warning(this, "No Passwords", "There are no passwords in the manager");
        return;
    }

    SelectPasswordDialog dialog(this, m_passwordList, mode);
    if (dialog.exec() == QDialog::Accepted) {
        int index = dialog.getSelectedIndex();
        if (mode == PasswordMode::EditMode) {
            editPassword(index);
        }
        else if(mode == PasswordMode::DeleteMode) {
            deletePassword(index);
        }
    }
}

void MainWindow::seletePasswordToEdit() {
    handlePasswordSelection(PasswordMode::EditMode);
}

void MainWindow::selectPasswordToDelete() {
    handlePasswordSelection(PasswordMode::DeleteMode);
}

void MainWindow::editPassword(int index) {
    if (index < 0 || index >= m_passwordList.size()) {
        return;
    }
    PasswordManager *password = m_passwordList[index];
    PasswordFormDialog dialog(this, password, PasswordMode::EditMode);

    if (dialog.exec() == QDialog::Accepted) {
        password->setServiceName(dialog.getServiceName());
        password->setUsername(dialog.getUsername());
        password->setGroup(dialog.getGroup());

        QByteArray salt = m_crypto->generateSaltToEncrypt();
        if (!m_crypto->generateKeyFromPassword("1234", salt)) {
            qWarning() << "Key derivation failed!";
            return;
        }

        QByteArray nonce;
        QByteArray encryptedPassword = m_crypto->encrypt(dialog.getPassword().toUtf8(), nonce);
        if (m_dbManager->editPassword(password, encryptedPassword, nonce, salt )) {
            updatePasswordTable();
            QMessageBox::information(this, "Updated", "Password updated succesfully");
        }
        else {
            QMessageBox::critical(this, "Erorr", "Failed to update password");
        }
    }
}

void MainWindow::deletePassword(int index) {
    if (index < 0 || index >= m_passwordList.size()) {
        return;
    }
    int id = m_passwordList[index]->getId();
    QString serviceName = m_passwordList[index]->getServiceName();
    QString username = m_passwordList[index]->getUsername();

    QMessageBox::StandardButton confirm = QMessageBox::question(
        this, "Delete",
        QString("Are you sure you want to delete:\n%1 | %2 | %3?")
            .arg(QString::number(id), serviceName, username),
        QMessageBox::Yes | QMessageBox::No
    );

    if (confirm == QMessageBox::Yes) {
        if (m_dbManager->deletePasswordById(id)) {
            m_passwordList.removeAt(index);
            updatePasswordTable();
            QMessageBox::information(this, "Success", "Password deleted successfully");
        }
        else {
            QMessageBox::critical(this, "Error", "Failed to delete password");
        }
    }
}


//////////////////////////////////////////////////////
void MainWindow::selectPasswordToExport() {
    ExportPasswordDialog dialog(this, m_passwordList);
    if (dialog.exec() == QDialog::Accepted) {
        QVector<PasswordManager*> selectedPasswords = dialog.selectPasswords();
        QMap<int, QString> decryptedMap = m_dbManager->fetchPasswordsToExport(selectedPasswords, m_crypto);

        if (dialog.isCSVChcecked()) {
            QString path = dialog.dir() + "/" + dialog.csvFileName() + ".csv";
            FileService::exportToCSV(path, selectedPasswords, decryptedMap);
        }
        if (dialog.isJSONChcecked()) {
            QString path = dialog.dir() + "/" + dialog.jsonFileName() + ".json";
            FileService::exportToJSON(path, selectedPasswords, decryptedMap);
        }
        if (dialog.isXMLChcecked()) {
            QString path = dialog.dir() + "/" + dialog.xmlFileName() + ".xml";
            FileService::exportToXML(path, selectedPasswords, decryptedMap);
        }

        QMessageBox::information(this, "Export succes", "Passwords saved to files");
    }
}

void MainWindow::importPasswords() {
    QString filePath = QFileDialog::getOpenFileName(this, "Import Passwords", "", "CSV (*.csv);;JSON (*.json);;XML (*.xml)");
    if (filePath.isEmpty()) {
        return;
    }

    QVector<PasswordManager*> newPasswords;

    if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        newPasswords = FileService::parseCSV(filePath);
    }
    else if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
        newPasswords = FileService::parseJSON(filePath);
    }
    else if (filePath.endsWith(".xml", Qt::CaseInsensitive)) {
        newPasswords = FileService::parseXML(filePath);
    }
    else {
        QMessageBox::warning(this, "Unsupported", "Unsupported file format");
    }
    if (!newPasswords.isEmpty()) {
        // QMap<PasswordManager*, QPair<QByteArray, QPair<QByteArray, QByteArray>>> cryptoData;
        // struct CryptoData {
        //     QByteArray cipher;
        //     QByteArray salt;
        //     QByteArray nonce;
        // };

        QMap<PasswordManager*, CryptoData> cryptoMap;


        // QMap<int, QByteArray> newEncryptedPasswords;
        // QMap<int, QByteArray> newSalts;
        // QMap<int, QByteArray> newNonces;
        // int index = 0;
        for (PasswordManager *p : newPasswords) {
            QByteArray salt = m_crypto->generateSaltToEncrypt();
            if (!m_crypto->generateKeyFromPassword("1234", salt)) {
                qWarning() << "Key derivation failed!";
                return;
            }
            QByteArray nonce;
            QByteArray encryptedPassword = m_crypto->encrypt(p->getPassword().toUtf8(), nonce);

            CryptoData cryptoData(encryptedPassword, salt, nonce);
            cryptoMap.insert(p, cryptoData);
        }
        if (m_dbManager->importPasswords(cryptoMap)) {
            QMessageBox::information(this, "Success", "Passwords imported successfully.");

        }
    }



}

void MainWindow::deleteAllPasswords() {
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "Backup",
                                                              "Whether to perform an export of passwords before deleting them?",
                                                              QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                                              QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    }

    if (reply == QMessageBox::Yes) {
        ExportPasswordDialog dialog(this, m_passwordList);
        if (dialog.exec() != QDialog::Accepted) {
            QMessageBox::warning(this, "Aborted", "Password removal aborted");
            return;
        }
    }

    reply = QMessageBox::question(this, "WARNIG",
                                        "WARNING!\nAre you sure you want to delete all passwords?",
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    if (m_dbManager->truncatePasswords()) {
        QMessageBox::information(this, "Success", "All password has been deleted");
        m_passwordList.clear();
        updatePasswordTable();
    }
}


void MainWindow::showPassword(QPushButton *button) {
    int index = button->property("index").toInt();
    bool isPasswordVisible = button->property("isPasswordVisible").toBool();
    QLineEdit *passwordEdit = qobject_cast<QLineEdit *>(ui->tableWidget->cellWidget(index, 3));

    if (!isPasswordVisible) {
        QString pass = m_dbManager->decryptPassword(m_passwordList[index]->getId(), m_crypto);
        button->setText("Hide");
        button->setProperty("isPasswordVisible", true);
        button->setStyleSheet("padding: 4px; background-color: #6c757d;");
        passwordEdit->setText(pass);
        passwordEdit->setEchoMode(QLineEdit::Normal);
        passwordEdit->setStyleSheet("background-color: #fff0f0; color: darkred; font-weight: bold;");
        passwordEdit->setCursorPosition(0);
    }
    else {
        passwordEdit->setText(generateDotStringForPasswordLineEdit(passwordEdit));
        passwordEdit->setEchoMode(QLineEdit::Password);
        passwordEdit->setStyleSheet("background: transparent; color: white; font-weight: normal;");
        button->setText("Show");
        button->setProperty("isPasswordVisible", false);
        button->setStyleSheet("padding: 4px; background-color: #28a745; color: white;");
    }
}

QString MainWindow::generateDotStringForPasswordLineEdit(QLineEdit *lineEdit) {
    QFontMetrics metrics(lineEdit->font());
    int charWidth = metrics.horizontalAdvance(QChar(0x2022));
    int totalWidth = lineEdit->width();
    int dotCount = totalWidth / charWidth;
    return QString(dotCount, QChar(0x2022));
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
        QWidget *widget = ui->tableWidget->cellWidget(row, 3);
        if (QLineEdit *passwordEdit = qobject_cast<QLineEdit *>(widget)) {
            QString dots = generateDotStringForPasswordLineEdit(passwordEdit);
            passwordEdit->setText(dots);
        }
    }
}

void MainWindow::moveSelectedRowUp() {
    int row = ui->tableWidget->currentRow();
    moveSelectedRowTo(row - 1);
}

void MainWindow::moveSelectedRowDown() {
    int row = ui->tableWidget->currentRow();
    moveSelectedRowTo(row + 1);
}

void MainWindow::moveSelectedRowToTop() {
    moveSelectedRowTo(0);
}

void MainWindow::moveSelectedRowToDown() {
    moveSelectedRowTo(m_passwordList.size() - 1);
}

void MainWindow::moveSelectedRowTo(int targetRow) {
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow == -1 || targetRow == currentRow || targetRow < 0 || targetRow >= m_passwordList.size()) {
        return;
    }

    PasswordManager* password = m_passwordList.takeAt(currentRow);
    m_passwordList.insert(targetRow, password);
    updatePasswordTable();
    ui->tableWidget->selectRow(targetRow);
    m_statusLabel->setText("Order has been changed - don't forget to save!");
    // QLabel*
    // ui->statusbar->showMessage("Order has been changed - don't forget to save!");
    setWindowModified(true);
}

void MainWindow::savePositions() {
    if (m_dbManager->savePositionsToDatabase(m_passwordList)) {
        m_statusLabel->setText("");
        // ui->statusbar->clearMessage();
        setWindowModified(false);
    }
}

void MainWindow::showAboutAuthor() {
    QMessageBox::information(this, "About Author", "Password Manager\n\n"
                                                   "Created by: Krystian Będuch\n"
                                                   "Contact: beduch_krystian@o2.pl\n"
                                                   "© 2025 All rights reserved.");
}

void MainWindow::showAboutTechnologies() {
    QMessageBox::information(this, "About Technologies",
                             "This application was built using:\n"
                             "- Qt 6.9\n"
                             "- C++20:\n"
                             "      * library: libsodium"
                             "- PostgreSQL 16 (for password storage)\n"
                             "- QTableWidget and Qt Widgets framework");
}
