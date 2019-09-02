#ifndef PCHECONFIG_H
#define PCHECONFIG_H

#include <QDialog>
#include <QDebug>
#include <QFile>
#include <QTextStream>

namespace Ui {
class PCHEConfig;
}

class PCHEConfig : public QDialog
{
    Q_OBJECT

public:
    explicit PCHEConfig(QWidget *parent = nullptr);
    ~PCHEConfig();

private:
    Ui::PCHEConfig *ui;


signals:
    void sendSignal();

private slots:
    void on_buttonBox_accepted();
};

#endif // PCHECONFIG_H
