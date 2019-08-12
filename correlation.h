#ifndef CORRELATION_H
#define CORRELATION_H
#include <QString>
#include <QVector>
#include <QDebug>

class Correlation
{
public:
    // Inputs to the constructor - add more later
    Correlation(QString inExpr = "--", QString inAuthor = "--",
                QVector<int> inReRange = {0,0}, bool reVar = true,
                QVector<double> inPrRange = {0,0}, bool prVar = false,
                QStringList inFluid = {"--"}, QStringList inSection = {"--"},
                QVector<double> inDiam = {0, 0}, bool diamVar = false,
                QStringList inChannelType = {"--"},
                QVector<double> inAngle = {0, 0}, bool angleVar = false,
                QString inBorder = {"--"},
                QVector<double> inLength = {0, 0}, bool lengthVar = false,
                QVector<double> inVisc = {0, 0}, bool viscVar = false,
                QVector<double> inTemp = {0, 0}, bool tempVar = false,
                QString inReference = "--", QString inNotes = "--");

    QString getExpr();
    QString getAuthor();
    QVector<int> getReRange();
    QVector<double> getPrRange();
    QStringList getFluid();
    QStringList getSection();
    QVector<double> getDiam();
    QStringList getChannel();
    QVector<double> getAngle();
    QVector<double> getLength();
    QVector<double> getVisc();
    QVector<double> getTemp();
    QStringList getBorder();
    QString getReference();
    QString getNotes();


    QPair<int, QList<bool>> compare(QVector<int> cReRange, bool cReVar,
                                    QVector<double> cPrRange, bool cPrVar,
                                    QStringList cFluid,
                                    QStringList cSection,
                                    QVector<double> cDiam, bool cDVar,
                                    QStringList cChannel,
                                    QVector<double> cAngle, bool cAngVar,
                                    QStringList cBorder,
                                    QVector<double> cLength, bool cLenVar,
                                    QVector<double> cVisc, bool cViscVar,
                                    QVector<double> cTemp, bool tempVar);

protected:
    QString expr, author, reference, notes;  // Values of the class
    QStringList fluid, section, channel, border;
    QVector<int> reRange;
    QVector<double> prRange, diamRange, angleRange, lengthRange, viscRange, tempRange;
    bool reV, prV, diamV, angleV, lV, viscV, tempV;


};

#endif // CORRELATION_H
