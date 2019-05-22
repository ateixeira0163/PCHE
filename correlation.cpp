#include "correlation.h"

Correlation::Correlation(QString inExpr, QString inAuthor, QVector<int> inNuRange, QVector<double> inPrRange, QString inFluid, QString inSection, double inAngle, QString inBorder)
{
    // Set the values

    // Compare input values to set "" to NULL
    author = inAuthor;
    expr = inExpr;
    nuRange = inNuRange;
    prRange = inPrRange;
    fluid = inFluid;
    section = inSection;
    border = inBorder;
    angle = inAngle;
}

void Correlation::setExpression(QString inExpr)
{
    expr = inExpr;
}

void Correlation::setNuRange(QVector<int> inNuRange)
{
    nuRange = inNuRange;
}

void Correlation::setPrRange(QVector<double> inPrRange)
{
    prRange = inPrRange;
}

QString Correlation::getExpression()
{
    return expr;
}

QVector<int> Correlation::getNuRange()
{
    return nuRange;
}

QVector<double> Correlation::getPrRange()
{
    return prRange;
}

QString Correlation::getAuthor()
{
    return author;
}

int Correlation::compare(QVector<int> aNuRange, QVector<double> aPrRange,QString aFluid,
                          QString aSection, double aAngle, QString aBorder)
{
    // Return some kind of points in order to classify
    int score = 0;

    // Comparing Re range
    if (aNuRange[0] == NULL || nuRange[0] == NULL) {
        qDebug() << "Could not compare, since there's no value for Re domain";
    }
    else if (aNuRange[0] >= nuRange[0] && aNuRange[1] <= nuRange[1]){
        qDebug() << "It's inside Reynolds range";
        score += 1;
    }
    else {
        qDebug() << "It's outside Reynolds range";
    }

    // Comparing Pr range
    if (aPrRange[0] == NULL || prRange[0] == NULL){
        qDebug() << "Could not compare, since there's no value for Pr domain";
    }

    else if (aPrRange[0] >= prRange[0] && aPrRange[1] <= prRange[1]){
        qDebug() << "It's inside Prandtl range";
        score += 1;
    }
    else {
        qDebug() << "It's outside Prandtl range";
    }

    // Comparing Fluid
    if (aFluid == nullptr){
        qDebug() << "Could not compare, since there's no value for Fluid";
    }
    else if (aFluid == fluid){
        qDebug() << "The fluid choosen matches the case";
        score += 1;
    }
    else {
        qDebug() << "The fluid does not match with the case";
    }

    // Comparing the section
    if (aSection == nullptr){
        qDebug() << "Could not compare, since there's no value for Section";
    }
    else if (aSection == section){
        qDebug() << "The section choosen matches the case";
        score += 1;
    }
    else {
        qDebug() << "The section does not matches the case";
    }

    // Comparing angle
    if (aAngle == NULL){
        qDebug() << "Could not compare, since there's no value for Angle";
    }
    else if (std::abs(angle - aAngle) < 0.001){
        qDebug() << "The angle choosen matches the case";
        score += 1;
    }
    else {
        qDebug() << "The angle does not matches the case";
    }

    // Comparing border type
    if (aBorder == nullptr){
        qDebug() << "Could not compare, since there's no value for Border type";
    }
    else if (aBorder == border){
        qDebug() << "The border type choosen matches the case";
        score += 1;
    }
    else {
        qDebug() << "The border does not matches the case";
    }


    return score;
}






















