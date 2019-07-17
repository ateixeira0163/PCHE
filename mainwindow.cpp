#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
using namespace std;

MainWindow::MainWindow(QWidget *parent) :   // Class MainWindow constructor
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initList();    

    ui->nusseltLabel->setText("0.023 Re <sup>0.8</sup> Pr <sup>0.3</sup>");
    ui->nusseltLabel->adjustSize();

    // Parameters init
    // Hot channel - Inlet
    ui->comboBoxHotP->addItem("Pa");
    ui->comboBoxHotT->addItem("ºC");

    // Hot channel - Outlet
    ui->comboBoxHotPOut->addItem("Pa");

    // Cold channel - Inlet
    ui->comboBoxColdP->addItem("Pa");
    ui->comboBoxColdT->addItem("ºC");

    // Cold channel - Outlet
    ui->comboBoxColdPOutlet->addItem("Pa");

    // Simulation parameters
    ui->comboBoxNu->addItem("User created1");
    ui->comboBoxNu->addItem("Add new");

    // Section init
    ui->comboBoxD->addItem("mm");   // addind cm and mm to the options
    ui->comboBoxD->addItem("cm");
    ui->comboBoxH->addItem("mm");
    ui->comboBoxH->addItem("cm");

    dSection = 0;
    hSection = 0;

    MainWindow::on_rectangleButton_clicked();

    // Correlations - TableView
    loadCorrelations();
    showCorrelations();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    prRangeStatement = true;
    reRangeStatement = true;
    alreadySearched = false;

}

MainWindow::~MainWindow()   // Class MainWindow destructor
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    aboutDialogW = new aboutDialog(this);   // Modaless type (we can move the main window behind)
    aboutDialogW->show();
}

void MainWindow::on_rectangleButton_clicked()
{
    QPixmap rectangleSection(":/rectangleSectionMesures.png");     // Create figure to appear
    rectangleSection = rectangleSection.scaledToWidth(ui -> labelChosenFrame ->width(), Qt::SmoothTransformation);
    ui -> labelChosenFrame -> setPixmap(rectangleSection);  // Set figure on label

    ui -> radioButtonRect -> setChecked(true);              // Change the radioButton check
    ui->labelHSection->show();  // show the 'h' option in case of circular
    ui->comboBoxH->show();
    ui->sectionHMesure->show();

    sectionType = "Rectangle";
}

void MainWindow::on_circularButton_clicked()
{
    QPixmap circularSection(":/circularSectionMesures.png");     // Create figure to appear
    circularSection = circularSection.scaledToWidth(ui -> labelChosenFrame ->width(), Qt::SmoothTransformation);
    ui -> labelChosenFrame -> setPixmap(circularSection);  // Set figure on label

    ui -> radioButtonCirc -> setChecked(true);              // Change the radioButton check
    ui->labelHSection->hide();  // hide the 'h' option in case of circular
    ui->comboBoxH->hide();
    ui->sectionHMesure->hide();

    sectionType = "Circular";
}

void MainWindow::on_semiCircularButton_clicked()
{
    QPixmap semiCircularSection(":/semiCircularSectionMesures.png");     // Create figure to appear
    semiCircularSection = semiCircularSection.scaledToWidth(ui -> labelChosenFrame ->width(), Qt::SmoothTransformation);
    ui -> labelChosenFrame -> setPixmap(semiCircularSection);  // Set figure on label

    ui -> radioButtonSemiCirc -> setChecked(true);              // Change the radioButton check
    ui->labelHSection->hide();  // hide the 'h' option in case of semi-circular
    ui->comboBoxH->hide();
    ui->sectionHMesure->hide();

    sectionType = "Semi-Circular";
}

void MainWindow::on_radioButtonRect_clicked()       // Just to assure that one runs the function to change de label
{
    MainWindow::on_rectangleButton_clicked();
}

void MainWindow::on_radioButtonCirc_clicked()
{
    MainWindow::on_circularButton_clicked();
}

void MainWindow::on_radioButtonSemiCirc_clicked()
{
    MainWindow::on_semiCircularButton_clicked();
}

void MainWindow::on_runButton_clicked()
{
    // Get all the parameters

    // Hot inlet
    hotFluid = ui->comboBoxHotFluid->currentText();
    pHotIn = ui->pHotInlet->value();
    tHotIn = ui->tHotInlet->value();
    reHotIn = ui->reHotInlet->value();

    // Cold inlet
    coldFluid = ui->comboBoxColdFluid->currentText();
    pColdIn = ui->pColdInlet->value();
    tColdIn = ui->tColdInlet->value();
    reColdIn = ui->reColdInlet->value();


    // Section parameters

    dSection = ui -> sectionDMesure -> value();
    if (ui->comboBoxD->currentText() == "cm")
    {
        dSection = dSection*10;
    }

    hSection = ui -> sectionHMesure -> value();
    if (ui->comboBoxH->currentText() == "cm")
    {
        hSection = hSection*10;
    }

    qDebug() << hotFluid << pHotIn << tHotIn << reHotIn;
    qDebug() << coldFluid << pColdIn << tColdIn << reColdIn;
    qDebug() << sectionType << dSection << hSection;

}

void MainWindow::on_comboBoxHotFluid_activated(const QString &arg1)
{
    whichComboBox = "Hot Fluid";
    if (arg1 == "Add new"){
        AddNewFluidWindow addNewFluidWindow;
        connect(&addNewFluidWindow, &AddNewFluidWindow::sendSignal, this, &MainWindow::addNewFluid);
        addNewFluidWindow.exec();
        qDebug() << "creating new fluid";
    }
}

void MainWindow::on_comboBoxColdFluid_activated(const QString &arg1)
{
    whichComboBox = "Cold Fluid";
    if (arg1 == "Add new"){
        AddNewFluidWindow addNewFluidWindow;
        connect(&addNewFluidWindow, &AddNewFluidWindow::sendSignal, this, &MainWindow::addNewFluid);
        addNewFluidWindow.exec();
        qDebug() << "creating new fluid";
    }
}

