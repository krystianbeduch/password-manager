#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "databasemanager.h"
#include "encryptionutils.h"
#include "passwordmanager.h"
#include "passwordmode.h"

#include <QByteArray>
#include <QDateTime>
#include <QDebug>
#include <QFontMetrics>
#include <QFile>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QVariant>
#include <QVector>
#include <QWidget>

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
    void addPasswordAction();
    void handlePasswordSelection(PasswordMode mode);
    void selectPasswordToEditAction();
    void selectPasswordToDeleteAction();
    void editPassword(int index);
    void deletePassword(int index);

    void exportPasswordsAction();
    void importPasswordsAction();
    void deleteAllPasswordsAction();
    void showPassword(QPushButton *button);

    void moveSelectedRowUpAction();
    void moveSelectedRowDownAction();
    void moveSelectedRowToTopAction();
    void moveSelectedRowToBottomAction();
    void savePositionsAction();

    void showAboutAuthorAction();
    void showAboutTechnologiesAction();
    void checkLoginTimeout();

    void onSortOptionChanged(int index);

private:
    Ui::MainWindow *ui;
    QVector<PasswordManager*> m_passwordList;
    int m_idCounter;
    DatabaseManager *m_dbManager;
    EncryptionUtils *m_crypto;

    QString m_host;
    int m_port;
    QString m_dbName;
    QString m_username;
    QString m_password;
    static constexpr auto DB_CONFIG_FILE_PATH = "database/db_config.json";

    QLabel *m_statusLabel;
    bool m_customSortActive;
    bool m_isLogged;
    QDateTime m_lastLoginTime;
    QTimer *m_authTimer;

    void setupConnections();
    void updatePasswordTable();
    void loadPasswordsToTable();
    [[nodiscard]] bool loadDatabaseConfig(const QString &configFilePath);
    [[nodiscard]] QString generateDotStringForPasswordLineEdit(QLineEdit *lineEdit);
    void resizeEvent(QResizeEvent *e);
    void moveSelectedRowTo(int targetRow);
    void sortPasswordListByColumn(int column, Qt::SortOrder order);
};
#endif // MAINWINDOW_H
