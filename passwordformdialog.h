#ifndef ADDPASSWORDDIALOG_H
#define ADDPASSWORDDIALOG_H

#include "ui_passwordformdialog.h"
#include "models/passwordmode.h"
#include "models/passwordmanager.h"
#include "database/databasemanager.h"
#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QRandomGenerator>

QT_BEGIN_NAMESPACE
namespace Ui {
class PasswordFormDialog;
}
QT_END_NAMESPACE

class PasswordFormDialog : public QDialog {
    Q_OBJECT
public:
    PasswordFormDialog(QWidget *parent = nullptr,
                       PasswordMode mode = PasswordMode::AddMode,
                       DatabaseManager *dbManager = nullptr);
    PasswordFormDialog(QWidget *parent,
                       PasswordManager *password,
                       PasswordMode mode,
                       DatabaseManager *dbManager);
     ~PasswordFormDialog();

    [[nodiscard]] QString serviceName() const;
    [[nodiscard]] QString username() const;
    [[nodiscard]] QString password() const;
    [[nodiscard]] QString group() const;

private slots:
    void onButtonClicked();
    void onGeneratePasswordClicked();
    void onAddGroupButtonClicked();
    void onDeleteGroupButtonClicked();

private:
    Ui::PasswordFormDialog *ui;    
    PasswordMode m_mode;
    DatabaseManager *m_dbManager;
    QStringList m_groupNames;

    void initUI();
    void connectSignals();
    [[nodiscard]] QString generateRandomPassword(int length = 24);
};

#endif // ADDPASSWORDDIALOG_H