void MainWindow::initList()
{
    fluidsLib.open(":/fluids.txt");
    string fluidName;
    double a, b;
    while(fluidsLib >> fluidName >> a >> b){
        fluidsName.push_back(fluidName.c_str());    // Adding the information to the vector to be used in the
        fluidsParam.push_back({a,b});               // calculations after
        ui->comboBoxHotFluid->addItem(fluidName.c_str());   // Add to the comboBox object
        ui->comboBoxColdFluid->addItem(fluidName.c_str());
    }
    fluidsLib.close();

    ui->comboBoxHotFluid->addItem("Add new");
    ui->comboBoxColdFluid->addItem("Add new");


    /*
      // For the Nusselt List

    nusseltNb.open("..//PCHEThermalEfficiency//nusselt.txt");
    string nuLines, token;
    size_t pos;
    while (getline(nusseltNb,nuLines)){
        // use nusseltParam to index elements
        int i = 0;
        while ((pos = nuLines.find("; ")) != string::npos) {
            token = nuLines.substr(0, pos);
            if (i == 0){
                nusseltName.push_back(token.c_str());
                ui->comboBoxNu->addItem(token.c_str());
            }
            else {
                nusseltExp.push_back(token.c_str());
            }
            nuLines.erase(0, pos + 2);
            i++;
        }
        nusseltNotes.push_back(nuLines.c_str());
    }
    nusseltNb.close();

    ui->comboBoxNu->addItem("Add new");

    ui->nusseltLabel->setText(nusseltExp[0].c_str());
    ui->nusseltLabel->adjustSize();*/
}

void MainWindow::addNewFluid()
{
    // This method is to add a new fluid to the comboBox object

    // First we remove the "Add new"
    ui->comboBoxHotFluid->removeItem(fluidsName.size());
    ui->comboBoxColdFluid->removeItem(fluidsName.size());

    // Then we open the file
    fluidsLib.open(":/fluids.txt");
    string fluidName;
    double a, b;
    while(fluidsLib >> fluidName >> a >> b){
    }
    fluidsLib.close();

    // Add to the vectors
    fluidsName.push_back(fluidName.c_str());
    fluidsParam.push_back({a,b});

    // Add to the comboBox object
    ui->comboBoxHotFluid->addItem(fluidName.c_str()); // fluidName will be the most recent
    ui->comboBoxColdFluid->addItem(fluidName.c_str());

    // Set to current
    if (whichComboBox == "Hot Fluid") ui->comboBoxHotFluid->setCurrentIndex(fluidsName.size()-1);
    else if (whichComboBox == "Cold Fluid")  ui->comboBoxColdFluid->setCurrentIndex(fluidsName.size()-1);

    ui->comboBoxHotFluid->addItem("Add new");
    ui->comboBoxColdFluid->addItem("Add new");

    qDebug() << "Fluid List refreshed.";

}

void MainWindow::addNewNusselt()
{
    qDebug() << "Nothing for now";
}

void MainWindow::loadCorrelations()
{
    ui->fluidBox->addItem("");          // Add empty options as defaut
    ui->fluidBox->setCurrentIndex(0);
    ui->sectionBox->addItem("");
    ui->sectionBox->setCurrentIndex(0);
    ui->borderBox->addItem("");
    ui->borderBox->setCurrentIndex(0);

    QFile file("..//PCHEThermalEfficiency//correlations.csv");  // Declare file
    if (!file.open(QFile::ReadOnly | QIODevice::Text)){         // Check if open was succesful
        qDebug() << file.errorString();                         // if not = return string of error
    }

    QStringList itemList;                       // List of string to store the information for each line
    while (!file.atEnd()){                      // Until the end
        QString line = file.readLine();         // Read each line
        itemList = line.split(';');             // define separator
        if (itemList.size() > 1){               // Certify that is not a empty line
            Correlation temp(itemList[0],itemList[1],           // The object is created
            {itemList[2].toInt(),itemList[3].toInt()},
            {itemList[4].toDouble(),itemList[5].toDouble()},
            itemList[6], itemList[7], itemList[8].toDouble(), itemList[9],
            itemList[10], itemList[11]);
            corList.push_back(temp);            // We add to the list

            // Add the already existing to the options
            if (ui->fluidBox->findText(itemList[6]) == -1 && itemList[6] != ""){ // not found yet
                ui->fluidBox->addItem(itemList[6]);
            }
            if (ui->sectionBox->findText(itemList[7]) == -1 && itemList[7] != ""){
                ui->sectionBox->addItem(itemList[7]);
            }
            if (ui->borderBox->findText(itemList[9]) == -1 && itemList[9] != ""){
                ui->borderBox->addItem(itemList[9]);
            }
        }
    }

    file.close(); // Close file

}

void MainWindow::on_addNewButton_clicked()
{
    AddCorrelation addCorrelationWindow;
    connect(&addCorrelationWindow, &AddCorrelation::sendNewSignal, this, &MainWindow::addCorrelations);
    addCorrelationWindow.exec();
}

void MainWindow::addCorrelations()
{
    QFile file("..//PCHEThermalEfficiency//correlations.csv");  // Declare file
    if (!file.open(QFile::ReadOnly | QIODevice::Text)){         // Check if open was succesful
        qDebug() << file.errorString();                         // if not = return string of error
    }

    QStringList itemList;                       // List of string to store the information for each line
    QString line;
    while (!file.atEnd()){                      // Until the end
        line = file.readLine();         // Read each line
    }

    itemList = line.split(';');             // define separator
    if (itemList.size() > 1){               // Certify that is not a empty line
        Correlation temp(itemList[0],itemList[1],           // The object is created
        {itemList[2].toInt(),itemList[3].toInt()},
        {itemList[4].toDouble(),itemList[5].toDouble()},
        itemList[6], itemList[7], itemList[8].toDouble(), itemList[9],
        itemList[10], itemList[11]);
        corList.push_back(temp);            // We add to the list

        // Add the already existing to the options
        if (ui->fluidBox->findText(itemList[6]) == -1 && itemList[6] != ""){ // not found yet
            ui->fluidBox->addItem(itemList[6]);
        }
        if (ui->sectionBox->findText(itemList[7]) == -1 && itemList[7] != ""){
            ui->sectionBox->addItem(itemList[7]);
        }
        if (ui->borderBox->findText(itemList[9]) == -1 && itemList[9] != ""){
            ui->borderBox->addItem(itemList[9]);
        }
    }


    file.close(); // Close file
    if (alreadySearched) on_searchButton_clicked();
    else showCorrelations();
}

