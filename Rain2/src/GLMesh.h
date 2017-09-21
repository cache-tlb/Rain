#ifndef GLMESH_H
#define GLMESH_H

#include "common.h"
#include "Vec3.h"
#include "Vec2.h"
#include "Material.h"
#include "VSShaderLib.h"

class GLMesh {
public:
    GLMesh();

    GLuint vao;
    GLuint vertex_coord_buff, texture_coord_buff, normal_buff, tri_index_buff;

    bool has_coords, has_normals;

    std::vector<float> vertices;    // x, y, z
    std::vector<float> texture_coords;      // x, y
    std::vector<float> normals;     // x, y, z
    std::vector<unsigned int> triangles;    // i, j, k

    void add_vertex(const Vec3f &pos, const Vec2f &uv, const Vec3f &normal);
    void add_face(const Vec3i &face);

    void add_instance_data(VSShaderLib::AttribType attribute_id, int attribute_unit_size, int instance_data_size, float *data);
    void add_sh_data(float *data);

    void compile();
    void clear();

    static std::shared_ptr<GLMesh> plane(int detailX = 1, int detailY = 1);
    static std::shared_ptr<GLMesh> cube();
    static std::shared_ptr<GLMesh> sphere(int detail = 6);
    static void LoadFromObjFile(const std::string &filepath, const std::string &mtl_base_path, std::vector<std::shared_ptr<GLMesh> > &meshes, std::vector<Material> &material_ptrs);

private:
    // QOpenGLFunctionsType *context;
};

template<typename _Tp>
class Indexer {
public:
    Indexer() {}
    ~Indexer() {}
    int add(const Vec3<_Tp> &v) {
        if (map_.count(v) <= 0) {
            map_[v] = unique.size();
            unique.push_back(v);
        }
        return map_[v];
    }

    std::vector<Vec3<_Tp> > unique;
    std::map<Vec3<_Tp>, int> map_;
private:
};

#endif // GLMESH_H
