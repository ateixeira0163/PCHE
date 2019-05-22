#ifndef ADDCORRELATION_H
#define ADDCORRELATION_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>
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

signals:
    void sendNewSignal();
};

#endif // ADDCORRELATION_H