void MainWindow::showCorrelations()
{
    // Function to show all the existant correlations without any comparison

    auto modelTable = new QStandardItemModel();
    for (int i = 0; i < corList.size(); i++){
        // Add author
        modelTable->appendRow(new QStandardItem(corList[i].getAuthor()));

        // Add Re Range
        QString printNuRange = ((corList[i].getNuRange()[0] == NULL && corList[i].getNuRange()[1] == NULL) ?
                    "Not specified": QString("[%0,%1]").arg(QString::number(corList[i].getNuRange()[0])).arg(QString::number(corList[i].getNuRange()[1])));
        QStandardItem *itemNuRange = new QStandardItem(printNuRange);
        modelTable->setItem(i,1,itemNuRange);

        // Add Pr Range
        QString printPrRange = ((corList[i].getPrRange()[0] == NULL && corList[i].getPrRange()[1] == NULL) ?
                    "Not specified": QString("[%0,%1]").arg(QString::number(corList[i].getPrRange()[0])).arg(QString::number(corList[i].getPrRange()[1])));
        QStandardItem *itemPrRange = new QStandardItem(printPrRange);
        modelTable->setItem(i,2,itemPrRange);

        // Add fluid
        QString printFluid = (corList[i].getFluid() == nullptr ?
                    "Not specified": corList[i].getFluid());
        QStandardItem *itemFluid = new QStandardItem(printFluid);
        modelTable->setItem(i,3,itemFluid);

        // Add section type
        QString printSection = (corList[i].getSection() == nullptr ?
                    "Not specified": corList[i].getSection());
        QStandardItem *itemSection = new QStandardItem(printSection);
        modelTable->setItem(i,4,itemSection);

        // Add angle
        QString printAngle = (corList[i].getAngle() == NULL ?
                    "Not specified": QString("%0°").arg(QString::number(corList[i].getAngle())));
        QStandardItem *itemAngle = new QStandardItem(printAngle);
        modelTable->setItem(i,5,itemAngle);

        QString printBorder = (corList[i].getBorder() == nullptr ?
                    "Not specified": corList[i].getBorder());
        QStandardItem *itemBorder = new QStandardItem(printBorder);
        modelTable->setItem(i,6,itemBorder);
    }

    // Set all horizontal header
    modelTable->setHeaderData(0,Qt::Horizontal,"Author");
    modelTable->setHeaderData(1,Qt::Horizontal,"Re - Range");
    modelTable->setHeaderData(2,Qt::Horizontal,"Pr - Range");
    modelTable->setHeaderData(3,Qt::Horizontal,"Fluid");
    modelTable->setHeaderData(4,Qt::Horizontal,"Section");
    modelTable->setHeaderData(5,Qt::Horizontal,"Angle");
    modelTable->setHeaderData(6,Qt::Horizontal,"Border");
    ui->tableView->setModel(modelTable);    // Set model for tableView
    ui->tableView->horizontalHeader()->show(); // show horizontal header

}

double MainWindow::mean(int begin, int end, QVector<double> vector)
{
    double mean = 0.00;
    for (int i = begin; i < end; i++){
        mean += vector[i];
    }
    mean /= (end - begin);
    return mean;
}

void MainWindow::calculateResults()
{
    // Function that calculates the parameters and stores them

    int indTi = importedData[0].indexOf(ui->tiBox->value());
    int indTf = importedData[0].indexOf(ui->tfBox->value());
    resultsMatrix.clear();

    // Means
    QVector<double> meansVector;
    /*
    double meanTwout1 = mean(indTi, indTf, importedData[1]); // T_water_out1
    double meanTwout2 = mean(indTi, indTf, importedData[2]); // T_water_out2
    double meanTwout = (meanTwout1 + meanTwout2)/2;         // T_water_out
    double meanTwin1 = mean(indTi, indTf, importedData[3]); // T_water_in1
    double meanTwin2 = mean(indTi, indTf, importedData[4]); // T_water_in2
    double meanTwin = (meanTwin1 + meanTwin2)/2;
    double meanTain = mean(indTi, indTf, importedData[5]);
    double meanPain = mean(indTi, indTf, importedData[6]);
    double meanQair = mean(indTi, indTf, importedData[7]);
    double meanPaout = mean(indTi, indTf, importedData[8]);
    double meanTamb = mean(indTi, indTf, importedData[9]); //T_ambient
    double meanTaout = mean(indTi, indTf, importedData[10]);
    double meanPwin = mean(indTi, indTf, importedData[11]);
    double meanPwout = mean(indTi, indTf, importedData[12]);
    */

    // LMTD
    QVector<double> logDiffT;
    double tOutA, tInA, tOutW, tInW;
    for (int i = indTi; i < indTf; i++){
        tOutW = (importedData[1][i] + importedData[2][i])/2;
        tInW = (importedData[3][i] + importedData[4][i])/2;
        tOutA = importedData[10][i];
        tInA = importedData[5][i];

        logDiffT.push_back((tOutA - tInW - tInA + tOutW)/log((tOutA - tInW)/(tInA - tOutW)));
    }
    meansVector.push_back(mean(0,logDiffT.size(),logDiffT)); // [0]
    resultsMatrix.push_back(logDiffT); // [0]

    // Perda de carga
    QVector<double> pressureDrop;
    for (int i = indTi; i < indTf; i++){
        pressureDrop.push_back(importedData[6][i] - importedData[8][i]);
    }
    meansVector.push_back(mean(0,pressureDrop.size(),pressureDrop)); // [1]
    resultsMatrix.push_back(pressureDrop);

    // Add options to plot results after
    QVector<QString> optionsForTable3 = {"LMTD", "Pressure Drop"};
    auto plotModelTable3 = new QStandardItemModel();

    for (int i = 0; i < optionsForTable3.size(); i++){
        QStandardItem *itemCheckBox = new QStandardItem(true);
        itemCheckBox->setCheckable(true);
        itemCheckBox->setCheckState(Qt::Unchecked);
        itemCheckBox->setText(optionsForTable3[i]);
        plotModelTable3->setItem(i,0,itemCheckBox);

        QStandardItem *itemMean = new QStandardItem(QString("%0").arg(QString::number(meansVector[i])));
        plotModelTable3->setItem(i,1,itemMean);

    }

    plotModelTable3->setHeaderData(0,Qt::Horizontal,"Result");
    plotModelTable3->setHeaderData(1,Qt::Horizontal,"Mean");
    ui->plotTable3->setModel(plotModelTable3);
    ui->plotTable3->resizeColumnsToContents();


}

