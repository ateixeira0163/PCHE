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

    // For test purposes
    QString author = "Kays";
    QString nuExpr = "0.24*Re^0.425*Pr^(1/3)";
    QVector<int> nuRange = {400,3000};
    QVector<double> prRange = {0.8,1.2};

    //Correlation test(author, nuExpr, nuRange, prRange);
    test = new Correlation(author, nuExpr, nuRange, prRange);
    QVector<int> z = test->getNuRange();
    qDebug() << z[0] << "," << z[1];

    Correlation test2;
    test2.getAuthor();


    ///
    QString a = "Ngo expression";
    QString b = "Ngo";
    QVector<int> c = {3500,22000};
    QVector<double> d = {0.75,2.2};
    QString e = "CO2 supercritical";
    QString f = "Rectangular";
    double g = 52;
    QString h = nullptr;

    Correlation cor0(a,b,c,d,e,f,g,h);

    Correlation cor1(QString a = "Ngo expression",
                     QString b = "Ngo",
                     QVector<int> c = {3500,22000},
                     QVector<double> d = {0.75,2.2},
                     QString e = "CO2 supercritical",
                     QString f = "Rectangular",
                     double g = 52,
                     QString h = nullptr);

    Correlation cor2(QString a = "I. Kim expression",
                     QString b = "I. Kim",
                     QVector<int> c = {350,800},
                     QVector<double> d = {0.66,0.66},
                     QString e = "Helium",
                     QString f = "Semi-circular",
                     double g = 15,
                     QString h = nullptr);

    Correlation cor3(QString a = "Berbish expression",
                     QString b = "Berbish",
                     QVector<int> c = {8242,57794},
                     QVector<double> d = {NULL,NULL},
                     QString e = "Ar",
                     QString f = "Semi-circular",
                     double g = 180,
                     QString h = nullptr);

    QVector<int> reRangeCase = {3600,20000};
    QVector<double> prRangeCase = {0.4,0.8};
    QString fluidCase = "Water";
    QString sectionCase = "Rectangular";
    double angleCase = 15;
    QString borderCase = nullptr;



    QVector<int> score;
    score.push_back(cor0.compare(reRangeCase,prRangeCase,fluidCase,
                                 sectionCase,angleCase,borderCase));
    qDebug() << score;


    // QFile test

    /*
    // Reading
    QFile file("..//PCHEThermalEfficiency//correlations.csv");
    if (!file.open(QFile::ReadOnly | QIODevice::Text)) {
            qDebug() << file.errorString();
    }

    QStringList wordList;
        while (!file.atEnd()) {
            QString line = file.readLine();
            wordList = line.split(',');

        }
    //qDebug() << wordList[0];
    qDebug() << wordList;
    file.close();

    // Writing
    if (!file.open(QFile::ReadWrite | QIODevice::Append | QIODevice::Text)) {
        qDebug() << file.errorString();
    }
    QTextStream out(&file);
    out << "A,B" << "C";
    file.close();
*/
    // End - Test section
}

MainWindow::~MainWindow()   // Class MainWindow destructor
{
    delete ui;
}

void MainWindow::on_actionAbout_triggered()
{
    /*                                      // Modal type (we cannot move the main window behind)
    aboutDialog aboutDialog;
    aboutDialog.setModal(true);
    aboutDialog.exec();*/
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
    fluidsLib.open("..//PCHEThermalEfficiency//fluids.txt");
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
    fluidsLib.open("..//PCHEThermalEfficiency//fluids.txt");
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
    QVector<int> nuRange = {ui->reMinBox->value(), ui->reMaxBox->value()};
    QVector<double> prRange = {ui->prMinBox->value(), ui->prMaxBox->value()};
    QString fluid = ui->fluidBox->currentText();
    QString section = ui->sectionBox->currentText();
    double angle = ui->angleBox->value();
    QString border = ui->borderBox->currentText();

   //test->compare(nuRange,prRange);
}

void MainWindow::on_addNewButton_clicked()
{
    AddCorrelation addCorrelationWindow;
    connect(&addCorrelationWindow, &AddCorrelation::sendNewSignal, this, &MainWindow::addNewNusselt);
    addCorrelationWindow.exec();
    qDebug() << "AddNewCorrelation window opened.";
}
