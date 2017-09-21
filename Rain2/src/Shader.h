#ifndef SHADER_H
#define SHADER_H

#include "VSShaderLib.h"
#include "VSMathLib.h"
#include "GLMesh.h"
#include "GLTexture.h"
// #include "OpenGLVersion.h"

class Shader {
public:
    // now the vertex and fragment source mean the file path
    static std::shared_ptr<Shader> load(std::shared_ptr<VSMathLib> vsml, const std::string &vertexSource, const std::string &fragmentSource);
    ~Shader();

    //GLuint program;
    VSShaderLib vsshader;

    void draw_mesh(std::shared_ptr<GLMesh> mesh);
    void draw_wireframe(std::shared_ptr<GLMesh> mesh, float line_width = 1);
    void draw_point(std::shared_ptr<GLMesh> mesh, float point_size = 1);
    void draw(std::shared_ptr<GLMesh> mesh);

    void draw_instance(std::shared_ptr<GLMesh> mesh, int amount);

    // before using renderTo, the uniforms should be set first.
    //void renderTo(Mesh *mesh, Texture *texture);

    void uniforms(const std::string &name, void *val);
    void uniforms(const std::string &name, float val);
    void uniforms(const std::string &name, int val);

private:
    // QOpenGLFunctionsType *context;
    Shader(std::shared_ptr<VSMathLib> vsml, const std::string &vertexSource, const std::string &fragmentSource, const Option &extra_attributes = Option());
    std::shared_ptr<VSMathLib> vsml;
};

#endif // SHADER_H
