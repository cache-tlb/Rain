#include "RainDrops.h"
#include "util.h"

Raindrops::Raindrops(int width, int height, double scale, std::shared_ptr<Canvas> dropAlpha, std::shared_ptr<Canvas> dropColor, const RainDropsOption &options) : 
    dropletsPixelDensity_(1), dropletsCounter_(0), textureCleaningIterations_(0), 
    width_(width), height_(height), scale_(scale), 
    dropAlpha_(dropAlpha), dropColor_(dropColor),
    options_(options) 
{
    init();
}

void Raindrops::init() {
    canvas_ = Canvas::create(width_, height_);
    ctx_ = canvas_->getContext();

    droplets_ = Canvas::create(width_*dropletsPixelDensity_, height_*dropletsPixelDensity_);
    dropletsCtx_ = droplets_->getContext();

    drops_.clear();
    dropsGfx_.clear();

    renderDropsGfx();
    update();
}

void Raindrops::drawDroplet(double x, double y, double r) {
    std::shared_ptr<Drop> drop(new Drop);
    drop->x = x*dropletsPixelDensity_;
    drop->y = y*dropletsPixelDensity_;
    drop->r = r*dropletsPixelDensity_;
    drawDrop(dropletsCtx_, drop);
}

void Raindrops::renderDropsGfx() {
    auto dropBuffer = Canvas::create(dropSize, dropSize);
    auto dropBufferCtx = dropBuffer->getContext();
    for (int i = 0; i < 255; i++) {
        auto drop = Canvas::create(dropSize, dropSize);
        auto dropCtx = drop->getContext();

        dropBufferCtx->clearRect(0, 0, dropSize, dropSize);

        // color
        dropBufferCtx->globalCompositeOperation = "source-over";
        dropBufferCtx->drawImage(dropColor_, 0, 0, dropSize, dropSize);

        // blue overlay, for depth
        dropBufferCtx->globalCompositeOperation = "screen";
        dropBufferCtx->fill_color_ = Vec4f(0, 0, double(i)/255., 1);
        dropBufferCtx->fillRect(0, 0, dropSize, dropSize);
        
        // alpha
        dropCtx->globalCompositeOperation = "source-over";
        dropCtx->drawImage(dropAlpha_, 0, 0, dropSize, dropSize);

        dropCtx->globalCompositeOperation = "source-in";
        dropCtx->drawImage(dropBuffer, 0, 0, dropSize, dropSize);

        dropsGfx_.push_back(drop);

    }

    // create circle that will be used as a brush to remove droplets
    clearDropletsGfx_ = Canvas::create(128,128);
    auto clearDropletsCtx = clearDropletsGfx_->getContext();
    clearDropletsCtx->fill_color_ = Vec4f(0,0,0,1);
    clearDropletsCtx->fillCircle(64,64,48);
}

void Raindrops::drawDrop(std::shared_ptr<CanvasContext> ctx, std::shared_ptr<Drop> drop) {
    if (dropsGfx_.size() > 0) {
        double x = drop->x, y = drop->y, r = drop->r;
        double spreadX = drop->spreadX, spreadY = drop->spreadY;
        
        double scaleX = 1, scaleY = 1.5;

        double d = std::max(0., std::min(1., ((r-options_.minR)/(deltaR()))*0.9));
        d *= 1./(((drop->spreadX + drop->spreadY)*0.5) + 1);

        ctx->globalAlpha = 1;
        ctx->globalCompositeOperation = "source-over";

        d = floor(d*(dropsGfx_.size()-1));
        ctx->drawImage(
            dropsGfx_[d],
            (x-(r*scaleX*(spreadX+1)))*scale_,
            (y-(r*scaleY*(spreadY+1)))*scale_,
            (r*2*scaleX*(spreadX+1))*scale_,
            (r*2*scaleY*(spreadY+1))*scale_
        );
    }
}

void Raindrops::clearDroplets(double x, double y, double r) {
    auto ctx = dropletsCtx_;
    ctx->globalCompositeOperation = "destination-out";
    ctx->drawImage(
        clearDropletsGfx_,
        (x-r)*dropletsPixelDensity_*scale_,
        (y-r)*dropletsPixelDensity_*scale_,
        (r*2)*dropletsPixelDensity_*scale_,
        (r*2)*dropletsPixelDensity_*scale_*1.25
    );
}

void Raindrops::clearCanvas() {
    ctx_->clearRect(0, 0, width_, height_);
}

bool Raindrops::addDrop(std::shared_ptr<Drop> drop) {
    if (drops_.size() >= options_.maxDrops*areaMultiplier() || !drop) return false;

    drops_.push_back(drop);
    return true;
}

