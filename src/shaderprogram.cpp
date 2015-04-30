#include "shaderprogram.h"
#include <la.h>


void ShaderProgram::create(const char *vertfile, const char *fragfile)
{
    prog.addShaderFromSourceFile(QOpenGLShader::Vertex  , vertfile);
    prog.addShaderFromSourceFile(QOpenGLShader::Fragment, fragfile);
    prog.link();

    // in (per vertex stream)
    attrPos = prog.attributeLocation("vs_Pos");
    attrNor = prog.attributeLocation("vs_Nor");
    attrCol = prog.attributeLocation("vs_Col");
    attrJointIDs = prog.attributeLocation("vs_JID");
    attrJointWeights = prog.attributeLocation("vs_JWeight");

    // uniform
    unifJointBindPos = prog.uniformLocation("u_BindMatrix");
    unifJointTransform = prog.uniformLocation("u_TransformMatrix");
    unifModel      = prog.uniformLocation("u_Model");
    unifModelInvTr = prog.uniformLocation("u_ModelInvTr");
    unifViewProj   = prog.uniformLocation("u_ViewProj");
    unifLight      = prog.uniformLocation("u_LightPosition");
    unifLightcolor = prog.uniformLocation("u_LightColor");
}

void ShaderProgram::setJointBindPosArray(const std::vector<glm::mat4> &jbp)
{
    prog.bind();

    if(unifJointBindPos != -1){
        std::vector<QMatrix4x4> qmats;
        for (unsigned int i = 0; i < jbp.size(); i++) {
            qmats.push_back(la::to_qmat(jbp[i]));
        }

        prog.setUniformValueArray(unifJointBindPos,
                                  qmats.data(), qmats.size());
    }
}

void ShaderProgram::setJointTransformArray(const std::vector<glm::mat4> &jt)
{
    prog.bind();

    if(unifJointTransform != -1){
        std::vector<QMatrix4x4> qmats;
        for (unsigned int i = 0; i < jt.size(); i++) {
            qmats.push_back(la::to_qmat(jt[i]));
        }

        prog.setUniformValueArray(unifJointTransform,
                                  qmats.data(), qmats.size());
    }
}

void ShaderProgram::setModelMatrix(const glm::mat4 &model)
{
    prog.bind();

    if (unifModel != -1) {
        prog.setUniformValue(unifModel, la::to_qmat(model));
    }

    if (unifModelInvTr != -1) {
        glm::mat4 modelinvtr = glm::transpose(model);
        modelinvtr = glm::inverse(glm::transpose(model));
        prog.setUniformValue(unifModelInvTr, la::to_qmat(modelinvtr));
    }
}

void ShaderProgram::setViewProjMatrix(const glm::mat4& vp)
{
    prog.bind();

    if(unifViewProj != -1){
        prog.setUniformValue(unifViewProj, la::to_qmat(vp));
    }
}

void ShaderProgram::setLightPosition(const glm::vec4 &pos)
{
    prog.bind();

    if(unifLight != -1) {
        QVector4D vec = QVector4D(pos[0], pos[1], pos[2], pos[3]);
        prog.setUniformValue(unifLight, vec);
    }
}

void ShaderProgram::setLightColor(const glm::vec4 &col)
{
    prog.bind();

    if(unifLightcolor != -1) {
        QVector4D vec = QVector4D(col[0], col[1], col[2], col[3]);
        prog.setUniformValue(unifLightcolor, vec);
    }
}


// This function, as its name implies, uses the passed in GL widget
void ShaderProgram::draw(GLWidget277 &f, Drawable &d)
{
    prog.bind();

    // Each of the following blocks checks that:
    //   * This shader has this attribute, and
    //   * This Drawable has a vertex buffer for this attribute.
    // If so, it binds the appropriate buffers to each attribute.

    if (attrPos != -1 && d.bindPos()) {
        prog.enableAttributeArray(attrPos);
        f.glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrNor != -1 && d.bindNor()) {
        prog.enableAttributeArray(attrNor);
        f.glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrCol != -1 && d.bindCol()) {
        prog.enableAttributeArray(attrCol);
        f.glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrJointIDs != -1 && d.bindJID()) {
        prog.enableAttributeArray(attrJointIDs);
        f.glVertexAttribIPointer(attrJointIDs, 2, GL_UNSIGNED_INT, 0, NULL);
    }

    if (attrJointWeights != -1 && d.bindJWeight()) {
        prog.enableAttributeArray(attrJointWeights);
        f.glVertexAttribPointer(attrJointWeights, 2, GL_FLOAT, false, 0, NULL);
    }
    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindIdx();
    f.glDrawElements(d.drawMode(), d.elemCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) prog.disableAttributeArray(attrPos);
    if (attrNor != -1) prog.disableAttributeArray(attrNor);
    if (attrCol!= -1) prog.disableAttributeArray(attrCol);
    if (attrJointIDs != -1) prog.disableAttributeArray(attrJointIDs);
    if (attrJointWeights != -1) prog.disableAttributeArray(attrJointWeights);

    f.printGLErrorLog();
}
