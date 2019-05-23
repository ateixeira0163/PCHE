#include "correlation.h"

Correlation::Correlation(QString inExpr, QString inAuthor, QVector<int> inNuRange, QVector<double> inPrRange, QString inFluid, QString inSection, double inAngle, QString inBorder)
{
    // Set the values

    // Compare input values to set "" to NULL
    expr = (inExpr == "" ? nullptr : inExpr);       // If/Else short statement
    author = (inAuthor == "" ? nullptr : inAuthor); // (condition ? if_true : if_false)
    nuRange = ((inNuRange[0] == 0 && inNuRange[1] == 0) ? QVector<int> {NULL,NULL} : inNuRange);
    prRange = ((inPrRange[0] < 0.00001 && inPrRange[1] < 0.00001) ? QVector<double> {NULL,NULL} : inPrRange);
    fluid = (inFluid == "" ? nullptr : inFluid);
    section = (inSection == "" ? nullptr : inSection);
    border = (inBorder == "" ? nullptr : inBorder);
    angle = ((fabs(inAngle) < 0.00001) ? NULL : inAngle);

}

QString Correlation::getAuthor()
{
    return author;
}

int Correlation::compare(QVector<int> cNuRange, QVector<double> cPrRange,QString cFluid,
                          QString cSection, double cAngle, QString cBorder)
{
    // Return some kind of points in order to classify
    int score = 0;

    // Comparing Re range
    if ((cNuRange[0] == 0 && cNuRange[1] == 0) || (nuRange[0] == NULL && nuRange[1] == NULL)) {
        qDebug() << "Could not compare, since there's no value for Re domain";
    }
    else if (cNuRange[0] >= nuRange[0] && cNuRange[1] <= nuRange[1]){
        qDebug() << "It's inside Reynolds range";
        score += 1;
    }
    else {
        qDebug() << "It's outside Reynolds range";
    }

    // Comparing Pr range
    if ((cPrRange[0] < 0.0001 && cPrRange[1] < 0.0001) || (prRange[0] == NULL && prRange[1] == NULL)){
        qDebug() << "Could not compare, since there's no value for Pr domain";
    }

    else if (cPrRange[0] >= prRange[0] && cPrRange[1] <= prRange[1]){
        qDebug() << "It's inside Prandtl range";
        score += 1;
    }
    else {
        qDebug() << "It's outside Prandtl range";
    }

    // Comparing Fluid
    if (cFluid == "" || fluid == nullptr){
        qDebug() << "Could not compare, since there's no value for Fluid";
    }
    else if (cFluid == fluid){
        qDebug() << "The fluid choosen matches the case";
        score += 1;
    }
    else {
        qDebug() << "The fluid does not match with the case";
    }

    // Comparing the section
    if (cSection == "" || section == nullptr){
        qDebug() << "Could not compare, since there's no value for Section";
    }
    else if (cSection == section){
        qDebug() << "The section choosen matches the case";
        score += 1;
    }
    else {
        qDebug() << "The section does not matches the case";
    }

    // Comparing angle
    if (fabs(cAngle) < 0.0001 || angle == NULL){
        qDebug() << "Could not compare, since there's no value for Angle";
    }
    else if (fabs(angle - cAngle) < 0.0001){
        qDebug() << "The angle choosen matches the case";
        score += 1;
    }
    else {
        qDebug() << "The angle does not matches the case";
    }

    // Comparing border type
    if (cBorder == "" || border == nullptr){
        qDebug() << "Could not compare, since there's no value for Border type";
    }
    else if (cBorder == border){
        qDebug() << "The border type choosen matches the case";
        score += 1;
    }
    else {
        qDebug() << "The border does not matches the case";
    }


    return score;
}






















