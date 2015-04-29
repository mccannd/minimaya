#include "joint.h"
#include <assert.h>
#include <cmath>
#include <iostream>


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



/// --- Animation utilities
///
///

// save a keyframe of this joint plus all children
void Joint::keyframeSnapshot(int frame)
{
    // overwrite a previous keyframe if the frame is the same
    if (keysRotation.contains(frame)) {
        keysRotation.remove(frame);
        keysPosition.remove(frame);
    }

    keysRotation[frame] = rotation;
    keysPosition[frame] = position;

    controlPointKeysRotation.clear();
    controlPointKeysPosition.clear();

    for (unsigned int i = 0; i < children.size(); i++) {
        children[i]->keyframeSnapshot(frame);
    }
}

// apply the state of a certain key to all nodes
void Joint::applyKeyframe(int index)
{
    if (index > frameRotation.size() ||
            frameRotation.size() == 0) return;

    //std::cout <<"Applying frame "<<index<<"\n";
    rotation = frameRotation[index];
    position = framePosition[index];

    for (unsigned int i = 0; i < children.size(); i++) {
        children[i]->applyKeyframe(index);
    }
}

// delete all keyframes
void Joint::clearKeyframes()
{
    keysPosition.clear();
    keysRotation.clear();

    controlPointKeysRotation.clear();
    controlPointKeysPosition.clear();

    frameRotation.clear();
    framePosition.clear();


    for (unsigned int i = 0; i < children.size(); i++) {
        children[i]->clearKeyframes();
    }
}

// make the extended vectors for smoothly interpolated animation
void Joint::makeControlPoints()
{
    if (keysPosition.size() < 1) return;

    controlPointKeysPosition.clear();
    controlPointKeysRotation.clear();

    if (keysPosition.size() == 1) {
        QList<int> frame = keysPosition.keys();

        controlPointKeysPosition[frame[0]] =
                (keysPosition[frame[0]]);
        controlPointKeysRotation[frame[0]] =
                (keysRotation[frame[0]]);

    } else {


        QList<int> keyList = keysPosition.keys();


        for (int i = 0; i < keyList.size(); i++) {
            if (i == 0) {
                controlPointKeysPosition[keyList[i]] =
                        keysPosition[keyList[i]];
                int t1 = keyList[i];
                int t2 = keyList[i+1];
                int time = (int) ((1.0 / 6.0) * (t2 - t1)) + t1;
                if (time == t1) time++;
                controlPointKeysPosition[keyList[i] + 1] =
                        keysPosition[keyList[i]];

//                std::cout<<"pos " << keyList[i] << "\n";
//                std::cout<<"pos " << keyList[i] + 1 << "\n";
            } else if (i == keyList.size() - 1){
                int t1 = keyList[i-1];
                int t2 = keyList[i];
                int time = t2 - (int) ((1.0 / 6.0) * (t2 - t1));
                if (time == t2) time--;
                controlPointKeysPosition[keyList[i] - 1] =
                        keysPosition[keyList[i]];
                controlPointKeysPosition[keyList[i]] =
                        keysPosition[keyList[i]];

//                std::cout<<"pos " << keyList[i] - 1 << "\n";
//                std::cout<<"pos " << keyList[i] << "\n";
            } else {
                // get the vectors before and after this key
                glm::vec4 prev = keysPosition[keyList[i - 1]];
                glm::vec4 next = keysPosition[keyList[i + 1]];
                //int t1 = keyList[i - 1];
                //int t2 = keyList[i + 1];
                // calculate a slope and divide by six
                glm::vec4 slope = (1.0f / 4.0f) * (next - prev);

                //int tslope = (int) ((1.0f / 4.0f) * (t2 - t1));
                controlPointKeysPosition[keyList[i] - 1] =
                        keysPosition[keyList[i]] - slope;
                controlPointKeysPosition[keyList[i]] =
                        keysPosition[keyList[i]];
                controlPointKeysPosition[keyList[i] + 1] =
                        keysPosition[keyList[i]] + slope;

//                std::cout<<"pos " << keyList[i] - 1 << "\n";
//                std::cout<<"pos " << keyList[i] << "\n";
//                std::cout<<"pos " << keyList[i] + 1 << "\n";
            }
        }

        keyList = keysRotation.keys();

        for (int i = 0; i < keyList.size() - 1; i++) {
            glm::quat inter1;
            glm::quat inter2;
            if (i == 0) {
                inter1 = glm::intermediate(keysRotation[keyList[i]],
                                           keysRotation[keyList[i]],
                                           keysRotation[keyList[i + 1]]);
            } else {
                inter1 = glm::intermediate(keysRotation[keyList[i - 1]],
                                           keysRotation[keyList[i]],
                                           keysRotation[keyList[i + 1]]);
            }

            if (i + 1 == keyList.size() - 1) {
                inter1 = glm::intermediate(keysRotation[keyList[i]],
                                           keysRotation[keyList[i + 1]],
                                           keysRotation[keyList[i + 1]]);
            } else {

                    inter1 = glm::intermediate(keysRotation[keyList[i]],
                                               keysRotation[keyList[i + 1]],
                                               keysRotation[keyList[i + 2]]);


            }

            controlPointKeysRotation[keyList[i]] = keysRotation[keyList[i]];
            controlPointKeysRotation[keyList[i] + 1] = inter1;
            controlPointKeysRotation[keyList[i + 1] - 1] = inter2;

//            std::cout<<"rot " << keyList[i] << "\n";
//            std::cout<<"rot " << keyList[i] + 1 << "\n";
//            std::cout<<"rot " << keyList[i+1] - 1 << "\n";


        }

        controlPointKeysRotation[keyList[keyList.size() - 1]] =
                keysRotation[keyList[keyList.size() - 1]];

//        std::cout<<"rot " << keyList[keyList.size() - 1] << "\n";
    }


    // calculate the control points for all joints
    for (unsigned int i = 0; i < children.size(); i++) {
        children[i]->makeControlPoints();
    }

}

