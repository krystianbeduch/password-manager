#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "databasemanager.h"
#include "passwordmanager.h"
#include "passwordMode.h"
#include "encryptionutils.h"

#include <QMainWindow>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <QMap>
// #include <QPair>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QByteArray>
#include <QFontMetrics>

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

    void selectPasswordToExport();
    void importPasswords();
    void deleteAllPasswords();
    void showPassword(QPushButton *button);

    void moveSelectedRowUp();
    void moveSelectedRowDown();
    void moveSelectedRowToTop();
    void moveSelectedRowToDown();
    void savePositions();

    void showAboutAuthor();
    void showAboutTechnologies();



    // void onRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);

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
    EncryptionUtils *m_crypto;
    QLabel *m_statusLabel;

    void updatePasswordTable();
    void loadPasswordsToTable();
    bool loadDatabaseConfig(const QString &configFilePath);
    QString generateDotStringForPasswordLineEdit(QLineEdit *lineEdit);
    void resizeEvent(QResizeEvent *e);
    void moveSelectedRowTo(int targetRow);





    // void exportToCSV(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted);
    // void exportToJSON(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted);
    // void exportToXML(const QString &path, QVector<PasswordManager*> &selected, const QMap<int, QString> &decrypted);
};
#endif // MAINWINDOW_H
