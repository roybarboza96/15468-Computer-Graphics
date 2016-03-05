/**
 * @file model.hpp
 * @brief Model class
 *
 * @author Eric Butler (edbutler)
 */

#ifndef _462_SCENE_MODEL_HPP_
#define _462_SCENE_MODEL_HPP_

#include "scene/scene.hpp"
#include "scene/mesh.hpp"
#include "scene/meshtree.hpp"

namespace _462 {

/**
 * A mesh of triangles.
 */
class Model : public Geometry
{
public:

    const Mesh* mesh;
    const MeshTree *tree;
    const Material* material;

    Model();
    virtual ~Model();

    virtual void render() const;
    virtual bool initialize();
	virtual bool intersection(Ray ray, real_t upperT, Intersect_rec *rec) const;
	bool intersect_tri(Ray ray, real_t upperT, Intersect_rec *rec, MeshVertex vertices[3]) const;
	virtual void getComponents(Ray ray, Intersect_rec *rec) const;
};


} /* _462 */

#endif /* _462_SCENE_MODEL_HPP_ */

