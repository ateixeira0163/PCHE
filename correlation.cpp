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

QVector<int> Correlation::getNuRange()
{
    return nuRange;
}

QVector<double> Correlation::getPrRange()
{
    return prRange;
}

QString Correlation::getFluid()
{
    return fluid;
}

QString Correlation::getSection()
{
    return section;
}

double Correlation::getAngle()
{
    return angle;
}

QString Correlation::getBorder()
{
    return border;
}

QPair<int, QList<bool>> Correlation::compare(QVector<int> cNuRange, QVector<double> cPrRange,QString cFluid,
                          QString cSection, double cAngle, QString cBorder)
{
    // Return some kind of points in order to classify
    int score = 0;
    QList<bool> results;

    // Comparing Re range
    if ((cNuRange[0] == 0 && cNuRange[1] == 0) || (nuRange[0] == NULL && nuRange[1] == NULL)) {
        //qDebug() << "Could not compare, since there's no value for Re domain";
        results.append(false);
    }
    else if (cNuRange[0] >= nuRange[0] && cNuRange[1] <= nuRange[1]){
        //qDebug() << "It's inside Reynolds range";
        score += 2;
        results.append(true);
    }
    else if (cNuRange[0] >= nuRange[0]){
        score += 1;
        results.append(false);
    }
    else if (cNuRange[1] <= nuRange[1]){
        score += 1;
        results.append(false);
    }
    else {
        //qDebug() << "It's outside Reynolds range";
        results.append(false);
    }

    // Comparing Pr range
    if ((cPrRange[0] < 0.0001 && cPrRange[1] < 0.0001) || (prRange[0] == NULL && prRange[1] == NULL)){
        //qDebug() << "Could not compare, since there's no value for Pr domain";
        results.append(false);
    }
    else if (cPrRange[0] >= prRange[0] && cPrRange[1] <= prRange[1]){
        //qDebug() << "It's inside Prandtl range";
        score += 2;
        results.append(true);
    }
    else if (cPrRange[0] >= prRange[0]){
        score += 1;
        results.append(false);
    }
    else if (cPrRange[1] <= prRange[1]){
        score += 1;
        results.append(false);
    }
    else {
        //qDebug() << "It's outside Prandtl range";
        results.append(false);
    }

    // Comparing Fluid
    if (cFluid == "" || fluid == nullptr){
        //qDebug() << "Could not compare, since there's no value for Fluid";
        results.append(false);
    }
    else if (cFluid == fluid){
        //qDebug() << "The fluid choosen matches the case";
        score += 2;
        results.append(true);
    }
    else {
        //qDebug() << "The fluid does not match with the case";
        results.append(false);
    }

    // Comparing the section
    if (cSection == "" || section == nullptr){
        //qDebug() << "Could not compare, since there's no value for Section";
        results.append(false);
    }
    else if (cSection == section){
        //qDebug() << "The section choosen matches the case";
        score += 2;
        results.append(true);
    }
    else {
        //qDebug() << "The section does not matches the case";
        results.append(false);
    }

    // Comparing angle
    if (fabs(cAngle) < 0.0001 || angle == NULL){
        //qDebug() << "Could not compare, since there's no value for Angle";
        results.append(false);
    }
    else if (fabs(angle - cAngle) < 0.0001){
        //qDebug() << "The angle choosen matches the case";
        score += 2;
        results.append(true);
    }
    else {
        //qDebug() << "The angle does not matches the case";
        results.append(false);
    }

    // Comparing border type
    if (cBorder == "" || border == nullptr){
        //qDebug() << "Could not compare, since there's no value for Border type";
        results.append(false);
    }
    else if (cBorder == border){
        //qDebug() << "The border type choosen matches the case";
        score += 2;
        results.append(true);
    }
    else {
        //qDebug() << "The border does not matches the case";
        results.append(false);
    }

    QPair<int,QList<bool>> finalResults = qMakePair(score,results);

    return finalResults;
}






















