#include "database/databasemanager.h"
#include "passwordformdialog.h"
#include "selectpassworddialog.h"
#include "exportpassworddialog.h"
#include "services/fileservice.h"
#include "encryption/cryptodata.h"
#include "logindialog.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_crypto(new EncryptionUtils)
    , m_statusLabel(new QLabel(this))
    , m_authTimer(new QTimer(this))
    , m_idCounter(1)
    , m_isLogged(false)
    , m_customSortActive(true)
{
    LoginDialog dialog;
    if (dialog.exec() != QDialog::Accepted) {
        this->close();
        return;
    }

    if (loadDatabaseConfig("config.json")) {
        m_dbManager = new DatabaseManager(m_host, m_port, m_dbName, m_username, m_password);
    }
    else {
        QMessageBox::critical(this, tr("Configuration Error"), tr("Failed to load database configuration"));
        this->close();
        return;
    }

    if (dialog.isNewProfileRequested()) {
        const QString pass = dialog.password();
        const QByteArray salt = m_crypto->generateSaltToEncrypt();
        if (!(m_crypto->generateKeyFromPassword(pass, salt))) {
            return;
        }
        QByteArray nonce;
        const QByteArray newMainPass = m_crypto->encrypt(pass.toUtf8(), nonce);

        CryptoData cryptoNewMainPassword(newMainPass, salt, nonce);
        if (!(m_dbManager->addMainPassword(cryptoNewMainPassword) &&
              m_crypto->verifyMainPassword(pass, cryptoNewMainPassword) &&
              m_dbManager->truncatePasswords() &&
              m_dbManager->insertSamplePasswordsData(m_crypto))) {

            QMessageBox::critical(this, tr("Error"), tr("Error while creating a new account"));
            return;
        }

        QMessageBox::information(this, tr("New profile created"), tr("New profile has been created\n"
                                                                     "Remember if you lose your password all your data will be lost!"));
    }
    else {
        const CryptoData cryptoMainPassword = m_dbManager->fetchMainPassword();
        if (!(m_crypto->verifyMainPassword(dialog.password(), cryptoMainPassword))) {
            QMessageBox::critical(this, tr("Wrong password"), tr("Incorrect password"));
            this->close();
            return;
        }
    }

    m_lastLoginTime = QDateTime::currentDateTime();
    connect(m_authTimer, &QTimer::timeout, this, &MainWindow::checkLoginTimeout);
    m_authTimer->start(60 * 1000); // 60s

    ui->setupUi(this);
    ui->statusbar->addPermanentWidget(m_statusLabel);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    setupConnections();
    loadPasswordsToTable();
}

MainWindow::~MainWindow() {
    delete m_dbManager;
    delete m_crypto;
    delete ui;
}

void MainWindow::setupConnections() {
    connect(ui->actionAddPassword, &QAction::triggered, this, &MainWindow::addPassword);
    connect(ui->actionDeletePassword, &QAction::triggered, this, &MainWindow::selectPasswordToDelete);
    connect(ui->actionEditPassword, &QAction::triggered, this, &MainWindow::selectPasswordToEdit);
    connect(ui->actionExportPasswords, &QAction::triggered, this, &MainWindow::selectPasswordToExport);
    connect(ui->actionImportPasswords, &QAction::triggered, this, &MainWindow::importPasswords);
    connect(ui->actionDeleteAllPasswords, &QAction::triggered, this, &MainWindow::deleteAllPasswords);
    connect(ui->actionMovePasswordUp, &QAction::triggered, this, &MainWindow::moveSelectedRowUp);
    connect(ui->actionMovePasswordDown, &QAction::triggered, this, &MainWindow::moveSelectedRowDown);
    connect(ui->actionMovePasswordBeginning, &QAction::triggered, this, &MainWindow::moveSelectedRowToTop);
    connect(ui->actionMovePasswordEnd, &QAction::triggered, this, &MainWindow::moveSelectedRowToBottom);
    connect(ui->actionSavePasswordPosition, &QAction::triggered, this, &MainWindow::savePositions);
    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionAboutAuthor, &QAction::triggered, this, &MainWindow::showAboutAuthor);
    connect(ui->actionAboutTechnologies, &QAction::triggered, this, &MainWindow::showAboutTechnologies);
    connect(ui->sortByComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onSortOptionChanged);
}