void MainWindow::on_comboBoxNu_activated(const QString &arg1)
{
    if (arg1 == "Add new"){
        AddCorrelation addCorrelationWindow;
        connect(&addCorrelationWindow, &AddCorrelation::sendNewSignal, this, &MainWindow::addNewNusselt);
        addCorrelationWindow.exec();
        qDebug() << "Nusselt window creation opened.";
    }
}

void MainWindow::on_searchButton_clicked()
{
    // Get all the input from the ui
    QVector<int> nuRange = (reRangeStatement ?
                                QVector<int> {ui->reMinBox->value(), ui->reMaxBox->value()} :
                                QVector<int> {ui->reMinBox->value(), ui->reMinBox->value()});
    QVector<double> prRange = (prRangeStatement ?
                                   QVector<double> {ui->prMinBox->value(), ui->prMaxBox->value()} :
                                   QVector<double> {ui->prMinBox->value(), ui->prMinBox->value()});

    QString fluid = ui->fluidBox->currentText();
    QString section = ui->sectionBox->currentText();
    double angle = ui->angleBox->value();
    QString border = ui->borderBox->currentText();

    // Calculate the max score based on nb of answers
    int maxScore = 0;
    if (nuRange != QVector<int> {0,0}) maxScore += 2;
    if (prRange != QVector<double> {0,0}) maxScore += 2;
    if (fluid != "") maxScore += 2;
    if (section != "") maxScore += 2;
    if (angle > 0) maxScore += 2;
    if (border != "") maxScore += 2;

    //rankList: (pos,(score((originalpos,author)))
    // Sorry for this type of approach, but it's to much work to change now
    // Please don't use QPair for more than 2 items (It's stupid, just look down to understand)
    rankList.clear();
    QList<QPair<int, QList<bool>>> resultsList;

    for (int i = 0; i < corList.size(); i++){
        // We compare with all the correlations and add results to list
        resultsList.push_back(corList[i].compare(nuRange,prRange,fluid,section,angle,border));
        int score = resultsList[i].first;
        // Code to input the data in descending order from score
        int pos = 0;
        int j = 0;
        while(j < rankList.size()){
            if (score < rankList[j].first) pos++;
            else break;
            j++;
        }
        // Insert to the list
        rankList.insert(pos, qMakePair(score,qMakePair(i,corList[i].getAuthor())));
    }

    auto modelTable = new QStandardItemModel(); // For TableView
    for (int i = 0; i < rankList.size(); i++){      // We add to the list of options to ListView
        modelTable->appendRow(new QStandardItem(rankList[i].second.second));    // Append first element to TableView

        QModelIndex iIndex = modelTable->index(i,0);    // create index to add icons
        // Add correspondent icon
        if (rankList[i].first == maxScore) modelTable->setData(iIndex,QIcon(":/checkmarkGreen.png"),Qt::DecorationRole);
        else if (rankList[i].first > 0) modelTable->setData(iIndex,QIcon(":/checkmarkYellow.png"),Qt::DecorationRole);
        else modelTable->setData(iIndex,QIcon(":/checkmarkRed.png"),Qt::DecorationRole);

        // Add % of coincidence
        double a = rankList[i].first;
        a = (a/maxScore)*100;
        QStandardItem *itemCoincidence = new QStandardItem(QString::number(a) + "%");
        modelTable->setItem(i,1,itemCoincidence);
        QModelIndex coincidenceIndex = modelTable->index(i,1);
        if (a > 99) modelTable->setData(coincidenceIndex,QIcon(":/checkmarkGreen.png"),Qt::DecorationRole);
        else modelTable->setData(coincidenceIndex,QIcon(":/checkmarkRed.png"),Qt::DecorationRole);

        //(condition ? if_true : if_false) -> print NuRange into TableView
        QString printNuRange = ((corList[rankList[i].second.first].getNuRange()[0] == NULL && corList[rankList[i].second.first].getNuRange()[1] == NULL) ?
                    "Not specified": QString("[%0,%1]").arg(QString::number(corList[rankList[i].second.first].getNuRange()[0])).arg(QString::number(corList[rankList[i].second.first].getNuRange()[1])));
        QStandardItem *itemNuRange = new QStandardItem(printNuRange);
        modelTable->setItem(i,2,itemNuRange);
        QModelIndex nuRangeIndex = modelTable->index(i,2);
        if (resultsList[rankList[i].second.first].second[0]) modelTable->setData(nuRangeIndex,QIcon(":/checkmarkGreen.png"),Qt::DecorationRole);
        else modelTable->setData(nuRangeIndex,QIcon(":/checkmarkRed.png"),Qt::DecorationRole);

        // print valid PrRange
        QString printPrRange = ((corList[rankList[i].second.first].getPrRange()[0] == NULL && corList[rankList[i].second.first].getPrRange()[1] == NULL) ?
                    "Not specified": QString("[%0,%1]").arg(QString::number(corList[rankList[i].second.first].getPrRange()[0])).arg(QString::number(corList[rankList[i].second.first].getPrRange()[1])));
        QStandardItem *itemPrRange = new QStandardItem(printPrRange);
        modelTable->setItem(i,3,itemPrRange);
        QModelIndex prRangeIndex = modelTable->index(i,3);
        if (resultsList[rankList[i].second.first].second[1]) modelTable->setData(prRangeIndex,QIcon(":/checkmarkGreen.png"),Qt::DecorationRole);
        else modelTable->setData(prRangeIndex,QIcon(":/checkmarkRed.png"),Qt::DecorationRole);

        // print valid fluid
        QString printFluid = (corList[rankList[i].second.first].getFluid() == nullptr ?
                    "Not specified": corList[rankList[i].second.first].getFluid());
        QStandardItem *itemFluid = new QStandardItem(printFluid);
        modelTable->setItem(i,4,itemFluid);
        QModelIndex fluidIndex = modelTable->index(i,4);
        if (resultsList[rankList[i].second.first].second[2]) modelTable->setData(fluidIndex,QIcon(":/checkmarkGreen.png"),Qt::DecorationRole);
        else modelTable->setData(fluidIndex,QIcon(":/checkmarkRed.png"),Qt::DecorationRole);

        // print valid section
        QString printSection = (corList[rankList[i].second.first].getSection() == nullptr ?
                    "Not specified": corList[rankList[i].second.first].getSection());
        QStandardItem *itemSection = new QStandardItem(printSection);
        modelTable->setItem(i,5,itemSection);
        QModelIndex sectionIndex = modelTable->index(i,5);
        if (resultsList[rankList[i].second.first].second[3]) modelTable->setData(sectionIndex,QIcon(":/checkmarkGreen.png"),Qt::DecorationRole);
        else modelTable->setData(sectionIndex,QIcon(":/checkmarkRed.png"),Qt::DecorationRole);

        // print valid angle
        QString printAngle = (corList[rankList[i].second.first].getAngle() == NULL ?
                    "Not specified": QString("%0°").arg(QString::number(corList[rankList[i].second.first].getAngle())));
        QStandardItem *itemAngle = new QStandardItem(printAngle);
        modelTable->setItem(i,6,itemAngle);
        QModelIndex angleIndex = modelTable->index(i,6);
        if (resultsList[rankList[i].second.first].second[4]) modelTable->setData(angleIndex,QIcon(":/checkmarkGreen.png"),Qt::DecorationRole);
        else modelTable->setData(angleIndex,QIcon(":/checkmarkRed.png"),Qt::DecorationRole);

        // print valid border
        QString printBorder = (corList[rankList[i].second.first].getBorder() == nullptr ?
                    "Not specified": corList[rankList[i].second.first].getBorder());
        QStandardItem *itemBorder = new QStandardItem(printBorder);
        modelTable->setItem(i,7,itemBorder);
        QModelIndex borderIndex = modelTable->index(i,7);
        if (resultsList[rankList[i].second.first].second[5]) modelTable->setData(borderIndex,QIcon(":/checkmarkGreen.png"),Qt::DecorationRole);
        else modelTable->setData(borderIndex,QIcon(":/checkmarkRed.png"),Qt::DecorationRole);

    }
    modelTable->setHeaderData(0,Qt::Horizontal,"Author");
    modelTable->setHeaderData(1,Qt::Horizontal,"Coincidence");
    modelTable->setHeaderData(2,Qt::Horizontal,"Re - Range");
    modelTable->setHeaderData(3,Qt::Horizontal,"Pr - Range");
    modelTable->setHeaderData(4,Qt::Horizontal,"Fluid");
    modelTable->setHeaderData(5,Qt::Horizontal,"Section");
    modelTable->setHeaderData(6,Qt::Horizontal,"Angle");
    modelTable->setHeaderData(7,Qt::Horizontal,"Border");
    ui->tableView->setModel(modelTable);    // Set model for tableView
    ui->tableView->horizontalHeader()->show();
    ui->tableView->resizeColumnsToContents();

    alreadySearched = true;
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    // Adequate index if list is order by search
    int ind = (alreadySearched ?
                 rankList[index.row()].second.first :
                 index.row());
    // Add informations to boxes
    ui->exprBoxInfo->setText(corList[ind].getExpr());
    ui->referenceBoxInfo->setPlainText(corList[ind].getReference());
    ui->notesBoxInfo->setPlainText(corList[ind].getNotes());

    // Clear graph widget to new plot
    ui->customPlot->clearGraphs();
    ui->customPlot->replot(); // refresh

    QScriptEngine myEngine;

    QString expression = corList[ind].getExpr();
    int reMin = corList[ind].getNuRange()[0];
    int reMax = corList[ind].getNuRange()[1];       // ADD VERIFICATION
    expression.replace(QString("("),QString("Math.pow("));
    expression.replace(QString("^"),QString(","));
    QString funExpr = "(function(Re, Pr) { return " + expression + ";})";
    QScriptValue funScript = myEngine.evaluate(funExpr);

    // generate some data:
    QVector<double> x(101), y(101);
    for (int i=0; i < 101; i++){
        x[i] = reMin + i*(reMax - reMin)/100;
        QScriptValueList args;
        args << x[i] << 1; //PR = 1 for now
        QScriptValue result = funScript.call(QScriptValue(), args);
        y[i] = result.toNumber();
    }

    // Add legend
    //ui->customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    ui->customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->customPlot->legend->setFont(legendFont);
    ui->customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
    // create graph and assign data to it:
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(x, y);
    ui->customPlot->graph(0)->setName(corList[ind].getAuthor());
    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("x");
    ui->customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    ui->customPlot->rescaleAxes();
    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->xAxis2->setTickLabels(false);
    ui->customPlot->yAxis2->setVisible(true);
    ui->customPlot->yAxis2->setTickLabels(false);
    // make left and bottom axes always transfer their ranges to right and top axes:
    connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));
    ui->customPlot->replot();
    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}

