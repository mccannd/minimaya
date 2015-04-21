#include "mytreewidget.h"

myTreeWidget::myTreeWidget(QWidget* parent) : QTreeWidget(parent)
{

}

myTreeWidget::~myTreeWidget()
{

}

void myTreeWidget::slot_receiveItemTop(QTreeWidgetItem * j)
{
    addTopLevelItem(j);
}
