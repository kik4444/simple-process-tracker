#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class ProcessDialog;
}

class ProcessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessDialog(QWidget *parent = nullptr);
    ~ProcessDialog();

signals:
    void processChosen(QString processName);

private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_listWidget_itemActivated(QListWidgetItem *item);

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::ProcessDialog *ui;

    QStringList processList;
};

#endif // PROCESSDIALOG_H
