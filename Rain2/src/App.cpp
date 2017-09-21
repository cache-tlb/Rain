#include "App.h"
#include <GL/freeglut.h>

std::shared_ptr<BasicWeather> weatherData[] = {
    std::shared_ptr<BasicWeather>(new RainWeather),
    std::shared_ptr<BasicWeather>(new StormWeather),
    std::shared_ptr<BasicWeather>(new FalloutWeather),
    std::shared_ptr<BasicWeather>(new DrizzleWeather),
    std::shared_ptr<BasicWeather>(new SunnyWeather)
};

App::App(int width, int height) : 
    textureBgSize_(384, 256), textureFgSize_(96, 64),
    width_(width), height_(height),
    is_running_(true)
{}

void App::loadTextures() {
    // TODO: load other textures
    cv::Mat dropAlphaIm = cv::imread("./data/drop-alpha.png", CV_LOAD_IMAGE_UNCHANGED);
    cv::Mat dropColorIm = cv::imread("./data/drop-color.png", CV_LOAD_IMAGE_UNCHANGED);
    cv::Mat textureRainFgIm = cv::imread("./data/texture-rain-fg.png");
    cv::Mat textureRainBgIm = cv::imread("./data/texture-rain-bg.png");

    dropColor_ = Canvas::fromCvMat(dropColorIm);
    dropAlpha_ = Canvas::fromCvMat4Chan(dropAlphaIm);
    textureRainFg_ = Canvas::fromCvMat(textureRainFgIm);
    textureRainBg_ = Canvas::fromCvMat(textureRainBgIm);
}

void App::init() {
    loadTextures();
    
    int dpi = 1; //window.devicePixelRatio;
    canvas_ = Canvas::create(width_*dpi, height_*dpi);

    RainDropsOption options;
    options.trailRate = 1;
    options.trailScaleRange = cv::Vec2d(0.2, 0.45);
    // options.collisionRadius = 0.45; old
    options.collisionRadius = 1.0;
    options.dropletsCleaningRadiusMultiplier = 0.28;
    raindrops_ = std::shared_ptr<Raindrops>(new Raindrops(canvas_->width(), canvas_->height(), dpi, dropAlpha_, dropColor_, options));

    textureFg_ = Canvas::create(textureFgSize_.width, textureFgSize_.height);
    textureFgCtx_ = textureFg_->getContext();
    textureBg_ = Canvas::create(textureBgSize_.width, textureBgSize_.height);
    textureBgCtx_ = textureBg_->getContext();

    generateTextures(textureRainFg_, textureRainBg_);

    RendererOptions render_opt;
    render_opt.brightness = 1.04;
    render_opt.alphaMultiply = 6;
    render_opt.alphaSubtract = 3;
    render_opt.parallaxBg = 0;
    render_opt.parallaxFg = 0;
    renderer_ = std::shared_ptr<Renderer>(new Renderer(canvas_, raindrops_->getCanvas(), textureFg_, textureBg_, render_opt));


    setupEvents();
}

void App::setupEvents() {
    setupParallax();
    setupWeather();
    setupFlash();
}

void App::setupParallax() {
    // TODO
}

void App::setupFlash() {
    // TODO
}

void App::setupWeather() {
    setupWeatherData();
    // TODO: add listener to weather change
    updateWeather();
}

void App::setupWeatherData() {
    // nothing here
}

void App::updateWeather() {
    std::shared_ptr<BasicWeather> data = weatherData[0];    // TODO
    RainDropsOption options;
    options.minR = data->minR;
    options.maxR = data->maxR;
    options.rainChance = data->rainChance;
    options.rainLimit = data->rainLimit;
    options.dropletsRate = data->dropletsRate;
    options.dropletsSize = data->dropletsSize;
    options.raining = data->raining;
    options.trailRate = data->trailRate;
    options.trailScaleRange = data->trailScaleRange;
    options.collisionRadiusIncrease = data->collisionRadiusIncrease;
    raindrops_->setOption(options);
    raindrops_->clearDrops();
}

void App::generateTextures(std::shared_ptr<Canvas> fg, std::shared_ptr<Canvas> bg, double alpha/* =1 */) {
    textureFgCtx_->globalAlpha = alpha;
    textureFgCtx_->drawImage(fg, 0, 0, textureFgSize_.width, textureFgSize_.height);

    textureBgCtx_->globalAlpha = alpha;
    textureBgCtx_->drawImage(bg, 0, 0, textureBgSize_.width, textureBgSize_.height);
}

void App::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (is_running_)
        raindrops_->update();
    renderer_->draw();
    glutSwapBuffers();
}

void App::resize(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
}

void App::processKeys(unsigned char key, int xx, int yy) {
    if (key == 27) {
        glutLeaveMainLoop();
    }
    switch (key) {
    case ' ':
        is_running_ = !is_running_; break;
    case 'r':
        renderer_->setType(1); break;
    case 'a':
        renderer_->setType(2); break;
    case 'i':
        renderer_->setType(0); break;
    default:
        break;
    }
}

void App::processMouseButtons(int button, int state, int xx, int yy) {

}

void App::processMouseMotion(int xx, int yy) {

}

void App::processMouseWheel(int button, int dir, int x, int y) {

}

void App::render() {
    draw();
}

void App::idle() {
    render();
}