void Joint::createAllFrames(unsigned int numFrames)
{
    if (keysPosition.size() < 1) {
        return;
    }

    frameRotation.clear();
    framePosition.clear();
    makeControlPoints();

    // create a number of frames equal to numFrames
    // pad the beginning if the first frame isnt a key

    QList<int> frames = controlPointKeysPosition.keys();

    int firstFrame = frames[0];
    if (firstFrame > 0) {
        for (int f = 0; f < firstFrame; f++) {
            frameRotation.push_back(controlPointKeysRotation[firstFrame]);
            framePosition.push_back(controlPointKeysPosition[firstFrame]);
        }
    }

//    std::cout<< frames.size() << " control points\n";

    for (unsigned int i = 0; i < frames.size() - 1; i += 3) {
        // find the time interval, in frames, between ctrlpts

        int keyframe1 = frames[i];

        int keyframe2 = frames[i + 3];

        //std::cout<<"Interpolating " <<keyframe1 << " " <<keyframe2 << "\n";
        for (int f = keyframe1; f < keyframe2; f++) {


            // calculate a time t for interpolation
            float t = (float)(f - keyframe1) /
                    (float)(keyframe2 - keyframe1);

            // calculate a new position based on ctrlpts
            // find the position through de castlejau
            glm::vec4 m1 = (1 - t) * controlPointKeysPosition[frames[i]] +
                    t * controlPointKeysPosition[frames[i + 1]];
            glm::vec4 m2 = (1 - t) * controlPointKeysPosition[frames[i + 1]] +
                    t * controlPointKeysPosition[frames[i + 2]];
            glm::vec4 m3 = (1 - t) * controlPointKeysPosition[frames[i + 2]] +
                    t * controlPointKeysPosition[frames[i + 3]];

//            std::cout<<m1.x <<" "<<m1.y<<" "<<m1.z<<"\n";
//            std::cout<<m2.x <<" "<<m2.y<<" "<<m2.z<<"\n";
//            std::cout<<m3.x <<" "<<m3.y<<" "<<m3.z<<"\n\n";

            // find second midpoints
            glm::vec4 mm1 = (1 - t) * m1 + t * m2;
            glm::vec4 mm2 = (1 - t) * m2 + t * m3;

//            std::cout<<mm1.x <<" "<<mm1.y<<" "<<mm1.z<<"\n";
//            std::cout<<mm2.x <<" "<<mm2.y<<" "<<mm2.z<<"\n";

            // set to final midpoint
            glm::vec4 newPosition = (1 - t) * mm1 + t * mm2;
//            std::cout<<"result" << newPosition.x <<" "<<newPosition.y<<
//                       " "<<newPosition.z<<"\n";
            framePosition.push_back(newPosition);


            // calculate a new rotation based on ctrlpts
            glm::quat newRotation = glm::squad(controlPointKeysRotation[frames[i]],
                                  controlPointKeysRotation[frames[i + 1]],
                                  controlPointKeysRotation[frames[i + 2]],
                                  controlPointKeysRotation[frames[i + 3]],
                                  t);
            frameRotation.push_back(newRotation);

        }


    }

    // pad the end if the last frame isnt a key
    int lastFrame = frames[frames.size() - 1];
    if (lastFrame < numFrames) {
        for (int f = lastFrame; f <= numFrames; f++) {
            frameRotation.push_back(keysRotation[lastFrame]);
            framePosition.push_back(keysPosition[lastFrame]);

        }
    }

    // calculate the frames for all joints
    for (unsigned int i = 0; i < children.size(); i++) {
        children[i]->createAllFrames(numFrames);
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
