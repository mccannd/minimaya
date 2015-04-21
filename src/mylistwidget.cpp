#include "mylistwidget.h"

myListWidget::myListWidget(QWidget* parent) : QListWidget(parent)
{
}

myListWidget::~myListWidget()
{

}

void myListWidget::slot_ReceiveFace(QListWidgetItem* f) {
    addItem(f);
}

void myListWidget::slot_ReceiveVertex(QListWidgetItem* v) {
    addItem(v);
}

void myListWidget::slot_ReceiveHalfEdge(QListWidgetItem* e) {
    addItem(e);
}