void Raindrops::clearTexture() {
    textureCleaningIterations_ = 50;
}

std::shared_ptr<Drop> Raindrops::createDrop(std::shared_ptr<Drop> drop) {
    if(drops_.size() >= options_.maxDrops*areaMultiplier()) return std::shared_ptr<Drop>();
    else return drop;
}

void Raindrops::clearDrops() {
    /*this.drops.forEach((drop)=>{
        setTimeout(()=>{
            drop.shrink=0.1+(random(0.5));
        },random(1200))
    })*/
    for (int i = 0; i < drops_.size(); i++) {
        auto drop = drops_[i];
        double shrink = 0.1 + random(0.5);
        double exe_time = random(1200.);
        drop->shrink = shrink;
        // Later later(exe_time, true, [drop, shrink](){drop->shrink = shrink;});
    }
    clearTexture();
}

void Raindrops::updateDroplets(double timeScale) {
    if(textureCleaningIterations_ > 0) {
        textureCleaningIterations_ -= 1*timeScale;
        dropletsCtx_->globalCompositeOperation = "destination-out";
        // this.dropletsCtx.fillStyle="rgba(0,0,0,"+(0.05*timeScale)+")";
        dropletsCtx_->fill_color_ = Vec4f(0, 0, 0, 0.05*timeScale);         // note this
        dropletsCtx_->fillRect(0, 0, width_*dropletsPixelDensity_, height_*dropletsPixelDensity_);
    }
    if(options_.raining) {
        dropletsCounter_ += options_.dropletsRate*timeScale*areaMultiplier();
        int n = dropletsCounter_;
        for (int i = 0; i < n; i++) {
            dropletsCounter_ -= 1;
            drawDroplet(
                random(width_/scale_),
                random(height_/scale_),
                random<double>(options_.dropletsSize[0], options_.dropletsSize[1], [](double n){return n*n;})
            );
        }
    }
    ctx_->drawImage(droplets_, 0, 0, width_, height_);
}

void Raindrops::updateRain(double timeScale, std::vector<std::shared_ptr<Drop> > &rainDrops) {
    rainDrops.clear();
    if (!options_.raining) return;
    double limit = options_.rainLimit*timeScale*areaMultiplier();
    int count = 0;
    while(chance(options_.rainChance*timeScale*areaMultiplier()) && count < limit){
        count++;
        double r = random<double>(options_.minR, options_.maxR, [](double n) { return n*n*n;});
        std::shared_ptr<Drop> drop(new Drop);
        drop->x = random<double>(width_/scale_);
        drop->y = random((height_/scale_)*options_.spawnArea[0], (height_/scale_)*options_.spawnArea[1]);
        drop->r = r;
        drop->momentum = 1+((r-options_.minR)*0.1)+random<double>(2.0);
        drop->spreadX = 1.5;
        drop->spreadY = 1.5;
        auto rainDrop = createDrop(drop);   // TODO: this can be refined
        if (rainDrop) 
            rainDrops.push_back(rainDrop);
    }
}

