#include "databasemanager.h"
#include "passwordformdialog.h"
#include "selectpassworddialog.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ui->tableWidget->setHorizontalHeaderLabels({"Name", "Username", "Password", "Addition date"});
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setColumnWidth(0, 25);
    // ui->tableWidget->setColumnWidth(0, 150);
    // ui->tableWidget->setColumnWidth(1, 150);
    // ui->tableWidget->setColumnWidth(2, 150);
    ui->tableWidget->setColumnWidth(4, 120);

    connect(ui->actionAddPassword, &QAction::triggered, this, &MainWindow::addPassword);
    connect(ui->actionDeletePassword, &QAction::triggered, this, &MainWindow::selectPasswordToDelete);
    connect(ui->actionEditPassword, &QAction::triggered, this, &MainWindow::seletePasswordToEdit);

    if (loadDatabaseConfig("config.json")) {
        m_dbManager = new DatabaseManager(m_host, m_port, m_dbName, m_username, m_password);
        loadPasswordsToTable();
    }
    else {
        QMessageBox::critical(this, "Configuration Error", "Failed to load database configuration");
    }
}

MainWindow::~MainWindow() {
    delete ui;
    delete m_dbManager;
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
        QString pass = row[3].toString();
        QString group = row[4].toString();
        QString date = row[5].toDateTime().toString(QStringLiteral("dd-MM-yyyy HH:mm"));

        PasswordManager *password = new PasswordManager(service, user, pass, date, group, id);
        m_passwordList.append(password);
    }
    updatePasswordTable();
}

void MainWindow::updatePasswordTable() {
    ui->tableWidget->setRowCount(m_passwordList.size());
    for (int i = 0; i < m_passwordList.size(); i++) {
        const PasswordManager *password = m_passwordList[i];
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(password->getId())));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(password->getServiceName()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(password->getUsername()));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(password->getPassword()));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(password->getAdditionalDate()));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(password->getGroup()));

        QWidget *actionWidget = new QWidget();
        QPushButton *editButton = new QPushButton("Edit");
        QPushButton *deleteButton = new QPushButton("Delete");
        editButton->setStyleSheet("padding: 2px;");
        deleteButton->setStyleSheet("padding: 2px;");

        QHBoxLayout *layout = new QHBoxLayout(actionWidget);
        layout->addWidget(editButton);
        layout->addWidget(deleteButton);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(5);
        actionWidget->setLayout(layout);

        ui->tableWidget->setCellWidget(i, 6, actionWidget);

        editButton->setProperty("index", i);
        deleteButton->setProperty("index", i);

        connect(editButton, &QPushButton::clicked, this, [this, editButton]() {
            int index = editButton->property("index").toInt();
            editPassword(index);
        });

        connect(deleteButton, &QPushButton::clicked, this, [this, deleteButton]() {
            int index = deleteButton->property("index").toInt();
            deletePassword(index);
        });
    }
}

void MainWindow::addPassword() {
    PasswordFormDialog dialog(this, PasswordMode::AddMode);
    if (dialog.exec() == QDialog::Accepted) {
        QString serviceName = dialog.getServiceName();
        QString username = dialog.getUsername();
        QString password = dialog.getPassword();
        QString group = dialog.getGroup();
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QString formattedDate = currentDateTime.toString(QStringLiteral("dd-MM-yyyy HH:mm"));

        int newId = -1;
        if (!m_dbManager->addPassword(serviceName, username, password, group, currentDateTime, newId)) {
            qDebug() << "Failed to add a password to the database!";
            return;
        }

        PasswordManager *newPassword = new PasswordManager(serviceName, username, password, formattedDate, group, newId);
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
    PasswordFormDialog dialog(this,
                              password->getServiceName(),
                              password->getUsername(),
                              password->getPassword(),
                              password->getGroup(),
                              PasswordMode::EditMode);

    if (dialog.exec() == QDialog::Accepted) {
        password->setServiceName(dialog.getServiceName());
        password->setUsername(dialog.getUsername());
        password->setPassword(dialog.getPassword());
        password->setGroup(dialog.getGroup());
        // qDebug() << password->getGroup() << " " << password->getPassword() << " " << password->getId();

        if (m_dbManager->editPassword(password)) {
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
