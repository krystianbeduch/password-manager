#ifndef EXPORTPASSWORDDIALOG_H
#define EXPORTPASSWORDDIALOG_H

#include "passwordmanager.h"
#include <QDialog>
#include <QVector>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

namespace Ui {
class ExportPasswordDialog;
}

class ExportPasswordDialog : public QDialog {
    Q_OBJECT

public:
    explicit ExportPasswordDialog(QWidget *parent = nullptr,
                                  const QVector<PasswordManager*> &passwords = {});
    ~ExportPasswordDialog();

    QVector<PasswordManager*> selectPasswords() const;
    bool isCSVChcecked() const;
    QString csvFileName() const;
    bool isJSONChcecked() const;
    QString jsonFileName() const;
    bool isXMLChcecked() const;
    QString xmlFileName() const;
    QString dir() const;


private slots:
    void onSelectAllToggled(bool checked);
    void onExportClicked();

private:
    Ui::ExportPasswordDialog *ui;

    QVector<PasswordManager*> m_passwords;
    QVector<PasswordManager*> m_selectedPasswords;
    QString m_dir;
};

#endif // EXPORTPASSWORDDIALOG_H