void MainWindow::on_plotButton_clicked()
{
    QModelIndex ind = ui->tableView->selectionModel()->currentIndex();
    on_tableView_doubleClicked(ind);
}

void MainWindow::on_prInputButton_clicked()
{
    // Change the input GUI for Prandtl number
    if (prRangeStatement == true){
        ui->prInputButton->setText("Number");
        ui->prMaxBox->hide();
        ui->prMinBox->setMinimumWidth(151);
        ui->prMinBox->setMaximumWidth(151);
        ui->prMinLabel->hide();
        ui->prMaxLabel->hide();
        prRangeStatement = false;
    }
    else{
        ui->prInputButton->setText("Range");
        ui->prMaxBox->show();
        ui->prMinBox->setMinimumWidth(71);
        ui->prMinBox->setMaximumWidth(71);
        ui->prMinLabel->show();
        ui->prMaxLabel->show();
        prRangeStatement = true;
    }
}

void MainWindow::on_reInputButton_clicked()
{
    // Function to hide/show some elements
    // Change the input GUI for Reynolds input
    if (reRangeStatement == true){
        ui->reInputButton->setText("Number");
        ui->reMaxBox->hide();
        ui->reMinBox->setMinimumWidth(151);
        ui->reMinBox->setMaximumWidth(151);
        ui->reMinLabel->hide();
        ui->reMaxLabel->hide();
        reRangeStatement = false;
    }
    else{
        ui->reInputButton->setText("Range");
        ui->reMaxBox->show();
        ui->reMinBox->setMinimumWidth(71);
        ui->reMinBox->setMaximumWidth(71);
        ui->reMinLabel->show();
        ui->reMaxLabel->show();
        reRangeStatement = true;
    }
}

