#include "correlationdetails.h"
#include "ui_correlationdetails.h"

CorrelationDetails::CorrelationDetails(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CorrelationDetails)
{
    ui->setupUi(this);
}

CorrelationDetails::~CorrelationDetails()
{
    delete ui;
}
