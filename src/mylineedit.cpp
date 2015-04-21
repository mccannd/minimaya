#include "mylineedit.h"

myLineEdit::myLineEdit(QWidget* parent) : QLineEdit(parent)
{
}

myLineEdit::~myLineEdit()
{

}


void myLineEdit::slot_setValue(double d) {
    setText(QString::number(d));
}
