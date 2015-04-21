#include "joint.h"
#include "myMath.h"

static const int joi_IDX_COUNT = 122;
static const int joi_VERT_COUNT = 62;

Joint::Joint()
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufNor(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    pos = vec4(0, 0, 0, 1);
    parent = NULL;
}

Joint::Joint(QString n, Joint* p, vec4 position, quat r)
    : bufIdx(QOpenGLBuffer::IndexBuffer),
      bufPos(QOpenGLBuffer::VertexBuffer),
      bufNor(QOpenGLBuffer::VertexBuffer),
      bufCol(QOpenGLBuffer::VertexBuffer)
{
    name = n;
    pos = position;
    parent = p;
    rot = r;
    bindMatrix = glm::inverse(getOverallTransformation());

    if (parent != NULL) {
        parent->children.push_back(this);
        parent->addChild(this);
    }

     QTreeWidgetItem::setText(0, name);
}

// concatenation of a joint's position and rotation
mat4 Joint::getLocalTransformation() {
    return glm::translate(mat4(), vec3(pos)) * glm::toMat4(rot);
}

// concatentation of this joint's local transformation
// with the transformations of its chain of parent joints.
mat4 Joint::getOverallTransformation() {
    if (parent != NULL) {
        return parent->getOverallTransformation() * getLocalTransformation();
    } else {
        return getLocalTransformation();
    }
}

void Joint::create()
{
    GLuint joi_idx[joi_IDX_COUNT];
    glm::vec4 joi_vert_pos[joi_VERT_COUNT];
    glm::vec4 joi_vert_col[joi_VERT_COUNT];
    glm::vec4 v;

    // Store on XZ cap verts (IDX 0 - 19)
    for (int i = 0; i < 20; i++) {
        v = glm::rotate(mat4(), i / 20.f * TWO_PI, glm::vec3(0, 1, 0)) * glm::vec4(0.5f, 0, 0, 1);
        joi_vert_pos[i] = v;
    }
    // Store XY cap verts (IDX 20 - 39)
    for (int i = 20; i < 40; i++) {
        v = glm::rotate(mat4(), (i - 20) / 20.f * TWO_PI, glm::vec3(0, 0, 1)) * glm::vec4(0, 0.5f, 0, 1);
        joi_vert_pos[i] = v;
    }

    // Store YZ cap verts (IDX 40 - 59)
    for (int i = 40; i < 60; i++) {
        v = glm::rotate(mat4(), (i - 40) / 20.f * TWO_PI, glm::vec3(1, 0, 0)) * glm::vec4(0, 0, 0.5f, 1);
        joi_vert_pos[i] = v;
    }

    // Create vertices for the parent pos and current pos
    joi_vert_pos[60] = vec4(0, 0, 0, 1);
    if (parent != NULL) {
        joi_vert_pos[61] = (vec4(0, 0, 0, 2) - this->pos);
    } else {
        joi_vert_pos[61] = joi_vert_pos[60];
    }

//    std::cout << name.toStdString() << " ";
//    std::cout << "Curr: " << joi_vert_pos[60];
//    std::cout << "Parent: " << joi_vert_pos[61] << "\n";

    if (!selected) {
        /// Joint Colors
        for (int i = 0; i < 20; i++) {
            joi_vert_col[i] = glm::vec4(0, 1, 0, 1);
        }

        for (int i = 20; i < 40; i++) {
            joi_vert_col[i] = glm::vec4(1, 0, 0, 1);
        }

        for (int i = 40; i < 60; i++) {
            joi_vert_col[i] = glm::vec4(0, 0, 1, 1);
        }

        joi_vert_col[60] = vec4(1, 1, 0, 1); // Yellow
        joi_vert_col[61] = vec4(1, 0, 0, 1); // Red
    } else {
        for (int i = 0; i < joi_VERT_COUNT - 2; i++) {
            joi_vert_col[i] = glm::vec4(0, 1, 1, 1);
        }
        joi_vert_col[60] = vec4(0, 1, 0.5, 1);
        joi_vert_col[61] = vec4(0, 0.5, 1, 1);
    }

    /// Joint Indices
    // Build indices XZ (19 lines, indices 0 - 37, up to vertex 0-19)
    for (int i = 0; i < 19; i++) {
        joi_idx[i * 2] = i;
        joi_idx[i * 2 + 1] = i + 1;
    }

    // Build indices XY (19 lines, indices 40 - 77, up to vertex 20-39)
    for (int i = 20; i < 39; i++) {
        joi_idx[i * 2] = i;
        joi_idx[i * 2 + 1] = i + 1;
    }

    // Build indices YZ (19 lines, indices 80 - 117, up to vertex 40-59)
    for (int i = 40; i < 59; i++) {
        joi_idx[i * 2] = i;
        joi_idx[i * 2 + 1] = i + 1;
    }

    // Connect the ends of t
    joi_idx[19 * 2] = 19;
    joi_idx[39 * 2] = 39;
    joi_idx[59 * 2] = 59;
    joi_idx[19 * 2 + 1] = 0;
    joi_idx[39 * 2 + 1] = 20;
    joi_idx[59 * 2 + 1] = 40;

    // Add in child to parent
    joi_idx[60 * 2] = 60;
    joi_idx[60 * 2 + 1] = 61;

    count = joi_IDX_COUNT;

    bufIdx.create();
    bufIdx.bind();
    bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufIdx.allocate(joi_idx, joi_IDX_COUNT * sizeof(GLuint));

    bufPos.create();
    bufPos.bind();
    bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufPos.allocate(joi_vert_pos, joi_VERT_COUNT * sizeof(glm::vec4));

    bufCol.create();
    bufCol.bind();
    bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
    bufCol.allocate(joi_vert_col, joi_VERT_COUNT * sizeof(glm::vec4));
}

void Joint::destroy()
{
    // Traverse Through Vector
     children.clear();
     if (parent != NULL) {
         for (std::vector<Joint*>::size_type i = 0;
              i < children.size(); i++) {
             if (this == parent->children[i]) {
                 parent->children.erase(parent->children.begin() + i);
             }
         }
     }

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
    return bufNor.bind();
}

bool Joint::bindCol()
{
    return bufCol.bind();
}
