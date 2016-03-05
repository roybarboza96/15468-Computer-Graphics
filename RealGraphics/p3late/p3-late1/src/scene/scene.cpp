/**
 * @file scene.cpp
 * @brief Function definitions for scenes.
 *
 * @author Eric Butler (edbutler)
 * @author Kristin Siu (kasiu)
 */

#include "scene/scene.hpp"
namespace _462 {


Geometry::Geometry():
    position(Vector3::Zero()),
    orientation(Quaternion::Identity()),
    scale(Vector3::Ones())
{

}

Geometry::~Geometry() { }


bool Geometry::initialize()
{
    make_inverse_transformation_matrix(&invMat, position, orientation, scale);
    make_transformation_matrix(&mat, position, orientation, scale);
    make_normal_matrix(&normMat, mat);
    return true;
}
SphereLight::SphereLight():
    position(Vector3::Zero()),
    color(Color3::White()),
    radius(real_t(0))
{
    attenuation.constant = 1;
    attenuation.linear = 0;
    attenuation.quadratic = 0;
}

Scene::Scene()
{
    reset();
}

Scene::~Scene()
{
    reset();
}

bool Scene::initialize()
{
    bool res = true;
    for (unsigned int i = 0; i < num_geometries(); i++)
        res &= geometries[i]->initialize();
    return res;
}


Geometry* const* Scene::get_geometries() const
{
    return geometries.empty() ? NULL : &geometries[0];
}

size_t Scene::num_geometries() const
{
    return geometries.size();
}

const SphereLight* Scene::get_lights() const
{
    return point_lights.empty() ? NULL : &point_lights[0];
}

size_t Scene::num_lights() const
{
    return point_lights.size();
}

Material* const* Scene::get_materials() const
{
    return materials.empty() ? NULL : &materials[0];
}

size_t Scene::num_materials() const
{
    return materials.size();
}

Mesh* const* Scene::get_meshes() const
{
    return meshes.empty() ? NULL : &meshes[0];
}

size_t Scene::num_meshes() const
{
    return meshes.size();
}

void Scene::reset()
{
    for ( GeometryList::iterator i = geometries.begin(); i != geometries.end(); ++i ) {
        delete *i;
    }
    for ( MaterialList::iterator i = materials.begin(); i != materials.end(); ++i ) {
        delete *i;
    }
    for ( MeshList::iterator i = meshes.begin(); i != meshes.end(); ++i ) {
        delete *i;
    }

    geometries.clear();
    materials.clear();
    meshes.clear();
    point_lights.clear();

    camera = Camera();

    background_color = Color3::Black();
    ambient_light = Color3::Black();
    refractive_index = 1.0;
}

void Scene::add_geometry( Geometry* g )
{
    geometries.push_back( g );
}

void Scene::add_material( Material* m )
{
    materials.push_back( m );
}

void Scene::add_mesh( Mesh* m )
{
    meshes.push_back( m );
}

void Scene::add_light( const SphereLight& l )
{
    point_lights.push_back( l );
}
bool Bound::intersects(Ray &ray) const{
    real_t id0=1.0/ray.d[0];
    real_t id1=1.0/ray.d[1];
    real_t id2=1.0/ray.d[2];
    real_t t1 = (lower[0]-ray.e[0])*id0;
    real_t t2 = (upper[0]-ray.e[0])*id0;
    real_t t3 = (lower[1]-ray.e[1])*id1;
    real_t t4 = (upper[1]-ray.e[1])*id1;
    real_t t5 = (lower[2]-ray.e[2])*id2;
    real_t t6 = (upper[2]-ray.e[2])*id2;
    real_t tl0=std::min(t1,t2);
    real_t tu0=std::max(t1,t2);
    real_t tl1=std::min(t3,t4);
    real_t tu1=std::max(t3,t4);
    real_t tl2=std::min(t5,t6);
    real_t tu2=std::max(t5,t6);
    real_t tl=std::max(std::max(tl0,tl1),tl2);
    real_t tu=std::min(std::min(tu0,tu1),tu2);
    return tl<tu;
}
} /* _462 */

