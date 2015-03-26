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

void MainWindow::refreshLists()
{
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
