#ifndef CANVASWRAPPER_H
#define CANVASWRAPPER_H

#include "common.h"
#include "GLTexture.h"
#include "Shader.h"
#include "Vec4.h"
#include <opencv2/opencv.hpp>

/*class CanvasContext;

class Canvas {
public:
    std::shared_ptr<CanvasContext> getContext();
    static std::shared_ptr<Canvas> create(int width, int height);
    static std::shared_ptr<Canvas> fromCvMat(const cv::Mat &im8uc3);
    static std::shared_ptr<Canvas> fromCvMat4Chan(const cv::Mat &im8uc4);
    cv::Mat getIm();
    cv::Mat getRGB();
    cv::Mat getAlpha();
    inline int width() const;
    inline int height()  const;

private:
    Canvas(int width, int height);
    cv::Mat buf_; // CV_32FC4
};

class CanvasContext {
public:
    CanvasContext(Canvas *canvas);

    void clearRect(int x, int y, int width, int height);
    void drawImage(std::shared_ptr<Canvas> image, int x, int y, int width, int height);
    void fillRect(int x, int y, int width, int height);
    void fillCircle(double cx, double cy, double r);

    void showTmpBuf();

    double globalAlpha;
    cv::Vec4f fill_color_;
    std::string globalCompositeOperation;   // "source-over", "source-in", "screen", "destination-out"

private:
    void clearTmp();
    void blend();
    cv::Mat buf_, tmp_buf_;
};

int Canvas::width() const {
    return buf_.cols;
}

int Canvas::height() const {
    return buf_.rows;
}*/

class CanvasContext;

class Canvas {
public:
    std::shared_ptr<CanvasContext> getContext();
    static std::shared_ptr<Canvas> create(int width, int height);
    static std::shared_ptr<Canvas> fromCvMat(const cv::Mat &im8uc3);
    static std::shared_ptr<Canvas> fromCvMat4Chan(const cv::Mat &im8uc4);

    std::shared_ptr<GLTexture> getBuf();

    cv::Mat getRGB();
    cv::Mat getAlpha();
    inline int width() const;
    inline int height()  const;

private:
    Canvas();
    Canvas(int width, int height);
    std::shared_ptr<GLTexture> buf_;
};

class CanvasContext {
public:
    CanvasContext(Canvas *canvas);

    void clearRect(int x, int y, int width, int height);
    void drawImage(std::shared_ptr<Canvas> image, double x, double y, double width, double height);
    void fillRect(int x, int y, int width, int height);
    void fillCircle(double cx, double cy, double r);
    
    double globalAlpha;
    Vec4f fill_color_;
    std::string globalCompositeOperation;   // "source-over", "source-in", "screen", "destination-out"

private:
    void clearTmp(int x, int y, int w, int h);
    void blend(int x, int y, int w, int h);
    void copyToCanvas(int x, int y, int w, int h);
    std::shared_ptr<GLTexture> read_buf_, write_buf_, canvas_buf_;

    const static std::string shader_root;
    inline static std::shared_ptr<GLMesh> reduce_mesh();
    static std::shared_ptr<GLMesh> reduce_mesh_;
    inline static std::shared_ptr<Shader> copy_shader();
    static std::shared_ptr<Shader> copy_shader_;     // 
    inline static std::shared_ptr<Shader> draw_image_shader();
    static std::shared_ptr<Shader> draw_image_shader_;
    inline static std::shared_ptr<Shader> blend_shader();
    static std::shared_ptr<Shader> blend_shader_;
    inline static std::shared_ptr<Shader> fillrect_shader();
    static std::shared_ptr<Shader> fillrect_shader_;
    inline static std::shared_ptr<Shader> fillcircle_shader();
    static std::shared_ptr<Shader> fillcircle_shader_;
};

int Canvas::width() const {
    return buf_->width;
}

int Canvas::height() const {
    return buf_->height;
}

inline std::shared_ptr<GLMesh> CanvasContext::reduce_mesh() {
    if (!reduce_mesh_) reduce_mesh_ = GLMesh::plane();
    return reduce_mesh_;
}

inline std::shared_ptr<Shader> CanvasContext::copy_shader() {
    if (!copy_shader_) 
        copy_shader_ = Shader::load(VSMathLib::getInstance(), shader_root + "copy_tex.vert.glsl", shader_root + "copy_tex.frag.glsl");   
    return copy_shader_;
}

inline std::shared_ptr<Shader> CanvasContext::draw_image_shader() {
    if (!draw_image_shader_) 
        draw_image_shader_ = Shader::load(VSMathLib::getInstance(), shader_root + "copy_tex.vert.glsl", shader_root + "draw_image.frag.glsl");
    return draw_image_shader_;
}

inline std::shared_ptr<Shader> CanvasContext::blend_shader() {
    if (!blend_shader_)
        blend_shader_ = Shader::load(VSMathLib::getInstance(), shader_root + "copy_tex.vert.glsl", shader_root + "blend_tex.frag.glsl");
    return blend_shader_;
}

inline std::shared_ptr<Shader> CanvasContext::fillrect_shader() {
    if (!fillrect_shader_)
        fillrect_shader_ = Shader::load(VSMathLib::getInstance(), shader_root + "copy_tex.vert.glsl", shader_root + "fill_rect.frag.glsl");
    return fillrect_shader_;
}

inline std::shared_ptr<Shader> CanvasContext::fillcircle_shader() {
    if (!fillcircle_shader_)
        fillcircle_shader_ = Shader::load(VSMathLib::getInstance(), shader_root + "copy_tex.vert.glsl", shader_root + "fill_circle.frag.glsl");
    return fillcircle_shader_;
}

#endif
