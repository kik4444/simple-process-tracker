#include "trackentry.h"
#include "ui_trackentry.h"

TrackEntry::TrackEntry(QWidget *parent) : QWidget(parent), ui(new Ui::TrackEntry)
{
    ui->setupUi(this);
}

void TrackEntry::setText(QString buttonText, QString lineText)
{
    ui->pushButton->setText(buttonText);
    ui->lineEdit->setText(lineText);
}

TrackEntry::~TrackEntry()
{
    delete ui;
}
