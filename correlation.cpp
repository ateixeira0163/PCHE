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
    // Set the values

    // Compare input values to set "" to NULL


    expr = inExpr;      // If/Else short statement
    author = inAuthor; // (condition ? if_true : if_false)
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
                                             QStringList cFluid,
                                             QStringList cSection,
                                             QVector<double> cDiam, bool cDVar,
                                             QStringList cChannel,
                                             QVector<double> cAngle, bool cAngVar,
                                             QStringList cBorder,
                                             QVector<double> cLength, bool cLenVar,
                                             QVector<double> cVisc, bool cViscVar,
                                             QVector<double> cTemp, bool tempVar)
{
    // Return some kind of points in order to classify
    int score = 0;
    QList<bool> results;
    double e = 1e-6;

    QList<QVector<double>*> rangesInput = {&cPrRange, &cDiam, &cAngle, &cLength, &cVisc, &cTemp};
    QList<QVector<double>*> ranges = {&prRange, &diamRange, &angleRange, &lengthRange, &viscRange, &tempRange};

    QList<QStringList*> stringListsInput = {&cFluid, &cSection, &cChannel, &cBorder};
    QList<QStringList*> stringLists = {&fluid, &section, &channel, &border};

    QList<bool*> varsInput = {&cPrVar, &cDVar, &cAngVar, &cLenVar, &cViscVar, &tempVar};
    QList<bool*> vars = {&prV, &diamV, &angleV, &lV, &viscV, &tempV};

    // Comparing Ranges
    for (int i = 0; i < ranges.size(); i++) {
        if (*rangesInput[i] == QVector<double> {0,0} || *ranges[i] == QVector<double> {0,0}){
            // Could not compare since there's no value
            results.append(false);
        }
        else if (*rangesInput[i] >= *ranges[0] && *rangesInput[i][0] <= *reRange[1]){
            // STOPPED HERE //////////////////////
        }
    }

    // Comparing Re range   // -- results[0] ==============================
    if ((cReRange[0] == 0 && cReRange[1] == 0) || (reRange[0] == NULL && reRange[1] == NULL)) {
        //qDebug() << "Could not compare, since there's no value for Re domain";
        results.append(false);
    }
    else if (cReRange[0] >= reRange[0] && cReRange[1] <= reRange[1]){
        //qDebug() << "It's inside Reynolds range";
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
        //qDebug() << "It's outside Reynolds range";
        results.append(false);
    }
    if (cReVar && reV){     // -- results[1] ==============================
        score += 2;
        results.append(true);
    }
    else results.append(false);


    // Comparing Pr range // -- results[2] ==============================
    if ((cPrRange[0] < e && cPrRange[1] < e) || (prRange[0] == NULL && prRange[1] == NULL)){
        //qDebug() << "Could not compare, since there's no value for Pr domain";
        results.append(false);
    }
    else if (cPrRange[0] >= prRange[0] && cPrRange[1] <= prRange[1]){
        //qDebug() << "It's inside Prandtl range";
        score += 2;
        results.append(true);
    }
    else if (cPrRange[1] >= prRange[0] && prRange[0] >= cPrRange[0]){
        score += 1;
        results.append(false);
    }
    else if (cPrRange[1] >= prRange[1] && prRange[1] >= cPrRange[0]){
        score += 1;
        results.append(false);
    }
    else {
        //qDebug() << "It's outside Prandtl range";
        results.append(false);
    }
    if (cPrVar && prV){     // -- results[3] ==============================
        score += 2;
        results.append(true);
    }
    else results.append(false);

    // Comparing Fluid    // -- results[4] ==============================

    if (cFluid == "--" || fluid == nullptr){
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

    // Comparing the section  // -- results[5] ==============================

    if (cSection == "--" || section == nullptr){
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

    // Comparing Diameter   // -- results[6] ==============================

    if ((cDiam[0] < e && cDiam[1] < e) || (diamRange[0] == NULL && diamRange[1] == NULL)){
        results.append(false);
    }
    else if (cDiam[0] >= diamRange[0] && cDiam[1] <= diamRange[1]){
        score += 2;
        results.append(true);
    }
    else if (cDiam[1] >= diamRange[0] && diamRange[0] >= cDiam[0]){
        score += 1;
        results.append(false);
    }
    else if (cDiam[1] >= diamRange[1] && diamRange[1] >= cDiam[0]){
        score += 1;
        results.append(false);
    }
    else {
        results.append(false);
    }
    if (cDVar && diamV){     // -- results[7] ==============================
        score += 2;
        results.append(true);
    }
    else results.append(false);

    // Comparing the channel type  // -- results[8] ==============================

    if (cChannel == "--" || channel == nullptr){
        results.append(false);
    }
    else if (cChannel == channel){
        score += 2;
        results.append(true);
    }
    else {
        results.append(false);
    }

    // Comparing angle      // -- results[9] ==============================

    if ((fabs(cAngle[0]) < e && fabs(cAngle[1]) < e) || (angleRange[0] == NULL && angleRange[1] == NULL)){
        //qDebug() << "Could not compare, since there's no value for Angle";
        results.append(false);
    }
    else if (cAngle[0] >= angleRange[0] && cAngle[1] <= angleRange[1]){
        score += 2;
        results.append(true);
    }
    else if (cAngle[1] >= angleRange[0] && angleRange[0] >= cAngle[0]){
        score += 1;
        results.append(false);
    }
    else if (cAngle[1] >= angleRange[1] && angleRange[1] >= cAngle[0]){
        score += 1;
        results.append(false);
    }
    else {
        results.append(false);
    }
    if (cAngVar && angleV){     // -- results[10] ==============================
        score += 2;
        results.append(true);
    }
    else results.append(false);

    // Comparing border type // -- results[11] ==============================

    if (cBorder == "--" || border == nullptr){
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

    // Comparing Length   // -- results[12] ==============================

    if ((cLength[0] < e && cLength[1] < e) || (lengthRange[0] == NULL && lengthRange[1] == NULL)){
        results.append(false);
    }
    else if (cLength[0] >= lengthRange[0] && cLength[1] <= lengthRange[1]){
        score += 2;
        results.append(true);
    }
    else if (cLength[1] >= lengthRange[0] && lengthRange[0] >= cLength[0]){
        score += 1;
        results.append(false);
    }
    else if (cLength[1] >= lengthRange[1] && lengthRange[1] >= cLength[0]){
        score += 1;
        results.append(false);
    }
    else {
        results.append(false);
    }
    if (cLenVar && lV){     // -- results[13] ==============================
        score += 2;
        results.append(true);
    }
    else results.append(false);

    // Comparing Viscosity   // -- results[14] ==============================

    if ((cVisc[0] < e && cVisc[1] < e) || (viscRange[0] == NULL && viscRange[1] == NULL)){
        results.append(false);
    }
    else if (cVisc[0] >= viscRange[0] && cVisc[1] <= viscRange[1]){
        score += 2;
        results.append(true);
    }
    else if (cVisc[1] >= viscRange[0] && viscRange[0] >= cVisc[0]){
        score += 1;
        results.append(false);
    }
    else if (cVisc[1] >= viscRange[1] && viscRange[1] >= cVisc[0]){
        score += 1;
        results.append(false);
    }
    else {
        results.append(false);
    }
    if (cViscVar && viscV){     // -- results[15] ==============================
        score += 2;
        results.append(true);
    }
    else results.append(false);

    // Comparing Temperature   // -- results[16] ==============================

    if ((cTemp[0] < e && cTemp[1] < e) || (tempRange[0] == NULL && tempRange[1] == NULL)){
        results.append(false);
    }
    else if (cTemp[0] >= tempRange[0] && cTemp[1] <= tempRange[1]){
        score += 2;
        results.append(true);
    }
    else if (cTemp[1] >= tempRange[0] && tempRange[0] >= cTemp[0]){
        score += 1;
        results.append(false);
    }
    else if (cTemp[1] >= tempRange[1] && tempRange[1] >= cTemp[0]){
        score += 1;
        results.append(false);
    }
    else {
        results.append(false);
    }
    if (tempVar && tempV){     // -- results[17] ==============================
        score += 2;
        results.append(true);
    }
    else results.append(false);


    QPair<int,QList<bool>> finalResults = qMakePair(score,results);

    return finalResults;
}
