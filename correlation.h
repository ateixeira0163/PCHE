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
                QString inBorder = nullptr, QString inReference = nullptr, QString inNotes = nullptr);

    QString getExpr();
    QString getAuthor();
    QVector<int> getNuRange();
    QVector<double> getPrRange();
    QString getFluid();
    QString getSection();
    double getAngle();
    QString getBorder();
    QString getReference();
    QString getNotes();


    QPair<int, QList<bool>> compare(QVector<int> cNuRange, QVector<double> cPrRange, QString cFluid,
                 QString cSection, double cAngle, QString cBorder);

protected:
    QString expr, author, fluid, section, border, reference, notes;                               // Values of the class
    QVector<int> nuRange;
    QVector<double> prRange;
    double angle;

};

#endif // CORRELATION_H
