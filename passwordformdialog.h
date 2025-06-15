#ifndef ADDPASSWORDDIALOG_H
#define ADDPASSWORDDIALOG_H

#include "databasemanager.h"
#include "group.h"
#include "passwordmanager.h"
#include "passwordmode.h"
#include "ui_passwordformdialog.h"

#include <QDialog>
#include <QInputDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QWidget>

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
    [[nodiscard]] Group group() const;

private slots:
    void onButtonClicked();
    void onGeneratePasswordClicked();
    void onAddGroupButtonClicked();
    void onDeleteGroupButtonClicked();
    void onEditGroupButtonClicked();

private:
    Ui::PasswordFormDialog *ui;    
    PasswordMode m_mode;
    DatabaseManager *m_dbManager;
    QList<Group> m_groupNames;

    void initUI();
    void connectSignals();
    [[nodiscard]] QString generateRandomPassword(int length = 24);
};

#endif // ADDPASSWORDDIALOG_H
