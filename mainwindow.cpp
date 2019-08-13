#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
using namespace std;

MainWindow::MainWindow(QWidget *parent) :   // Class MainWindow constructor
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Correlations - TableView
    loadCorrelations();
    showCorrelations();
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
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


    QVector<double> muT = {260, 280, 300, 320, 340, 360, 380, 400};
    QVector<double> mu1bar = {16.55e-6, 17.56e-6, 18.54e-6, 19.49e-6, 20.41e-6, 21.32e-6, 22.20e-6, 23.06e-6};
    QVector<double> mu5bar = {16.62e-6, 17.62e-6, 18.60e-6, 19.54e-6, 20.47e-6, 21.37e-6, 22.24e-6, 23.10e-6};
    QVector<double> mu10bar = {16.70e-6, 17.70e-6, 18.67e-6, 19.62e-6, 20.54e-6, 21.43e-6, 22.31e-6, 23.16e-6};
    for (int i = 0; i < muT.size(); i++){
        muAir[1].append(qMakePair(muT[i], mu1bar[i]));
        muAir[5].append(qMakePair(muT[i], mu5bar[i]));
        muAir[10].append(qMakePair(muT[i], mu10bar[i]));
    }

    QVector<double> cpWT = {0.01, 10, 20, 25, 30, 40, 50, 60, 70, 80, 90, 99.606};
    QVector<double> cpW1 = {4.2194e+3, 4.1952e+3, 4.1841e+3, 4.1813e+3, 4.1798e+3, 4.1794e+3, 4.1813e+3, 4.1850e+3, 4.1901e+3, 4.1968e+3, 4.2052e+3, 4.2152e+3};
    for (int i = 0; i < cpWT.size(); i++) cpW[1].append(qMakePair(cpWT[i], cpW1[i]));   // In this case, (simple interpolation) use only P = 1

    QVector<double> cpAirT = {260, 280, 300, 320, 340, 360, 380, 400};
    QVector<double> cpAir1bar = {1.006e+3, 1.006e+3, 1.007e+3, 1.007e+3, 1.009e+3, 1.010e+3, 1.012e+3, 1.014e+3};
    QVector<double> cpAir5bar = {1.015e+3, 1.014e+3, 1.013e+3, 1.013e+3, 1.013e+3, 1.014e+3, 1.016e+3, 1.018e+3};
    QVector<double> cpAir10bar = {1.026e+3, 1.023e+3, 1.021e+3, 1.020e+3, 1.019e+3, 1.019e+3, 1.020e+3, 1.022e+3};
    for (int i = 0; i < cpAirT.size(); i++){
        cpAir[1].append(qMakePair(cpAirT[i], cpAir1bar[i]));
        cpAir[5].append(qMakePair(cpAirT[i], cpAir5bar[i]));
        cpAir[10].append(qMakePair(cpAirT[i], cpAir10bar[i]));
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

// ============= Correlations Tab methods ============== //


void MainWindow::loadCorrelations()
{
    if (!alreadySearched){
        ui->fluidBox->addItem("--");          // Add empty options as defaut
        ui->fluidBox->setCurrentIndex(0);
        ui->sectionBox->addItem("--");
        ui->sectionBox->setCurrentIndex(0);
        ui->borderBox->addItem("--");
        ui->borderBox->setCurrentIndex(0);
        ui->channelBox->addItem("--");
        ui->channelBox->setCurrentIndex(0);
    }

    QFile file("..//PCHEThermalEfficiency//correlations.csv");  // Declare file
    if (!file.open(QFile::ReadOnly | QIODevice::Text)){         // Check if open was succesful
        qDebug() << file.errorString();                         // if not = return string of error
    }

    QStringList itemList;  // List of string to store the information for each line
    int rangeValues[] = {2, 3, 6, 8, 10, 11, 12}; // Index for all parameters that have multiple data separated by '/'
    QList<QStringList> rangeParam;
    int stringListsValues[] = {4, 5, 7, 9}; //
    QList<QStringList> stringLists;         //
    QList<QComboBox*> boxesList = {ui->fluidBox, ui->sectionBox, ui->channelBox, ui->borderBox};
    corList.clear();

    while (!file.atEnd()){                      // Until the end
        QString line = file.readLine();         // Read each line
        itemList = line.split(';');             // define separator
        if (itemList.size() > 1){               // Certify that is not a empty line

            rangeParam.clear();
            for (int i = 0; i < 7; i++){
                rangeParam.push_back(itemList[rangeValues[i]].split('/'));
            }
            stringLists.clear();    //
            for (int i = 0; i < 4; i++){
                stringLists.push_back(itemList[stringListsValues[i]].split('/'));
            }

            // The object is created
            Correlation temp(itemList[0],    //[0] - Expression
                        itemList[1],         //[1] - Author
                        {rangeParam[0][0].toInt(),rangeParam[0][1].toInt()}, QVariant(rangeParam[0][2]).toBool(),   // [2] - Reynolds
                        {rangeParam[1][0].toDouble(), rangeParam[1][1].toDouble()}, QVariant(rangeParam[1][2]).toBool(),    // [3] - Prandtl         
                        stringLists[0], //[4] - Fluid
                        stringLists[1],   //[5] - Section
                        {rangeParam[2][0].toDouble(), rangeParam[2][1].toDouble()}, QVariant(rangeParam[2][2]).toBool(),    // [6] - Diameter
                        stringLists[2],    //[7] - Channel type
                        {rangeParam[3][0].toDouble(), rangeParam[3][1].toDouble()}, QVariant(rangeParam[3][2]).toBool(),    // [8] - Angle
                        stringLists[3],  // [9] - Border type
                        {rangeParam[4][0].toDouble(), rangeParam[4][1].toDouble()}, QVariant(rangeParam[4][2]).toBool(),    //[10] - Length
                        {rangeParam[5][0].toDouble(), rangeParam[5][1].toDouble()}, QVariant(rangeParam[5][2]).toBool(),    //[11] - Viscosity
                        {rangeParam[6][0].toDouble(), rangeParam[6][1].toDouble()}, QVariant(rangeParam[6][2]).toBool(),    // [12] - Temperature
                        itemList[13],
                        itemList[14]);
            corList.push_back(temp);            // We add to the list

            // Add the already existing to the options
            for (int i = 0; i < stringLists.size(); i++){
                for (int j = 0; j < stringLists[i].size(); j++){
                    if (boxesList[i]->findText(stringLists[i][j]) == -1 && stringLists[i][j] != "--"){
                        boxesList[i]->addItem(stringLists[i][j]);
                    }
                }
            }
        }
    }

    file.close(); // Close file

    if (alreadySearched) on_searchButton_clicked();
    else showCorrelations();

    //Test purpose
    /*
    QPair<int, QList<bool>> r = corList[1].compare(QVector<int> {0,0}, true,
                                                   QVector<double> {0,0}, false,
                                                   "CO",
                                                   "hh",
                                                   QVector<double> {0,0}, false,
                                                   "--",
                                                   QVector<double> {0,0}, false,
                                                   "--",
                                                   QVector<double> {0,0}, false,
                                                   QVector<double> {0,0}, false,
                                                   QVector<double> {0,0}, false);

    qDebug() << r << r.second.size();*/

}

void MainWindow::on_addNewButton_clicked()
{
    AddCorrelation addCorrelationWindow;
    connect(&addCorrelationWindow, &AddCorrelation::sendNewSignal, this, &MainWindow::loadCorrelations);
    addCorrelationWindow.exec();
}

void MainWindow::showCorrelations()
{

    // Function to show all the existant correlations without any comparison

    auto modelTable = new QStandardItemModel();
    QList<QVector<double> > allRanges;
    QList<bool> allVars;
    QList<QStringList> allStringLists;
    int allRangesTableOrder[] = {1, 3, 7, 10, 13, 15, 17};
    int allStringsTableOrder[] = {5, 6, 9, 12};

    for (int i = 0; i < corList.size(); i++){
        // Add author
        modelTable->appendRow(new QStandardItem(corList[i].getAuthor()));

        allRanges.clear();
        allRanges = corList[i].getAllRanges();
        allVars.clear();
        allVars = corList[i].getAllVars();
        allStringLists.clear();
        allStringLists = corList[i].getAllStringLists();

        for (int k = 0; k < allRanges.size(); k++){
            QString *kRangeString = new QString((allRanges[k] == QVector<double> {0,0} ?
                                                "--" : (fabs(allRanges[k][0] - allRanges[k][1]) < 1e-8 ?
                                                        QString("[%0]").arg(QString::number(allRanges[k][0])) :
                                                        QString("[%0,%1]").arg(QString::number(allRanges[k][0])).arg(QString::number(allRanges[k][1])))));
            QStandardItem *itemRange = new QStandardItem(*kRangeString);
            delete kRangeString;
            modelTable->setItem(i,allRangesTableOrder[k],itemRange);

            QStandardItem *itemVar = new QStandardItem("");
            modelTable->setItem(i,allRangesTableOrder[k]+1,itemVar);
            QModelIndex varIndex = modelTable->index(i,allRangesTableOrder[k]+1);
            if (allVars[k]) modelTable->setData(varIndex,QIcon(":/fx.png"),Qt::DecorationRole);
        }

        for (int k = 0; k < allStringLists.size(); k++){
            QString kStringListsString;
            for (int n = 0; n < allStringLists[k].size(); n++){
                kStringListsString += allStringLists[k][n];
                if (n != allStringLists[k].size() - 1) kStringListsString += " \n";
            }
            QStandardItem *itemString = new QStandardItem(kStringListsString);
            modelTable->setItem(i,allStringsTableOrder[k],itemString);
        }

    }

    // Set all horizontal header
    modelTable->setHeaderData(0,Qt::Horizontal,"Author");
    modelTable->setHeaderData(1,Qt::Horizontal,"Re - Range");
    modelTable->setHeaderData(2,Qt::Horizontal,"");
    modelTable->setHeaderData(3,Qt::Horizontal,"Pr - Range");
    modelTable->setHeaderData(4,Qt::Horizontal,"");
    modelTable->setHeaderData(5,Qt::Horizontal,"Fluid");
    modelTable->setHeaderData(6,Qt::Horizontal,"Section");
    modelTable->setHeaderData(7,Qt::Horizontal,"Diameter");
    modelTable->setHeaderData(8,Qt::Horizontal,"");
    modelTable->setHeaderData(9,Qt::Horizontal,"Channel");
    modelTable->setHeaderData(10,Qt::Horizontal,"Angle");
    modelTable->setHeaderData(11,Qt::Horizontal,"");
    modelTable->setHeaderData(12,Qt::Horizontal,"Border");
    modelTable->setHeaderData(13,Qt::Horizontal,"Length");
    modelTable->setHeaderData(14,Qt::Horizontal,"");
    modelTable->setHeaderData(15,Qt::Horizontal,"Viscosity");
    modelTable->setHeaderData(16,Qt::Horizontal,"");
    modelTable->setHeaderData(17,Qt::Horizontal,"Temperature");
    modelTable->setHeaderData(18,Qt::Horizontal,"");
    ui->tableView->setModel(modelTable);    // Set model for tableView
    ui->tableView->horizontalHeader()->show(); // show horizontal header
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();

}

void MainWindow::on_searchButton_clicked()
{
    //! Function that gets inputed values, search and classify correlations based on database
    // Get all the input from the ui

    QList<QDoubleSpinBox*> minBoxes = {ui->prMinBox, ui->diamMinBox, ui->angleBox, ui->lMinBox, ui->viscMinBox, ui->tempMinBox};
    QList<QDoubleSpinBox*> maxBoxes = {ui->prMaxBox, ui->diamMaxBox, ui->angleMaxBox, ui->lMaxBox, ui->viscMaxBox, ui->tempMaxBox};
    QList<QPushButton*> rangeButtons = {ui->prRangeButton, ui->dRangeButton, ui->aRangeButton, ui->lRangeButton, ui->viscRangeButton, ui->tempRangeButton};
    QList<QVector<double>> rangeValues;

    for (int i = 0; i < rangeButtons.size(); i++){
        rangeValues.append(rangeButtons[i]->isChecked() ?
                               QVector<double> {minBoxes[i]->value(), minBoxes[i]->value()} :
                               QVector<double> {minBoxes[i]->value(), (fabs(maxBoxes[i]->value()) < 1e-8 ?
                                                                           minBoxes[i]->value() :
                                                                           maxBoxes[i]->value())});
    }

    QVector<int> reRange = (ui->reRangeButton->isChecked() ?
                                QVector<int> {ui->reMinBox->value(), ui->reMinBox->value()} :
                                QVector<int> {ui->reMinBox->value(), (ui->reMaxBox->value() == 0 ?
                                                                          ui->reMinBox->value() :
                                                                          ui->reMaxBox->value())});
    QString fluid = ui->fluidBox->currentText();
    QString section = ui->sectionBox->currentText();
    QString channel = ui->channelBox->currentText();
    QString border = ui->borderBox->currentText();

    bool reCheckBox = ui->reCheckBox->checkState();
    bool prCheckBox = ui->prCheckBox->checkState();
    bool dCheckBox = ui->dCheckBox->checkState();
    bool angleCheckBox = ui->angleCheckBox->checkState();
    bool lCheckBox = ui->lCheckBox->checkState();
    bool viscCheckBox = ui->viscCheckBox->checkState();
    bool tempCheckBox = ui->tempCheckBox->checkState();

    // Calculate the max score based on nb of answers
    int maxScore = 0;
    if (reRange != QVector<int> {0,0}) maxScore += 2;
    if (rangeValues[0] != QVector<double> {0,0}) maxScore += 2;
    if (fluid != "--") maxScore += 2;
    if (section != "--") maxScore += 2;
    if (rangeValues[1] != QVector<double> {0,0}) maxScore += 2;
    if (channel != "--") maxScore += 2;
    if (rangeValues[2] != QVector<double> {0,0}) maxScore += 2;
    if (border != "--") maxScore += 2;
    if (rangeValues[3] != QVector<double> {0,0}) maxScore += 2;
    if (rangeValues[4] != QVector<double> {0,0}) maxScore += 2;

    if (reCheckBox) maxScore += 2;
    if (prCheckBox) maxScore += 2;
    if (dCheckBox) maxScore += 2;
    if (angleCheckBox) maxScore += 2;
    if (lCheckBox) maxScore += 2;
    if (viscCheckBox) maxScore += 2;
    if (tempCheckBox) maxScore += 2;

    //rankList: (pos,(score((originalpos,author)))
    // Sorry for this type of approach, but it's to much work to change now
    // Please don't use QPair for more than 2 items (It's stupid, just look down to understand)
    rankList.clear();
    QList<QPair<int, QList<bool>>> resultsList;

    for (int i = 0; i < corList.size(); i++){
        // We compare with all the correlations and add results to list
        resultsList.push_back(corList[i].compare(reRange, reCheckBox,
                                                 rangeValues[0], prCheckBox,
                                                 fluid,
                                                 section,
                                                 rangeValues[1], dCheckBox,
                                                 channel,
                                                 rangeValues[2], angleCheckBox,
                                                 border,
                                                 rangeValues[3], lCheckBox,
                                                 rangeValues[4], viscCheckBox,
                                                 rangeValues[5], tempCheckBox));
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
    int allRangesTableOrder[] = {2, 4, 8, 11, 14, 16, 18};
    int allStringsTableOrder[] = {6, 7, 10, 13};
    QList<QVector<double> > allRanges;
    QList<bool> allVars;
    QList<QStringList> allStringLists;

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

        allRanges.clear();
        allRanges = corList[rankList[i].second.first].getAllRanges();
        allVars.clear();
        allVars = corList[rankList[i].second.first].getAllVars();
        allStringLists.clear();
        allStringLists = corList[rankList[i].second.first].getAllStringLists();

        for (int k = 0; k < allRanges.size(); k++){
            QString *kRangeString = new QString((allRanges[k] == QVector<double> {0,0} ?
                                                "--" : (fabs(allRanges[k][0] - allRanges[k][1]) < 1e-8 ?
                                                        QString("[%0]").arg(QString::number(allRanges[k][0])) :
                                                        QString("[%0,%1]").arg(QString::number(allRanges[k][0])).arg(QString::number(allRanges[k][1])))));
            QStandardItem *itemRange = new QStandardItem(*kRangeString);
            delete kRangeString;
            modelTable->setItem(i,allRangesTableOrder[k],itemRange);
            QModelIndex rangeIndex = modelTable->index(i,allRangesTableOrder[k]);
            if (resultsList[rankList[i].second.first].second[k]) modelTable->setData(rangeIndex,QIcon(":/checkmarkGreen.png"),Qt::DecorationRole);
            else modelTable->setData(rangeIndex,QIcon(":/checkmarkRed.png"),Qt::DecorationRole);

            QStandardItem *itemVar = new QStandardItem("");
            modelTable->setItem(i,allRangesTableOrder[k]+1,itemVar);
            QModelIndex varIndex = modelTable->index(i,allRangesTableOrder[k]+1);
            if (allVars[k]) modelTable->setData(varIndex,QIcon(":/fx.png"),Qt::DecorationRole);
        }

        for (int k = 0; k < allStringLists.size(); k++){
            QString kStringListsString;
            for (int n = 0; n < allStringLists[k].size(); n++){
                kStringListsString += allStringLists[k][n];
                if (n != allStringLists[k].size() - 1) kStringListsString += " \n";
            }
            QStandardItem *itemString = new QStandardItem(kStringListsString);
            modelTable->setItem(i,allStringsTableOrder[k],itemString);

            QModelIndex stringIndex = modelTable->index(i,allStringsTableOrder[k]);
            if (resultsList[rankList[i].second.first].second[k+7]) modelTable->setData(stringIndex,QIcon(":/checkmarkGreen.png"),Qt::DecorationRole);
            else modelTable->setData(stringIndex,QIcon(":/checkmarkRed.png"),Qt::DecorationRole);
        }
    }


    modelTable->setHeaderData(0,Qt::Horizontal,"Author");
    modelTable->setHeaderData(1,Qt::Horizontal,"Coincidence");
    modelTable->setHeaderData(2,Qt::Horizontal,"Re - Range");
    modelTable->setHeaderData(3,Qt::Horizontal,"");
    modelTable->setHeaderData(4,Qt::Horizontal,"Pr - Range");
    modelTable->setHeaderData(5,Qt::Horizontal,"");
    modelTable->setHeaderData(6,Qt::Horizontal,"Fluid");
    modelTable->setHeaderData(7,Qt::Horizontal,"Section");
    modelTable->setHeaderData(8,Qt::Horizontal,"Diameter");
    modelTable->setHeaderData(9,Qt::Horizontal,"");
    modelTable->setHeaderData(10,Qt::Horizontal,"Channel");
    modelTable->setHeaderData(11,Qt::Horizontal,"Angle");
    modelTable->setHeaderData(12,Qt::Horizontal,"");
    modelTable->setHeaderData(13,Qt::Horizontal,"Border");
    modelTable->setHeaderData(14,Qt::Horizontal,"Length");
    modelTable->setHeaderData(15,Qt::Horizontal,"");
    modelTable->setHeaderData(16,Qt::Horizontal,"Viscosity");
    modelTable->setHeaderData(17,Qt::Horizontal,"");
    modelTable->setHeaderData(18,Qt::Horizontal,"Temperature");
    modelTable->setHeaderData(19,Qt::Horizontal,"");
    ui->tableView->setModel(modelTable);    // Set model for tableView
    ui->tableView->horizontalHeader()->show();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->resizeRowsToContents();

    alreadySearched = true;
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
        QString fluid;
        QString section;
        QString channel;
        QString border;
        QString reference;
        QString notes;
        QList<bool> allVars;
        QList<QVector<double>> allRanges;

        // Iterate all correlation list to add again except the excluded one
        for (int i = 0; i < corList.size(); i++){
            k = &corList[i]; // to search just one time
            expr = k->getExpr();
            author = k->getAuthor();
            reference = k->getReference();
            notes = k->getNotes();
            allVars = k->getAllVars();
            allRanges = k->getAllRanges();

            fluid.clear();
            for (int j = 0; j < k->getFluid().size(); j++){
                fluid += k->getFluid()[j];
                if (j != k->getFluid().size() - 1) fluid += "/";
            }
            section.clear();
            for (int j = 0; j < k->getSection().size(); j++){
                section += k->getSection()[j];
                if (j != k->getSection().size() - 1) section += "/";
            }
            channel.clear();
            for (int j = 0; j < k->getChannel().size(); j++){
                channel += k->getChannel()[j];
                if (j != k->getChannel().size() - 1) channel += "/";
            }
            border.clear();
            for (int j = 0; j < k->getBorder().size(); j++){
                border += k->getBorder()[j];
                if (j != k->getBorder().size() - 1) border += "/";
            }

            out << expr << ";"
                << author << ";"
                << allRanges[0][0] << "/" << allRanges[0][1] << "/" << allVars[0] << ";"
                << allRanges[1][0] << "/" << allRanges[1][1] << "/" << allVars[1] << ";"
                << fluid << ";"
                << section << ";"
                << allRanges[2][0] << "/" << allRanges[2][1] << "/" << allVars[2] << ";"
                << channel << ";"
                << allRanges[3][0] << "/" << allRanges[3][1] << "/" << allVars[3] << ";"
                << border << ";"
                << allRanges[4][0] << "/" << allRanges[4][1] << "/" << allVars[4] << ";"
                << allRanges[5][0] << "/" << allRanges[5][1] << "/" << allVars[5] << ";"
                << allRanges[6][0] << "/" << allRanges[6][1] << "/" << allVars[6] << ";"
                << reference << ";"
                << notes << ";\n";
        }
        file.close();
        // If there was already a research, it's refreshed
        if (alreadySearched) on_searchButton_clicked();
        else showCorrelations();
    }
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
    int reMin = corList[ind].getReRange()[0];
    int reMax = corList[ind].getReRange()[1];       // ADD VERIFICATION
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

void MainWindow::on_reRangeButton_clicked()
{
    if (ui->reRangeButton->isChecked()){
        ui->reRangeButton->setText("[*,*]");
        ui->reMaxBox->hide();
        ui->reMinBox->setSpecialValueText("");
    }
    else{
        ui->reRangeButton->setText("[*]");
        ui->reMaxBox->show();
        ui->reMinBox->setSpecialValueText("min");
    }
}

void MainWindow::on_prRangeButton_clicked()
{
    if (ui->prRangeButton->isChecked()){
        ui->prRangeButton->setText("[*,*]");
        ui->prMaxBox->hide();
        ui->prMinBox->setSpecialValueText("");
    }
    else{
        ui->prRangeButton->setText("[*]");
        ui->prMaxBox->show();
        ui->prMinBox->setSpecialValueText("min");
    }
}

void MainWindow::on_dRangeButton_clicked()
{
    if (ui->dRangeButton->isChecked()){
        ui->dRangeButton->setText("[*,*]");
        ui->diamMaxBox->hide();
        ui->diamMinBox->setSpecialValueText("");
    }
    else{
        ui->dRangeButton->setText("[*]");
        ui->diamMaxBox->show();
        ui->diamMinBox->setSpecialValueText("min");
    }
}

void MainWindow::on_aRangeButton_clicked()
{
    if (ui->aRangeButton->isChecked()){
        ui->aRangeButton->setText("[*,*]");
        ui->angleMaxBox->hide();
        ui->angleBox->setSpecialValueText("");
    }
    else{
        ui->aRangeButton->setText("[*]");
        ui->angleMaxBox->show();
        ui->angleBox->setSpecialValueText("min");
    }
}

void MainWindow::on_lRangeButton_clicked()
{
    if (ui->lRangeButton->isChecked()){
        ui->lRangeButton->setText("[*,*]");
        ui->lMaxBox->hide();
        ui->lMinBox->setSpecialValueText("");
    }
    else{
        ui->lRangeButton->setText("[*]");
        ui->lMaxBox->show();
        ui->lMinBox->setSpecialValueText("min");
    }
}

void MainWindow::on_viscRangeButton_clicked()
{
    if (ui->viscRangeButton->isChecked()){
        ui->viscRangeButton->setText("[*,*]");
        ui->viscMaxBox->hide();
        ui->viscMinBox->setSpecialValueText("");
    }
    else{
        ui->viscRangeButton->setText("[*]");
        ui->viscMaxBox->show();
        ui->viscMinBox->setSpecialValueText("min");
    }
}

void MainWindow::on_tempRangeButton_clicked()
{
    if (ui->tempRangeButton->isChecked()){
        ui->tempRangeButton->setText("[*,*]");
        ui->tempMaxBox->hide();
        ui->tempMinBox->setSpecialValueText("");
    }
    else{
        ui->tempRangeButton->setText("[*]");
        ui->tempMaxBox->show();
        ui->tempMinBox->setSpecialValueText("min");
    }
}


// ============ Results methods =========== //


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


// ============ Experimental Analysis methods ======= //


void MainWindow::calculateResults()
{
    // Function that calculates the parameters and stores them

    int indTi = importedData[0].indexOf(ui->tiBox->value());
    int indTf = importedData[0].indexOf(ui->tfBox->value());
    double mw = ui->massFlowBox->value();

    if ( indTi == -1 || indTi > indTf ){
        indTi = 0;
        ui->tiBox->setValue(int(importedData[0][0]));
    }
    if ( indTf == -1 ){
        indTf = importedData[0].size() - 1;
        ui->tfBox->setValue(int(importedData[0][importedData[0].size() - 1]));
    }

    resultsMatrix.clear();

    // Means
    QVector<double> meansVector;

    // LMTD / Pressure Drop / Air Velocity
    QVector<double> logDiffT, pressureDrop, uAir, reAir, qW, qAir, qLosses, globalU;
    double tOutA, tInA, tOutW, tInW, lmtd, pOutA, pInA, uA, muA, cpA, mAir;
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
        if (int(muA) != 0) reAir.push_back( ((((pOutA + pInA)/2)*1e+05)/(287.058 * (((tOutA + tInA)/2) + 273.15)))*(uA/muA)*modelParameters["dH"]);
        else reAir.push_back(0);

        // qW
        // P for cpW = 1
        qW.push_back(mw*1e-3 * interpolate((tOutW + tInW)/2, 1, cpW)*(tOutW - tInW));

        // qAir
        cpA = interpolate(((tOutA + tInA)/2)+273.15, (pOutA + pInA)/2, cpAir);
        mAir = pOutA*1e+5*importedData[7][i] / (287.058*(tOutA + 273.15)*3600); // Mass flow rate of air
        if (int(cpA) != 0) qAir.push_back(mAir*cpA*(tInA - tOutA));
        else qAir.push_back(0);

        // heat losses
        qLosses.push_back(mAir*cpA*(tInA - tOutA) - mw*1e-3 * interpolate((tOutW + tInW)/2, 1, cpW)*(tOutW - tInW));

        // U global heat transfer coefficient
        globalU.push_back(qW[i-indTi]/(modelParameters["heatTransferArea"]*(int(lmtd) == 0 ? 1 : lmtd)));
    }


    meansVector.push_back(mean(0,logDiffT.size(),logDiffT)); // [0]
    meansVector.push_back(mean(0,pressureDrop.size(),pressureDrop)); // [1]
    meansVector.push_back(mean(0,uAir.size(),uAir)); // [2]
    meansVector.push_back(mean(0,reAir.size(),reAir)); // [3]
    meansVector.push_back(mean(0, qW.size(), qW)); // [4]
    meansVector.push_back(mean(0, qAir.size(), qAir)); // [5]
    meansVector.push_back(mean(0, qLosses.size(), qLosses)); // [6]
    meansVector.push_back(mean(0, globalU.size(), globalU)); // [7]
    resultsMatrix.push_back(logDiffT); // [0]
    resultsMatrix.push_back(pressureDrop); // [1]
    resultsMatrix.push_back(uAir); // [2]
    resultsMatrix.push_back(reAir); // [3]
    resultsMatrix.push_back(qW);
    resultsMatrix.push_back(qAir);
    resultsMatrix.push_back(qLosses);
    resultsMatrix.push_back(globalU);

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
    QVector<QString> optionsForTable3;
    optionsForTable3 = {"LMTD", "Pressure Drop", "Air Velocity", "Reynolds Nb (Air)",
                                         "Heat transfered to water", "Heat lost from Air", "Lost heat",
                                        "Global heat transfer coefficient"};
    auto plotModelTable3 = new QStandardItemModel();

    plot3CheckBoxes.clear();
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
        plot3CheckBoxes.push_back(itemCheckBox); // Copy the adress for later use
        QStandardItem *itemMean = new QStandardItem(QString("%0").arg(QString::number(meansVector[i])));
        plotModelTable3->setItem(i,1,itemMean);

    }

    connect(plotModelTable3, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(cellBoxChanged(QStandardItem*)));
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

