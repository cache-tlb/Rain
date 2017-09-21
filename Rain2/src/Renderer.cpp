#include "Renderer.h"

const std::string Renderer::shader_root = "./shaders/";

Renderer::Renderer(std::shared_ptr<Canvas> canvas, std::shared_ptr<Canvas> canvasLiquid, std::shared_ptr<Canvas> imageFg, std::shared_ptr<Canvas> imageBg, const RendererOptions &options) :
    canvas_(canvas), canvasLiquid_(canvasLiquid), imageFg_(imageFg), imageBg_(imageBg), options_(options), 
    show_type_(0)
{
    init();
}

void Renderer::init() {
    width_ = canvas_->width();
    height_ = canvas_->height();

    // init shader
    rain_shader_ = Shader::load(VSMathLib::getInstance(), shader_root + "copy_tex.vert.glsl", shader_root + "rain.frag.glsl");
    water_shader_ = Shader::load(VSMathLib::getInstance(), shader_root + "copy_tex.vert.glsl", shader_root + "show_tex.frag.glsl");
    Vec2f resolution = Vec2f(width_, height_);
    rain_shader_->uniforms("u_resolution", (void*)resolution.val);
    rain_shader_->uniforms("u_textureRatio", float(imageBg_->width())/float(imageBg_->height()));
    rain_shader_->uniforms("u_renderShine", 0);
    rain_shader_->uniforms("u_renderShadow", options_.renderShadow);
    rain_shader_->uniforms("u_minRefraction", options_.minRefraction);
    rain_shader_->uniforms("u_refractionDelta", options_.maxRefraction - options_.minRefraction);
    rain_shader_->uniforms("u_brightness", options_.brightness);
    rain_shader_->uniforms("u_alphaMultiply", options_.alphaMultiply);
    rain_shader_->uniforms("u_alphaSubtract", options_.alphaSubtract);
    rain_shader_->uniforms("u_parallaxBg", options_.parallaxBg);
    rain_shader_->uniforms("u_parallaxFg", options_.parallaxFg);

    // create texture;

    rain_shader_->uniforms("u_waterMap", 0);
    rain_shader_->uniforms("u_textureShine", 1);
    rain_shader_->uniforms("u_textureFg", 2);
    rain_shader_->uniforms("u_textureBg", 3);

    plane_ = GLMesh::plane();

    draw();
}

void Renderer::draw() {
    if (show_type_ == 0) {
        Vec2f parallax(parallaxX_, parallaxY_);
        // rain_shader_->uniforms("u_parallax", (void*)parallax.val);
        canvasLiquid_->getBuf()->bind(0);
        // canvasLiquid_->getBuf()->bind(1);
        imageFg_->getBuf()->bind(2);
        imageBg_->getBuf()->bind(3);
        rain_shader_->draw_mesh(plane_);
    } else if (show_type_ == 1) {
        water_shader_->uniforms("type", 0);
        water_shader_->uniforms("flip_y", 1);
        water_shader_->draw_mesh(plane_);
    } else {
        water_shader_->uniforms("type", 1);
        water_shader_->uniforms("flip_y", 1);
        water_shader_->draw_mesh(plane_);
    }
}