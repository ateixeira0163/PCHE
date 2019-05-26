#ifndef CORRELATIONDETAILS_H
#define CORRELATIONDETAILS_H

#include <QDialog>

namespace Ui {
class CorrelationDetails;
}

class CorrelationDetails : public QDialog
{
    Q_OBJECT

public:
    explicit CorrelationDetails(QWidget *parent = nullptr);
    ~CorrelationDetails();

private:
    Ui::CorrelationDetails *ui;
    //QString expr;
};

#endif // CORRELATIONDETAILS_H
