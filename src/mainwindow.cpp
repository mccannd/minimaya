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


    ui->joints_tree->addTopLevelItem(ui->mygl->root_joint);
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
    ui->mygl->skeleton_bound = false;
    ui->mygl->divideEdge();
}



void MainWindow::on_vertices_list_itemClicked(QListWidgetItem *item)
{
    ui->mygl->vertexSelected(item);
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->mygl->skeleton_bound = false;
    ui->mygl->deleteVertex();
}

void MainWindow::on_pushButton_6_clicked()
{
    ui->mygl->resetMesh();
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->mygl->subdivideMesh(ui->edge_list, ui->faces_list, ui->vertices_list);
}

void MainWindow::on_pushButton_9_clicked()
{
    ui->mygl->selectNextEdge(ui->edge_list);
}

void MainWindow::on_pushButton_8_clicked()
{
    ui->mygl->selectSymEdge(ui->edge_list);
}

void MainWindow::on_pushButton_10_clicked()
{
    ui->mygl->skeleton_bound = false;
    ui->mygl->importOBJ();
}

// rotate joint by quaternion angles
void MainWindow::on_pushButton_14_clicked()
{
    if (ui->quat_x->value() == 0 &&
            ui->quat_y->value() == 0 &&
            ui->quat_z->value() == 0) {
        return;
    }

    glm::vec3 axis = glm::vec3(ui->quat_x->value(),
                               ui->quat_y->value(),
                               ui->quat_z->value());
    axis = glm::normalize(axis);

    ui->mygl->selected_joint->rotate(ui->quat_rotation->value() * DEG2RAD,
                                     axis[0], axis[1], axis[2]);
    ui->mygl->selected_joint->create();

    refreshJointData(ui->mygl->selected_joint);
    ui->mygl->updateSkeletonTransformations();

}

// rotate joint by euler angles
void MainWindow::on_pushButton_13_clicked()
{
    if (ui->mygl->selected_joint == NULL) { return; }
    ui->mygl->selected_joint->rotate(ui->euler_x->value() * DEG2RAD,
                                     ui->euler_y->value() * DEG2RAD,
                                     ui->euler_z->value() * DEG2RAD);
    ui->mygl->selected_joint->create();

    refreshJointData(ui->mygl->selected_joint);
    ui->mygl->updateSkeletonTransformations();
}

// moving the selected joint
void MainWindow::on_pushButton_15_clicked()
{
    if (ui->mygl->selected_joint == NULL) { return; }
    ui->mygl->selected_joint->move(ui->joint_x->value(),
                                   ui->joint_y->value(),
                                   ui->joint_z->value());
    ui->mygl->selected_joint->create();
    ui->mygl->updateSkeletonTransformations();
}

void MainWindow::refreshJointData(QTreeWidgetItem *j)
{
    Joint* sJoint = (Joint*) j;
    // refresh the quaternion angle information
    ui->quat_rotation->setValue(glm::angle(sJoint->rotation) * RAD2DEG);
    glm::vec3 axis = glm::axis(sJoint->rotation);
    ui->quat_x->setValue(axis[0]);
    ui->quat_y->setValue(axis[1]);
    ui->quat_z->setValue(axis[2]);

    // refresh the euler angle information
    glm::vec3 euler = glm::eulerAngles(sJoint->rotation);
    ui->euler_x->setValue(euler[0] * RAD2DEG);
    ui->euler_y->setValue(euler[1] * RAD2DEG);
    ui->euler_z->setValue(euler[2] * RAD2DEG);

    // refresh the position
    ui->joint_x->setValue(sJoint->position[0]);
    ui->joint_y->setValue(sJoint->position[1]);
    ui->joint_z->setValue(sJoint->position[2]);

    ui->joint_name->setText(sJoint->name);
}

// make mygl select the item
void MainWindow::on_joints_tree_itemClicked(QTreeWidgetItem *item, int column)
{
    ui->mygl->selectJoint(item);
}

// add a joint to the currently selected joint
void MainWindow::on_pushButton_12_clicked()
{
    if (ui->mygl->selected_joint == NULL) { return; }
    ui->mygl->addJoint();
    ui->mygl->skeleton_bound = false;
}

