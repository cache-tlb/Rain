#include "GLTexture.h"

static const GLenum fboBuffs[] = { 
    GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, 
    GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7,
    GL_COLOR_ATTACHMENT8, GL_COLOR_ATTACHMENT9, GL_COLOR_ATTACHMENT10, GL_COLOR_ATTACHMENT11, 
    GL_COLOR_ATTACHMENT12, GL_COLOR_ATTACHMENT13, GL_COLOR_ATTACHMENT14, GL_COLOR_ATTACHMENT15, };

GLTexture::GLTexture(int w, int h, const TextureOptions &options)
	: width(w), height(h)
{
    if (!options.check()) {
        printf("warning: bad texture option");
    }

	format = options.format;
	type = options.type;
    internal_format = GL_RGBA8;
	mag_filter = options.mag_filter;
	min_filter = options.min_filter;
	GLenum wrapS = options.wrapS, wrapT = options.wrapT;

    num_output = options.num_output;
    is_fp16 = options.is_fp16;

    // default internal_format 
    if (format == GL_RGBA) {
        if (type == GL_UNSIGNED_BYTE) internal_format = GL_RGBA8;
        else if (type == GL_FLOAT) {
            if (is_fp16) internal_format = GL_RGBA16F_ARB;
            else internal_format = GL_RGBA32F;
        } else printf("only support GL_FLOAT and GL_UNSIGNED_BYTE for texture data type\n");
    } else {
        printf("weird: format != GL_RGBA not supported now.\n");
    }

	framebuffer = 0;
	renderbuffer = 0;

    ids.resize(num_output);
    for (int k = 0; k < num_output; k++) {
        glGenTextures(1, &ids[k]);
        glBindTexture(GL_TEXTURE_2D, ids[k]);
        // TODO gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

        if (width*height < 8192 * 8192) {
            if (type == GL_FLOAT) {
                float *f = new float[width*height * 4];
                for (int i = 0; i < width*height * 4; i++) {
                    f[i] = 0.f;
                }
                glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, f);
                delete[] f;
            }
            else {
                unsigned char *uc = new unsigned char[width*height * 4];
                for (int i = 0; i < width*height * 4; i++) {
                    uc[i] = 0;
                }
                glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, uc);
                delete[] uc;
            }
        } else {
            unsigned char *uc = new unsigned char[width*height*4];
            for (int i = 0; i < width*height*4; i++) {
                uc[i] = 0;
            }
            glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, uc);
            delete[] uc;
        }
    }
}

void GLTexture::bind(int unit /* = 0 */, int which /*= 0*/) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ids[which]);
}

void GLTexture::unbind(int unit /* = 0 */) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

bool GLTexture::canDrawTo() {
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, ids[0], 0);
	bool result = glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT;
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
	return result;
}

void GLTexture::drawTo(const std::function< void(void) >& callback, int clear_flag) {
	GLint v[4];
	preDrawTo(v, clear_flag);
	callback();
	postDrawTo(v);
}

void GLTexture::drawTo(const std::function< void(void) >& callback, int clear_flag, int vx, int vy, int vw, int vh) {
    GLint v[4];
    preDrawTo(v, clear_flag, vx, vy, vw, vh);
    callback();
    postDrawTo(v);
}



void GLTexture::preDrawTo(GLint v[4], int clear_flag, int vx, int vy, int vw, int vh) {
	// note: try glPushAttrib( GL_VIEWPORT_BIT );
	glGetIntegerv(GL_VIEWPORT, v);
	if (!framebuffer) glGenFramebuffers(1, &framebuffer);
	if (!renderbuffer) glGenRenderbuffers(1, &renderbuffer);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, framebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, renderbuffer);
    glDrawBuffers(num_output, fboBuffs);
	GLint renderbuffer_width, renderbuffer_height;
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &renderbuffer_width);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &renderbuffer_height);
	if (width != renderbuffer_width || height != renderbuffer_height) {
		// verify this
		glRenderbufferStorage(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, width, height);
	}
    for (int i = 0; i < num_output; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, ids[i], 0);         // TODO: note here for multiple output texture, also change bind function if multiple output added.
    }
	glFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderbuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT) {
		std::cerr << "Rendering to this texture is not supported (incomplete framebuffer)" << std::endl;
	}
    if (vw < 0 || vh < 0) {
        glViewport(0, 0, width, height);
    } else {
        glViewport(vx, vy, vw, vh);
    }
	if (clear_flag & 1)
		glClear(GL_COLOR_BUFFER_BIT);
	if (clear_flag & 2)
		glClear(GL_DEPTH_BUFFER_BIT);

}

