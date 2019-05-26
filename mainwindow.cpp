#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
using namespace std;

MainWindow::MainWindow(QWidget *parent) :   // Class MainWindow constructor
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Testing Git on lab
    initList();
    loadCorrelations();

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
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    prRangeStatement = true;
    reRangeStatement = true;

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

    qDebug() << "Fluids list init complete.";

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

}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{

    /*QMessageBox::information(
               this,
               tr("Choosen correlation"),
               corList[rankList[index.row()].first].getExpr());*/

    CorrelationDetails correlationDetailsWindow;
    correlationDetailsWindow.exec();
    /*
    AddCorrelation addCorrelationWindow;
    connect(&addCorrelationWindow, &AddCorrelation::sendNewSignal, this, &MainWindow::addCorrelations);
    addCorrelationWindow.exec();*/
}


void MainWindow::on_prInputButton_clicked()
{
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
