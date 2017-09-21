#ifndef GLTEXTURE_H
#define GLTEXTURE_H

#include "common.h"
#include <opencv2/opencv.hpp>

struct TextureOptions {
    GLenum format, type;
    GLenum wrapS, wrapT;
    GLint internal_format;
    GLint mag_filter, min_filter;
    int num_output;
    bool is_fp16;

    TextureOptions() : 
        format(GL_RGBA), type(GL_UNSIGNED_BYTE), 
        // internal_format(GL_RGBA32F), 
        mag_filter(GL_LINEAR), min_filter(GL_LINEAR), 
        wrapS(GL_REPEAT), wrapT(GL_REPEAT),
        num_output(1), is_fp16(false) {}
    bool check () const {
        return (format == GL_RGBA) && 
               (type == GL_UNSIGNED_BYTE || type == GL_FLOAT) &&
               (wrapS == GL_CLAMP_TO_EDGE || wrapS == GL_MIRRORED_REPEAT || wrapS == GL_REPEAT) &&
               (wrapT == GL_CLAMP_TO_EDGE || wrapT == GL_MIRRORED_REPEAT || wrapT == GL_REPEAT) &&
               (mag_filter == GL_NEAREST || mag_filter == GL_LINEAR) && 
               (min_filter == GL_NEAREST || min_filter == GL_LINEAR || min_filter == GL_NEAREST_MIPMAP_NEAREST || min_filter == GL_LINEAR_MIPMAP_NEAREST || min_filter == GL_NEAREST_MIPMAP_LINEAR || min_filter == GL_LINEAR_MIPMAP_LINEAR);
    }
};

class GLTexture {
public:
    ~GLTexture();

    void bind(int unit = 0, int which = 0);
    static void unbind(int unit = 0);
    bool canDrawTo();
    void drawTo(const std::function< void(void) >& callback, int clear_flag);
    void drawTo(const std::function< void(void) >& callback, int clear_flag, int vx, int vy, int vw, int vh);
    void swapWith(std::shared_ptr<GLTexture> that);

    void toCvMat(cv::Mat &im_32fc4, int which);
    void setDataFromCvMat32FC4(cv::Mat &im_32fc4, int x_offset, int y_offset);
    void toArray(float *f, int which);

    void preDrawTo(GLint v[4], int clear_flag, int vx = 0, int vy = 0, int vw = -1, int vh = -1);
    void postDrawTo(GLint v[4]);

    std::vector<GLuint> ids;

    int width, height;
    int num_output;
    bool is_fp16;

    GLenum format;
    GLenum type;
    GLint internal_format;

    GLint mag_filter, min_filter;
    static std::shared_ptr<GLTexture> fromCvMat(cv::Mat &im_c3, const TextureOptions &options);
    static std::shared_ptr<GLTexture> create(int w, int h, const TextureOptions &options);

protected:
    GLTexture(int w, int h, const TextureOptions &options);

    GLuint framebuffer, renderbuffer;
    // QOpenGLFunctionsType *context;
};

#endif // GLTEXTURE_H
