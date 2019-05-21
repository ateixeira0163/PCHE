#include "addnewfluidwindow.h"
#include "ui_addnewfluidwindow.h"
#include "mainwindow.h"
using namespace std;

AddNewFluidWindow::AddNewFluidWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewFluidWindow)
{
    ui->setupUi(this);
}


AddNewFluidWindow::~AddNewFluidWindow()
{
    delete ui;
}

void AddNewFluidWindow::on_buttonBox_accepted()
{
    // To add the new fluid to the .txt
    fluidsLib.open("..//PCHEThermalEfficiency//fluids.txt", fstream::in | fstream::out | fstream::app);
    fluidsLib << ui->newFluidName->text().toStdString() << " " << ui->newFluidP1->value() << " " << ui->newFluidP2->value() << "\n";
    fluidsLib.close();

    emit sendSignal();
}
