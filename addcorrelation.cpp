#include "addcorrelation.h"
#include "ui_addcorrelation.h"
using namespace std;

AddCorrelation::AddCorrelation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddCorrelation)
{
    ui->setupUi(this);


    // To add the existent options
    // Test commit
    QFile file("..//PCHEThermalEfficiency//correlations.csv");
    if (!file.open(QFile::ReadOnly | QIODevice::Text)){
        qDebug() << file.errorString();
    }

    QStringList wordList;
    while (!file.atEnd()){
        QString line = file.readLine();
        wordList = line.split(',');
        if (wordList.size() > 1){
            qDebug() << wordList.size();
            if (ui->fluidBox->findText(wordList[6]) == -1 || wordList[6] == " "){ // not found yet
                ui->fluidBox->addItem(wordList[6]);
            }
            if (ui->sectionBox->findText(wordList[7]) == -1 || wordList[7] == " "){
                ui->sectionBox->addItem(wordList[7]);
            }
            if (ui->borderBox->findText(wordList[9]) == -1 || wordList[9] == " "){
                ui->borderBox->addItem(wordList[9]);
            }
        }
    }
    file.close();
}

AddCorrelation::~AddCorrelation()
{
    delete ui;
}


void AddCorrelation::on_buttonBox_accepted()
{
    // To add the new fluid to the .txt

    // Using QFile
    QFile file("..//PCHEThermalEfficiency//correlations.csv");
    if (!file.open(QFile::ReadWrite | QIODevice::Append | QIODevice::Text)) {
        qDebug() << file.errorString();
    }

    QTextStream out(&file);

    out << ui->exprBox->text() << ","
        << ui->authorBox->text() << ","
        << ui->reMinBox->value() << "," << ui->reMaxBox->value() << ","
        << ui->prMinBox->value() << "," << ui->prMaxBox->value() << ","
        << ui->fluidBox->currentText() << ","
        << ui->sectionBox->currentText() << ","
        << ui->angleBox->value() << ","
        << ui->borderBox->currentText() << ",\n";
    file.close();
    emit sendNewSignal();

    // Use QFile???
/*
    correlationLib.open("..//PCHEThermalEfficiency//nusselt.txt", fstream::in | fstream::out | fstream::app);
    if (verifyExpression(ui->exprBox->text())){
        correlationLib << ui->exprBox->text().toStdString() << "; "
                       << ui->authorBox->text().toStdString() << "; "
                       << ui->reMinBox->value() << "; " << ui->reMaxBox->value() << "; "
                       << ui->prMinBox->value() << "; " << ui->prMaxBox->value() << "; "
                       << ui->fluidBox->currentText().toStdString() << "; "
                       << ui->sectionBox->currentText().toStdString() << "; "
                       << ui->angleBox->value() << "; "
                       << ui->borderBox->currentText().toStdString() << "; ";
                       //<< ui->nusseltNotes->toPlainText().toStdString() << "\n" ;
        emit sendNewSignal();
    }
    else {
        QMessageBox::warning(this, tr("Invalid Expression"),tr("The expression could not be computed. \n Please rewrite the notation used."));

    }
    correlationLib.close();
    */

}

bool AddCorrelation::verifyExpression(QString exp)
{
    // To add after some sort of verification
    return true; // if the expression is correct
}
