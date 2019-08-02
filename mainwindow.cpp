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

    // Model parameters input as default for now // CHANGE LATER
    modelParameters["chNb"] = 10;       // channels number
    modelParameters["wTh"] = 0.0005;    // wall thickness [m]
    modelParameters["chWidth"] = 0.002; // channels width [m]
    modelParameters["chHeight"] = 0.001;// channels height [m]
    modelParameters["strLenght"] = 0.04;// straight lenght [m]
    modelParameters["zzNb"] = 4;        // zigzag number in straight lenght
    modelParameters["zzAngle"] = 35;    // zigzag angle [º]
    modelParameters["heatTransferArea"] = modelParameters["chWidth"] * (modelParameters["strLenght"]/(modelParameters["zzNb"]*2*qCos(qDegreesToRadians(modelParameters["zzAngle"]))))*
            2*modelParameters["zzNb"]*modelParameters["chNb"];  // Heat Transfer Area [m]
    modelParameters["dH"] = 2*modelParameters["chWidth"]*modelParameters["chHeight"]/(modelParameters["chWidth"] + modelParameters["chHeight"]); // hidraulic Diameter   2*a*b/(a+b) for rectangle


    QVector<int> muT = {260, 280, 300, 320, 340, 360, 380, 400};
    QVector<double> mu1bar = {16.55e-6, 17.56e-6, 18.54e-6, 19.49e-6, 20.41e-6, 21.32e-6, 22.20e-6, 23.06e-6};
    QVector<double> mu5bar = {16.62e-6, 17.62e-6, 18.60e-6, 19.54e-6, 20.47e-6, 21.37e-6, 22.24e-6, 23.10e-6};
    QVector<double> mu10bar = {16.70e-6, 17.70e-6, 18.67e-6, 19.62e-6, 20.54e-6, 21.43e-6, 22.31e-6, 23.16e-6};
    for (int i = 0; i < muT.size(); i++){;
        muAir[1].append(qMakePair(muT[i], mu1bar[i]));
        muAir[5].append(qMakePair(muT[i], mu5bar[i]));
        muAir[10].append(qMakePair(muT[i], mu10bar[i]));
    }

    /* For testing puporses
    interpolate(390, 7.5, muAir);
    interpolate(290,5,muAir);
    interpolate(290,2,muAir);
    interpolate(335,10,muAir);
    interpolate(320,10,muAir); */

    rangeRect = new QCPItemRect(ui->customPlotData);
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

double MainWindow::interpolate(double T, double P, QMap<int, QVector<QPair<int, double> > > prop)
{
    // First we assure that  T and P are in the range of data
    if (P < prop.firstKey() || P > prop.lastKey() || T < prop.first()[0].first || T > prop.first()[prop.first().size() - 1].first){
        //qDebug() << "Out of range";
        return 0;
    }
    else{
        QVector<double> xInf(2), xSup(2);     // xInf -> x inferior pressure, xSup -> x superior pressure
        QList<int> keysList = prop.uniqueKeys();    // keys (Pressure) organized in ascending order

        // First we find the lower bound value for pressure
        int lP = 0, uP; // prop.value(prop.uniqueKeys()[lB]) indicates the lower bound
        while (lP != keysList.size()){
            if (P <= keysList[lP]){
                if (P < keysList[lP]) --lP;
                break;
            } ++lP;
        }

        // Now to find the temperature lower bound
        int lT = 0, uT;
        while (lT != prop.first().size()){
            if (T <= prop.first()[lT].first){
                if (T < prop.first()[lT].first) --lT;
                break;
            }
            ++lT;
        }

        // Now we interpolate

        // In this case the pressure inputed exists so no need for interpolation
        uP = (P == keysList[lP] ? lP : lP + 1);
        uT = (T == prop.first()[lT].first ? lT : lT + 1);

        xInf = {prop[keysList[lP]][lT].second, prop[keysList[uP]][lT].second};
        xSup = {prop[keysList[lP]][uT].second, prop[keysList[uP]][uT].second};

        // Interpolating inferior bound
        double propInf, propSup, propResult;
        propInf = xInf[0] + ((xInf[1] - xInf[0])/(lP == uP ? 1 : keysList[uP] - keysList[lP]))*(P - keysList[lP]);
        propSup = xSup[0] + ((xSup[1] - xSup[0])/(lP == uP ? 1 : keysList[uP] - keysList[lP]))*(P - keysList[lP]);
        propResult = propInf + ((propSup - propInf)/(lT == uT ? 1 : prop.first()[uT].first - prop.first()[lT].first))*(T - prop.first()[lT].first);

        return propResult;
    }

    return 0;
}

