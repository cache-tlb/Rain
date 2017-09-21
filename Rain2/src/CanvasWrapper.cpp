#include "CanvasWrapper.h"

Canvas::Canvas(int width, int height) {
    TextureOptions opt;
    opt.type = GL_FLOAT;
    buf_ = GLTexture::create(width, height, opt);
}

Canvas::Canvas() {}

std::shared_ptr<CanvasContext> Canvas::getContext() {
    return std::shared_ptr<CanvasContext>(new CanvasContext(this));
}

std::shared_ptr<Canvas> Canvas::create(int width, int height) {
    return std::shared_ptr<Canvas>(new Canvas(width, height));
}

std::shared_ptr<Canvas> Canvas::fromCvMat(const cv::Mat &im8uc3) {
    TextureOptions opt;
    opt.type = GL_FLOAT;
    cv::Mat im;
    im8uc3.convertTo(im, CV_32F, 1./255);
    cv::cvtColor(im, im, CV_BGR2RGBA);
    std::shared_ptr<Canvas> res(new Canvas());
    res->buf_ = GLTexture::create(im.cols, im.rows, opt);
    res->buf_->setDataFromCvMat32FC4(im, 0, 0);
    return res;
}

std::shared_ptr<Canvas> Canvas::fromCvMat4Chan(const cv::Mat &im8uc4) {
    TextureOptions opt;
    opt.type = GL_FLOAT;
    cv::Mat im;
    im8uc4.convertTo(im, CV_32F, 1./255);
    cv::cvtColor(im, im, CV_BGRA2RGBA);
    std::shared_ptr<Canvas> res(new Canvas());
    res->buf_ = GLTexture::create(im.cols, im.rows, opt);
    res->buf_->setDataFromCvMat32FC4(im, 0, 0);
    return res;
}

std::shared_ptr<GLTexture> Canvas::getBuf() {
    return buf_;
}

cv::Mat Canvas::getRGB() {
    cv::Mat im;
    buf_->toCvMat(im, 0);
    cv::cvtColor(im, im, CV_RGBA2BGR);
    return im;
}

cv::Mat Canvas::getAlpha() {
    cv::Mat im;
    buf_->toCvMat(im, 0);
    std::vector<cv::Mat> channels;
    cv::split(im, channels);
    return channels.back();
}

//////////////////////////////////////////////////////////////////////////

const std::string CanvasContext::shader_root = "./shaders/";
std::shared_ptr<GLMesh> CanvasContext::reduce_mesh_;
std::shared_ptr<Shader> CanvasContext::copy_shader_;
std::shared_ptr<Shader> CanvasContext::draw_image_shader_;
std::shared_ptr<Shader> CanvasContext::blend_shader_;
std::shared_ptr<Shader> CanvasContext::fillrect_shader_;
std::shared_ptr<Shader> CanvasContext::fillcircle_shader_;

CanvasContext::CanvasContext(Canvas *canvas) {
    canvas_buf_ = canvas->getBuf();
    int w = canvas_buf_->width, h = canvas_buf_->height;
    TextureOptions opt;
    read_buf_ = GLTexture::create(w, h, opt);
    write_buf_ = GLTexture::create(w, h, opt);
    globalAlpha = 1.0;
    fill_color_ = Vec4f(0, 0, 0, 1);
    globalCompositeOperation = "source-over";
}

void CanvasContext::copyToCanvas(int x, int y, int w, int h) {
    auto shader = copy_shader();
    write_buf_->bind(0);
    shader->uniforms("tex", 0);
    shader->uniforms("tex_width", canvas_buf_->width);
    shader->uniforms("tex_height", canvas_buf_->height);
    shader->uniforms("x", x);
    shader->uniforms("y", y);
    shader->uniforms("w", w);
    shader->uniforms("h", h);
    auto cb = [this, shader]() {
        shader->draw_mesh(reduce_mesh());
    };
    canvas_buf_->drawTo(cb, 2, x, y, w, h);
}

void CanvasContext::clearRect(int x, int y, int width, int height) {
    // clearTmp(x, y, width, height);
    auto cb = [this]() {
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    };
    canvas_buf_->drawTo(cb, 3, x, y, width, height);
}

