#pragma once

#include <QMainWindow>
#include <QListWidgetItem>


namespace Ui
{
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_faces_list_itemChanged(QListWidgetItem *item);

    void on_edge_list_itemChanged(QListWidgetItem *item);

    void on_pushButton_5_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

    void on_vertices_list_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

public slots:
    void refreshLists();
    void refreshVertexData(float x, float y, float z);
};