bool MainWindow::loadDatabaseConfig(const QString &configFilePath) {
    QFile configFile(configFilePath);
    if (!configFile.open(QIODevice::ReadOnly)) {
        qCritical() << tr("Could not open config file");
        return false;
    }

    const QByteArray configData = configFile.readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(configData);
    if (doc.isNull() || !doc.isObject()) {
        qCritical() << tr("Failed to parse config file");
        return false;
    }

    const QJsonObject &jsonObject = doc.object();
    if (!jsonObject.contains("database") || !jsonObject["database"].isObject()) {
        qCritical() << "Invalid config file: missing 'database' object";
        return false;
    }

    const QJsonObject dbConfig = jsonObject["database"].toObject();
    m_host = dbConfig["host"].toString();
    m_port = dbConfig["port"].toInt();
    m_dbName = dbConfig["db_name"].toString();
    m_username = dbConfig["username"].toString();
    m_password = dbConfig["password"].toString();
    return true;
}

void MainWindow::loadPasswordsToTable() {
    const QVector<QVector<QVariant>> records = m_dbManager->fetchAllPasswords();
    m_passwordList.clear();
    m_passwordList.reserve(records.size());

    for (const auto &row : records) {
        m_passwordList.append(new PasswordManager(row[0].toInt(),                           // ID
                                                  row[1].toString(),                        // Service
                                                  row[2].toString(),                        // User
                                                  Group(row[3].toInt(), row[4].toString()), // Group
                                                  row[5].toDateTime(),                      // Date
                                                  row[6].toInt()));                         // Position
    }
    updatePasswordTable();
}

void MainWindow::updatePasswordTable() {
    ui->tableWidget->setRowCount(m_passwordList.size());
    for (int i = 0; i < m_passwordList.size(); i++) {
        const PasswordManager *password = m_passwordList[i];
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(password->id())));

        QLineEdit *serviceEdit = new QLineEdit(password->serviceName());
        serviceEdit->setReadOnly(true);
        serviceEdit->setFrame(false);
        serviceEdit->setStyleSheet("background: transparent;");
        serviceEdit->setCursorPosition(0);
        ui->tableWidget->setCellWidget(i, 1, serviceEdit);

        QLineEdit *usernameEdit = new QLineEdit(password->username());
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

        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(password->group().groupName()));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(password->formattedDate()));

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

        connect(editButton, &QPushButton::clicked, this, [this, index = i]() {
            editPassword(index);
        });

        connect(deleteButton, &QPushButton::clicked, this, [this, index = i]() {
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
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);    // Group
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);    // Date
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);    // Buttons

    ui->tableWidget->setDragEnabled(true);
    ui->tableWidget->setAcceptDrops(true);
    ui->tableWidget->setDropIndicatorShown(true);
    ui->tableWidget->setDragDropMode(QAbstractItemView::InternalMove);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    const QSize tableSize = ui->tableWidget->sizeHint();
    setMinimumSize(tableSize.width() + 50, tableSize.height() + 100);
}

void MainWindow::addPassword() {
    PasswordFormDialog dialog(this, PasswordMode::AddMode, m_dbManager);
    if (dialog.exec() == QDialog::Accepted) {
        const QString serviceName = dialog.serviceName();
        const QString username = dialog.username();
        const Group group = dialog.group();

        const std::optional<CryptoData> cryptoDataOpt = m_crypto->prepareCryptoData(m_crypto->mainPassword(), dialog.password());
        if (!cryptoDataOpt.has_value()) {
            qDebug() << tr("Error during generation of cryptographic data");
            return;
        }

        CryptoData cryptoData = cryptoDataOpt.value();
        PasswordManager *newPassword = new PasswordManager(serviceName, username, group);
        if (!m_dbManager->addPassword(newPassword, cryptoData)) {
            qDebug() << tr("Failed to add a password to the database");
            return;
        }
        m_passwordList.append(newPassword);
        updatePasswordTable();
        QMessageBox::information(this, tr("Password added"), tr("The password has been added to the manager"));
    }
}

