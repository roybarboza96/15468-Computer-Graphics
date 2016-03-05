/**
 * @file triangle.cpp
 * @brief Function definitions for the Triangle class.
 *
 * @author Eric Butler (edbutler)
 */

#include "scene/triangle.hpp"
#include "application/opengl.hpp"
#include "math/math.hpp"

namespace _462 {

Triangle::Triangle()
{
    vertices[0].material = 0;
    vertices[1].material = 0;
    vertices[2].material = 0;
    isBig=true;
}

Triangle::~Triangle() { }

void Triangle::render() const
{
    bool materials_nonnull = true;
    for ( int i = 0; i < 3; ++i )
        materials_nonnull = materials_nonnull && vertices[i].material;

    // this doesn't interpolate materials. Ah well.
    if ( materials_nonnull )
        vertices[0].material->set_gl_state();

    glBegin(GL_TRIANGLES);

#if REAL_FLOAT
    glNormal3fv( &vertices[0].normal.x );
    glTexCoord2fv( &vertices[0].tex_coord.x );
    glVertex3fv( &vertices[0].position.x );

    glNormal3fv( &vertices[1].normal.x );
    glTexCoord2fv( &vertices[1].tex_coord.x );
    glVertex3fv( &vertices[1].position.x);

    glNormal3fv( &vertices[2].normal.x );
    glTexCoord2fv( &vertices[2].tex_coord.x );
    glVertex3fv( &vertices[2].position.x);
#else
    glNormal3dv( &vertices[0].normal.x );
    glTexCoord2dv( &vertices[0].tex_coord.x );
    glVertex3dv( &vertices[0].position.x );

    glNormal3dv( &vertices[1].normal.x );
    glTexCoord2dv( &vertices[1].tex_coord.x );
    glVertex3dv( &vertices[1].position.x);

    glNormal3dv( &vertices[2].normal.x );
    glTexCoord2dv( &vertices[2].tex_coord.x );
    glVertex3dv( &vertices[2].position.x);
#endif

    glEnd();

    if ( materials_nonnull )
        vertices[0].material->reset_gl_state();
}

bool Triangle::intersection(Ray ray, real_t upperT, Intersect_rec *rec) const
{



	//Just standard intersection test done and done by the shirley method

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

	if (beta < 0 || beta >(1 - gamma))
		return false;





	//set some important variables
	rec->t = t;
	rec->beta = beta;
	rec->gamma = gamma;



	return true;
}

/*
* Gets all the necessary components to needed to get the the color and send the rays
*/
void Triangle::getComponents(Ray ray, Intersect_rec *rec) const
{



	Ray trans_ray = Ray(invMat.transform_vector(ray.e), invMat.transform_vector(ray.d));


	real_t beta = rec->beta;
	real_t gamma = rec->gamma;
	rec->position = mat.transform_vector(trans_ray.atTime(rec->t));
	rec->normal = vertices[0].normal * (1 - beta - gamma)
		+ vertices[1].normal * beta
		+ vertices[2].normal * gamma;
	rec->normal = normalize(normMat * rec->normal);




	//get materials
	rec->mat_ambient = (vertices[0].material->ambient * (1 - beta - gamma))
		+ (vertices[1].material->ambient * beta)
		+ (vertices[2].material->ambient * gamma);
	rec->mat_diffuse = (vertices[0].material->diffuse * (1 - beta - gamma))
		+ (vertices[1].material->diffuse * beta)
		+ (vertices[2].material->diffuse * gamma);
	rec->mat_specular = (vertices[0].material->specular * (1 - beta - gamma))
		+ (vertices[1].material->specular * beta)
		+ (vertices[2].material->specular * gamma);
	rec->refract_index = (vertices[0].material->refractive_index * (1 - beta - gamma))
		+ (vertices[1].material->refractive_index * beta)
		+ (vertices[2].material->refractive_index * gamma);



	//interpolate to get the texture

	rec->tex_coord = (vertices[0].tex_coord * (1 - beta - gamma))
		+ (vertices[1].tex_coord * beta)
		+ (vertices[2].tex_coord * gamma);

	double dummy = 0.0;
	Color3 texColor1 = vertices[0].material->texture.get_texture_pixel(
		(int)(modf(rec->tex_coord.x, &dummy) * vertices[0].material->texture.width)
		, (int)(modf(rec->tex_coord.y, &dummy) * vertices[0].material->texture.height));

	Color3 texColor2 = vertices[1].material->texture.get_texture_pixel(
		(int)(modf(rec->tex_coord.x, &dummy) * vertices[1].material->texture.width)
		, (int)(modf(rec->tex_coord.y, &dummy) * vertices[1].material->texture.height));

	Color3 texColor3 = vertices[2].material->texture.get_texture_pixel(
		(int)(modf(rec->tex_coord.x, &dummy) * vertices[2].material->texture.width)
		, (int)(modf(rec->tex_coord.y, &dummy) * vertices[2].material->texture.height));

	rec->tex_color = (texColor1 * (1 - beta - gamma))
		+ (texColor2 * beta)
		+ (texColor3 * gamma);





}



} /* _462 */
