#ifndef ADDCORRELATION_H
#define ADDCORRELATION_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QComboBox>

namespace Ui {
class AddCorrelation;
}

class AddCorrelation : public QDialog
{
    Q_OBJECT

    struct newOptions{
        QComboBox* newBox;
        QPushButton* newButton;
    };

public:
    explicit AddCorrelation(QWidget *parent = nullptr);
    ~AddCorrelation();

private slots:
    void on_buttonBox_accepted();
    void moreOptions(bool hide);
    void on_plusFluidButton_clicked();
    void deleteFluidNewOptions();

    void on_moreButton_clicked();

private:
    Ui::AddCorrelation *ui;
    bool verifyInputValues();
    QList<QString> comboBoxOptions[4];
    QList<newOptions*> newFluidsBoxes;


signals:
    void sendNewSignal();
};

#endif // ADDCORRELATION_H
