#pragma once

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <scene/joint.h>

class myTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    myTreeWidget(QWidget* parent = NULL);
    ~myTreeWidget();
public slots:
    void slot_receiveItemTop(QTreeWidgetItem *);
};