void CanvasContext::drawImage(std::shared_ptr<Canvas> image, double x, double y, double width, double height) {
    int x1 = floor(x), y1 = floor(y);
    int x2 = ceil(x+width), y2 = ceil(y+height);
    int ww = x2 - x1, hh = y2 - y1;
    // clearTmp(x1, y1, ww, hh);
    image->getBuf()->bind(0);
    auto shader = draw_image_shader();
    shader->uniforms("tex", 0);
    shader->uniforms("x_int", x1);
    shader->uniforms("y_int", y1);
    shader->uniforms("w_int", ww);
    shader->uniforms("h_int", hh);
    shader->uniforms("x_float", float(x));
    shader->uniforms("y_float", float(y));
    shader->uniforms("w_float", float(width));
    shader->uniforms("h_float", float(height));
    auto cb = [this, shader]() {
        shader->draw_mesh(reduce_mesh());
    };
    read_buf_->drawTo(cb, 2, x1, y1, ww, hh);
    blend(x1, y1, ww, hh);
}

void CanvasContext::fillRect(int x, int y, int width, int height) {
    // clearTmp(x, y, width, height);
    auto shader = fillrect_shader();
    shader->uniforms("fill_color", (void*)fill_color_.val);
    auto cb = [this, shader]() {
        shader->draw_mesh(reduce_mesh());
    };
    read_buf_->drawTo(cb, 2, x, y, width, height);
    blend(x, y, width, height);
}

void CanvasContext::fillCircle(double cx, double cy, double r) {
    int x = floor(cx - r), y = floor(cy - r);
    int w = 2*r, h = 2*r;
    clearTmp(x, y, w, h);
    auto shader = fillcircle_shader();
    shader->uniforms("fill_color", (void*)fill_color_.val);
    auto cb = [this, shader]() {
        shader->draw_mesh(reduce_mesh());
    };
    read_buf_->drawTo(cb, 2, x, y, w, h);
    blend(x, y, w, h);
}


void CanvasContext::clearTmp(int x, int y, int w, int h) {
    glClearColor(0,0,0,0);
    auto cb = [this]() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    };
    write_buf_->drawTo(cb, 2, x, y, w, h);
    glClearColor(0,0,0,0);
}

void CanvasContext::blend(int x, int y, int w, int h) {
    /*
    for (int i = 0; i < buf_.rows*buf_.cols; i++) {
        cv::Vec4f c;
        cv::Vec4f src_pixel = tmp_buf_.at<cv::Vec4f>(i);
        cv::Vec4f dst_pixel = buf_.at<cv::Vec4f>(i);
        if (globalCompositeOperation == "source-over") {
            // if (src_pixel[3] > 0) c = src_pixel;
            // else c = dst_pixel;
            c = src_pixel*src_pixel[3] + dst_pixel*(1-src_pixel[3]);
            c[3] = std::max(src_pixel[3], dst_pixel[3]);
        } else if (globalCompositeOperation == "source-in") {
            c = src_pixel;
            c[3] = dst_pixel[3];
        } else if (globalCompositeOperation == "screen") {
            cv::Vec4f c1 = -src_pixel + cv::Vec4f(1,1,1,1);
            cv::Vec4f c2 = -dst_pixel + cv::Vec4f(1,1,1,1);
            cv::multiply(c1,c2,c);
            c = -c + cv::Vec4f(1,1,1,1);
        } else if (globalCompositeOperation == "destination-out") {
            c = dst_pixel*(1-src_pixel[3]);
            c[3] = 1;                           // may not have this
        } else {
            qDebug() << "fatal: unsupported composite operation";
        }
        buf_.at<cv::Vec4f>(i) = c;
    }*/
    int mode = -1;
    if (globalCompositeOperation == "source-over") {
        mode = 0;
    } else if (globalCompositeOperation == "source-in") {
        mode = 1;
    } else if (globalCompositeOperation == "screen") {
        mode = 2;
    } else if (globalCompositeOperation == "destination-out") {
        mode = 3;
    } else {
        qDebug() << "fatal: unsupported composite operation";
    }
    auto shader = blend_shader();
    read_buf_->bind(0);
    canvas_buf_->bind(1);
    shader->uniforms("src", 0);
    shader->uniforms("dst", 1);
    shader->uniforms("mode", mode);
    shader->uniforms("x", x);
    shader->uniforms("y", y);
    shader->uniforms("w", w);
    shader->uniforms("h", h);
    shader->uniforms("tex_width", write_buf_->width);
    shader->uniforms("tex_height", write_buf_->height);
    auto cb = [this, shader]() {
        shader->draw_mesh(reduce_mesh());
    };
    write_buf_->drawTo(cb, 2, x, y, w, h);
    copyToCanvas(x, y, w, h);
}
