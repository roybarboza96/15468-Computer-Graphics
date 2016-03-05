/**
 * @file model.cpp
 * @brief Model class
 *
 * @author Eric Butler (edbutler)
 * @author Zeyang Li (zeyangl)
 */

#include "scene/model.hpp"
#include "scene/material.hpp"
#include "application/opengl.hpp"
#include "scene/triangle.hpp"
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>


namespace _462 {

Model::Model() : mesh( 0 ), material( 0 ) { }
Model::~Model() { }

void Model::render() const
{
    if ( !mesh )
        return;
    if ( material )
        material->set_gl_state();
    mesh->render();
    if ( material )
        material->reset_gl_state();







}
bool Model::initialize(){
    Geometry::initialize();



    return true;
}


/*
*  intersect_tri - just a copy of the triangle intersection test
*/
bool Model::intersect_tri(Ray ray, real_t upperT, Intersect_rec *rec,
	MeshVertex vertices[3]) const
{


	Ray trans_ray = Ray(invMat.transform_vector(ray.e), invMat.transform_vector(ray.d));

	Vector3 pos = invMat.transform_vector(position);


	Vector3 a = vertices[0].position + pos;
	Vector3 b = vertices[1].position + pos;
	Vector3 c = vertices[2].position + pos;

	Matrix3 A = column_matrix3(a - b, a - c, trans_ray.d);

	real_t detA = A.det();

	Matrix3 abovet = column_matrix3(a - b, a - c, a - trans_ray.e);

	real_t t = abovet.det() / detA;

	if (t < EPS || t > upperT)
		return false;


	Matrix3 aboveGamma = column_matrix3(a - b, a - trans_ray.e, trans_ray.d);

	real_t gamma = aboveGamma.det() / detA;

	if (gamma < 0 || gamma > 1)
		return false;

	Matrix3 aboveBeta = column_matrix3(a - trans_ray.e, a - c, trans_ray.d);

	real_t beta = aboveBeta.det() / detA;

	if (beta < 0 || beta > (1 - gamma))
		return false;


	//sets some important variables to use to get the components later
	rec->t = t;
	rec->beta = beta;
	rec->gamma = gamma;
	rec->vertices[0] = vertices[0];
	rec->vertices[1] = vertices[1];
	rec->vertices[2] = vertices[2];


	return true;
}

//Looping through all the triangles
bool Model::intersection(Ray ray, real_t upperT, Intersect_rec *rec) const
{
	MeshTriangle* listOfTri = (MeshTriangle*)(mesh->get_triangles());
	MeshVertex* listOfVerts = (MeshVertex*)(mesh->get_vertices());

	MeshVertex vertices[3];

	int i = 0;
	bool intersected = false;
	real_t upperTTemp = upperT;






	for (; i < mesh->num_triangles(); i++)
	{
		MeshTriangle current = listOfTri[i];


		vertices[0] = listOfVerts[current.vertices[0]];
		vertices[1] = listOfVerts[current.vertices[1]];
		vertices[2] = listOfVerts[current.vertices[2]];

		//if intersected set variables and update the upper time limit
		if (intersect_tri(ray, upperTTemp, rec, vertices))
		{
			intersected = true;
			upperTTemp = rec->t;
		}


	}

	if (intersected)
	{
		return true;
	}
	else
		return false;
}


//Gets all needed components
void Model::getComponents(Ray ray, Intersect_rec *rec) const
{


	Ray trans_ray = Ray(invMat.transform_vector(ray.e), invMat.transform_vector(ray.d));

	MeshVertex vertices[3];
	vertices[0] = rec->vertices[0];
	vertices[1] = rec->vertices[1];
	vertices[2] = rec->vertices[2];

	real_t beta = rec->beta;
	real_t gamma = rec->gamma;


	rec->position = mat.transform_vector(trans_ray.atTime(rec->t));

	rec->normal = vertices[0].normal * (1 - beta - gamma)
		+ vertices[1].normal * beta
		+ vertices[2].normal * gamma;
	rec->normal = normalize(normMat * rec->normal);





	rec->tex_coord = (vertices[0].tex_coord * (1 - beta - gamma))
		+ (vertices[1].tex_coord * beta)
		+ (vertices[2].tex_coord * gamma);


	double dummy = 0.0;
	real_t width = material->texture.width;
	real_t height = material->texture.height;
	Color3 color1 = material->texture.get_texture_pixel(
		(int)(vertices[0].tex_coord.x * width), (int)(vertices[0].tex_coord.y * height));
	Color3 color2 = material->texture.get_texture_pixel(
		(int)(vertices[1].tex_coord.x * width), (int)(vertices[1].tex_coord.y * height));
	Color3 color3 = material->texture.get_texture_pixel(
		(int)(vertices[2].tex_coord.x * width), (int)(vertices[2].tex_coord.y * height));



	rec->tex_color = material->texture.get_texture_pixel(
		(int)(modf(rec->tex_coord.x, &dummy) * material->texture.width)
		, (int)(modf(rec->tex_coord.y, &dummy) * material->texture.height));

	rec->mat_ambient = material->ambient;
	rec->mat_diffuse = material->diffuse;
	rec->mat_specular = material->specular;
	rec->refract_index = material->refractive_index;

	



}

} /* _462 */
