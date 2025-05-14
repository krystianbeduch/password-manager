#include "addpassworddialog.h"
#include "deletepassworddialog.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // ui->tableWidget->setHorizontalHeaderLabels({"Name", "Username", "Password", "Addition date"});
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // ui->tableWidget->setColumnWidth(0, 150);
    // ui->tableWidget->setColumnWidth(1, 150);
    // ui->tableWidget->setColumnWidth(2, 150);
    ui->tableWidget->setColumnWidth(3, 120);

    connect(ui->actionAddPassword, &QAction::triggered, this, &MainWindow::addPassword);
    connect(ui->actionDeletePassword, &QAction::triggered, this, &MainWindow::deletePassword);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::addPassword() {
    // Tworzenie instancji AddPasswordDialog i wywolanie okna dialogowego
    AddPasswordDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString serviceName = dialog.getServiceName();
        QString username = dialog.getUsername();
        QString password = dialog.getPassword();
        QString group = dialog.getGroup();
        QString additionalDate = QDateTime::QDateTime::currentDateTime().toString("dd-MM-yyyy HH:mm");

        PasswordManager *newPassword = new PasswordManager(serviceName, username, password, additionalDate, group, idCounter++);
        int row = ui->tableWidget->rowCount();

        passwordList.append(newPassword);
        updatePasswordTable();
    }
}

void MainWindow::updatePasswordTable() {
    ui->tableWidget->setRowCount(passwordList.size());
    for (int i = 0; i < passwordList.size(); ++i) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(passwordList[i]->getServiceName()));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(passwordList[i]->getUsername()));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(passwordList[i]->getPassword()));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(passwordList[i]->getAdditionalDate()));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(passwordList[i]->getGroup()));

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

        ui->tableWidget->setCellWidget(i, 5, actionWidget);

        deleteButton->setProperty("index", i);

        connect(deleteButton, &QPushButton::clicked, this, [this, deleteButton]() {
            int index = deleteButton->property("index").toInt();
            QString serviceName = passwordList[index]->getServiceName();
            QString username = passwordList[index]->getUsername();
            QMessageBox::StandardButton confirm = QMessageBox::question(
                this, "Delete",
                QString("Are you sure you want to delete:\n%1 (%2)?").arg(serviceName, username),
                QMessageBox::Yes | QMessageBox::No
            );

            if (confirm == QMessageBox::Yes) {
                passwordList.removeAt(index);
                updatePasswordTable();
            }
        });
    }
}

void MainWindow::deletePassword() {
    if (passwordList.isEmpty()) {
        QMessageBox::warning(this, "No Passwords", "There are no passwords in the manager");
    }
    else {
        DeletePasswordDialog dialog(this, passwordList);
        if (dialog.exec() == QDialog::Accepted) {
            int index = dialog.getSelectedIndex();
            if (index >= 0 && passwordList.size()) {
                passwordList.removeAt(index);
                updatePasswordTable();
            }
        }
    }

}
