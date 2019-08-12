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
    void on_moreButton_clicked();
    void on_plusFluidButton_clicked();
    void deleteFluidNewOptions();   
    void on_plusSectionButton_clicked();
    void deleteSectionNewOptions();
    void on_plusChannelButton_clicked();
    void deleteChannelNewOptions();
    void on_plusBorderButton_clicked();
    void deleteBorderNewOptions();

private:
    Ui::AddCorrelation *ui;
    bool verifyInputValues();
    QList<QString> comboBoxOptions[4];
    QList<newOptions*> newFluidsBoxes;
    QList<newOptions*> newSectionBoxes;
    QList<newOptions*> newChannelBoxes;
    QList<newOptions*> newBorderBoxes;

signals:
    void sendNewSignal();
};

#endif // ADDCORRELATION_H
