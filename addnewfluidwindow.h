#ifndef ADDNEWFLUIDWINDOW_H
#define ADDNEWFLUIDWINDOW_H

#include <QDialog>
#include <QDebug>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>

namespace Ui {
class AddNewFluidWindow;
}

class AddNewFluidWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AddNewFluidWindow(QWidget *parent = nullptr);
    ~AddNewFluidWindow();

public slots:
    void on_buttonBox_accepted();

private:
    Ui::AddNewFluidWindow *ui;
    std::fstream fluidsLib;

signals:
    void sendSignal();
};

#endif // ADDNEWFLUIDWINDOW_H
