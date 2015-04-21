#ifndef MYDOUBLESPINBOX_H
#define MYDOUBLESPINBOX_H

#include <QDoubleSpinBox>

class myDoubleSpinBox : public QDoubleSpinBox
{
public:
    myDoubleSpinBox(QWidget* parent = NULL);
    ~myDoubleSpinBox();
};

#endif // MYDOUBLESPINBOX_H