void MainWindow::cellBoxChanged(QStandardItem* cell)
{
    //! Function that let the user only check boxes that have the same unit for comparison
    //! Because if not, there'll be to many different axis to plot
    /* Just to remember:
     * [0] LMTD
     * [1] Pressure Drop
     * [2] Air velocity
     * [3] Reynolds nb (Air)
     * [4] Heat transfered to water
     * [5] Heat lost from air
     * [6] Heat losses
     * [7] U
     * So different axis are:
     * [0] °C
     * [1] bar
     * [2] m/s
     * [3] --
     * [4][5][6] J
     * [7] W/(m2*K)
     * */

    /* CHANGE FUNCTION TO WORK LATER
    qDebug() << "Cell box has be changed by user";
    QModelIndex indResults;
    qDebug() << "beginning: " << ui->plotTable3->model()->signalsBlocked();
    ui->plotTable3->model()->blockSignals(true);
    int cellInd = cell->index().row();
    qDebug() << "mid: " << ui->plotTable3->model()->signalsBlocked();

    for (int i = 0; i < ui->plotTable3->model()->rowCount(); i++){
        indResults = ui->plotTable3->model()->index(i,0,QModelIndex());

        qDebug() << " cellrow: " << cellInd <<
                    " i: " << i <<
                    " Check state: " << (indResults.data(Qt::CheckStateRole) == Qt::Checked ? "Checked" : "Unchecked");

        if ((cellInd == 4 || cellInd == 5 || cellInd == 6)
                && (i == 4 || i == 5 || i == 6)){
            qDebug() << "1st case";
            if (indResults.data(Qt::CheckStateRole) == Qt::Checked) plot3CheckBoxes[i]->setCheckState(Qt::Checked);
            else plot3CheckBoxes[i]->setCheckState(Qt::Unchecked);
        }
        else if (cellInd == i){ plot3CheckBoxes[i]->setCheckState(Qt::Checked); qDebug() << "2nd case";}
        else {plot3CheckBoxes[i]->setCheckState(Qt::Unchecked); qDebug() << "3rd case";}
        qDebug() << plot3CheckBoxes[i]->checkState();

    }


    ui->plotTable3->model()->blockSignals(false);
    qDebug() << "end: " << ui->plotTable3->model()->signalsBlocked();
    */

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
        //int indTf = importedData[0].indexOf(ui->tfBox->value());

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
        ui->plotResults2->yAxis->setLabel("ºC"); // Change that to only make possible to select data that have the same unit

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


// =========== Other functions ==================== //


double MainWindow::mean(int begin, int end, QVector<double> vector)
{
    double mean = 0.00;
    for (int i = begin; i < end; i++){
        mean += vector[i];
    }
    mean /= (end - begin);
    return mean;
}

double MainWindow::interpolate(double T, double P, QMap<int, QVector<QPair<double, double> > > prop)
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
}