void MainWindow::on_deleteButton_clicked()
{
    // Open message box to be sure of the deletion
    QMessageBox::StandardButton verify;
    verify = QMessageBox::question(this, "Confirmation", "Are you sure?",
                                   QMessageBox::Yes|QMessageBox::No);
    if (verify == QMessageBox::Yes){
        // Get the correct index from corList
        int index = (alreadySearched ?
                     rankList[ui->tableView->selectionModel()->currentIndex().row()].second.first :
                     ui->tableView->selectionModel()->currentIndex().row());

        // Remove item choosen from corList
        corList.remove(index);

        // Write correlations.csv again
        QFile file("..//PCHEThermalEfficiency//correlations.csv");

        // Verify if file was successfuly opened (Truncate overwrites the file)
        if (!file.open(QFile::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
            qDebug() << file.errorString();
        }
        QTextStream out(&file); // Define TextStream for writing

        // Alocate variables to input
        Correlation *k;
        QString expr;
        QString author;
        QVector<int> reRange;
        QVector<double> prRange;
        QString fluid;
        QString section;
        double angle;
        QString border;
        QString reference;
        QString notes;

        // Iterate all correlation list to add again except the excluded one
        for (int i = 0; i < corList.size(); i++){
            k = &corList[i]; // to search just one time
            // Convert Null values to "" or 0 to write into file
            expr = (k->getExpr() == nullptr ? "" : k->getExpr());
            author = (k->getAuthor() == nullptr ? "" : k->getAuthor());
            reRange = (k->getNuRange()[0] == NULL ? QVector<int> {0,0} : k->getNuRange());
            prRange = (k->getPrRange()[0] == NULL ? QVector<double> {0,0} : k->getPrRange());
            fluid = (k->getFluid() == nullptr ? "" : k->getFluid());
            section = (k->getSection() == nullptr ? "" : k->getSection());
            angle = (k->getAngle() == NULL ? 0 : k->getAngle());
            border = (k->getBorder() == nullptr ? "" : k->getBorder());
            reference = (k->getReference() == nullptr ? "" : k->getReference());
            notes = (k->getNotes() == nullptr ? "" : k->getNotes());

            out << expr << ";"
                << author << ";"
                << reRange[0] << ";" << reRange[1] << ";"
                << prRange[0] << ";" << prRange[1] << ";"
                << fluid << ";"
                << section << ";"
                << angle << ";"
                << border << ";"
                << reference << ";"
                << notes << ";" << "\n";
        }
        file.close();
        // If there was already a research, it's refreshed
        if (alreadySearched) on_searchButton_clicked();
        else showCorrelations();
    }
}

void MainWindow::on_importResults_clicked()
{
    // Import data results and show options on QTableView
    // Get file path - Only .csv are accepted
    importedFileName = QFileDialog::getOpenFileName(this, tr("Import Data"),
                                                   QDir::homePath(), "CSV File (*.csv)");
    // To make sure the user choose a file
    if (importedFileName != nullptr){
        // Open file in readOnly mode
        QFile file(importedFileName);
        if (!file.open(QFile::ReadOnly | QIODevice::Text)){
            qDebug() << file.errorString();
        }
        // Lists to store each item separate by ";"
        QStringList headerList;
        QString header = file.readLine(); // To eliminate the header (first line)
        headerList = header.split(';');

        file.close();   // Close file

        // Create options to plot
        auto plotModelTable = new QStandardItemModel();
        for (int i = 0; i < headerList.size() - 2; i++){    //ignore first and second column
            QStandardItem *itemCheckBox = new QStandardItem(true);
            itemCheckBox->setCheckable(true);
            itemCheckBox->setCheckState(Qt::Unchecked);
            itemCheckBox->setText(headerList[i+2]);
            plotModelTable->setItem(i,0,itemCheckBox);
        }
        ui->plotTable->setModel(plotModelTable);
        ui->plotTable->resizeColumnsToContents();
    }
}

void MainWindow::on_plotResults_clicked()
{
    // Plot data based on user choosen data on TableView
    if (importedFileName != nullptr){
        QModelIndex indData;
        QVector<int> choosenData;
        // Get all the choosen data to be plotted
        for (int i = 0; i < ui->plotTable->model()->rowCount(); i++){
            indData = ui->plotTable->model()->index(i,0,QModelIndex());
            if (indData.data(Qt::CheckStateRole) == Qt::Checked){
                choosenData.push_back(i+2); // Correction from not considering the first 2 columns
            }
        }
        if (choosenData.size() > 0){
            // Open file in readOnly mode
            QFile file(importedFileName);
            if (!file.open(QFile::ReadOnly | QIODevice::Text)){
                qDebug() << file.errorString();
            }

            // Create a data matrix and a row to add to data w/ push_back
            QVector<QVector<double>> data;
            QVector<double> row;
            // Lists to store each item separate by ";"
            QStringList itemList, headerList;
            QString header = file.readLine(); // To eliminate the header (first line)
            headerList = header.split(';');
            QString line;

            // Read until the end of file
            while (!file.atEnd()){
                line = file.readLine();     // Get line
                itemList = line.split(';'); // Separate by ";"
                if (itemList.size() > 1){   // To make sure that it's not an empty line
                    row.clear();            // Clear vector
                    row.push_back(itemList[1].toDouble()); // Add second column
                    for (int i = 0; i < choosenData.size(); i++){  // For all elements in a row
                        row.push_back(itemList[choosenData[i]].toDouble());  // Add to vector
                    }
                    data.push_back(row);    // Add that vector to the matrix
                }
            }
            file.close();   // Close file

            // Clear graphs
            ui->resultsPlot->clearGraphs();
            ui->resultsPlot->replot();

            // Get columns
            QVector<QVector<double>> dataSet;
            QVector<double> rowSet;
            for (int i = 0; i < data[0].size(); i++){
                rowSet.clear();
                for (int j = 0; j < data.size(); j++){
                    rowSet.push_back(data[j][i]);
                }
                dataSet.push_back(rowSet);
            }
            data.clear(); // erase from memory

            // Style
            ui->resultsPlot->legend->setVisible(true);
            ui->resultsPlot->legend->setFont(QFont("Helvetica",9));
            ui->resultsPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
            ui->resultsPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

            // Create graphs
            QPen pen;
            for (int i = 0; i < choosenData.size(); i++){
                if (headerList[choosenData[i]].contains("C", Qt::CaseInsensitive)){
                    ui->resultsPlot->addGraph(); // Default axis
                }
                else if (headerList[choosenData[i]].contains("bar", Qt::CaseInsensitive)){
                    ui->resultsPlot->addGraph(ui->resultsPlot->xAxis, ui->resultsPlot->yAxis2);
                }
                else {
                    ui->resultsPlot->addGraph();
                }
                pen.setColor(QColor((choosenData.size()-i)*254/choosenData.size(),(i)*254/choosenData.size(), 100, 255));
                ui->resultsPlot->graph(i)->setPen(pen);
                ui->resultsPlot->graph(i)->setLineStyle(QCPGraph::lsLine);
                ui->resultsPlot->graph(i)->setData(dataSet[0],dataSet[i+1]);
                ui->resultsPlot->graph(i)->setName(headerList[choosenData[i]]);
            }

            // Set Axis labels
            ui->resultsPlot->xAxis->setLabel(headerList[1]);
            ui->resultsPlot->yAxis->setLabel("°C");
            ui->resultsPlot->yAxis2->setLabel("bar");

            // Set Ranges
            ui->resultsPlot->rescaleAxes();
            // Set opposites axis to be visible
            ui->resultsPlot->xAxis2->setVisible(true);
            ui->resultsPlot->xAxis2->setTickLabels(false);
            ui->resultsPlot->yAxis2->setVisible(true);
            ui->resultsPlot->yAxis2->setTickLabels(true);
            connect(ui->resultsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->resultsPlot->xAxis2, SLOT(setRange(QCPRange)));
            //connect(ui->resultsPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->resultsPlot->yAxis2, SLOT(setRange(QCPRange)));
            ui->resultsPlot->replot();
            ui->resultsPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
        }
        else {
            QMessageBox::warning(
                        this,
                        tr("Attention"),
                        tr("Please choose something to plot"));
        }
    }
    else {
        QMessageBox::warning(
                    this,
                    tr("Attention"),
                    tr("You need to import a file first"));
    }
}

void MainWindow::on_importResultsButton_clicked()
{
    // Import data results and show options on QTableView
    // Get file path - Only .csv are accepted
    importedFileNameData = QFileDialog::getOpenFileName(this, tr("Import Data"),
                                                   QDir::homePath(), "CSV File (*.csv)");

    // To make sure the user choose a file
    if (importedFileNameData != nullptr){
        // Open file in readOnly mode
        QFile file(importedFileNameData);
        if (!file.open(QFile::ReadOnly | QIODevice::Text)){
            qDebug() << file.errorString();
        }

        QStringList headerList;
        QString header;
        // To ignore all the lines up do 21.
        // Save .csv in .csv - UTF-8 to work
        for (int i = 0; i < 21; i++){
            header = file.readLine();
        }
        headerList = header.split(';');

        // --- Add data to variable importedData to be used later
        importedData.clear(); // Clear data if there's any

        QString line;
        QStringList itemList;
        QVector<double> row;
        QVector<QVector<double>> data;
        // Read until the end of the file
        while (!file.atEnd()){
            line = file.readLine();
            itemList = line.split(';');
            if (itemList.size() > 1){
                row.clear();
                for (int i = 0; i < 25; i+=2){ // Until now we have 13 columns**
                    // Reads for i = {0, 2, 4, ..., 24}
                    row.push_back(itemList[i].toDouble());
                }
                data.push_back(row);
            }
        }
        // Now we have the data in columns. To easy access change to rows
        QVector<double> rowSet;
        for (int i = 0; i < data[0].size(); i++){
            rowSet.clear();
            for (int j = 0; j < data.size(); j++){
                rowSet.push_back(data[j][i]);
            }
            importedData.push_back(rowSet);
        }

        for (int i = 0; i < data.size(); i++) {
            importedData[0][i] *= ui->timeIntervalBox->value();
            importedData[6][i] = 1000*importedData[6][i]*0.75 - 3 + 1.01325; // P_in (air) conversion from mA -> bar
            importedData[7][i] = 1000*importedData[7][i]*1.05 - 4.2; // Q (air) conversion mA -> kg/m^3
            importedData[8][i] = 1000*importedData[8][i]*0.75 - 3 + 1.01325; // P_out (air)
            importedData[11][i] = 1000*importedData[11][i]*0.25 - 0.1 + 1.01325; // P_in (water)
            importedData[12][i] = 1000*importedData[12][i]*0.25 - 0.1 + 1.01325; // P_out (water)
        }

        data.clear();

        // --- Data have been imported to importedData.
        file.close();

        // Create options to plot
        auto plotModelTable = new QStandardItemModel();
        for (int i = 2; i < headerList.size(); i+=2){   // Take only the useful information
            QStandardItem *itemCheckBox = new QStandardItem(true);
            itemCheckBox->setCheckable(true);
            itemCheckBox->setCheckState(Qt::Unchecked);
            itemCheckBox->setText(headerList[i]);
            plotModelTable->setItem(i/2-1,0,itemCheckBox);
        }
        ui->plotTable2->setModel(plotModelTable);
        ui->plotTable2->resizeColumnsToContents();

        // Calculate results:
        calculateResults();
    }
}

void MainWindow::on_plotResultsButton_clicked()
{
    // Plot data based on user choosen data on TableView
    // Verify if there's a selected file
    if (importedFileNameData != nullptr){
        QModelIndex indData;
        QVector<int> choosenData;
        // Get all the choosen data to be plotted
        for (int i = 0; i < ui->plotTable2->model()->rowCount(); i++){
            indData = ui->plotTable2->model()->index(i,0,QModelIndex());
            if (indData.data(Qt::CheckStateRole) == Qt::Checked){
                //choosenData.push_back(2*i+2); // Correction from not considering the first 2 columns
                choosenData.push_back(i+1);
            }
        }
        if (choosenData.size() > 0){

            // Open file in readOnly mode
            QFile file(importedFileNameData);
            if (!file.open(QFile::ReadOnly | QIODevice::Text)){
                qDebug() << file.errorString();
            }

            // Lists to store each item separate by ";"
            QStringList  headerList1, headerList;
            QString header;
            // To ignore all the lines up do 21.
            // Save .csv in .csv - UTF-8 to work properly
            for (int i = 0; i < 21; i++){
                header = file.readLine();
            }
            headerList1 = header.split(';');
            for (int i = 0; i < headerList1.size(); i+=2){
                headerList.push_back(headerList1[i]);
            }

            file.close();   // Close file

            // Clear graphs
            ui->customPlotData->clearGraphs();
            ui->customPlotData->replot();

            // Style
            ui->customPlotData->legend->setVisible(true);
            ui->customPlotData->legend->setFont(QFont("Helvetica",9));
            ui->customPlotData->legend->setBrush(QBrush(QColor(255,255,255,230)));
            ui->customPlotData->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

            // Create graphs
            QPen pen;
            for (int i = 0; i < choosenData.size(); i++){
                if (headerList[choosenData[i]].contains("(C)", Qt::CaseInsensitive)){
                    ui->customPlotData->addGraph(); // Default axis
                    qDebug() << "C";
                }
                else if (headerList[choosenData[i]].contains("(ADC)", Qt::CaseInsensitive)){
                    ui->customPlotData->addGraph(ui->customPlotData->xAxis, ui->customPlotData->yAxis2);
                    qDebug() << "bar";
                }
                else {
                    ui->customPlotData->addGraph();
                    qDebug() << "else";
                }
                pen.setColor(QColor((choosenData.size()-i)*254/choosenData.size(),(i)*254/choosenData.size(), 100, 255));
                ui->customPlotData->graph(i)->setPen(pen);
                ui->customPlotData->graph(i)->setLineStyle(QCPGraph::lsLine);
                ui->customPlotData->graph(i)->setData(importedData[0],importedData[choosenData[i]]);
                ui->customPlotData->graph(i)->setName(headerList[choosenData[i]]);
            }

            // Set Axis labels
            ui->customPlotData->xAxis->setLabel("s");
            ui->customPlotData->yAxis->setLabel("°C");
            ui->customPlotData->yAxis2->setLabel("bar");

            // Set Ranges
            ui->customPlotData->rescaleAxes();
            // Set opposites axis to be visible
            ui->customPlotData->xAxis2->setVisible(true);
            ui->customPlotData->xAxis2->setTickLabels(false);
            ui->customPlotData->yAxis2->setVisible(true);
            ui->customPlotData->yAxis2->setTickLabels(true);
            connect(ui->customPlotData->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlotData->xAxis2, SLOT(setRange(QCPRange)));
            //connect(ui->customPlotData->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlotData->yAxis2, SLOT(setRange(QCPRange)));
            ui->customPlotData->replot();
            ui->customPlotData->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

}
        else {
            QMessageBox::warning(
                        this,
                        tr("Attention"),
                        tr("Please choose something to plot"));
        }
    }
    else {
        QMessageBox::warning(
                    this,
                    tr("Attention"),
                    tr("You need to import a file first"));
    }
}

void MainWindow::on_plotResultsButton2_clicked()
{
    // Function to calculate the results
    // Verify if there's a selected file already
    if (importedFileNameData != nullptr){
        // use importedData variable to access the data already imported

        int indTi = importedData[0].indexOf(ui->tiBox->value());
        int indTf = importedData[0].indexOf(ui->tfBox->value());

        // Get all choosen results to be plotted
        QModelIndex indResults;
        QVector<int> choosenResults;
        for (int i = 0; i < ui->plotTable3->model()->rowCount(); i++){
            indResults = ui->plotTable3->model()->index(i,0,QModelIndex());
            if (indResults.data(Qt::CheckStateRole) == Qt::Checked){
                choosenResults.push_back(i);
            }
        }

        // Plot results
        ui->plotResults2->clearGraphs();
        ui->plotResults2->replot();
        // Style
        ui->plotResults2->legend->setVisible(true);
        ui->plotResults2->legend->setFont(QFont("Helvetica",9));
        ui->plotResults2->legend->setBrush(QBrush(QColor(255,255,255,230)));
        ui->plotResults2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

        for (int i = 0; i < choosenResults.size(); i++){
            ui->plotResults2->addGraph(); // For now defaut axis
            ui->plotResults2->graph(i)->setLineStyle(QCPGraph::lsLine);
            ui->plotResults2->graph(i)->setData(importedData[0].mid(indTi,resultsMatrix[choosenResults[i]].size()),resultsMatrix[choosenResults[i]]);
            ui->plotResults2->graph(i)->setName(ui->plotTable3->model()->data(ui->plotTable3->model()->index(choosenResults[i],0)).toString());
        }

        ui->plotResults2->xAxis->setLabel("[s]");
        ui->plotResults2->yAxis->setLabel("ºC");

        // Set Ranges
        ui->plotResults2->rescaleAxes();
        // Set opposites axis to be visible
        ui->plotResults2->xAxis2->setVisible(true);
        ui->plotResults2->xAxis2->setTickLabels(false);
        ui->plotResults2->yAxis2->setVisible(true);
        ui->plotResults2->yAxis2->setTickLabels(true);
        connect(ui->plotResults2->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->plotResults2->xAxis2, SLOT(setRange(QCPRange)));
        connect(ui->plotResults2->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->plotResults2->yAxis2, SLOT(setRange(QCPRange)));
        ui->plotResults2->replot();
        ui->plotResults2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    }
    else {
        QMessageBox::warning(
                    this,
                    tr("Attention"),
                    tr("You need to import a file first"));
    }
}























