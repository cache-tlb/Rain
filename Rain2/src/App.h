#ifndef APP_H
#define APP_H

#include "CanvasWrapper.h"
#include "RainDrops.h"
#include "Renderer.h"

struct BasicWeather {
    bool raining;
    double minR, maxR;
    double rainChance, rainLimit, dropletsRate;
    cv::Vec2d dropletsSize;
    double trailRate;
    cv::Vec2d trailScaleRange;
    std::shared_ptr<Canvas> fg, bg;
    std::shared_ptr<Canvas> flashFg, flashBg;
    double flashChance, collisionRadiusIncrease;

    BasicWeather() :
        raining(true),
        minR(20), maxR(50),
        rainChance(0.35), rainLimit(6), dropletsRate(50),
        dropletsSize(3, 5.5),
        trailRate(1),
        trailScaleRange(0.25, 0.35),
        flashChance(0), collisionRadiusIncrease(0.0002)
    {}
};

struct RainWeather : public BasicWeather {
    RainWeather() : BasicWeather() {
        rainChance = 0.35;
        dropletsRate = 50;
        raining = true;
        // bg, fg
    }
};

struct StormWeather : public BasicWeather {
    StormWeather() : BasicWeather() {
        maxR = 55;
        rainChance = 0.4;
        dropletsRate = 80;
        dropletsSize = cv::Vec2d(3,5.5);
        trailRate = 2.5;
        trailScaleRange = cv::Vec2d(0.25,0.4);
        // fg:textureRainFg,
        // bg:textureRainBg,
        // flashFg:textureStormLightningFg,
        // flashBg:textureStormLightningBg,
        flashChance = 0.1;
    }
};

struct FalloutWeather : public BasicWeather {
    FalloutWeather() : BasicWeather() {
        minR = 30;
        maxR = 60;
        rainChance = 0.35;
        dropletsRate = 20;
        trailRate = 4;
        // fg:textureFalloutFg,
        // bg:textureFalloutBg,
        collisionRadiusIncrease = 0;
    }
};

struct DrizzleWeather : public BasicWeather {
    DrizzleWeather() : BasicWeather() {
        minR = 10;
        maxR = 40;
        rainChance = 0.15;
        rainLimit = 2;
        dropletsRate = 10;
        dropletsSize = cv::Vec2d(3.5,6);
        // fg:textureDrizzleFg,
        // bg:textureDrizzleBg
    }
};

struct SunnyWeather : public BasicWeather {
    SunnyWeather() : BasicWeather() {
        rainChance = 0;
        rainLimit = 0;
        // droplets = 0;
        raining = false;
        // fg:textureSunFg,
        // bg:textureSunBg
    }
};

class App {
public:
    App(int width, int height);
    void init();
    void loadTextures();
    void updateWeather();
    void setupEvents();
    void setupParallax();   // TODO
    void setupFlash();      // TODO
    void setupWeather();
    void setupWeatherData();
    void flash(std::shared_ptr<Canvas> baseBg, std::shared_ptr<Canvas> baseFg, std::shared_ptr<Canvas> flashBg, std::shared_ptr<Canvas>flashFg);    // TODO
    void generateTextures(std::shared_ptr<Canvas> fg, std::shared_ptr<Canvas> bg, double alpha=1);

    void draw();

    // for glut
    void resize(int w, int h);
    void render();
    void processKeys(unsigned char key, int xx, int yy);
    void processMouseButtons(int button, int state, int xx, int yy);
    void processMouseMotion(int xx, int yy);
    void processMouseWheel(int button, int dir, int x, int y);
    void idle();

private:
    std::shared_ptr<Canvas> textureRainFg_, textureRainBg_,
                            textureStormLightningFg_, textureStormLightningBg_,
                            textureFalloutFg_, textureFalloutBg_,
                            textureSunFg_, textureSunBg_,
                            textureDrizzleFg_, textureDrizzleBg_,
                            dropColor_, dropAlpha_;

    std::shared_ptr<Canvas> textureFg_, textureBg_;
    std::shared_ptr<CanvasContext> textureFgCtx_, textureBgCtx_;

    std::shared_ptr<Canvas> canvas_;

    std::shared_ptr<Raindrops> raindrops_;

    cv::Size textureFgSize_, textureBgSize_;

    std::shared_ptr<Renderer> renderer_;

    int width_, height_;

    bool is_running_;
};

#endif