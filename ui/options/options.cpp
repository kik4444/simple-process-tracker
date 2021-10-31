#include "options.h"
#include "ui_options.h"

Options::Options(QWidget *parent) : QWidget(parent), ui(new Ui::Options)
{
    ui->setupUi(this);

    ui->runningIntervalSpinBox->setValue(MainWindow::userSettings["runningPollInterval"].toUInt());
    ui->stoppedIntervalSpinBox->setValue(MainWindow::userSettings["stoppedPollInterval"].toUInt());
}

Options::~Options()
{
    delete ui;
}

void Options::on_confirmBox_rejected()
{
    this->~Options();
}

void Options::on_confirmBox_accepted()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "simple-process-tracker", "config");
    settings.setValue("runningPollInterval", ui->runningIntervalSpinBox->value());
    settings.setValue("stoppedPollInterval", ui->stoppedIntervalSpinBox->value());

    MainWindow::userSettings["runningPollInterval"] = ui->runningIntervalSpinBox->value();
    MainWindow::userSettings["stoppedPollInterval"] = ui->stoppedIntervalSpinBox->value();

    this->~Options();
}
