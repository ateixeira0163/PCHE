#include "addcorrelation.h"
#include "ui_addcorrelation.h"

AddCorrelation::AddCorrelation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddCorrelation)
{
    ui->setupUi(this);

    ui->fluidBox->addItem("");          // Add empty options as defaut
    ui->fluidBox->setCurrentIndex(0);
    ui->sectionBox->addItem("");
    ui->sectionBox->setCurrentIndex(0);
    ui->borderBox->addItem("");
    ui->borderBox->setCurrentIndex(0);
    // To add the existent options

    QFile file("..//PCHEThermalEfficiency//correlations.csv");
    if (!file.open(QFile::ReadOnly | QIODevice::Text)){
        qDebug() << file.errorString();
    }

    QStringList wordList;
    while (!file.atEnd()){
        QString line = file.readLine();
        wordList = line.split(';');
        if (wordList.size() > 1){
            if (ui->fluidBox->findText(wordList[6]) == -1 && wordList[6] != ""){ // not found yet
                ui->fluidBox->addItem(wordList[6]);
            }
            if (ui->sectionBox->findText(wordList[7]) == -1 && wordList[7] != ""){
                ui->sectionBox->addItem(wordList[7]);
            }
            if (ui->borderBox->findText(wordList[9]) == -1 && wordList[9] != ""){
                ui->borderBox->addItem(wordList[9]);
            }
        }
    }
    file.close();

    // Add a way to add multiple fluids while input a correlation

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

    out << ui->exprBox->text() << ";"
        << ui->authorBox->text() << ";"
        << ui->reMinBox->value() << ";" << ui->reMaxBox->value() << ";"
        << ui->prMinBox->value() << ";" << ui->prMaxBox->value() << ";"
        << ui->fluidBox->currentText() << ";"
        << ui->sectionBox->currentText() << ";"
        << ui->angleBox->value() << ";"
        << ui->borderBox->currentText() << ";"
        << ui->referenceBox->toPlainText() << ";"
        << ui->notesBox->toPlainText() << ";\n";
    file.close();
    emit sendNewSignal();

    QMessageBox::information(
                this,
                tr("New correlation"),
                tr("The new correlation has been added"));

}
