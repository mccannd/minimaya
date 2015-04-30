#pragma once

#include <glwidget277.h>
#include <la.h>

#include <QOpenGLFunctions_3_2_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>


class ShaderProgram
{
public:
    // This defines a class which can be rendered by our shader program.
    // Make any geometry a subclass of ShaderProgram::Drawable in order to render it with the ShaderProgram class.
    class Drawable
    {
    public:
        virtual GLenum drawMode() = 0;
        virtual int elemCount() = 0;
        virtual bool bindIdx() = 0;
        virtual bool bindPos() = 0;
        virtual bool bindNor() = 0;
        virtual bool bindCol() = 0;
        virtual bool bindJID() = 0;
        virtual bool bindJWeight() = 0;
    };

public:
    QOpenGLShaderProgram prog;

    int attrPos;
    int attrNor;
    int attrCol;
    int attrJointIDs; // used for skeleton shader
    int attrJointWeights; // used for skeleton shader

    int unifJointBindPos; // used for skeleton shader
    int unifJointTransform; // used for skeleton shader
    int unifModel;
    int unifModelInvTr;
    int unifViewProj;
    int unifColor;
    int unifLight; // used for light vector
    int unifLightcolor;

public:
    void create(const char *vertfile, const char *fragfile);
    void setModelMatrix(const glm::mat4 &model);
    void setViewProjMatrix(const glm::mat4& vp);
    void setJointBindPosArray(const std::vector<glm::mat4>& jbp);
    void setJointTransformArray(const std::vector<glm::mat4>& jt);
    void setLightPosition(const glm::vec4& pos);
    void setLightColor(const glm::vec4& col);
    void draw(GLWidget277 &f, Drawable &d);
};
