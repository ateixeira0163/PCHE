#ifndef CORRELATION_H
#define CORRELATION_H
#include <QString>
#include <QVector>
#include <QDebug>

class Correlation
{
public:
    // Inputs to the constructor - add more later
    Correlation(QString inExpr = nullptr, QString inAuthor = nullptr,
                QVector<int> inNuRange = {NULL,NULL}, QVector<double> inPrRange = {NULL,NULL},
                QString inFluid = nullptr, QString inSection = nullptr, double inAngle = NULL,
                QString inBorder = nullptr);

    void setExpression(QString expr);           // Functions to modify the interior of the class
    void setNuRange(QVector<int> nuRange);
    void setPrRange(QVector<double> prRange);

    QString getExpression();                    // To get the protected values
    QVector<int> getNuRange();
    QVector<double> getPrRange();
    QString getAuthor();

    int compare(QVector<int> aNuRange, QVector<double> aPrRange, QString aFluid,
                 QString aSection, double aAngle, QString aBorder);

protected:
    QString expr, author, fluid, section, border;                               // Values of the class
    QVector<int> nuRange;
    QVector<double> prRange;
    double angle;

};

#endif // CORRELATION_H
