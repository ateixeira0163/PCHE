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
                QVector<int> inNuRange = {NULL,NULL}, bool reVar = true,
                QVector<double> inPrRange = {NULL,NULL}, bool prVar = false,
                QString inFluid = nullptr, QString inSection = nullptr,
                QVector<double> inDiam = {NULL, NULL}, bool diamVar = false,
                QString inChannelType = nullptr,
                QVector<double> inAngle = {NULL, NULL}, bool angleVar = false,
                QString inBorder = nullptr,
                QVector<double> inLength = {NULL, NULL}, bool lengthVar = false,
                QVector<double> inVisc = {NULL, NULL}, bool viscVar = false,
                QVector<double> inTemp = {NULL, NULL}, bool tempVar = false,
                QString inReference = nullptr, QString inNotes = nullptr);

    QString getExpr();
    QString getAuthor();
    QVector<int> getNuRange();
    QVector<double> getPrRange();
    QString getFluid();
    QString getSection();
    QVector<double> getDiam();
    QString getChannel();
    QVector<double> getAngle();
    QVector<double> getLength();
    QVector<double> getVisc();
    QVector<double> getTemp();
    QString getBorder();
    QString getReference();
    QString getNotes();


    QPair<int, QList<bool>> compare(QVector<int> cNuRange, bool cReVar,
                                    QVector<double> cPrRange, bool cPrVar,
                                    QString cFluid,
                                    QString cSection,
                                    QVector<double> cDiam, bool cDVar,
                                    QString cChannel,
                                    QVector<double> cAngle, bool cAngVar,
                                    QString cBorder,
                                    QVector<double> cLength, bool cLenVar,
                                    QVector<double> cVisc, bool cViscVar,
                                    QVector<double> cTemp, bool tempVar);

protected:
    QString expr, author, fluid, section, channel, border, reference, notes;                               // Values of the class
    QVector<int> nuRange;
    QVector<double> prRange, diamRange, angleRange, lengthRange, viscRange, tempRange;
    bool reV, prV, diamV, angleV, lV, viscV, tempV;


};

#endif // CORRELATION_H
