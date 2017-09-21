#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#include "CanvasWrapper.h"

struct RendererOptions {
    bool renderShadow;
    float minRefraction, maxRefraction;
    float brightness;
    float alphaMultiply, alphaSubtract;
    float parallaxBg, parallaxFg;

    RendererOptions() : 
        renderShadow(false), 
        minRefraction(256), maxRefraction(512), 
        brightness(1), 
        alphaMultiply(20), alphaSubtract(5),
        parallaxBg(5), parallaxFg(20)
    {}
};

class Renderer {
public:
    Renderer(std::shared_ptr<Canvas> canvas, std::shared_ptr<Canvas> canvasLiquid, std::shared_ptr<Canvas> imageFg, std::shared_ptr<Canvas> imageBg, const RendererOptions &options);
    void init();
    void draw();
    inline void setType(int type);

private:
    static const std::string shader_root;
    std::shared_ptr<Canvas> canvas_, canvasLiquid_;
    int width_, height_;
    std::shared_ptr<Canvas> imageFg_, imageBg_;
    // std::vector<?> textures_;
    float parallaxX_, parallaxY_;
    RendererOptions options_;
    std::shared_ptr<GLMesh> plane_;

    std::shared_ptr<Shader> rain_shader_, water_shader_;
    int show_type_;
};

inline void Renderer::setType(int type) {
    show_type_ = type;
}

#endif