void Raindrops::updateDrops(double timeScale) {
    std::vector<std::shared_ptr<Drop> > newDrops;
    
    updateDroplets(timeScale); 
    updateRain(timeScale, newDrops);

    std::sort(drops_.begin(), drops_.end(), [this](const std::shared_ptr<Drop> &a, const std::shared_ptr<Drop> &b) {
        /*let va=(a.y*(this.width/this.scale))+a.x;
        let vb=(b.y*(this.width/this.scale))+b.x;
        return va>vb?1:va==vb?0:-1;*/
        double va = a->y*(width_/scale_) + a->x;
        double vb = b->y*(width_/scale_) + b->x;
        return va > vb;                         // note this
    });

    for (int i = 0; i < drops_.size(); i++) {
        auto drop = drops_[i];
        if(drop->killed){
        } else {
            // update gravity
            // (chance of drops "creeping down")
            if(chance((drop->r - (options_.minR*options_.dropFallMultiplier)) * (0.1/deltaR()) * timeScale)) {
                drop->momentum += random<double>((drop->r/options_.maxR)*4);
            }
            // clean small drops
            if(options_.autoShrink && drop->r <= options_.minR && chance(0.05*timeScale)){
                drop->shrink += 0.01;
            }
            //update shrinkage
            drop->r -= drop->shrink*timeScale;
            if (drop->r <= 0) drop->killed = true;

            // update trails
            if (options_.raining) {
                drop->lastSpawn += drop->momentum*timeScale*options_.trailRate;
                if (drop->lastSpawn > drop->nextSpawn) {
                    std::shared_ptr<Drop> new_drop(new Drop);
                    new_drop->r = drop->r*random(options_.trailScaleRange[0], options_.trailScaleRange[1]);
                    new_drop->x = drop->x + random(-drop->r, drop->r)*0.1;
                    new_drop->spreadY = drop->momentum*0.1;
                    new_drop->y = drop->y - (drop->r*(drop->spreadY + 1)) + new_drop->r*(new_drop->spreadY+1);
                    new_drop->parent = drop;
                    auto trailDrop = createDrop(new_drop);

                    if (trailDrop) {
                        newDrops.push_back(trailDrop);

                        drop->r *= pow(0.97,timeScale);
                        drop->lastSpawn = 0;
                        drop->nextSpawn = random(options_.minR, options_.maxR) - (drop->momentum*2*options_.trailRate) + (options_.maxR - drop->r);
                    }
                }
            }

            //normalize spread
            drop->spreadX *= pow(0.4, timeScale);
            drop->spreadY *= pow(0.7, timeScale);

            //update position
            bool moved = (drop->momentum > 0);
            if (moved && !drop->killed) {
                drop->y += drop->momentum*options_.globalTimeScale;
                drop->x += drop->momentumX*options_.globalTimeScale;
                if (drop->y > (height_/scale_) + drop->r) {
                    drop->killed = true;
                }
            }

            // collision
            bool checkCollision = (moved || drop->isNew) && (!drop->killed);
            drop->isNew = false;
            if (checkCollision) {
                for (int j = 1; j + i < drops_.size() && j < 70; j++) {
                    //basic check
                    std::shared_ptr<Drop> drop2 = drops_[i+j];
                    if( drop != drop2 && drop->r > drop2->r && drop->parent != drop2 && drop2->parent != drop && !drop2->killed ) {
                        double dx = drop2->x - drop->x;
                        double dy = drop2->y - drop->y;
                        double d = sqrt((dx*dx)+(dy*dy));
                        //if it's within acceptable distance
                        if ( d < (drop->r + drop2->r)*(options_.collisionRadius+(drop->momentum*options_.collisionRadiusIncrease*timeScale))) {
                            const double pi = PI;
                            double r1 = drop->r;
                            double r2 = drop2->r;
                            double a1 = pi*(r1*r1);
                            double a2 = pi*(r2*r2);
                            double targetR = sqrt((a1+(a2*0.8))/pi);    // note : this can be refined
                            if (targetR > options_.maxR){
                                targetR = options_.maxR;                // note: not sure
                            }
                            drop->r = targetR;
                            drop->momentumX += dx*0.1;
                            drop->spreadX = 0;
                            drop->spreadY = 0;
                            drop2->killed = true;
                            drop->momentum = std::max(drop2->momentum, std::min(40., drop->momentum+(targetR*options_.collisionBoostMultiplier) + options_.collisionBoost));
                        }
                    }
                }
            }

            //slowdown momentum
            drop->momentum -= std::max(1., (options_.minR*0.5) - drop->momentum)*0.1*timeScale;
            if (drop->momentum < 0) drop->momentum = 0;
            drop->momentumX *= pow(0.7, timeScale);

            if(!drop->killed){
                newDrops.push_back(drop);
                if (moved && options_.dropletsRate > 0) 
                // if (options_.dropletsRate > 0) 
                    clearDroplets(drop->x, drop->y, drop->r*options_.dropletsCleaningRadiusMultiplier);
                drawDrop(ctx_, drop);
            }
        }
    }
    newDrops.swap(drops_);
}

void Raindrops::update() {
    clearCanvas();

    static bool initialzed = false;
    static ElapsedTimer timer;
    if (!initialzed) {
        timer.reset();
        initialzed = true;
    }

    double deltaT = timer.elapsed_seconds();
    double timeScale = deltaT / (1./60.0);
    // let timeScale=deltaT/((1/60)*1000);  js: deltaT is in ms
    // printf("fps:%llf\n", 1./deltaT);
    if (timeScale > 1.1) timeScale = 1.1;
    timeScale *= options_.globalTimeScale;
    timer.reset();

    updateDrops(timeScale);


//     cv::Mat im = canvas_->getRGB();
//     cv::Mat alpha = canvas_->getAlpha();
//     cv::Mat dropletsAlpha = droplets_->getAlpha();
//     for (int i = 0; i < im.rows*im.cols; i++) {
//         // im.at<cv::Vec3f>(i) *= alpha.at<float>(i);
//     }
//     cv::imshow("liquid", im);
//     cv::imshow("liquidalpha", dropletsAlpha);
//     cv::waitKey(1);
    // requestAnimationFrame(this.update.bind(this));   TODO
}