void MainWindow::handlePasswordSelection(PasswordMode mode) {
    if (m_passwordList.isEmpty()) {
        QMessageBox::warning(this, tr("No Passwords"), tr("There are no passwords in the manager"));
        return;
    }

    SelectPasswordDialog dialog(this, m_passwordList, mode);
    if (dialog.exec() == QDialog::Accepted) {
        int index = dialog.selectedIndex();
        if (mode == PasswordMode::EditMode) {
            editPassword(index);
        }
        else if(mode == PasswordMode::DeleteMode) {
            deletePassword(index);
        }
    }
}

void MainWindow::selectPasswordToEdit() {
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
    PasswordFormDialog dialog(this, password, PasswordMode::EditMode, m_dbManager);

    if (dialog.exec() == QDialog::Accepted) {
        password->setServiceName(dialog.serviceName());
        password->setUsername(dialog.username());
        password->setGroup(dialog.group());

        const std::optional<CryptoData> cryptoDataOpt = m_crypto->prepareCryptoData(m_crypto->mainPassword(), dialog.password());
        if (!cryptoDataOpt.has_value()) {
            qDebug() << tr("Error during generation of cryptographic data");
            return;
        }

        CryptoData cryptoData = cryptoDataOpt.value();
        if (m_dbManager->editPassword(password, cryptoData)) {
            updatePasswordTable();
            QMessageBox::information(this, tr("Updated"), tr("Password updated succesfully"));
        }
        else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to update password"));
        }
    }
}

void MainWindow::deletePassword(int index) {
    if (index < 0 || index >= m_passwordList.size()) {
        return;
    }
    const int id = m_passwordList[index]->id();
    const QString serviceName = m_passwordList[index]->serviceName();
    const QString username = m_passwordList[index]->username();

    QMessageBox::StandardButton confirm = QMessageBox::question(this, tr("Delete"),
                                                                tr("Are you sure you want to delete:\n%1 | %2 | %3?")
                                                                    .arg(QString::number(id), serviceName, username),
                                                                QMessageBox::Yes | QMessageBox::No
    );

    if (confirm == QMessageBox::Yes) {
        if (m_dbManager->deletePasswordById(id)) {
            m_passwordList.removeAt(index);
            updatePasswordTable();
            QMessageBox::information(this, tr("Success"), tr("Password deleted successfully"));
        }
        else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to delete password"));
        }
    }
}

void MainWindow::selectPasswordToExport() {
    ExportPasswordDialog dialog(this, m_passwordList);
    if (dialog.exec() == QDialog::Accepted) {
        const QVector<PasswordManager*> selectedPasswords = dialog.selectPasswords();
        const QHash<int, QString> decryptedMap = m_dbManager->fetchPasswordsToExport(selectedPasswords, m_crypto);

        if (dialog.isCSVChecked()) {
            FileService::exportToCSV(dialog.dir() + "/" + dialog.csvFileName() + ".csv",
                                     selectedPasswords, decryptedMap);
        }
        if (dialog.isJSONChecked()) {
            FileService::exportToJSON(dialog.dir() + "/" + dialog.jsonFileName() + ".json",
                                      selectedPasswords, decryptedMap);
        }
        if (dialog.isXMLChecked()) {
            FileService::exportToXML(dialog.dir() + "/" + dialog.xmlFileName() + ".xml",
                                     selectedPasswords, decryptedMap);
        }

        QMessageBox::information(this, tr("Export succes"), tr("Passwords saved to files"));
    }
}

void MainWindow::importPasswords() {
    const QString filePath = QFileDialog::getOpenFileName(this, tr("Import Passwords"), "", "CSV (*.csv);;JSON (*.json);;XML (*.xml)");
    if (filePath.isEmpty()) {
        return;
    }

    QVector<PasswordManager*> newPasswords;
    if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        newPasswords = FileService::parseCSV(filePath, m_dbManager);
    }
    else if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
        newPasswords = FileService::parseJSON(filePath, m_dbManager);
    }
    else if (filePath.endsWith(".xml", Qt::CaseInsensitive)) {
        newPasswords = FileService::parseXML(filePath, m_dbManager);
    }
    else {
        QMessageBox::warning(this, tr("Unsupported"), tr("Unsupported file format"));
    }

    if (!newPasswords.isEmpty()) {
        QHash<PasswordManager*, CryptoData> cryptoMap;
        for (const auto &p : std::as_const(newPasswords)) {
            const std::optional<CryptoData> cryptoDataOpt = m_crypto->prepareCryptoData(QString::fromUtf8(m_crypto->mainPassword()), p->password());
            if (!cryptoDataOpt.has_value()) {
                qDebug() << tr("Error during generation of cryptographic data");
                return;
            }
            cryptoMap.insert(p, cryptoDataOpt.value());
        }

        if (!m_dbManager->addPasswordList(cryptoMap)) {
            qDebug() << tr("Failed to import passwords to the database");
            return;
        }
        m_passwordList.append(newPasswords);
        updatePasswordTable();
        QMessageBox::information(this, tr("Import success"), tr("The password has been added to the manager"));
    }
}

