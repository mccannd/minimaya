#pragma once

#include <QTreeWidgetItem>
#include <la.h>
#include <shaderprogram.h>

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QVector>



class Joint : public QTreeWidgetItem, public ShaderProgram::Drawable
{
private:
    int count;
    QOpenGLBuffer bufIdx;
    QOpenGLBuffer bufPos;
    QOpenGLBuffer bufCol;

public:
    Joint();
    Joint(Joint* parent);
    Joint(float pos_x, float pos_y, float pos_z);
    Joint(Joint *parent, float pos_x, float pos_y, float pos_z);
    Joint(Joint *parent, float pos_x, float pos_y, float pos_z,
          float e_x, float e_y, float e_z);
    Joint(float angle, glm::vec3 axis); // quat, not euler
    Joint(Joint *parent, float pos_x, float pos_y, float pos_z,
          float angle, glm::vec3 axis);

    ~Joint();

    // fields
    QString name;
    Joint* parent = NULL;
    std::vector<Joint*> children = {};
    glm::vec4 position; //relative to parent joint
    glm::quat rotation;
    glm::mat4 bind_matrix;
    glm::vec4 color = glm::vec4(0.1, 0.1, 0.6, 1);

    unsigned int numChildren();
    Joint* getChild(unsigned int index);
    void move(float pos_x, float pos_y, float pos_z); // local

    void rotate(float e_x, float e_y, float e_z); // euler angles
    void rotate(float angle, float a_x, float a_y, float a_z); //quat
    glm::mat4 getLocalTransformation();
    glm::mat4 getOverallTransformation();
    void setBindMatrix();
    void rename(QString name);

    // animation utilities
    QVector<glm::quat> keysRotation;
    QVector<glm::vec4> keysPosition;
    void keyframeSnapshot(); // save the state of rotation and position
    void applyKeyframe(unsigned int index); // apply this key's properties
    void clearKeyframes();

    // graphics: drawable lines
    void create();
    void destroy();

    void createJointVertexPositions(std::vector<glm::vec4> &pos,
                               std::vector<glm::vec4> &col,
                               std::vector<GLuint> &idx);

    virtual GLenum drawMode();
    virtual int elemCount();
    virtual bool bindIdx();
    virtual bool bindPos();
    virtual bool bindNor();
    virtual bool bindCol();
    virtual bool bindJID();
    virtual bool bindJWeight();
};


