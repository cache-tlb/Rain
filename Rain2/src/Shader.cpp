#include "Shader.h"

std::shared_ptr<Shader> Shader::load(std::shared_ptr<VSMathLib> vsml, const std::string &vertexSource, const std::string &fragmentSource) {
    return std::shared_ptr<Shader>(new Shader(vsml, vertexSource, fragmentSource));
}

Shader::Shader(std::shared_ptr<VSMathLib> vsml, const std::string &vertexSource, const std::string &fragmentSource, const Option &extra_attributes)
    : vsml(vsml), vsshader()
{
    vsshader.init();
    vsshader.loadShader(VSShaderLib::VERTEX_SHADER, vertexSource);
    vsshader.loadShader(VSShaderLib::FRAGMENT_SHADER, fragmentSource);
    vsshader.setProgramOutput(0, "outputF");
    vsshader.setProgramOutput(1, "outputF1");
    vsshader.setProgramOutput(2, "outputF2");
    vsshader.setProgramOutput(3, "outputF3");
    vsshader.setProgramOutput(4, "outputF4");
    vsshader.setProgramOutput(5, "outputF5");
    vsshader.setProgramOutput(6, "outputF6");
    vsshader.setProgramOutput(7, "outputF7");

    vsshader.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");
    vsshader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "uv");
    vsshader.setVertexAttribName(VSShaderLib::NORMAL_ATTRIB, "normal");

    for (auto it = extra_attributes.begin(); it != extra_attributes.end(); it++) {
        int at = parseTo<int>(it->first);
        vsshader.setVertexAttribName(VSShaderLib::AttribType(at), (it->second).c_str());
    }

    vsshader.prepareProgram();
}

void Shader::draw(std::shared_ptr<GLMesh> mesh) {
    // set the uniforms
    glUseProgram(vsshader.getProgramIndex());
    vsml->setUniformName(VSMathLib::NORMAL, "m_normal");
    vsml->setUniformName(VSMathLib::NORMAL_MODEL, "m_normalModel");
    vsml->setUniformName(VSMathLib::MODEL, "m_model");
    vsml->setUniformName(VSMathLib::VIEW, "m_view");
    vsml->setUniformName(VSMathLib::VIEW_MODEL, "m_viewModel");
    vsml->setUniformName(VSMathLib::PROJECTION, "m_projection");
    vsml->setUniformName(VSMathLib::PROJ_VIEW_MODEL, "m_pvm");
    vsml->matricesToGL();
    glBindVertexArray(mesh->vao);
    glDrawElements(GL_TRIANGLES, mesh->triangles.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Shader::draw_instance(std::shared_ptr<GLMesh> mesh, int amount) {
    glUseProgram(vsshader.getProgramIndex());
    vsml->setUniformName(VSMathLib::NORMAL, "m_normal");
    vsml->setUniformName(VSMathLib::NORMAL_MODEL, "m_normalModel");
    vsml->setUniformName(VSMathLib::MODEL, "m_model");
    vsml->setUniformName(VSMathLib::VIEW, "m_view");
    vsml->setUniformName(VSMathLib::VIEW_MODEL, "m_viewModel");
    vsml->setUniformName(VSMathLib::PROJECTION, "m_projection");
    vsml->setUniformName(VSMathLib::PROJ_VIEW_MODEL, "m_pvm");
    vsml->matricesToGL();
    glBindVertexArray(mesh->vao);
    glDrawElementsInstanced(GL_TRIANGLES, mesh->triangles.size(), GL_UNSIGNED_INT, 0, amount);
    glBindVertexArray(0);
}

void Shader::draw_mesh(std::shared_ptr<GLMesh> mesh) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    draw(mesh);
}

void Shader::draw_wireframe(std::shared_ptr<GLMesh> mesh, float line_width) {
    float old_width = 1;
    glGetFloatv(GL_LINE_WIDTH, &old_width);

    glLineWidth(line_width);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    draw(mesh);

    glLineWidth(old_width);
}

void Shader::draw_point(std::shared_ptr<GLMesh> mesh, float point_size) {
    float old_size = 1;
    glGetFloatv(GL_POINT_SIZE, &old_size);

    glPointSize(point_size);
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    draw(mesh);

    glPointSize(old_size);
}

/*void Shader::renderTo(Mesh *mesh, Texture *texture) {
std::function<void(void)> callback = [this, mesh]() {
this->draw(mesh);
};
texture->drawTo(callback);
}*/

void Shader::uniforms(const std::string &name, void *val) {
    glUseProgram(vsshader.getProgramIndex());
    vsshader.setUniform(name, val);
}

void Shader::uniforms(const std::string &name, float val) {
    glUseProgram(vsshader.getProgramIndex());
    vsshader.setUniform(name, val);
}

void Shader::uniforms(const std::string &name, int val) {
    glUseProgram(vsshader.getProgramIndex());
    vsshader.setUniform(name, val);
}

Shader::~Shader() {}
