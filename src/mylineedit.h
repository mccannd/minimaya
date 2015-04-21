#pragma once

#include <QLineEdit>

class myLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    myLineEdit(QWidget* parent = NULL);
    ~myLineEdit();
public slots:
    void slot_setValue(double);
};

