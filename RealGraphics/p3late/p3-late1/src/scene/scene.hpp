/**
 * @file scene.hpp
 * @brief Class definitions for scenes.
 *
 */

#ifndef _462_SCENE_SCENE_HPP_
#define _462_SCENE_SCENE_HPP_

#include "math/vector.hpp"
#include "math/quaternion.hpp"
#include "math/matrix.hpp"
#include "math/camera.hpp"
#include "scene/material.hpp"
#include "scene/mesh.hpp"
//#include "scene/bvh.hpp"
#include "ray.hpp"
#include <string>
#include <vector>
#include <cfloat>


namespace _462 {
class Geometry;

//represents an intersection between a ray and a geometry
struct Intersection{
};


typedef struct Intersect_rec
{
	real_t t;
	Vector3 position;
	Color3 testColor;
	Vector3 normal;
	Color3 mat_ambient;
	Color3 mat_diffuse;
	Color3 mat_specular;
	real_t refract_index;
	Vector2 tex_coord;
	Color3 tex_color;

	//used only for triangle and model
	real_t beta;
	real_t gamma;

	//used only for model
	MeshVertex vertices[3];
}
Intersect_rec;

//axis aligned bounding box - may be useful in a BVH
class Bound{
public:
    //the lowest coordinates in the box
    Vector3 lower;
    //the highest coordinates in the box
    Vector3 upper;
    Bound(Vector3 l,Vector3 u):lower(l),upper(u){}
    Bound(Vector3 a):lower(a),upper(a){}
    Bound(Bound a,Bound b){
        lower.x=std::min(a.lower.x,b.lower.x);
        lower.y=std::min(a.lower.y,b.lower.y);
        lower.z=std::min(a.lower.z,b.lower.z);
        
        upper.x=std::max(a.upper.x,b.upper.x);
        upper.y=std::max(a.upper.y,b.upper.y);
        upper.z=std::max(a.upper.z,b.upper.z);
    }
    Bound(){
        lower.x=INFINITY;
        lower.y=INFINITY;
        lower.z=INFINITY;
        
        upper.x=-INFINITY;
        upper.y=-INFINITY;
        upper.z=-INFINITY;
    }
    bool intersects(Ray &ray) const;
    real_t dim(int i){return upper[i]-lower[i];}
    void assertIn(Vector3 other){
        for(int i =0;i<3;i++){
            assert(lower[i]<=other[i] && other[i]<=upper[i]);
        }
    }
};

//typedef struct Intersection Intersection;

class Geometry
{
public:
    Geometry();
    virtual ~Geometry();
    /*
       World transformation are applied in the following order:
       1. Scale
       2. Orientation
       3. Position
    */

    // The world position of the object.
    Vector3 position;

    // The world orientation of the object.
    // Use Quaternion::to_matrix to get the rotation matrix.
    Quaternion orientation;

    // The world scale of the object.
    Vector3 scale;

    // Forward transformation matrix
    Matrix4 mat;

    // Inverse transformation matrix
    Matrix4 invMat;
    // Normal transformation matrix
    Matrix3 normMat;
    bool isBig;
    /**
     * Renders this geometry using OpenGL in the local coordinate space.
     */
    virtual void render() const = 0;

	
	virtual bool intersection(Ray ray, real_t uppperT, Intersect_rec* rec) const{
		return false;
	}
	virtual void getComponents(Ray ray, Intersect_rec* rec) const = 0;
	
    virtual bool initialize();
};


struct SphereLight
{
    struct Attenuation
    {
        real_t constant;
        real_t linear;
        real_t quadratic;
    };

    SphereLight();

    bool intersect(const Ray& r, real_t& t);

    // The position of the light, relative to world origin.
    Vector3 position;
    // The color of the light (both diffuse and specular)
    Color3 color;
    // attenuation
    Attenuation attenuation;
    real_t radius;
};

/**
 * The container class for information used to render a scene composed of
 * Geometries.
 */
class Scene
{
public:

    /// the camera
    Camera camera;
    /// the background color
    Color3 background_color;
    /// the amibient light of the scene
    Color3 ambient_light;
    /// the refraction index of air
    real_t refractive_index;

    /// Creates a new empty scene.
    Scene();

    /// Destroys this scene. Invokes delete on everything in geometries.
    ~Scene();

    bool initialize();

    // accessor functions
    Geometry* const* get_geometries() const;
    size_t num_geometries() const;
    const SphereLight* get_lights() const;
    size_t num_lights() const;
    Material* const* get_materials() const;
    size_t num_materials() const;
    Mesh* const* get_meshes() const;
    size_t num_meshes() const;

    /// Clears the scene, and invokes delete on everything in geometries.
    void reset();

    // functions to add things to the scene
    // all pointers are deleted by the scene upon scene deconstruction.
    void add_geometry( Geometry* g );
    void add_material( Material* m );
    void add_mesh( Mesh* m );
    void add_light( const SphereLight& l );
    
private:

    typedef std::vector< SphereLight > SphereLightList;
    typedef std::vector< Material* > MaterialList;
    typedef std::vector< Mesh* > MeshList;
    typedef std::vector< Geometry* > GeometryList;

    // list of all lights in the scene
    SphereLightList point_lights;
    // all materials used by geometries
    MaterialList materials;
    // all meshes used by models
    MeshList meshes;
    // list of all geometries. deleted in dctor, so should be allocated on heap.
    GeometryList geometries;
private:

    // no meaningful assignment or copy
    Scene(const Scene&);
    Scene& operator=(const Scene&);

};
} /* _462 */

#endif /* _462_SCENE_SCENE_HPP_ */
