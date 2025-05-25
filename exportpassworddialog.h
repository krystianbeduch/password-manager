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

    [[nodiscard]] QVector<PasswordManager*> selectPasswords() const;
    [[nodiscard]] bool isCSVChecked() const;
    [[nodiscard]] QString csvFileName() const;
    [[nodiscard]] bool isJSONChecked() const;
    [[nodiscard]] QString jsonFileName() const;
    [[nodiscard]] bool isXMLChecked() const;
    [[nodiscard]] QString xmlFileName() const;
    [[nodiscard]] bool isValidFilename(QCheckBox *checkBox, QLineEdit *lineEdit, const QString &formatName);
    [[nodiscard]] QString dir() const;




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
