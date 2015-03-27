#include "mainwindow.h"
#include <ui_mainwindow.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::refreshVertexData(float x, float y, float z)
{
    ui->vertexX->setValue(x);
    ui->vertexY->setValue(y);
    ui->vertexZ->setValue(z);
}

void MainWindow::refreshLists()
{
    while(ui->edge_list->count()>0)
    {
      ui->edge_list->takeItem(0);
    }
    while(ui->vertices_list->count()>0)
    {
      ui->vertices_list->takeItem(0);
    }
    while(ui->faces_list->count()>0)
    {
      ui->faces_list->takeItem(0);
    }

    for (unsigned int i = 0; i < ui->mygl->getMesh()->edges.size(); i++) {
        if (ui->mygl->getMesh()->edges[i] != NULL) {
            ui->edge_list->addItem(ui->mygl->getMesh()->edges[i]);
        }
    }
    for (unsigned int i = 0; i < ui->mygl->getMesh()->faces.size(); i++) {
        if (ui->mygl->getMesh()->faces[i] != NULL) {
            ui->faces_list->addItem(ui->mygl->getMesh()->faces[i]);
        }
    }
    for (unsigned int i = 0; i < ui->mygl->getMesh()->vertices.size(); i++) {
        if (ui->mygl->getMesh()->vertices[i] != NULL) {
            ui->vertices_list->addItem(ui->mygl->getMesh()->vertices[i]);
        }
    }
}

void MainWindow::on_faces_list_itemChanged(QListWidgetItem *item)
{
    ui->mygl->faceSelected(item);
}

void MainWindow::on_edge_list_itemChanged(QListWidgetItem *item)
{
    ui->mygl->edgeSelected(item);
}

void MainWindow::on_pushButton_5_clicked()
{
    float r = (float) ui->faceRed->value() / 100.0;
    float g = (float) ui->faceGreen->value() / 100.0;
    float b = (float) ui->faceBlue->value() / 100.0;

    ui->mygl->recolorFace(r, g, b);
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->mygl->moveVertex(ui->vertexX->value(),
                         ui->vertexY->value(),
                         ui->vertexZ->value());
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->mygl->triangulateFace();
}

void MainWindow::on_pushButton_clicked()
{
    ui->mygl->divideEdge();
}



void MainWindow::on_vertices_list_itemClicked(QListWidgetItem *item)
{
    ui->mygl->vertexSelected(item);
}
