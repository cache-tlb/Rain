#ifndef MATERIAL_H
#define MATERIAL_H

#include "common.h"
#include "Vec3.h"

class Material {
public:
    Material() : reflectance_(1,1,1) {}
    virtual ~Material() {}

    inline void SetReflectance(float r, float g, float b) {
        reflectance_.x = r;
        reflectance_.y = g;
        reflectance_.z = b;
    }

    inline void SetReflectance(const Vec3f& diffuse) {
        reflectance_ = diffuse;
    }

    inline Vec3f GetReflectance() const {
        return reflectance_;
    }

    inline void SetDiffuseMapName(const std::string &diffuse_name) {
        diffuse_tex_name = diffuse_name;
    }

    inline std::string GetDiffuseMapName() const {
        return diffuse_tex_name;
    }

protected:
    Vec3f reflectance_;
    std::string diffuse_tex_name;
};

#endif // MATERIAL_H