void MainWindow::calculateResults()
{
    // Function that calculates the parameters and stores them

    int indTi = importedData[0].indexOf(ui->tiBox->value());
    int indTf = importedData[0].indexOf(ui->tfBox->value());

    if ( indTi == -1 || indTi > indTf ){
        indTi = 0;
        ui->tiBox->setValue(importedData[0][0]);
    }
    if ( indTf == -1 ){
        indTf = importedData[0].size() - 1;
        ui->tfBox->setValue(importedData[0][importedData[0].size() - 1]);
    }

    resultsMatrix.clear();

    // Means
    QVector<double> meansVector;

    // LMTD / Pressure Drop / Air Velocity
    QVector<double> logDiffT, pressureDrop, uAir, reAir, qW;
    double tOutA, tInA, tOutW, tInW, lmtd, pOutA, pInA, uA, muA;
    for (int i = indTi; i < indTf; i++){
        // LMTD
        tOutW = (importedData[1][i] + importedData[2][i])/2;
        tInW = (importedData[3][i] + importedData[4][i])/2;
        tOutA = importedData[10][i];
        tInA = importedData[5][i];
        lmtd = (isnan((tOutA - tInW - tInA + tOutW)/log((tOutA - tInW)/(tInA - tOutW))) ? 0 : (tOutA - tInW - tInA + tOutW)/log((tOutA - tInW)/(tInA - tOutW)));
        logDiffT.push_back(lmtd);

        // Pressure Drop
        pressureDrop.push_back(importedData[6][i] - importedData[8][i]);

        // Air velocity
        uA = importedData[7][i]/(3600*modelParameters["chNb"]*modelParameters["chWidth"]*modelParameters["chHeight"]); // 3600: h->s
        uAir.push_back(uA);

        // Reynolds number
        pOutA = importedData[8][i];
        pInA = importedData[6][i];
        muA = interpolate(((tOutA + tInA)/2)+273.15, (pOutA + pInA)/2, muAir);
        if (muA != 0) reAir.push_back( ((((pOutA + pInA)/2)*1e+05)/(287.058 * (((tOutA + tInA)/2) + 273.15)))*(uA/muA)*modelParameters["dH"]);
        else reAir.push_back(0);

        // qW
        // Add CPW data to do interpolation
    }

    meansVector.push_back(mean(0,logDiffT.size(),logDiffT)); // [0]
    meansVector.push_back(mean(0,pressureDrop.size(),pressureDrop)); // [1]
    meansVector.push_back(mean(0,uAir.size(),uAir)); // [2]
    meansVector.push_back(mean(0,reAir.size(),reAir)); // [3]
    resultsMatrix.push_back(logDiffT); // [0]
    resultsMatrix.push_back(pressureDrop); // [1]
    resultsMatrix.push_back(uAir); // [2]
    resultsMatrix.push_back(reAir); // [3]

    // Get all choosen results to be plotted
    QModelIndex indResults;
    QVector<bool> choosenResults;

    // In order to store the already marked cases
    if (initImport == true){
        for (int i = 0; i < ui->plotTable3->model()->rowCount(); i++){
            indResults = ui->plotTable3->model()->index(i,0,QModelIndex());
            if (indResults.data(Qt::CheckStateRole) == Qt::Checked){
                choosenResults.push_back(true);
            }
            else{
                choosenResults.push_back(false);
            }
        }
    }

    // Add options to plot results after
    QVector<QString> optionsForTable3 = {"LMTD", "Pressure Drop", "Air Velocity", "Reynolds Nb (Air)"};
    auto plotModelTable3 = new QStandardItemModel();


    for (int i = 0; i < optionsForTable3.size(); i++){
        QStandardItem *itemCheckBox = new QStandardItem(true);
        itemCheckBox->setCheckable(true);

        if (!initImport){
            itemCheckBox->setCheckState(Qt::Unchecked);
        }
        else{
            if (choosenResults[i]){
                itemCheckBox->setCheckState(Qt::Checked);
            }
            else{
                itemCheckBox->setCheckState(Qt::Unchecked);
            }
        }
        itemCheckBox->setText(optionsForTable3[i]);
        plotModelTable3->setItem(i,0,itemCheckBox);

        QStandardItem *itemMean = new QStandardItem(QString("%0").arg(QString::number(meansVector[i])));
        plotModelTable3->setItem(i,1,itemMean);
    }

    plotModelTable3->setHeaderData(0,Qt::Horizontal,"Result");
    plotModelTable3->setHeaderData(1,Qt::Horizontal,"Mean");
    ui->plotTable3->setModel(plotModelTable3);
    ui->plotTable3->resizeColumnsToContents();

    // Add means to plot 1

    QModelIndex indPlots;
    QVector<bool> choosenPlots;

    // To store the already marked plots
    if (initImport == true){ // Verify if table is not empty
        for (int i = 0; i < ui->plotTable2->model()->rowCount(); i++){
            indPlots = ui->plotTable2->model()->index(i,0,QModelIndex());
            if (indPlots.data(Qt::CheckStateRole) == Qt::Checked) choosenPlots.push_back(true);
            else choosenPlots.push_back(false);
        }
    }

    auto plotModelTable2 = new QStandardItemModel();

    for (int i = 0; i < headerListPlot.size()/2 - 1; i++){
        QStandardItem *itemCheckBoxPlot = new QStandardItem(true);
        itemCheckBoxPlot->setCheckable(true);
        if (!initImport) itemCheckBoxPlot->setCheckState(Qt::Unchecked);
        else{
            if (choosenPlots[i]) itemCheckBoxPlot->setCheckState(Qt::Checked);
            else itemCheckBoxPlot->setCheckState(Qt::Unchecked);
        }
        itemCheckBoxPlot->setText(headerListPlot[(i*2) + 2]);
        plotModelTable2->setItem(i,0,itemCheckBoxPlot);

        QStandardItem *plotMean = new QStandardItem(QString("%0").arg(QString::number(mean(indTi,indTf,importedData[i+1]))));
        plotModelTable2->setItem(i,1,plotMean);
    }

    plotModelTable2->setHeaderData(0, Qt::Horizontal, "Data");
    plotModelTable2->setHeaderData(1, Qt::Horizontal, "Mean");
    ui->plotTable2->setModel(plotModelTable2);
    ui->plotTable2->resizeColumnsToContents();
    ui->plotTable2->horizontalHeader()->show();
    ui->plotTable2->verticalHeader()->show();


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

        //QStringList headerListPlot;
        headerListPlot.clear();
        QString header;
        // To ignore all the lines up do 21.
        // Save .csv in .csv - UTF-8 to work
        for (int i = 0; i < 21; i++){
            header = file.readLine();
        }
        headerListPlot = header.split(';');

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

        // Calculate results:
        calculateResults();
        initImport = true;
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

            // Add rectangle for permanent range

            rangeRect->setVisible(true);
            rangeRect->setPen(QPen(Qt::transparent));
            rangeRect->setBrush(QBrush(QColor(255, 0, 0, 20)));
            rangeRect->topLeft->setCoords(ui->tiBox->value(),300);
            rangeRect->bottomRight->setCoords(ui->tfBox->value(),0);

            // Style
            ui->customPlotData->legend->setVisible(true);
            ui->customPlotData->legend->setFont(QFont("Helvetica",9));
            ui->customPlotData->legend->setBrush(QBrush(QColor(255,255,255,230)));
            ui->customPlotData->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
            //ui->customPlotData->axisRect()->addAxis(QCPAxis::atRight)->setLabel("Q"); //Find way to add another axis for flow rate

            // Create graphs
            QPen pen;
            for (int i = 0; i < choosenData.size(); i++){
                if (headerList[choosenData[i]].contains("(C)", Qt::CaseInsensitive)){
                    ui->customPlotData->addGraph(); // Default axis
                    //qDebug() << "C";
                }
                else if (headerList[choosenData[i]].contains("(ADC)", Qt::CaseInsensitive)){
                    ui->customPlotData->addGraph(ui->customPlotData->xAxis, ui->customPlotData->yAxis2);
                    //qDebug() << "bar";
                }
                else {
                    ui->customPlotData->addGraph();
                    //qDebug() << "else";
                }
                pen.setColor(QColor((choosenData.size()-i)*254/choosenData.size(),(i)*254/choosenData.size(), 100, 255));
                ui->customPlotData->graph(i)->setPen(pen);
                ui->customPlotData->graph(i)->setLineStyle(QCPGraph::lsLine);
                ui->customPlotData->graph(i)->setData(importedData[0],importedData[choosenData[i]]);
                ui->customPlotData->graph(i)->setName(headerList[choosenData[i]]);
            }

            // Create error bars graphs

            QVector<double> eXData;                                        // Create list for X axis that jumps 'eSpacing' points
            int eSpacing = 10;  // Change this parameter to change how many points show the error bar
            for (int i = 0; i < importedData[0].size(); i += eSpacing){
                eXData.push_back(importedData[0][i]);
            }

            QVector<double> e(eXData.size());
            QVector<double>* eYData;
            errorVector.clear();

            for (int i = 0; i < choosenData.size(); i++){   // Create a matrix for y axis that jumps 'eSpacing' points
                eYData = new QVector<double>;
                for (int j = 0; j < importedData[0].size(); j += eSpacing){
                    eYData->push_back(importedData[choosenData[i]][j]);
                }

                QCPErrorBars* errorBars;
                if (headerList[choosenData[i]].contains("(C)", Qt::CaseInsensitive)){
                    ui->customPlotData->addGraph(); // Default axis
                    errorBars = new QCPErrorBars(ui->customPlotData->xAxis, ui->customPlotData->yAxis);
                }
                else if (headerList[choosenData[i]].contains("(ADC)", Qt::CaseInsensitive)){
                    ui->customPlotData->addGraph(ui->customPlotData->xAxis, ui->customPlotData->yAxis2);
                    errorBars = new QCPErrorBars(ui->customPlotData->xAxis, ui->customPlotData->yAxis2);
                }
                else {
                    ui->customPlotData->addGraph();
                    errorBars = new QCPErrorBars(ui->customPlotData->xAxis, ui->customPlotData->yAxis);
                }

                pen.setColor(QColor((choosenData.size()-i)*254/choosenData.size(),(i)*254/choosenData.size(), 100, 255));
                ui->customPlotData->graph(choosenData.size() + i)->setData(eXData, *eYData);
                ui->customPlotData->graph(choosenData.size() + i)->setVisible(false);
                ui->customPlotData->graph(choosenData.size() + i)->removeFromLegend();
                ui->customPlotData->graph(choosenData.size() + i)->setSelectable(QCP::SelectionType::stNone);


                // error element

                //errorBars->setVisible(false);
                errorBars->removeFromLegend();
                errorBars->setAntialiased(false);
                errorBars->setDataPlottable(ui->customPlotData->graph(choosenData.size() + i));
                errorBars->setPen(pen);
                errorBars->setSelectable(QCP::SelectionType::stNone);

                for (int k = 0; k < e.size(); k++){
                    e[k] = 1;
                }
                errorBars->setData(e);
                errorVector.push_back(errorBars);
            }

            // Set Axis labels
            ui->customPlotData->xAxis->setLabel("[s]");
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
            ui->customPlotData->setInteractions(QCP::iSelectPlottables);
            //connect(ui->customPlotData, SIGNAL(selectionChangedByUser()), this, SLOT(graphSelectionChanged()));
            //ui->customPlotData->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

            // Adding a context menu to hide/move legend
            ui->customPlotData->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(ui->customPlotData, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

            // Adding rangeSlider connection
            ui->plotRangeSlider->setEnabled(true);
            //ui->plotRangeSlider->setTracking(false); // To change only after let go the button
            connect(ui->plotRangeSlider, SIGNAL(valuesChanged(int, int)), this, SLOT(refreshRange(int, int)));

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

void MainWindow::refreshRange(int minValue, int maxValue)
{
    ui->tiBox->setValue(importedData[0][minValue*importedData[0].size()/100]);
    ui->tfBox->setValue(importedData[0][maxValue*importedData[0].size()/100]);
    calculateResults();
    rangeRect->topLeft->setCoords(ui->tiBox->value(),300);
    rangeRect->bottomRight->setCoords(ui->tfBox->value(),0);
    ui->customPlotData->replot();
}

void MainWindow::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (ui->customPlotData->legend->selectTest(pos, false) >= 0 && ui->customPlotData->legend->visible()){ // Verify if clicked on legend
        menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
        menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
        menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
        menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
        menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
        menu->addSeparator();
        menu->addAction("Hide", this, SLOT(hideLegend()))->setData(true);
    }
    else{
        if (!ui->customPlotData->legend->visible()){
            menu->addAction("Show legend", this, SLOT(hideLegend()))->setData(false);
        }
        if (ui->customPlotData->selectedGraphs().size() > 0){
            menu->addAction("Show confidence details", this, SLOT(showConfidenceDetails()));

            for (int i = 0; i < ui->customPlotData->graphCount()/2; i++){ // Pass through every graph except graphs added to error bars
                if (ui->customPlotData->selectedGraphs().first() == ui->customPlotData->graph(i) && errorVector[i]->visible() == true){
                    menu->addAction("Hide error bar", this, SLOT(hideErrorBar()))->setData(false);
                }
                else if (ui->customPlotData->selectedGraphs().first() == ui->customPlotData->graph(i) && errorVector[i]->visible() == false){
                    menu->addAction("Show error bar", this, SLOT(hideErrorBar()))->setData(true);
                }
            }
        }
        else{
            menu->addAction("Show all error bars", this, SLOT(hideAllErrorBars()))->setData(true);
            menu->addAction("Hide all error bars", this, SLOT(hideAllErrorBars()))->setData(false);
        }
    }
    menu->popup(ui->customPlotData->mapToGlobal(pos));
}

void MainWindow::hideAllErrorBars()
{
    // Function that shows/hides all error bars from the graph
    if (QAction* contextAction = qobject_cast<QAction*>(sender())){
        bool dataBool = contextAction->data().toBool();
        for (int i = 0; i < ui->customPlotData->graphCount()/2; i++){ // Pass through every graph except graphs added to error bars
                errorVector[i]->setVisible(dataBool);
        }
        ui->customPlotData->replot();
    }
}

void MainWindow::hideErrorBar()
{
    // Find a way to hide and show after replotting
    if (QAction* contextAction = qobject_cast<QAction*>(sender())){
        bool dataBool = contextAction->data().toBool();
        for (int i = 0; i < ui->customPlotData->graphCount()/2; i++){ // Pass through every graph except graphs added to error bars
            if (ui->customPlotData->selectedGraphs().first() == ui->customPlotData->graph(i)){
                errorVector[i]->setVisible(dataBool);
            }
        }
        ui->customPlotData->replot();
    }
}

void MainWindow::showConfidenceDetails()
{
    // Function to open window showing confidence details
    if (ui->customPlotData->selectedGraphs().size() > 0){
        for (int i = 0; i < ui->customPlotData->graphCount()/2; i++){ // Pass through every graph except graphs added to error bars
            if (ui->customPlotData->selectedGraphs().first() == ui->customPlotData->graph(i)){
                qDebug() << "same" << i;
                // Add call here for opening window showing confidence details

            }
        }
    }
}

void MainWindow::moveLegend()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
    {
        bool ok;
        int dataInt = contextAction->data().toInt(&ok);
        if (ok)
        {
            ui->customPlotData->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
            ui->customPlotData->replot();
        }
    }
}

void MainWindow::hideLegend()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender())){
        bool dataBool = contextAction->data().toBool();

        if (dataBool){ // if it's being shown
            ui->customPlotData->legend->setVisible(false);
        }
        else{
            ui->customPlotData->legend->setVisible(true);
        }
        ui->customPlotData->replot();
    }
}

void MainWindow::on_plotResultsButton2_clicked()
{
    // Verify if there's a selected file already
    if (importedFileNameData != nullptr){
        // use importedData variable to access the data already imported
        calculateResults();


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

void MainWindow::on_pushButton_clicked()
{
    ui->plotRangeSlider->setValues((ui->tiBox->value()/ui->timeIntervalBox->value())*100/importedData[0].size(),
            (ui->tfBox->value()/ui->timeIntervalBox->value())*100/importedData[0].size());
    calculateResults();
}