void MainWindow::deleteAllPasswords() {
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              tr("Backup"),
                                                              tr("Whether to perform an export of passwords before deleting them?"),
                                                              QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                                              QMessageBox::Yes);
    if (reply == QMessageBox::Cancel) {
        return;
    }

    if (reply == QMessageBox::Yes) {
        ExportPasswordDialog dialog(this, m_passwordList);
        if (dialog.exec() != QDialog::Accepted) {
            QMessageBox::warning(this, tr("Aborted"), tr("Password removal aborted"));
            return;
        }
    }

    reply = QMessageBox::question(this, tr("WARNIG"),
                                        tr("WARNING!\nAre you sure you want to delete all passwords?"),
                                        QMessageBox::Yes | QMessageBox::No,
                                        QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    if (m_dbManager->truncatePasswords()) {
        QMessageBox::information(this, tr("Success"), tr("All password has been deleted"));
        m_passwordList.clear();
        updatePasswordTable();
    }
}

void MainWindow::showPassword(QPushButton *button) {
    int index = button->property("index").toInt();
    bool isPasswordVisible = button->property("isPasswordVisible").toBool();
    QLineEdit *passwordEdit = qobject_cast<QLineEdit *>(ui->tableWidget->cellWidget(index, 3));

    if (!isPasswordVisible) {
        QString pass = m_dbManager->decryptPassword(m_passwordList[index]->id(), m_crypto);
        button->setText(tr("Hide"));
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
        button->setText(tr("Show"));
        button->setProperty("isPasswordVisible", false);
        button->setStyleSheet("padding: 4px; background-color: #28a745; color: white;");
    }
}

