#include "correlation.h"

Correlation::Correlation(QString inExpr, QString inAuthor,
                         QVector<int> inReRange, bool reVar,
                         QVector<double> inPrRange, bool prVar,
                         QStringList inFluid, QStringList inSection,
                         QVector<double> inDiam, bool diamVar,
                         QStringList inChannelType,
                         QVector<double> inAngle, bool angleVar,
                         QStringList inBorder,
                         QVector<double> inLength, bool lengthVar,
                         QVector<double> inVisc, bool viscVar,
                         QVector<double> inTemp, bool tempVar,
                         QString inReference, QString inNotes)
{
    expr = inExpr;
    author = inAuthor;
    reRange = inReRange;
    reV = reVar;
    prRange = inPrRange;
    prV = prVar;
    fluid = inFluid;
    section = inSection;
    diamRange = inDiam;
    diamV = diamVar;
    channel = inChannelType;
    angleRange = inAngle;
    angleV = angleVar;
    border = inBorder;
    lengthRange = inLength;
    lV = lengthVar;
    viscRange = inVisc;
    viscV = viscVar;
    tempRange = inTemp;
    tempV = tempVar;
    reference = inReference;
    notes = inNotes;

    /*
    expr = (inExpr == "--" ? nullptr : inExpr);       // If/Else short statement
    author = (inAuthor == "--" ? nullptr : inAuthor); // (condition ? if_true : if_false)
    reRange = ((inReRange[0] == 0 && inReRange[1] == 0) ? QVector<int> {NULL,NULL} : inReRange);
    reV = reVar;
    prRange = ((inPrRange[0] < 0.00001 && inPrRange[1] < 0.00001) ? QVector<double> {NULL,NULL} : inPrRange);
    prV = prVar;
    fluid = (inFluid == "--" ? nullptr : inFluid);
    section = (inSection == "--" ? nullptr : inSection);
    diamRange = ((inDiam[0] < 1e-6 && inDiam[1] < 1e-6) ? QVector<double> {NULL,NULL} : inDiam);
    diamV = diamVar;
    channel = (inChannelType == "--" ? nullptr : inChannelType);
    angleRange = ((fabs(inAngle[0]) < 1e-6 && fabs(inAngle[1]) < 1e-6) ? QVector<double> {NULL,NULL} : inAngle);
    angleV = angleVar;
    border = (inBorder == "--" ? nullptr : inBorder);
    lengthRange = ((inLength[0] < 1e-6 && inLength[1] < 1e-6) ? QVector<double> {NULL,NULL} : inLength);
    lV = lengthVar;
    viscRange = ((inVisc[0] < 1e-6 && inVisc[1] < 1e-6) ? QVector<double> {NULL, NULL} : inVisc);
    viscV = viscVar;
    tempRange = ((fabs(inTemp[0]) < 1e-6 && fabs(inTemp[1]) < 1e-6) ? QVector<double> {NULL,NULL} : inTemp);
    tempV = tempVar;
    reference = (inReference == "" ? nullptr : inReference);
    notes = (inNotes == "" ? nullptr : inNotes);*/

}

QString Correlation::getExpr()
{
    return expr;
}

QString Correlation::getAuthor()
{
    return author;
}

QVector<int> Correlation::getReRange()
{
    return reRange;
}

QVector<double> Correlation::getPrRange()
{
    return prRange;
}

QStringList Correlation::getFluid()
{
    return fluid;
}

QStringList Correlation::getSection()
{
    return section;
}

QVector<double> Correlation::getDiam()
{
    return diamRange;
}

QStringList Correlation::getChannel()
{
    return channel;
}

QVector<double> Correlation::getAngle()
{
    return angleRange;
}

QStringList Correlation::getBorder()
{
    return border;
}

QVector<double> Correlation::getLength()
{
    return lengthRange;
}

QVector<double> Correlation::getVisc()
{
    return viscRange;
}

