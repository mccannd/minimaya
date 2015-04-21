#pragma once

#include <QListWidget>
#include <QListWidgetItem>

class myListWidget : public QListWidget
{
    Q_OBJECT
public:
    myListWidget(QWidget* parent = NULL);
    ~myListWidget();

public slots:
    void slot_ReceiveFace(QListWidgetItem*);
    void slot_ReceiveVertex(QListWidgetItem*);
    void slot_ReceiveHalfEdge(QListWidgetItem*);
signals:
};
