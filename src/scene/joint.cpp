#include "joint.h"
#include <assert.h>
#include <cmath>


/// --- Contstructors ---
///
///

Joint::Joint()
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    rotation = glm::quat();
    position = glm::vec4(0, 0, 0, 1);
    name = "New Joint";
    setText(0, name);
}

Joint::Joint(Joint *parent)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    rotation = glm::quat();
    position = glm::vec4(0, 0, 0, 1);
    if (parent != NULL) {
        this->parent = parent;
        parent->children.push_back(this);
        parent->addChild(this);
    }
    name = "New Joint";
    setText(0, name);
}

Joint::Joint(float pos_x, float pos_y, float pos_z)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    rotation = glm::quat();
    position = glm::vec4(pos_x, pos_y, pos_z, 1);
    name = "New Joint";
    setText(0, name);
}

Joint::Joint(Joint *parent, float pos_x, float pos_y, float pos_z)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    rotation = glm::quat();
    position = glm::vec4(pos_x, pos_y, pos_z, 1);
    if (parent != NULL) {
        this->parent = parent;
        parent->children.push_back(this);
        parent->addChild(this);
    }
    name = "New Joint";
    setText(0, name);

}

Joint::Joint(Joint *parent, float pos_x, float pos_y, float pos_z,
             float e_x, float e_y, float e_z)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    rotation = glm::quat(glm::vec3(e_x, e_y, e_z));
    position = glm::vec4(pos_x, pos_y, pos_z, 1);
    if (parent != NULL) {
        this->parent = parent;
        parent->children.push_back(this);
        parent->addChild(this);
    }
    name = "New Joint";
    setText(0, name);
}

Joint::Joint(float angle, glm::vec3 axis)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    rotation = glm::angleAxis(angle, axis);
    position = glm::vec4(0, 0, 0, 1);
    name = "New Joint";
    setText(0, name);
}

Joint::Joint(Joint *parent, float pos_x, float pos_y, float pos_z,
             float angle, glm::vec3 axis)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    rotation = glm::angleAxis(angle, axis);
    position = glm::vec4(pos_x, pos_y, pos_z, 1);
    if (parent != NULL) {
        this->parent = parent;
        parent->children.push_back(this);
        parent->addChild(this);
    }
    name = "New Joint";
    setText(0, name);
}

Joint::~Joint()
{
    this->destroy();

    // delete every child of this joint too
    for (unsigned int i = 0; i < children.size(); i++) {
        if (children[i] != NULL) {
            delete children[i];
        }
    }
}


/// --- OpenGL utilities
///
///

void Joint::createJointVertexPositions(std::vector<glm::vec4> &pos,
                                std::vector<glm::vec4> &col,
                                std::vector<GLuint> &idx)
{
    glm::vec4 v;
    glm::mat4 transformation = this->getOverallTransformation();

    // store the xy disk
    for (int i = 0; i < 12; i++) {
        float x = 0.5 * cos(i / 12.0f * TWO_PI);
        float y = 0.5 * sin(i / 12.0f * TWO_PI);
        v = glm::vec4(x, y, 0, 1);
        pos.push_back(transformation * v);
        if (i > 0) {
            idx.push_back(i - 1);
            idx.push_back(i);
        }
        col.push_back(color);
    }
    idx.push_back(11);
    idx.push_back(0); // complete the ring's cycle

    // store the yz disk
    for (int i = 12; i < 24; i++) {
        float y = 0.5 * cos((i - 12) / 12.0f * TWO_PI);
        float z = 0.5 * sin((i - 12) / 12.0f * TWO_PI);
        v = glm::vec4(0, y, z, 1);
        pos.push_back(transformation * v);
        if (i > 12) {
            idx.push_back(i - 1);
            idx.push_back(i);
        }
        col.push_back(color);
    }
    idx.push_back(23);
    idx.push_back(12);

    // store the xz disk
    for (int i = 24; i < 36; i++) {
        float x = 0.5 * cos((i - 24) / 12.0f * TWO_PI);
        float z = 0.5 * sin((i - 24) / 12.0f * TWO_PI);
        v = glm::vec4(x, 0, z, 1);
        pos.push_back(transformation * v);
        if (i > 24) {
            idx.push_back(i - 1);
            idx.push_back(i);
        }
        col.push_back(color);
    }
    idx.push_back(35);
    idx.push_back(24);
}

void Joint::create()
{
    std::vector<glm::vec4> positions;
    std::vector<glm::vec4> colors;
    std::vector<GLuint> indices;

    createJointVertexPositions(positions, colors, indices);

    // final links: check if there is a parent
    // if there is, make a line between with a gradent
    if (parent != NULL) {
        positions.push_back(getOverallTransformation()
                            * glm::vec4(0, 0, 0, 1));
        colors.push_back(glm::vec4(1, 1, 0, 1));
        indices.push_back(36);
        positions.push_back(parent->getOverallTransformation()
                            * glm::vec4(0, 0, 0, 1));
        colors.push_back(glm::vec4(1, 0, 0, 1));
        indices.push_back(37);
    }

    count = indices.size();

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(indices.data(),
                    indices.size() * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(positions.data(),
                    positions.size() * sizeof(glm::vec4));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(colors.data(),
                    colors.size() * sizeof(glm::vec4));
}

void Joint::destroy()
{
    bufIdx.destroy();
    bufPos.destroy();
    bufCol.destroy();
}

GLenum Joint::drawMode()
{
    return GL_LINES;
}

int Joint::elemCount()
{
    return count;
}

bool Joint::bindIdx()
{
    return bufIdx.bind();
}

bool Joint::bindPos()
{
    return bufPos.bind();
}

bool Joint::bindNor()
{
    return 1;
}

bool Joint::bindCol()
{
    return bufCol.bind();
}

bool Joint::bindJID()
{
    return 1;
}

bool Joint::bindJWeight()
{
    return 1;
}

/// --- Geometric information utilities
///
///

glm::mat4 Joint::getLocalTransformation()
{
    glm::vec3 pos = glm::vec3(position[0], position[1], position[2]);
    glm::mat4 translation_mat = glm::translate(glm::mat4(1.0), pos);
    glm::mat4 rotation_mat = glm::toMat4(rotation);
    return translation_mat * rotation_mat;
}

glm::mat4 Joint::getOverallTransformation()
{
    // default: identity matrix
    glm::mat4 parent_transformation = glm::mat4(1);

    // recursively ascend the tree to find a chain of transformations
    if (parent != NULL) {
        parent_transformation = parent->getOverallTransformation();
    }

    glm::mat4 local_transformation = this->getLocalTransformation();

    return parent_transformation * local_transformation;
}

// move in local space
void Joint::move(float pos_x, float pos_y, float pos_z)
{
    position = glm::vec4(pos_x, pos_y, pos_z, 1);
}

// rotate using euler angles
void Joint::rotate(float e_x, float e_y, float e_z)
{
    rotation = glm::quat(glm::vec3(e_x, e_y, e_z));
}

// rotate about axis
void Joint::rotate(float angle, float a_x, float a_y, float a_z)
{
    rotation = glm::angleAxis(angle, glm::vec3(a_x, a_y, a_z));
}

unsigned int Joint::numChildren()
{
    return children.size();
}

Joint* Joint::getChild(unsigned int index)
{
    assert (index < numChildren());

    return children[index];
}

// should be called just after creation
void Joint::setBindMatrix()
{
    bind_matrix = this->getOverallTransformation();
}

void Joint::rename(QString name)
{
    this->name = name;
    this->setText(0, name);
}