QVector<double> Correlation::getTemp()
{
    return tempRange;
}

QString Correlation::getReference()
{
    return reference;
}

QString Correlation::getNotes()
{
    return notes;
}

QPair<int, QList<bool>> Correlation::compare(QVector<int> cReRange, bool cReVar,
                                             QVector<double> cPrRange, bool cPrVar,
                                             QString cFluid,
                                             QString cSection,
                                             QVector<double> cDiam, bool cDVar,
                                             QString cChannel,
                                             QVector<double> cAngle, bool cAngVar,
                                             QString cBorder,
                                             QVector<double> cLength, bool cLenVar,
                                             QVector<double> cVisc, bool cViscVar,
                                             QVector<double> cTemp, bool tempVar)
{
    // Return some kind of points in order to classify
    int score = 0;
    QList<bool> results;

    QList<QVector<double>*> rangesInput = {&cPrRange, &cDiam, &cAngle, &cLength, &cVisc, &cTemp};
    QList<QVector<double>*> ranges = {&prRange, &diamRange, &angleRange, &lengthRange, &viscRange, &tempRange};

    QList<QString*> stringInput = {&cFluid, &cSection, &cChannel, &cBorder};
    QList<QStringList*> stringLists = {&fluid, &section, &channel, &border};

    QList<bool*> varsInput = {&cReVar, &cPrVar, &cDVar, &cAngVar, &cLenVar, &cViscVar, &tempVar};
    QList<bool*> vars = {&reV, &prV, &diamV, &angleV, &lV, &viscV, &tempV};

    // Comparing Re range   // -- results[0] ==============================

    if ((cReRange == QVector<int> {0,0}) || reRange == QVector<int> {0,0}) {
        results.append(false);
    }
    else if (cReRange[0] >= reRange[0] && cReRange[1] <= reRange[1]){
        score += 2;
        results.append(true);
    }
    else if (cReRange[1] >= reRange[0] && reRange[0] >= cReRange[0]){
        score += 1;
        results.append(false);
    }
    else if (cReRange[1] >= reRange[1] && reRange[1] >= cReRange[0]){
        score += 1;
        results.append(false);
    }
    else {
        results.append(false);
    }

    // Comparing Ranges
    for (int i = 0; i < ranges.size(); i++) {
        if (*rangesInput[i] == QVector<double> {0,0} || *ranges[i] == QVector<double> {0,0}){
            results.append(false);
        }
        else if (rangesInput[i]->data()[0] >= ranges[i]->data()[0] && rangesInput[i]->data()[1] <= ranges[i]->data()[1]){
            score += 2;
            results.append(true);
        }
        else if (rangesInput[i]->data()[1] >= ranges[i]->data()[0] && rangesInput[i]->data()[0] >= ranges[i]->data()[0]){
            score += 1;
            results.append(false);
        }
        else if (rangesInput[i]->data()[1] >= ranges[i]->data()[1] && rangesInput[i]->data()[1] >= ranges[i]->data()[0]){
            score += 1;
            results.append(false);
        }
        else {
            results.append(false);
        }
    }

    // Comparing textes
    for (int i = 0; i < stringLists.size(); i++){
        if (*stringInput[i] == "--" || stringLists[i]->first() == "--"){
            // Could not compare since there's nothing
            results.append(false);
        }
        else{
            for (int j = 0; j < stringLists[i]->size(); j++){
                if (*stringInput[i] == stringLists[i]->at(j)){
                    score += 2;
                    results.append(true);
                }
            }
        }
        if (results.size() == ranges.size() + i + 1){
            results.append(false);
        }
    }

    // Comparing Vars
    for (int i = 0; i < vars.size(); i++){
        if (*vars[i] && *varsInput[i]){
            score += 2;
            results.append(true);
        }
        else results.append(false);
    }

    QPair<int,QList<bool>> finalResults = qMakePair(score,results);

    return finalResults;
}
