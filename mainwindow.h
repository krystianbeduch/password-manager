#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

#include "databasemanager.h"
#include "passwordmanager.h"
#include "PasswordMode.h"
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addPassword();
    void handlePasswordSelection(PasswordMode mode);
    void seletePasswordToEdit();
    void selectPasswordToDelete();
    void editPassword(int index);
    void deletePassword(int index);

private:
    Ui::MainWindow *ui;
    QVector<PasswordManager*> m_passwordList;
    int m_idCounter = 1;
    DatabaseManager *m_dbManager;
    QString m_host;
    int m_port;
    QString m_dbName;
    QString m_username;
    QString m_password;

    void updatePasswordTable();
    void loadPasswordsToTable();

    bool loadDatabaseConfig(const QString &configFilePath);
};
#endif // MAINWINDOW_H
