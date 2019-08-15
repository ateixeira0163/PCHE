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

    // =========== Correlations Tab methods ======= //

    void loadCorrelations();
    void on_addNewButton_clicked();
    void showCorrelations();
    void on_searchButton_clicked();
    void correlationCellChanged();
    void on_deleteButton_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_importCorResultsButton_clicked();
    void on_plotButton_clicked();
    void correlationContextMenuRequest(QPoint pos);
    void moveLegendC();
    void hideLegendC();
    void on_reRangeButton_clicked();
    void on_prRangeButton_clicked();
    void on_dRangeButton_clicked();
    void on_aRangeButton_clicked();
    void on_lRangeButton_clicked();
    void on_viscRangeButton_clicked();
    void on_tempRangeButton_clicked();

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

    // Functions

    // Details
    QString whichComboBox;

    // Correlation
    QVector<Correlation> corList;
    QList<QPair<int, QPair<int,QString> > > rankList;
    bool alreadySearched;
    QString importedCorrelation = nullptr;
    QVector<QVector<double>> importedCorrelationData;

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