QString MainWindow::generateDotStringForPasswordLineEdit(QLineEdit *lineEdit) {
    QFontMetrics metrics(lineEdit->font());
    int dotCount = lineEdit->width() / metrics.horizontalAdvance(QChar(0x2022));
    lineEdit->setCursorPosition(0);
    return QString(dotCount, QChar(0x2022));
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    for (int row = 0; row < ui->tableWidget->rowCount(); row++) {
        QWidget *widget = ui->tableWidget->cellWidget(row, 3);
        if (QLineEdit *passwordEdit = qobject_cast<QLineEdit *>(widget)) {
            QString dots = generateDotStringForPasswordLineEdit(passwordEdit);
            passwordEdit->setText(dots);
            passwordEdit->setCursorPosition(0);
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

void MainWindow::moveSelectedRowToBottom() {
    moveSelectedRowTo(m_passwordList.size() - 1);
}

void MainWindow::moveSelectedRowTo(int targetRow) {
    if (!m_customSortActive) {
        QMessageBox::warning(this, tr("Moving locked"), tr("Item moving locked, change sorting to \"Sort by your own items\""));
        return;
    }

    int currentRow = ui->tableWidget->currentRow();
    if (currentRow == -1 || targetRow == currentRow || targetRow < 0 || targetRow >= m_passwordList.size()) {
        return;
    }

    PasswordManager* password = m_passwordList.takeAt(currentRow);
    m_passwordList.insert(targetRow, password);
    updatePasswordTable();
    ui->tableWidget->selectRow(targetRow);
    m_statusLabel->setText(tr("Order has been changed - don't forget to save!"));
    setWindowModified(true);
}

void MainWindow::savePositions() {
    if (!m_customSortActive) {
        QMessageBox::warning(this, tr("Save positions locked"), tr("Change the sorting to \"Sort by your own items\" to save your own order"));
        return;
    }

    if (!isWindowModified())  {
        QMessageBox::warning(this, tr("Position not changed"), tr("List items in the table have not been changed"));
        return;
    }

    if (m_dbManager->savePositionsToDatabase(m_passwordList)) {
        m_statusLabel->setText("");
        setWindowModified(false);
    }
}

void MainWindow::showAboutAuthor() {
    QMessageBox::information(this, tr("About Author"), tr("Password Manager\n\n"
                                                          "Created by: Krystian Będuch\n"
                                                          "Contact: beduch_krystian@o2.pl\n"
                                                          "© 2025 All rights reserved."));
}

void MainWindow::showAboutTechnologies() {
    QMessageBox::information(this, tr("About Technologies"),
                             tr("This application was built using:\n"
                                "- Qt 6.9\n"
                                "- C++20:\n"
                                "      * library: libsodium\n"
                                "- PostgreSQL 16 (for password storage)\n"
                                "- ChaCha20-Poly1305 (for password encryption)\n"
                                "- QTableWidget and Qt Widgets framework"));
}

void MainWindow::checkLoginTimeout() {
    const int timeoutMinutes = 5;

    if (m_lastLoginTime.msecsTo(QDateTime::currentDateTime()) >= timeoutMinutes * 60 * 1000) {
        m_authTimer->stop();

        LoginDialog dialog;
        if (dialog.exec() == QDialog::Accepted) {
            CryptoData cryptoMainPassword = m_dbManager->fetchMainPassword();
            if (!(m_crypto->verifyMainPassword(dialog.password(), cryptoMainPassword))) {
                QMessageBox::critical(this, "Wrong password", "Incorrect password");
                qApp->quit();
                return;
            }

            m_crypto->setMainPassword(dialog.password().toUtf8());
            m_lastLoginTime = QDateTime::currentDateTime();
            m_authTimer->start(60 * 1000);
        }
        else {
            qApp->quit();
        }
    }
}

void MainWindow::sortPasswordListByColumn(int column, Qt::SortOrder order) {
    if (column > 0) {
        m_customSortActive = false;
        m_statusLabel->setText("");
        setWindowModified(false);
    }
    else {
        m_customSortActive = true;
    }

    auto comparator = [column, order](const PasswordManager* a, const PasswordManager* b) {
        switch (column) {
        case 0: // Position
            return order == Qt::AscendingOrder ? a->position() < b->position() : a->position() > b->position();
        case 1: // ID
            return order == Qt::AscendingOrder ? a->id() < b->id() : a->id() > b->id();
        case 2: // Service
            return order == Qt::AscendingOrder
                       ? a->serviceName().compare(b->serviceName(), Qt::CaseInsensitive) < 0
                       : a->serviceName().compare(b->serviceName(), Qt::CaseInsensitive) > 0;
        case 3: // Username
            return order == Qt::AscendingOrder
                       ? a->username().compare(b->username(), Qt::CaseInsensitive) < 0
                       : a->username().compare(b->username(), Qt::CaseInsensitive) > 0;
        case 4: // Group
            return order == Qt::AscendingOrder
                       ? a->group().groupName().compare(b->group().groupName(), Qt::CaseInsensitive) < 0
                       : a->group().groupName().compare(b->group().groupName(), Qt::CaseInsensitive) > 0;
        case 5: // Date
            return order == Qt::AscendingOrder ? a->additionalDate() < b->additionalDate()
                                               : a->additionalDate() > b->additionalDate();
        default:
            return false;
        }
    };

    std::sort(m_passwordList.begin(), m_passwordList.end(), comparator);
    updatePasswordTable();


}

void MainWindow::onSortOptionChanged(int index) {
    const QVector<QPair<int, Qt::SortOrder>> sortOptions = {
        {0, Qt::AscendingOrder},
        {1, Qt::AscendingOrder}, {1, Qt::DescendingOrder},
        {2, Qt::AscendingOrder}, {2, Qt::DescendingOrder},
        {3, Qt::AscendingOrder}, {3, Qt::DescendingOrder},
        {4, Qt::AscendingOrder}, {4, Qt::DescendingOrder},
        {5, Qt::AscendingOrder}, {5, Qt::DescendingOrder}
    };

    if (index >= 0 && index < sortOptions.size()) {
        const auto& [column, order] = sortOptions[index];
        sortPasswordListByColumn(column, order);
    }
}
