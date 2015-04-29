#pragma once

#include <QMainWindow>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include "scene/keyframeselectable.h"


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

    void on_pushButton_2_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_15_clicked();


    void on_joints_tree_itemClicked(QTreeWidgetItem *item, int column);

    void on_pushButton_12_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_11_clicked();

    void on_displaySkeletonBox_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

    void on_keyframe_save_clicked();

    void on_keys_list_itemClicked(QListWidgetItem *item);

    void on_keyframe_play_clicked();

    void on_keyframe_clear_clicked();

private:
    Ui::MainWindow *ui;
    int numAnimframes = 72;

public slots:
    void refreshLists();
    void refreshVertexData(float x, float y, float z);
    void refreshJointData(QTreeWidgetItem* j);
};