void GLTexture::postDrawTo(GLint v[4]) {
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
	glBindRenderbuffer(GL_RENDERBUFFER_EXT, 0);
	glViewport(v[0], v[1], v[2], v[3]);

}

void GLTexture::toCvMat(cv::Mat &im_32fc4, int which) {
    im_32fc4.create(height, width, CV_32FC4);
    glBindTexture(GL_TEXTURE_2D, ids[which]);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, im_32fc4.data);
}

void GLTexture::toArray(float *f, int which) {
    glBindTexture(GL_TEXTURE_2D, ids[which]);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, f);
}

std::shared_ptr<GLTexture> GLTexture::create(int w, int h, const TextureOptions &options) {
    return std::shared_ptr<GLTexture>(new GLTexture(w, h, options));
}

std::shared_ptr<GLTexture> GLTexture::fromCvMat(cv::Mat &im_c3, const TextureOptions &options) {
	int w = im_c3.cols, h = im_c3.rows;
    if (im_c3.channels() != 3) qDebug() << "image channel != 3 in GLTexture::fromCvMat";
    std::shared_ptr<GLTexture> texture(new GLTexture(w, h, options));
    void *data = NULL;
    std::vector<float> data_f;
    std::vector<uchar> data_uc;
    if (texture->type == GL_FLOAT) {
        if (im_c3.type() != CV_32FC3) qDebug() << "GLTexture::fromCvMat: im type not 32fc3";
        data_f.resize(w*h * 4);
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                cv::Vec3f c = im_c3.at<cv::Vec3f>(i, j);    // rgb
                data_f[i*w * 4 + j * 4 + 0] = c[0];
                data_f[i*w * 4 + j * 4 + 1] = c[1];
                data_f[i*w * 4 + j * 4 + 2] = c[2];
                data_f[i*w * 4 + j * 4 + 3] = 1;
            }
        }
        data = &data_f[0];
    }
    else if (texture->type == GL_UNSIGNED_BYTE) {
        if (im_c3.type() != CV_8UC3) qDebug() << "GLTexture::fromCvMat: im type not 8uc3";
        data_uc.resize(w*h * 4);
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                cv::Vec3b c = im_c3.at<cv::Vec3b>(i, j);
                data_uc[i*w * 4 + j * 4 + 0] = c[2];        // bgr
                data_uc[i*w * 4 + j * 4 + 1] = c[1];
                data_uc[i*w * 4 + j * 4 + 2] = c[0];
                data_uc[i*w * 4 + j * 4 + 3] = 1;
            }
        }
        data = &data_uc[0];
    }
    else {
        qDebug() << "GLTexture::fromCvMat: texture.type not supported now";
    }

	glBindTexture(GL_TEXTURE_2D, texture->ids[0]);
    // note: glTexImage2D will clamp the value to [0,1]
	glTexImage2D(GL_TEXTURE_2D, 0, texture->internal_format, texture->width, texture->height, 0, texture->format, texture->type, data);

	if (texture->min_filter != GL_NEAREST && texture->min_filter != GL_LINEAR) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	return texture;
}

void GLTexture::setDataFromCvMat32FC4(cv::Mat &im_32fc4, int x_offset, int y_offset) {
    glBindTexture(GL_TEXTURE_2D, ids[0]);
    // note: glTexSubImage2D will not clamp the input value to [0,1]
    glTexSubImage2D(GL_TEXTURE_2D, 0, x_offset, y_offset, im_32fc4.cols, im_32fc4.rows, GL_RGBA, GL_FLOAT, im_32fc4.data);
    if (min_filter != GL_NEAREST && min_filter != GL_LINEAR) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}

GLTexture::~GLTexture() {
    for (int i = 0; i < ids.size(); i++) {
        glDeleteTextures(1, &ids[i]);
    }
}

void GLTexture::swapWith(std::shared_ptr<GLTexture> that) {
    if (that->num_output != num_output)
        printf("error in GLTexture::swapWith: number of output not equal!\n");
    std::swap(this->ids, that->ids);
    std::swap(this->height, that->height);
    std::swap(this->width, that->width);
}
