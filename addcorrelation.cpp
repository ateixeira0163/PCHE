#include "addcorrelation.h"
#include "ui_addcorrelation.h"

AddCorrelation::AddCorrelation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddCorrelation)
{
    ui->setupUi(this);

    // The idea is to hide these elements in order to already have a vbox layout placed beforehand
    ui->hiddenFluidBox->hide();
    ui->hiddenFluidButton->hide();
    ui->hiddenFluidBox_2->hide();
    ui->hiddenFluidButton_2->hide();


    /* Order list for the inputed parameters:
     * [0] - Expression
     * [1] - Author
     * [2] - Re (min,max,chB)
     * [3] - Pr (min,max,chB)
     * [4] - Fluid
     * [5] - Section
     * [6] - Diameter (min,max,chB)
     * [7] - ChannelType
     * [8] - Angle (min,max,ChB)
     * [9] - Border
     * [10] - Lenght (min,max,chB)
     * [11] - Viscosity (min,max,chB)
     * [12] - TempMin (min,max,chB)
     * [13] - Reference
     * [14] - Notes
    */

    ui->moreButton->setCheckable(true);
    ui->moreButton->setChecked(false);

    ui->fluidBox->addItem("--");          // Add empty options as defaut
    ui->fluidBox->setCurrentIndex(0);
    ui->sectionBox->addItem("--");
    ui->sectionBox->setCurrentIndex(0);
    ui->borderBox->addItem("--");
    ui->borderBox->setCurrentIndex(0);
    ui->channelTypeBox->addItem("--");
    ui->channelTypeBox->setCurrentIndex(0);

    // To add the existent options reading from file
    QFile file("..//PCHEThermalEfficiency//correlations.csv");
    if (!file.open(QFile::ReadOnly | QIODevice::Text)){
        qDebug() << file.errorString();
    }

    QStringList wordList;
    int opt[] = {4, 5, 7, 9};
    QComboBox* listOptions[] = {ui->fluidBox, ui->sectionBox, ui->channelTypeBox, ui->borderBox};

    while (!file.atEnd()){
        QString line = file.readLine();
        wordList = line.split(';');
        if (wordList.size() > 1){
            // Here we add the existent options in the database
            // if (word is not found in options already and word is not empty line) -> add word to options

            for (int i = 0; i < 4; i++){
                if (listOptions[i]->findText(wordList[opt[i]]) == -1 && wordList[opt[i]] != "--"){
                    listOptions[i]->addItem(wordList[opt[i]]);
                    comboBoxOptions[i].push_back(wordList[opt[i]]);
                }
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

    if(verifyInputValues()){ // Add function that verifies all given inputs
        QTextStream out(&file);

        out << ui->exprBox->text() << ";"
            << ui->authorBox->text() << ";"
            << ui->reMinBox->value() << "/" << ui->reMaxBox->value() << "/" << ui->reVarCheck->checkState() << ";"
            << ui->prMinBox->value() << "/" << ui->prMaxBox->value() << "/" << ui->prVarCheck->checkState() << ";"
            << ui->fluidBox->currentText() << ";"
            << ui->sectionBox->currentText() << ";"
            << ui->dMinBox->value() << "/" << ui->dMaxBox->value() << "/" << ui->dVarCheck->checkState() << ";"
            << ui->channelTypeBox->currentText() << ";"
            << ui->angleBox->value() << "/" << ui->angleBoxMax->value() << "/" << ui->angleVarCheck->checkState() << ";"
            << ui->borderBox->currentText() << ";"
            << ui->lMinBox->value() << "/" << ui->lMaxBox->value() << "/" << ui->lVarCheck->checkState() << ";"
            << ui->viscMinBox->value() << "/" << ui->viscMaxBox->value() << "/" << ui->viscVarCheck->checkState() << ";"
            << ui->tempBoxMin->value() << "/" << ui->tempBoxMax->value() << "/" << ui->tempVarCheck->checkState() << ";"
            << ui->referenceBox->toPlainText() << ";"
            << ui->notesBox->toPlainText() << ";\n";
        file.close();
        emit sendNewSignal();

        QMessageBox::information(
                    this,
                    tr("New correlation"),
                    tr("The new correlation has been added"));

        /* Code to get new fluids
        for (int i = 0; i < newFluidsBoxes.size(); i++){
            qDebug() << newFluidsBoxes[i]->newBox->currentText();
        } */
    }
    else{
        QMessageBox::information(
                    this,
                    tr("Invalid values"),
                    tr("There's some invalid values"));
    }

}

bool AddCorrelation::verifyInputValues()
{
    //! Function that verifies all given inputs and highlights if input is invalid
    // Return true if all is ok

    return true;
}

void AddCorrelation::on_plusFluidButton_clicked()
{
    // Creation of new objects
    QComboBox* moreFluids = new QComboBox;
    QPushButton* moreFluidsButton = new QPushButton;
    newOptions* newFluidItems = new newOptions;

    // Add then to List to be used later
    newFluidItems->newBox = moreFluids;
    newFluidItems->newButton = moreFluidsButton;
    newFluidsBoxes.push_back(newFluidItems);

    // Add options existing options to it
    moreFluids->addItem("--");
    moreFluids->setCurrentIndex(0);
    if (comboBoxOptions[0].size() != 0){
        for (int i = 0; i < comboBoxOptions[0].size(); i++){
            moreFluids->addItem(comboBoxOptions[0][i]);
        }
    }
    moreFluids->setEditable(true);
    moreFluidsButton->setText("-");
    moreFluidsButton->setMaximumWidth(25);

    // Add to Vertical Layout
    ui->fluidVLayout->addWidget(moreFluids);
    ui->fluidVBLayout->addWidget(moreFluidsButton);

    // Connect newly created items
    connect(moreFluidsButton, SIGNAL(clicked()), this, SLOT(deleteFluidNewOptions()));
}

void AddCorrelation::deleteFluidNewOptions()
{
    for (int i = 0; i < newFluidsBoxes.size(); i++){
        if (newFluidsBoxes[i]->newButton == sender()){  // 'i' is the index in 'newFluidsBoxes'
            newFluidsBoxes[i]->newButton->hide();
            newFluidsBoxes[i]->newBox->hide();
            newFluidsBoxes.removeAt(i);
            return;
        }
    }
}