void MainWindow::on_pushButton_16_clicked()
{
    ui->mygl->selected_joint->rename(ui->joint_name->toPlainText());
}

void MainWindow::on_pushButton_11_clicked()
{
    ui->mygl->skeleton_bound = false;
    ui->mygl->importJSON();
    ui->joints_tree->addTopLevelItem(ui->mygl->root_joint);
}

// toggle seleton visibility
void MainWindow::on_displaySkeletonBox_clicked()
{
    ui->mygl->skeleton_visible = !ui->mygl->skeleton_visible;
}

void MainWindow::on_pushButton_17_clicked()
{
    ui->mygl->skeleton_bound = false;
    ui->mygl->resetSkeleton();
    ui->joints_tree->addTopLevelItem(ui->mygl->root_joint);
}

void MainWindow::on_pushButton_18_clicked()
{

    ui->mygl->bindSkeleton();
    ui->mygl->updateSkeletonList();
    ui->mygl->updateSkeletonTransformations();
    ui->mygl->skeleton_bound = true;

    // create an empty timeline
    ui->keys_list->clear();
    for (int i = 0; i < numAnimframes; i++) {

        KeyframeSelectable* kfs = new KeyframeSelectable(i);
        kfs->setText(QString("%1").arg(i));
        ui->keys_list->addItem(kfs);
    }
}

void MainWindow::on_keyframe_save_clicked()
{
    if (ui->mygl->skeleton_bound) {
        int index = ui->keys_list->currentRow();
        ui->mygl->root_joint->keyframeSnapshot(index);
        ui->keys_list->currentItem()->setText(
                    QString("%1 (Keyframe)").arg(index));
        ui->mygl->root_joint->createAllFrames(numAnimframes);
    }
}

void MainWindow::on_keys_list_itemClicked(QListWidgetItem *item)
{
    KeyframeSelectable* kfs = (KeyframeSelectable*) item;
    unsigned int index = kfs->keyID;
    ui->mygl->root_joint->applyKeyframe(index);
    // make the window respond to the new transformations
    ui->mygl->updateSkeletonTransformations();
}

void MainWindow::on_keyframe_play_clicked()
{
    if (ui->mygl->skeleton_bound) {
        ui->mygl->playAnimation(ui->keys_list);
    }
}

void MainWindow::on_keyframe_clear_clicked()
{
    if (ui->mygl->skeleton_bound) {
        ui->mygl->root_joint->clearKeyframes();
        // create an empty timeline
        ui->keys_list->clear();
        for (int i = 0; i < numAnimframes; i++) {

            KeyframeSelectable* kfs = new KeyframeSelectable(i);
            kfs->setText(QString("%1").arg(i));
            ui->keys_list->addItem(kfs);
        }
    }
}

void MainWindow::on_vertex_normals_checkbox_clicked()
{
    ui->mygl->smoothNormals();
}

void MainWindow::on_light_x_valueChanged(double arg1)
{
    ui->mygl->updateLightPosition((float) arg1,
                                  (float) ui->light_y->value(),
                                  (float) ui->light_z->value());
}

void MainWindow::on_light_y_valueChanged(double arg1)
{

    ui->mygl->updateLightPosition((float) ui->light_x->value(),
                                  (float) arg1,
                                  (float) ui->light_z->value());
}

void MainWindow::on_light_z_valueChanged(double arg1)
{
    ui->mygl->updateLightPosition((float) ui->light_x->value(),
                                  (float) ui->light_y->value(),
                                  (float) arg1);
}

void MainWindow::on_light_r_valueChanged(double arg1)
{
    ui->mygl->updateLightColor((float) arg1,
                               (float) ui->light_g->value(),
                               (float) ui->light_b->value());
}

void MainWindow::on_light_g_valueChanged(double arg1)
{
    ui->mygl->updateLightColor((float) ui->light_r->value(),
                               (float) arg1,
                               (float) ui->light_b->value());
}

void MainWindow::on_light_b_valueChanged(double arg1)
{
    ui->mygl->updateLightColor((float) ui->light_r->value(),
                               (float) ui->light_g->value(),
                               (float) arg1);
}
