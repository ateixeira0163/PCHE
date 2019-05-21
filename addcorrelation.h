#ifndef ADDCORRELATION_H
#define ADDCORRELATION_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <QFile>
#include <QTextStream>

namespace Ui {
class AddCorrelation;
}

class AddCorrelation : public QDialog
{
    Q_OBJECT

public:
    explicit AddCorrelation(QWidget *parent = nullptr);
    ~AddCorrelation();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::AddCorrelation *ui;
    std::fstream correlationLib;

    bool verifyExpression(QString exp);

signals:
    void sendNewSignal();
};

#endif // ADDCORRELATION_H
