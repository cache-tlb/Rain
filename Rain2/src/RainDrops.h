#ifndef RAINDROPS_H
#define RAINDROPS_H

#include "common.h"
#include "CanvasWrapper.h"
#include <opencv2/opencv.hpp>

struct Drop {
    double x, y, r;
    double spreadX, spreadY;
    double momentum, momentumX;
    double lastSpawn, nextSpawn;
    std::shared_ptr<Drop> parent;
    bool isNew, killed;
    double shrink;

    Drop() : 
        x(0), y(0), r(0), 
        spreadX(0), spreadY(0), 
        momentum(0), momentumX(0),
        lastSpawn(0), nextSpawn(0),
        isNew(true), killed(false),
        shrink(0) 
    {}

};

struct RainDropsOption {
    double minR, maxR;
    int maxDrops;
    double rainChance;
    int rainLimit;
    double dropletsRate;
    cv::Vec2d dropletsSize;
    double dropletsCleaningRadiusMultiplier;
    bool raining;
    double globalTimeScale;
    double trailRate;
    bool autoShrink;
    cv::Vec2d spawnArea;
    cv::Vec2d trailScaleRange;
    double collisionRadius;
    double collisionRadiusIncrease;
    double dropFallMultiplier;
    double collisionBoostMultiplier;
    double collisionBoost;

    RainDropsOption() :
        minR(10), maxR(40),
        maxDrops(40), rainChance(0.3), rainLimit(3), dropletsRate(50),
        dropletsSize(2,4), dropletsCleaningRadiusMultiplier(0.43),
        raining(true), globalTimeScale(1), trailRate(1), autoShrink(true),
        spawnArea(-0.1,0.95), trailScaleRange(0.2,0.5),
        collisionRadius(0.65), collisionRadiusIncrease(0.01),
        dropFallMultiplier(1), collisionBoostMultiplier(0.05), collisionBoost(1) 
    {}
};

const int dropSize = 64;

class Raindrops {
public:
    Raindrops(int width, int height, double scale, std::shared_ptr<Canvas> dropAlpha, std::shared_ptr<Canvas> dropColor, const RainDropsOption &options);
    void init();
    void drawDroplet(double x, double y, double r); // can be refined
    void renderDropsGfx();
    void update();
    void drawDrop(std::shared_ptr<CanvasContext> ctx, std::shared_ptr<Drop> drop);  // drawDrop can be refined
    void clearDroplets(double x, double y, double r = 30);
    void clearCanvas();
    std::shared_ptr<Drop> createDrop(std::shared_ptr<Drop> drop);   // createDrop can be refined
    bool addDrop(std::shared_ptr<Drop> drop);
    void updateRain(double timeScale, std::vector<std::shared_ptr<Drop> > &rainDrops);
    void clearDrops();
    void clearTexture();
    void updateDroplets(double timeScale);
    void updateDrops(double timeScale);
    
    inline void setOption(const RainDropsOption &options) {
        options_ = options;
    }

    inline std::shared_ptr<Canvas> getCanvas() const;

    inline double deltaR() const;
    inline double area() const;
    inline double areaMultiplier() const;

private:
    int width_, height_;
    double scale_;
    double dropletsPixelDensity_;
    double dropletsCounter_;
    double textureCleaningIterations_;

    std::shared_ptr<Canvas> dropAlpha_;
    std::shared_ptr<Canvas> dropColor_;
    RainDropsOption options_;

    std::shared_ptr<Canvas> canvas_;
    std::shared_ptr<CanvasContext> ctx_;
    
    std::shared_ptr<Canvas> droplets_;
    std::shared_ptr<CanvasContext> dropletsCtx_;

    std::vector<std::shared_ptr<Drop> > drops_;
    std::vector<std::shared_ptr<Canvas> > dropsGfx_;
    std::shared_ptr<Canvas> clearDropletsGfx_;
};


inline double Raindrops::deltaR() const {
    return options_.maxR - options_.minR;
}

inline double Raindrops::area() const {
    return double(width_*height_)/scale_;
}

inline double Raindrops::areaMultiplier() const {
    return sqrt(area()/(1024*768));
}

inline std::shared_ptr<Canvas> Raindrops::getCanvas() const {
    return canvas_;
}

#endif
