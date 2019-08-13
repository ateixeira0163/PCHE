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
    ui->hiddenSectionBox->hide();
    ui->hiddenSectionBox_2->hide();
    ui->hiddenSectionButton->hide();
    ui->hiddenSectionButton_2->hide();
    ui->hiddenChannelBox->hide();
    ui->hiddenChannelBox_2->hide();
    ui->hiddenChannelButton->hide();
    ui->hiddenChannelButton_2->hide();
    ui->hiddenBorderBox->hide();
    ui->hiddenBorderBox_2->hide();
    ui->hiddenBorderButton->hide();
    ui->hiddenBorderButton_2->hide();

    // Hide elements to be shown if user click '+ more' button afterwards
    moreOptions(true);
    ui->moreButton->setCheckable(true);
    ui->moreButton->setChecked(false);

    // Set range buttons to be checkable
    ui->reInputTypeButton->setCheckable(true);
    ui->reInputTypeButton->setChecked(false);
    ui->prInputTypeButton->setCheckable(true);
    ui->prInputTypeButton->setChecked(false);
    ui->angleInputTypeButton->setCheckable(true);
    ui->angleInputTypeButton->setChecked(false);
    ui->dInputTypeButton->setCheckable(true);
    ui->dInputTypeButton->setChecked(false);
    ui->lenInputTypeButton->setCheckable(true);
    ui->lenInputTypeButton->setChecked(false);
    ui->tempInputTypeButton->setCheckable(true);
    ui->tempInputTypeButton->setChecked(false);
    ui->viscInputTypeButton->setCheckable(true);
    ui->viscInputTypeButton->setChecked(false);


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
    QList<QStringList> stringLists;
    int opt[] = {4, 5, 7, 9};
    QComboBox* listOptions[] = {ui->fluidBox, ui->sectionBox, ui->channelTypeBox, ui->borderBox};

    while (!file.atEnd()){
        QString line = file.readLine();
        wordList = line.split(';');
        if (wordList.size() > 1){
            // Here we add the existent options in the database
            // if (word is not found in options already and word is not empty line) -> add word to options

            stringLists.clear();    //
            for (int i = 0; i < 4; i++){
                stringLists.push_back(wordList[opt[i]].split('/'));
            }

            for (int i = 0; i < stringLists.size(); i++){
                for (int j = 0; j < stringLists[i].size(); j++){
                    if (listOptions[i]->findText(stringLists[i][j]) == -1 && stringLists[i][j] != "--"){
                        listOptions[i]->addItem(stringLists[i][j]);
                        comboBoxOptions[i].push_back(stringLists[i][j]);
                    }
                }
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

    if(verifyInputValues()){ // Add function that verifies all given inputs
        QTextStream out(&file);

        QString fluidsInput = ui->fluidBox->currentText();
        for (int i = 0; i < newFluidsBoxes.size(); i++){
            if (newFluidsBoxes[i]->newBox->currentText() != "" && newFluidsBoxes[i]->newBox->currentText() != "--"){
                fluidsInput += "/";
                fluidsInput += newFluidsBoxes[i]->newBox->currentText();
            }
        }
        QString sectionInput = ui->sectionBox->currentText();
        for (int i = 0; i < newSectionBoxes.size(); i++){
            if (newSectionBoxes[i]->newBox->currentText() != "" && newSectionBoxes[i]->newBox->currentText() != "--"){
                sectionInput += "/";
                sectionInput += newSectionBoxes[i]->newBox->currentText();
            }
        }
        QString channelInput = ui->channelTypeBox->currentText();
        for (int i = 0; i < newChannelBoxes.size(); i++){
            if (newChannelBoxes[i]->newBox->currentText() != "" && newChannelBoxes[i]->newBox->currentText() != "--"){
                channelInput += "/";
                channelInput += newChannelBoxes[i]->newBox->currentText();
            }
        }
        QString borderInput = ui->borderBox->currentText();
        for (int i = 0; i < newBorderBoxes.size(); i++){
            if (newBorderBoxes[i]->newBox->currentText() != "" && newBorderBoxes[i]->newBox->currentText() != "--"){
                borderInput += "/";
                borderInput += newBorderBoxes[i]->newBox->currentText();
            }
        }

        out << ui->exprBox->text() << ";"
            << ui->authorBox->text() << ";"
            << ui->reMinBox->value() << "/" << ((ui->reInputTypeButton->isChecked() || ui->reMaxBox->value() == 0) ? ui->reMinBox->value() : ui->reMaxBox->value()) << "/" << ui->reVarCheck->checkState() << ";"
            << ui->prMinBox->value() << "/" << ((ui->prInputTypeButton->isChecked() || fabs(ui->prMaxBox->value()) < 1e-10) ? ui->prMinBox->value() : ui->prMaxBox->value()) << "/" << ui->prVarCheck->checkState() << ";"
            << fluidsInput << ";"
            << sectionInput << ";"
            << ui->dMinBox->value() << "/" << ((ui->dInputTypeButton->isChecked() || fabs(ui->dMaxBox->value()) < 1e-10) ? ui->dMinBox->value() : ui->dMaxBox->value()) << "/" << ui->dVarCheck->checkState() << ";"
            << channelInput << ";"
            << ui->angleBox->value() << "/" << ((ui->angleInputTypeButton->isChecked() || fabs(ui->angleBoxMax->value()) < 1e-10) ? ui->angleBox->value() : ui->angleBoxMax->value()) << "/" << ui->angleVarCheck->checkState() << ";"
            << borderInput << ";"
            << ui->lMinBox->value() << "/" << ((ui->lenInputTypeButton->isChecked() || fabs(ui->lMaxBox->value()) < 1e-10) ? ui->lMinBox->value() : ui->lMaxBox->value()) << "/" << ui->lVarCheck->checkState() << ";"
            << ui->viscMinBox->value() << "/" << ((ui->viscInputTypeButton->isChecked() || fabs(ui->viscMaxBox->value()) < 1e-10) ? ui->viscMinBox->value() : ui->viscMaxBox->value()) << "/" << ui->viscVarCheck->checkState() << ";"
            << ui->tempBoxMin->value() << "/" << ((ui->tempInputTypeButton->isChecked() || fabs(ui->tempBoxMax->value()) < 1e-10) ? ui->tempBoxMin->value() : ui->tempBoxMax->value()) << "/" << ui->tempVarCheck->checkState() << ";"
            << ui->referenceBox->toPlainText() << ";"
            << ui->notesBox->toPlainText() << ";\n";
        file.close();
        emit sendNewSignal();

        QMessageBox::information(
                    this,
                    tr("New correlation"),
                    tr("The new correlation has been added"));

    }
    else{
        QMessageBox::information(
                    this,
                    tr("Invalid values"),
                    tr("There's some invalid values"));
    }

}

void AddCorrelation::moreOptions(bool hide)
{
    if (hide){
        ui->sectionBox->hide();
        ui->sectionLabel->hide();
        ui->plusSectionButton->hide();
        ui->diameterLabel->hide();
        ui->dMaxBox->hide();
        ui->dMinBox->hide();
        ui->dVarCheck->hide();
        ui->dInputTypeButton->hide();
        ui->channelLabel->hide();
        ui->channelTypeBox->hide();
        ui->plusChannelButton->hide();
        ui->angleBox->hide();
        ui->angleBoxMax->hide();
        ui->angleLabel->hide();
        ui->angleVarCheck->hide();
        ui->angleInputTypeButton->hide();
        ui->borderBox->hide();
        ui->borderLabel->hide();
        ui->plusBorderButton->hide();
        ui->lengthLabel->hide();
        ui->lMaxBox->hide();
        ui->lMinBox->hide();
        ui->lVarCheck->hide();
        ui->lenInputTypeButton->hide();
        ui->viscMaxBox->hide();
        ui->viscMinBox->hide();
        ui->viscVarCheck->hide();
        ui->viscosityLabel->hide();
        ui->viscInputTypeButton->hide();
        ui->tempBoxMax->hide();
        ui->tempBoxMin->hide();
        ui->tempVarCheck->hide();
        ui->temperatureLabel->hide();
        ui->tempInputTypeButton->hide();
        ui->referenceBox->hide();
        ui->referenceLabel->hide();
        ui->notesBox->hide();
        ui->notesLabel->hide();
        for (int i = 0; i < newFluidsBoxes.size(); i++){
            newFluidsBoxes[i]->newButton->hide();
            newFluidsBoxes[i]->newBox->hide();
        }
        for (int i = 0; i < newSectionBoxes.size(); i++){
            newSectionBoxes[i]->newButton->hide();
            newSectionBoxes[i]->newBox->hide();
        }
        for (int i = 0; i < newChannelBoxes.size(); i++){
            newChannelBoxes[i]->newButton->hide();
            newChannelBoxes[i]->newBox->hide();
        }
        for (int i = 0; i < newBorderBoxes.size(); i++){
            newBorderBoxes[i]->newButton->hide();
            newBorderBoxes[i]->newBox->hide();
        }
    }
    else{
        ui->sectionBox->show();
        ui->sectionLabel->show();
        ui->plusSectionButton->show();
        ui->diameterLabel->show();
        ui->dMaxBox->show();
        ui->dMinBox->show();
        ui->dVarCheck->show();
        ui->dInputTypeButton->show();
        ui->channelLabel->show();
        ui->channelTypeBox->show();
        ui->plusChannelButton->show();
        ui->angleBox->show();
        ui->angleBoxMax->show();
        ui->angleLabel->show();
        ui->angleVarCheck->show();
        ui->angleInputTypeButton->show();
        ui->borderBox->show();
        ui->borderLabel->show();
        ui->plusBorderButton->show();
        ui->lengthLabel->show();
        ui->lMaxBox->show();
        ui->lMinBox->show();
        ui->lVarCheck->show();
        ui->lenInputTypeButton->show();
        ui->viscMaxBox->show();
        ui->viscMinBox->show();
        ui->viscVarCheck->show();
        ui->viscosityLabel->show();
        ui->viscInputTypeButton->show();
        ui->tempBoxMax->show();
        ui->tempBoxMin->show();
        ui->tempVarCheck->show();
        ui->temperatureLabel->show();
        ui->tempInputTypeButton->show();
        ui->referenceBox->show();
        ui->referenceLabel->show();
        ui->notesBox->show();
        ui->notesLabel->show();
        for (int i = 0; i < newFluidsBoxes.size(); i++){
            newFluidsBoxes[i]->newButton->show();
            newFluidsBoxes[i]->newBox->show();
        }
        for (int i = 0; i < newSectionBoxes.size(); i++){
            newSectionBoxes[i]->newButton->show();
            newSectionBoxes[i]->newBox->show();
        }
        for (int i = 0; i < newChannelBoxes.size(); i++){
            newChannelBoxes[i]->newButton->show();
            newChannelBoxes[i]->newBox->show();
        }
        for (int i = 0; i < newBorderBoxes.size(); i++){
            newBorderBoxes[i]->newButton->show();
            newBorderBoxes[i]->newBox->show();
        }
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
    moreFluidsButton->setMaximumWidth(30);

    // Add to Vertical Layout
    ui->fluidVLayout->addWidget(moreFluids);
    ui->fluidVBLayout->addWidget(moreFluidsButton);

    // Connect newly created items
    connect(moreFluidsButton, SIGNAL(clicked()), this, SLOT(deleteFluidNewOptions()));
}

void AddCorrelation::deleteFluidNewOptions()
{
    //! Delete more options to fluids that were added
    // The idea is to search the sender pointer in the list and erase it
    for (int i = 0; i < newFluidsBoxes.size(); i++){
        if (newFluidsBoxes[i]->newButton == sender()){  // 'i' is the index in 'newFluidsBoxes'
            newFluidsBoxes[i]->newButton->hide();
            newFluidsBoxes[i]->newBox->hide();
            newFluidsBoxes.removeAt(i);
            return;
        }
    }
}

void AddCorrelation::on_moreButton_clicked()
{
    moreOptions(!ui->moreButton->isChecked());
}

void AddCorrelation::on_plusSectionButton_clicked()
{
    // Creation of new objects
    QComboBox* moreSection = new QComboBox;
    QPushButton* moreSectionButton = new QPushButton;
    newOptions* newSectionItems = new newOptions;

    // Add then to List to be used later
    newSectionItems->newBox = moreSection;
    newSectionItems->newButton = moreSectionButton;
    newSectionBoxes.push_back(newSectionItems);

    // Add options existing options to it
    moreSection->addItem("--");
    moreSection->setCurrentIndex(0);
    if (comboBoxOptions[1].size() != 0){
        for (int i = 0; i < comboBoxOptions[1].size(); i++){
            moreSection->addItem(comboBoxOptions[1][i]);
        }
    }
    moreSection->setEditable(true);
    moreSectionButton->setText("-");
    moreSectionButton->setMaximumWidth(30);

    // Add to Vertical Layout
    ui->sectionVLayout->addWidget(moreSection);
    ui->sectionVBLayout->addWidget(moreSectionButton);

    // Connect newly created items
    connect(moreSectionButton, SIGNAL(clicked()), this, SLOT(deleteSectionNewOptions()));
}

void AddCorrelation::deleteSectionNewOptions()
{
    //! Delete more options to sections that were added
    // The idea is to search the sender pointer in the list and erase it
    for (int i = 0; i < newSectionBoxes.size(); i++){
        if (newSectionBoxes[i]->newButton == sender()){  // 'i' is the index in 'newSectionBoxes'
            newSectionBoxes[i]->newButton->hide();
            newSectionBoxes[i]->newBox->hide();
            newSectionBoxes.removeAt(i);
            return;
        }
    }
}

void AddCorrelation::on_plusBorderButton_clicked()
{
    // Creation of new objects
    QComboBox* moreBorder = new QComboBox;
    QPushButton* moreBorderButton = new QPushButton;
    newOptions* newBorderItems = new newOptions;

    // Add then to List to be used later
    newBorderItems->newBox = moreBorder;
    newBorderItems->newButton = moreBorderButton;
    newBorderBoxes.push_back(newBorderItems);

    // Add options existing options to it
    moreBorder->addItem("--");
    moreBorder->setCurrentIndex(0);
    if (comboBoxOptions[3].size() != 0){
        for (int i = 0; i < comboBoxOptions[3].size(); i++){
            moreBorder->addItem(comboBoxOptions[3][i]);
        }
    }
    moreBorder->setEditable(true);
    moreBorderButton->setText("-");
    moreBorderButton->setMaximumWidth(30);

    // Add to Vertical Layout
    ui->borderVLayout->addWidget(moreBorder);
    ui->borderVBLayout->addWidget(moreBorderButton);

    // Connect newly created items
    connect(moreBorderButton, SIGNAL(clicked()), this, SLOT(deleteBorderNewOptions()));
}

void AddCorrelation::deleteBorderNewOptions()
{
    //! Delete more options to Borders that were added
    // The idea is to search the sender pointer in the list and erase it
    for (int i = 0; i < newBorderBoxes.size(); i++){
        if (newBorderBoxes[i]->newButton == sender()){  // 'i' is the index in 'newBorderBoxes'
            newBorderBoxes[i]->newButton->hide();
            newBorderBoxes[i]->newBox->hide();
            newBorderBoxes.removeAt(i);
            return;
        }
    }
}

void AddCorrelation::on_plusChannelButton_clicked()
{
    // Creation of new objects
    QComboBox* moreChannel = new QComboBox;
    QPushButton* moreChannelButton = new QPushButton;
    newOptions* newChannelItems = new newOptions;

    // Add then to List to be used later
    newChannelItems->newBox = moreChannel;
    newChannelItems->newButton = moreChannelButton;
    newChannelBoxes.push_back(newChannelItems);

    // Add options existing options to it
    moreChannel->addItem("--");
    moreChannel->setCurrentIndex(0);
    if (comboBoxOptions[2].size() != 0){
        for (int i = 0; i < comboBoxOptions[2].size(); i++){
            moreChannel->addItem(comboBoxOptions[2][i]);
        }
    }
    moreChannel->setEditable(true);
    moreChannelButton->setText("-");
    moreChannelButton->setMaximumWidth(30);

    // Add to Vertical Layout
    ui->channelVLayout->addWidget(moreChannel);
    ui->channelVBLayout->addWidget(moreChannelButton);

    // Connect newly created items
    connect(moreChannelButton, SIGNAL(clicked()), this, SLOT(deleteChannelNewOptions()));
}

void AddCorrelation::deleteChannelNewOptions()
{
    //! Delete more options to Channels that were added
    // The idea is to search the sender pointer in the list and erase it
    for (int i = 0; i < newChannelBoxes.size(); i++){
        if (newChannelBoxes[i]->newButton == sender()){  // 'i' is the index in 'newChannelBoxes'
            newChannelBoxes[i]->newButton->hide();
            newChannelBoxes[i]->newBox->hide();
            newChannelBoxes.removeAt(i);
            return;
        }
    }
}

void AddCorrelation::on_reInputTypeButton_clicked()
{
    if (ui->reInputTypeButton->isChecked()){
        ui->reInputTypeButton->setText("[*,*]");
        ui->reMaxBox->hide();
        ui->reMinBox->setSpecialValueText("");
    }
    else{
        ui->reInputTypeButton->setText("[*]");
        ui->reMaxBox->show();
        ui->reMinBox->setSpecialValueText("min");
    }
}

void AddCorrelation::on_prInputTypeButton_clicked()
{
    if (ui->prInputTypeButton->isChecked()){
        ui->prInputTypeButton->setText("[*,*]");
        ui->prMaxBox->hide();
        ui->prMinBox->setSpecialValueText("");
    }
    else{
        ui->prInputTypeButton->setText("[*]");
        ui->prMaxBox->show();
        ui->prMinBox->setSpecialValueText("min");
    }
}

void AddCorrelation::on_dInputTypeButton_clicked()
{
    if (ui->dInputTypeButton->isChecked()){
        ui->dInputTypeButton->setText("[*,*]");
        ui->dMaxBox->hide();
        ui->dMinBox->setSpecialValueText("");
    }
    else{
        ui->dInputTypeButton->setText("[*]");
        ui->dMaxBox->show();
        ui->dMinBox->setSpecialValueText("min");
    }
}

void AddCorrelation::on_angleInputTypeButton_clicked()
{
    if (ui->angleInputTypeButton->isChecked()){
        ui->angleInputTypeButton->setText("[*,*]");
        ui->angleBoxMax->hide();
        ui->angleBox->setSpecialValueText("");
    }
    else{
        ui->angleInputTypeButton->setText("[*]");
        ui->angleBoxMax->show();
        ui->angleBox->setSpecialValueText("min");
    }
}

void AddCorrelation::on_lenInputTypeButton_clicked()
{
    if (ui->lenInputTypeButton->isChecked()){
        ui->lenInputTypeButton->setText("[*,*]");
        ui->lMaxBox->hide();
        ui->lMinBox->setSpecialValueText("");
    }
    else{
        ui->lenInputTypeButton->setText("[*]");
        ui->lMaxBox->show();
        ui->lMinBox->setSpecialValueText("min");
    }
}

void AddCorrelation::on_viscInputTypeButton_clicked()
{
    if (ui->viscInputTypeButton->isChecked()){
        ui->viscInputTypeButton->setText("[*,*]");
        ui->viscMaxBox->hide();
        ui->viscMinBox->setSpecialValueText("");
    }
    else{
        ui->viscInputTypeButton->setText("[*]");
        ui->viscMaxBox->show();
        ui->viscMinBox->setSpecialValueText("min");
    }
}

void AddCorrelation::on_tempInputTypeButton_clicked()
{
    if (ui->tempInputTypeButton->isChecked()){
        ui->tempInputTypeButton->setText("[*,*]");
        ui->tempBoxMax->hide();
        ui->tempBoxMin->setSpecialValueText("");
    }
    else{
        ui->tempInputTypeButton->setText("[*]");
        ui->tempBoxMax->show();
        ui->tempBoxMin->setSpecialValueText("min");
    }
}
