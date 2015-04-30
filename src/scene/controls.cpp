#include "controls.h"
#include <iostream>

static const int CONTROLS_IDX_COUNT = 128;
static const int CONTROLS_VERT_COUNT = 68;

Controls::Controls()
  : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufNor(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer) {
    dir = vec4(0, 0, -1, 0); // Inwards
    ori = vec4(0, 0, 1, 1); // Origin

    TRS = 0;
}

Controls::Controls(vec4 d, vec4 o)
  : bufIdx(QOpenGLBuffer::IndexBuffer),
    bufPos(QOpenGLBuffer::VertexBuffer),
    bufNor(QOpenGLBuffer::VertexBuffer),
    bufCol(QOpenGLBuffer::VertexBuffer) {
    dir = normalize(d);
    ori = o;
}

///---------------------
/// Getters and Setters
/// --------------------

void Controls::setDirection(vec4 v1) {
    dir - normalize(v1);
}

vec4 Controls::getDirection() const {
    return dir;
}

/// ------------------------------------------
/// Controls Drawable Functions (Only in .ccp file)
/// ------------------------------------------

void Controls::create()
{
    GLuint CONTROLS_idx[CONTROLS_IDX_COUNT];
    vec4 CONTROLS_vert_pos[CONTROLS_VERT_COUNT];
    vec4 CONTROLS_vert_col[CONTROLS_VERT_COUNT];
    glm::vec4 v;

    mat4 X_mat4 = translate(mat4(), vec3(1, 0, 0));
    mat4 Y_mat4 = translate(mat4(), vec3(0, 1, 0));
    mat4 Z_mat4 = translate(mat4(), vec3(0, 0, 1));

    switch(TRS) {
        case 0: // Translate
            break;
        case 1: // Rotate
            // Store on XZ cap verts (IDX 0 - 19)
            for (int i = 0; i < 20; i++) {
                v = glm::rotate(X_mat4, i / 20.f * TWO_PI, glm::vec3(0, 1, 0)) * glm::vec4(0.1f, 0, 0, 1);
                CONTROLS_vert_pos[i] = v;
            }
            // Store XY cap verts (IDX 20 - 39)
            for (int i = 20; i < 40; i++) {
                v = glm::rotate(Y_mat4, (i - 20) / 20.f * TWO_PI, glm::vec3(0, 0, 1)) * glm::vec4(0, 0.1f, 0, 1);
                CONTROLS_vert_pos[i] = v;
            }

            // Store YZ cap verts (IDX 40 - 59)
            for (int i = 40; i < 60; i++) {
                v = glm::rotate(Z_mat4, (i - 40) / 20.f * TWO_PI, glm::vec3(1, 0, 0)) * glm::vec4(0, 0, 0.1f, 1);
                CONTROLS_vert_pos[i] = v;
            }

            /// Joint Indices
            // Build indices XZ (19 lines, indices 0 - 37, up to vertex 0-19)
            for (int i = 0; i < 19; i++) {
                CONTROLS_idx[i * 2] = i;
                CONTROLS_idx[i * 2 + 1] = i + 1;
            }

            // Build indices XY (19 lines, indices 40 - 77, up to vertex 20-39)
            for (int i = 20; i < 39; i++) {
                CONTROLS_idx[i * 2] = i;
                CONTROLS_idx[i * 2 + 1] = i + 1;
            }

            // Build indices YZ (19 lines, indices 80 - 117, up to vertex 40-59)
            for (int i = 40; i < 59; i++) {
                CONTROLS_idx[i * 2] = i;
                CONTROLS_idx[i * 2 + 1] = i + 1;
            }

            // Connect the ends of t
            CONTROLS_idx[19 * 2] = 19;
            CONTROLS_idx[39 * 2] = 39;
            CONTROLS_idx[59 * 2] = 59;
            CONTROLS_idx[19 * 2 + 1] = 0;
            CONTROLS_idx[39 * 2 + 1] = 20;
            CONTROLS_idx[59 * 2 + 1] = 40;

            // Add in child to parent
            CONTROLS_idx[60 * 2] = 60;
            CONTROLS_idx[60 * 2 + 1] = 61;

            /// Joint Colors
            for (int i = 0; i < 20; i++) {
                CONTROLS_vert_col[i] = glm::vec4(1, 0, 0, 1);
            }

            for (int i = 20; i < 40; i++) {
                CONTROLS_vert_col[i] = glm::vec4(0, 1, 0, 1);
            }

            for (int i = 40; i < 60; i++) {
                CONTROLS_vert_col[i] = glm::vec4(0, 0, 1, 1);
            }

            CONTROLS_vert_col[60] = vec4(1, 1, 0, 1); // Yellow
            CONTROLS_vert_col[61] = vec4(1, 0, 0, 1); // Red

            break;
        case 2: // Scale
        CONTROLS_vert_pos[0] = vec4(-0.1, -0.1, 1, 1);
        CONTROLS_vert_pos[1] = vec4(-0.1, 0.1, 1, 1);
        CONTROLS_vert_pos[2] = vec4(0.1, 0.1, 1, 1);
        CONTROLS_vert_pos[3] = vec4(0.1, -0.1, 1, 1);

        CONTROLS_vert_pos[4] = vec4(-0.1, 0, 0.9, 1);
        CONTROLS_vert_pos[5] = vec4(-0.1, 0, 1.1, 1);
        CONTROLS_vert_pos[6] = vec4(0.1, 0, 1.1, 1);
        CONTROLS_vert_pos[7] = vec4(0.1, 0, 0.9, 1);

        CONTROLS_vert_pos[8] = vec4(0, -0.1, 0.9, 1);
        CONTROLS_vert_pos[9] = vec4(0, -0.1, 1.1, 1);
        CONTROLS_vert_pos[10] = vec4(0, 0.1, 1.1, 1);
        CONTROLS_vert_pos[11] = vec4(0, 0.1, 0.9, 1);

        CONTROLS_idx[0] = 0;
        CONTROLS_idx[1] = 1;
        CONTROLS_idx[2] = 1;
        CONTROLS_idx[3] = 2;
        CONTROLS_idx[4] = 2;
        CONTROLS_idx[5] = 3;
        CONTROLS_idx[6] = 3;
        CONTROLS_idx[7] = 0;

        CONTROLS_idx[8] = 4;
        CONTROLS_idx[9] = 5;
        CONTROLS_idx[10] = 5;
        CONTROLS_idx[11] = 6;
        CONTROLS_idx[12] = 6;
        CONTROLS_idx[13] = 7;
        CONTROLS_idx[14] = 7;
        CONTROLS_idx[15] = 4;

        CONTROLS_idx[16] = 8;
        CONTROLS_idx[17] = 9;
        CONTROLS_idx[18] = 9;
        CONTROLS_idx[19] = 10;
        CONTROLS_idx[20] = 10;
        CONTROLS_idx[21] = 11;
        CONTROLS_idx[22] = 11;
        CONTROLS_idx[23] = 8;

        // Y
        CONTROLS_vert_pos[12] = vec4(-0.1, 0.9, 0, 1);
        CONTROLS_vert_pos[13] = vec4(-0.1, 1.1, 0, 1);
        CONTROLS_vert_pos[14] = vec4(0.1, 1.1, 0, 1);
        CONTROLS_vert_pos[15] = vec4(0.1, 0.9, 0, 1);

        CONTROLS_vert_pos[16] = vec4(-0.1, 1, -0.1, 1);
        CONTROLS_vert_pos[17] = vec4(-0.1, 1, 0.1, 1);
        CONTROLS_vert_pos[18] = vec4(0.1, 1, 0.1, 1);
        CONTROLS_vert_pos[19] = vec4(0.1, 1, -0.1, 1);

        CONTROLS_vert_pos[20] = vec4(0, 0.9, -0.1, 1);
        CONTROLS_vert_pos[21] = vec4(0, 0.9, 0.1, 1);
        CONTROLS_vert_pos[22] = vec4(0, 1.1, 0.1, 1);
        CONTROLS_vert_pos[23] = vec4(0, 1.1, -0.1, 1);

        CONTROLS_idx[24] = 12;
        CONTROLS_idx[25] = 13;
        CONTROLS_idx[26] = 13;
        CONTROLS_idx[27] = 14;
        CONTROLS_idx[28] = 14;
        CONTROLS_idx[29] = 15;
        CONTROLS_idx[30] = 15;
        CONTROLS_idx[31] = 12;

        CONTROLS_idx[32] = 16;
        CONTROLS_idx[33] = 17;
        CONTROLS_idx[34] = 17;
        CONTROLS_idx[35] = 18;
        CONTROLS_idx[36] = 18;
        CONTROLS_idx[37] = 19;
        CONTROLS_idx[38] = 19;
        CONTROLS_idx[39] = 16;

        CONTROLS_idx[40] = 20;
        CONTROLS_idx[41] = 21;
        CONTROLS_idx[42] = 21;
        CONTROLS_idx[43] = 22;
        CONTROLS_idx[44] = 22;
        CONTROLS_idx[45] = 23;
        CONTROLS_idx[46] = 23;
        CONTROLS_idx[47] = 20;

        // X
        CONTROLS_vert_pos[24] = vec4(0.9, -0.1, 0, 1);
        CONTROLS_vert_pos[25] = vec4(0.9, 0.1, 0, 1);
        CONTROLS_vert_pos[26] = vec4(1.1, 0.1, 0, 1);
        CONTROLS_vert_pos[27] = vec4(1.1, -0.1, 0, 1);

        CONTROLS_vert_pos[28] = vec4(0.9, 0, -0.1, 1);
        CONTROLS_vert_pos[29] = vec4(0.9, 0, 0.1, 1);
        CONTROLS_vert_pos[30] = vec4(1.1, 0, 0.1, 1);
        CONTROLS_vert_pos[31] = vec4(1.1, 0, -0.1, 1);

        CONTROLS_vert_pos[32] = vec4(1, -0.1, -0.1, 1);
        CONTROLS_vert_pos[33] = vec4(1, -0.1, 0.1, 1);
        CONTROLS_vert_pos[34] = vec4(1, 0.1, 0.1, 1);
        CONTROLS_vert_pos[35] = vec4(1, 0.1, -0.1, 1);

        CONTROLS_idx[48] = 24;
        CONTROLS_idx[49] = 25;
        CONTROLS_idx[50] = 25;
        CONTROLS_idx[51] = 26;
        CONTROLS_idx[52] = 26;
        CONTROLS_idx[53] = 27;
        CONTROLS_idx[54] = 27;
        CONTROLS_idx[55] = 24;

        CONTROLS_idx[56] = 28;
        CONTROLS_idx[57] = 29;
        CONTROLS_idx[58] = 29;
        CONTROLS_idx[59] = 30;
        CONTROLS_idx[60] = 30;
        CONTROLS_idx[61] = 31;
        CONTROLS_idx[62] = 31;
        CONTROLS_idx[63] = 28;

        CONTROLS_idx[64] = 32;
        CONTROLS_idx[65] = 33;
        CONTROLS_idx[66] = 33;
        CONTROLS_idx[67] = 34;
        CONTROLS_idx[68] = 34;
        CONTROLS_idx[69] = 35;
        CONTROLS_idx[70] = 35;
        CONTROLS_idx[71] = 32;


        for (int i = 72; i < 122; i++) {
            CONTROLS_idx[i] = 0;
        }

        for (int i = 0; i < 12; i++) {
            CONTROLS_vert_col[i] = glm::vec4(0, 0, 1, 1);
        }

        for (int i = 12; i < 24; i++) {
            CONTROLS_vert_col[i] = glm::vec4(0, 1, 0, 1);
        }

        for (int i = 24; i < 36; i++) {
            CONTROLS_vert_col[i] = glm::vec4(1, 0, 0, 1);
        }
            break;
    }
    // The Lines
    CONTROLS_vert_pos[62] = vec4(0, 0, 0, 1);
    CONTROLS_vert_pos[63] = vec4(1, 0, 0, 1); // X
    CONTROLS_vert_pos[64] = vec4(0, 0, 0, 1);
    CONTROLS_vert_pos[65] = vec4(0, 1, 0, 1); // Y
    CONTROLS_vert_pos[66] = vec4(0, 0, 0, 1);
    CONTROLS_vert_pos[67] = vec4(0, 0, 1, 1); // Z

    CONTROLS_idx[122] = 62; // X
    CONTROLS_idx[123] = 63;
    CONTROLS_idx[124] = 64; // Y
    CONTROLS_idx[125] = 65;
    CONTROLS_idx[126] = 66; // Z
    CONTROLS_idx[127] = 67;

    CONTROLS_vert_col[62] = vec4(1, 0, 0, 1);
    CONTROLS_vert_col[63] = vec4(1, 0, 0, 1);
    CONTROLS_vert_col[64] = vec4(0, 1, 0, 1);
    CONTROLS_vert_col[65] = vec4(0, 1, 0, 1);
    CONTROLS_vert_col[66] = vec4(0, 0, 1, 1);
    CONTROLS_vert_col[67] = vec4(0, 0, 1, 1);

    if (active == 0) {
        CONTROLS_vert_col[62] = vec4(1, 0, 0, 1);
        CONTROLS_vert_col[64] = vec4(1, 0, 0, 1);
        CONTROLS_vert_col[66] = vec4(1, 0, 0, 1);
    } else if (active == 1) {
        CONTROLS_vert_col[62] = vec4(0, 1, 0, 1);
        CONTROLS_vert_col[64] = vec4(0, 1, 0, 1);
        CONTROLS_vert_col[66] = vec4(0, 1, 0, 1);
    } else if (active == 2) {
        CONTROLS_vert_col[62] = vec4(0, 0, 1, 1);
        CONTROLS_vert_col[64] = vec4(0, 0, 1, 1);
        CONTROLS_vert_col[66] = vec4(0, 0, 1, 1);
    } else {
        CONTROLS_vert_col[62] = vec4(0, 0, 0, 1);
        CONTROLS_vert_col[64] = vec4(0, 0, 0, 1);
        CONTROLS_vert_col[66] = vec4(0, 0, 1, 1);
    }

  count = CONTROLS_IDX_COUNT;

  bufIdx.create();
  bufIdx.bind();
  bufIdx.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufIdx.allocate(CONTROLS_idx, CONTROLS_IDX_COUNT * sizeof(GLuint));

  bufPos.create();
  bufPos.bind();
  bufPos.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufPos.allocate(CONTROLS_vert_pos, CONTROLS_VERT_COUNT * sizeof(vec4));

  bufCol.create();
  bufCol.bind();
  bufCol.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufCol.allocate(CONTROLS_vert_col, CONTROLS_VERT_COUNT * sizeof(vec4));
}

void Controls::destroy()
{
  bufIdx.destroy();
  bufPos.destroy();
  bufNor.destroy();
  bufCol.destroy();
}

GLenum Controls::drawMode()
{
  return GL_LINES;
}

int Controls::elemCount()
{
  return count;
}

bool Controls::bindIdx()
{
  return bufIdx.bind();
}

bool Controls::bindPos()
{
  return bufPos.bind();
}

bool Controls::bindNor()
{
  return bufNor.bind();
}

bool Controls::bindCol()
{
  return bufCol.bind();
}

bool Controls::bindJID()
{
    return 1;
}

bool Controls::bindJWeight()
{
    return 1;
}
