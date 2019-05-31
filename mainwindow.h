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
    void on_searchButton_clicked();
    void on_addNewButton_clicked();

    void initList();
    void addNewFluid();
    void addNewNusselt();
    void loadCorrelations();
    void addCorrelations();
    void showCorrelations();


    void on_tableView_doubleClicked(const QModelIndex &index);


    void on_prInputButton_clicked();

    void on_reInputButton_clicked();

    void on_deleteButton_clicked();



    void on_plotButton_clicked();

    void on_importResults_clicked();

    void on_plotResults_clicked();

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


};

#endif // MAINWINDOW_H
