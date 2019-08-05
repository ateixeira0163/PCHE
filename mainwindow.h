#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "aboutdialog.h"
#include "addnewfluidwindow.h"
#include "addcorrelation.h"
#include "correlation.h"
#include <string>
#include <QActionGroup>
#include <QMessageBox>
#include <QDebug>
#include <vector>
#include <iostream>
#include <fstream>
#include <QString>
#include <QStringList>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QTextStream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <QtScript/QScriptEngine>
#include "qcustomplot.h"
#include <QtMath>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    // ============ Input Tab methods ========== //

    void on_actionAbout_triggered();
    void on_rectangleButton_clicked();
    void on_circularButton_clicked();
    void on_semiCircularButton_clicked();
    void on_radioButtonRect_clicked();
    void on_radioButtonCirc_clicked();
    void on_radioButtonSemiCirc_clicked();
    void on_runButton_clicked();
    void on_comboBoxHotFluid_activated(const QString &arg1);
    void on_comboBoxColdFluid_activated(const QString &arg1);
    void on_comboBoxNu_activated(const QString &arg1);
    void initList();
    void addNewFluid();
    void addNewNusselt();

    // =========== Correlations Tab methods ======= //

    void loadCorrelations();
    void on_addNewButton_clicked();
    void addCorrelations();
    void showCorrelations();
    void on_searchButton_clicked();
    //void on_prInputButton_clicked();
    //void on_reInputButton_clicked();
    void on_deleteButton_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_plotButton_clicked();

    // =========== Results Tab methods ============ //

    void on_importResults_clicked();
    void on_plotResults_clicked();

    // =========== Experimental Analysis methods === //

    void calculateResults();
    void cellBoxChanged(QStandardItem* cell);
    void on_importResultsButton_clicked();
    void on_plotResultsButton_clicked();
    void refreshRange(int minValue, int maxValue);
    void contextMenuRequest(QPoint pos);
    void hideAllErrorBars();
    void hideErrorBar();
    void showConfidenceDetails();
    void moveLegend();
    void hideLegend();
    void on_plotResultsButton2_clicked();
    void on_pushButton_clicked();

    // ========== Other functions ============== //

    double mean(int begin, int end, QVector<double> vector);
    double interpolate(double T, double P, QMap<int, QVector<QPair<double,double>>> prop);

private:
    Ui::MainWindow *ui;
    aboutDialog *aboutDialogW;  // To create a pointer to the object -> aboutDialogWindow

    // Variables

    // Hot Inlet
    QString hotFluid;
    double pHotIn;    // Inlet parameters
    double tHotIn;
    double reHotIn;

    // Hot Outlet
    double pHotOut;

    // Cold Inlet
    QString coldFluid;
    double pColdIn;
    double tColdIn;
    double reColdIn;

    // Cold Out
    double pColdOut;

    // Section
    QString sectionType;
    double dSection;
    double hSection;

    // Information
    std::fstream fluidsLib;
    std::vector<std::string> fluidsName;
    std::vector<std::vector<double>> fluidsParam;

    std::fstream nusseltNb;

    // Functions

    // Details
    QString whichComboBox;

    // Correlation
    QVector<Correlation> corList;
    QList<QPair<int, QPair<int,QString> > > rankList;
    bool prRangeStatement, reRangeStatement, alreadySearched;

    // Plot results
    QString importedFileName = nullptr;
    QString importedFileNameData = nullptr;
    QVector<QVector<double>> resultsMatrix;
    bool initImport = false;
    QStringList headerListPlot;
    QCPItemRect* rangeRect;
    QList<QStandardItem*> plot3CheckBoxes;
    QVector<QCPErrorBars*> errorVector;

    // Data
    QVector<QVector<double>> importedData;

    // Parameters for the model
    QMap<QString, double> modelParameters;
    QMap<int, QVector<QPair<double,double>>> muAir, cpW, cpAir;



};

#endif // MAINWINDOW_H
