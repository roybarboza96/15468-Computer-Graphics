/**
 * @file raytacer.cpp
 * @brief Raytracer class
 *
 * Implement these functions for project 4.
 *
 * @author H. Q. Bovik (hqbovik)
 * @bug Unimplemented
 */

#include "raytracer.hpp"
#include "scene/scene.hpp"
#include "math/quickselect.hpp"
#include "p3/randomgeo.hpp"
#include <SDL_timer.h>
namespace _462 {





//number of rows to render before updating the result
static const unsigned STEP_SIZE = 1;
static const unsigned CHUNK_SIZE = 1;

Raytracer::Raytracer() {
        scene = 0;
        width = 0;
        height = 0;
    }

Raytracer::~Raytracer() { }

/**
 * Initializes the raytracer for the given scene. Overrides any previous
 * initializations. May be invoked before a previous raytrace completes.
 * @param scene The scene to raytrace.
 * @param width The width of the image being raytraced.
 * @param height The height of the image being raytraced.
 * @return true on success, false on error. The raytrace will abort if
 *  false is returned.
 */
bool Raytracer::initialize(Scene* scene, size_t num_samples,
               size_t width, size_t height)
{


    this->scene = scene;
    this->num_samples = num_samples;
    this->width = width;
    this->height = height;





    current_row = 0;

    projector.init(scene->camera);
    scene->initialize();
    photonMap.initialize(scene);
    return true;
}
//compute ambient lighting

/*
 *   trace_ray - does the ray casting and what not
                  there is a ray the depth to tell how many recursive rays should be shot
				  a bool shadow to tell if it is a shadow ray,
				  and the light position and intersection position because
				  I don't know how to use c++ with the const things giving me a lot of compile errors
				  especially when I try to use light.intersect

				  Warning it is super slow as no optimization was done on it
				  */
Color3 Raytracer::trace_ray(Ray &ray, int depth,bool shadow, Vector3 l_pos, Vector3 int_pos){
    //TODO: render something more interesting

	//looping variables
	int i = 0;
	int j = 0;
	int k = 0;




	//struct to hold info for the current geom
	//that we are testing for intersection
	Intersect_rec rec;

	rec.position = Vector3(0.0, 0.0, 0.0);
	rec.t = -1;
	rec.tex_coord = Vector2(0.0, 0.0);



	//struct to hold info for the currently EARLIEST
	//intersection
	Intersect_rec inter_rec;

	inter_rec.position = Vector3(0.0, 0.0, 0.0);
	inter_rec.t = -1;
	inter_rec.tex_coord = Vector2(0.0, 0.0);




	
	//lists of lights and geometries
	Geometry* const* listOfGeo = (Geometry* const *)(this->scene->get_geometries());
	const SphereLight* lights = scene->get_lights();


	//temp value for the upper time limit
	//of when to check for an intersection
	real_t temp = DBL_MAX;



	//index of the geometry in the list at which we intersect
	int inter_index = 0;

	//flag to tell if there was an intersection or not
	bool intersected = false;

	real_t intTime = 0.0;

	//run through all of the geometries to find a intersection
	//all ray will do this but only the shadow ray will return a result
	//right after the intersection test are done
	for (; i < scene->num_geometries(); i++)
	{
		const Geometry& geom = *listOfGeo[i];
		
		if (geom.intersection(ray, temp, &rec))
		{

			//if shadow ray then just return immediately with color
			//black indicating that object is in the way
			if (shadow)
			{
				Vector3 pos = ray.atTime(rec.t);

				real_t dis1 = fabs(squared_distance(l_pos, int_pos));
				real_t dis2 = fabs(squared_distance(pos, int_pos));


				//makes sure that the object that got intersected isn't past the light
				//tried to use the light.intersect function but was running into
				//compile errors thus I just pas the intersection point and the light point
				//to test
				if (dis1 < dis2)
					continue;
				else
					return Color3(0.0, 0.0, 0.0);
			}


			//update some values to use on later
			intersected = true;

			temp = rec.t;

			inter_index = i;
			inter_rec = rec;

			
		}
	}
	
	


	if (intersected)
	{

		Color3 lightColor = Color3(0.0, 0.0, 0.0);

		Color3 direct_ill = Color3(0.0, 0.0, 0.0);

		//get the earliet intersected geometry and get the necessary components
		//from it
		const Geometry& intersected = *listOfGeo[inter_index];
		intersected.getComponents(ray, &inter_rec);

		//see if we need to compute shadow rays
		if (inter_rec.refract_index <= 0.0)
		{
			//Cast the shadow rays


			for (j = 0; j < scene->num_lights(); j++)
			{
				const SphereLight& light = lights[j];


				//sample light source
				Vector3 lightPoint =
					Vector3(random_gaussian(), random_gaussian(), random_gaussian());
				lightPoint = normalize(lightPoint);
				lightPoint = light.radius*lightPoint + light.position;
				Vector3 dirL = lightPoint - inter_rec.position;




				//create shadowRay
				Ray shadowRay = Ray(inter_rec.position, dirL);

				const Ray shray = Ray(inter_rec.position, dirL);
				
				

				//ray cast the shadow ray and see if it hits something
				Color3 shadowColor = trace_ray(shadowRay, 0, true, lightPoint, inter_rec.position);


				//calculate the term specific to the current light
				//Black means that it did hit something
				if (shadowColor != Color3(0.0, 0.0, 0.0))
				{


					real_t dis = distance(inter_rec.position, lightPoint);
					real_t atten = (light.attenuation.quadratic * (dis*dis))
						+ (light.attenuation.linear * dis)
						+ (light.attenuation.constant);

					Vector3 lightVector = normalize(lightPoint - inter_rec.position);


					Color3 c_i =
						Color3(light.color.r / atten, light.color.g / atten, light.color.b / atten);
					lightColor +=
						c_i
						* inter_rec.mat_diffuse
						* fmax(dot(inter_rec.normal, lightVector), 0.0);


				}


			}

			//compute direct illumination
			direct_ill = lightColor + (scene->ambient_light * inter_rec.mat_ambient);
			direct_ill *= inter_rec.tex_color;
		}



		//if recursive depth is at one or below then
		//just return direct illumination term and this is where
		//recursion will stop
		if (depth > 0)
		{
			//compute specular reflection
			Color3 specularReflect = Color3(0.0, 0.0, 0.0);

			//compute the reflected ray
			Vector3 incoming = normalize(ray.d);
			Vector3 reflected =
				incoming - (2 * inter_rec.normal * dot(incoming, inter_rec.normal));
			Ray reflectRay = Ray(inter_rec.position, reflected);


			//recursive call for the reflected ray
			specularReflect = trace_ray(reflectRay, depth - 1, false, Vector3(0.0, 0.0, 0.0), Vector3(0.0,0.0,0.0));



			//whatever is returned multiply with the material specular term and the
			//texture color
			specularReflect *= inter_rec.mat_specular * inter_rec.tex_color;


			//seeing if we need to send out a transmission ray
			if (inter_rec.refract_index > 0.0)
			{
				//set some variables to keep track of the math easier
				Vector3 d = normalize(ray.d);

				real_t n_t = inter_rec.refract_index;
				real_t d_dot_n = dot(d, inter_rec.normal);
				real_t n = scene->refractive_index;
				real_t total_internal = 1.0 - (((n*n)*(1 - (d_dot_n * d_dot_n))) / (n_t * n_t));

				Vector3 t;
				real_t c;

				//if going into dielectric
				if (d_dot_n < 0.0)
				{
					t = (n*(d - (inter_rec.normal*d_dot_n))) / (n_t)
						-inter_rec.normal * sqrt(total_internal);
					c = dot(-d, inter_rec.normal);
				}
				else
				{

					//going from dielectric to something else
					//so we just need to reverse the our normal direction
					//,change the dielectric constant things
					//and then just apply our equation to get the out bound angle.
					d_dot_n = dot(d, -inter_rec.normal);
					n_t = scene->refractive_index;
					n = inter_rec.refract_index;


					total_internal = 1.0 - (((n*n)*(1 - (d_dot_n * d_dot_n))) / (n_t * n_t));
					//if it is not total internal reflecltion
					if (total_internal > 0.0)
					{
						//calculate the direction of the transmission ray
						t = (n*(d - ((-inter_rec.normal)*d_dot_n))) / (n_t)
							-(-inter_rec.normal) * sqrt(total_internal);

						Vector3 t_norm = normalize(t);
						c = dot(-d, -inter_rec.normal);
					}
					else
					{
						//if it is just return the specular reflect thing
						return specularReflect;
					}


				}





				//ray cast the transmission ray
				Ray transmissionRay = Ray(inter_rec.position, t);
				Color3 refractColor = trace_ray(transmissionRay, depth - 1, false, Vector3(0.0,0.0,0.0), Vector3(0.0,0.0,0.0));

				//Get the approximation constant

				real_t r_0 = pow((n_t - 1) / (n_t + 1), 2);
				real_t r = r_0 + ((1 - r_0)*(pow(1 - c, 5)));

				//return with the fresnell thing in mind
				return specularReflect * r + (1 - r) * refractColor;

			}
			else
			{

				//return the direct illumination term with the specular term
				//Opaque objects
				return direct_ill + specularReflect;

			}


			////returns direct_ill of the color
			//notice that we return black  if we are in a dielectric
			//and the depth goes to zero

			return direct_ill;
		}

		return direct_ill;

	}
	else if (shadow)
	{
		//makes sure that the shadow if it "somehow made it this far
		//means that it did not hit anything thus just return White
		return Color3(1.0, 1.0, 1.0);
	}
	else
	{
		//else this did not intersect and is not a shadow ray
		//thus just return the background color which is black
		return scene->background_color;
	}
	



	

	
	//return background color of just black
	//just to make sure
	return scene->background_color;
}

/**
 * Performs a raytrace on the given pixel on the current scene.
 * The pixel is relative to the bottom-left corner of the image.
 * @param scene The scene to trace.
 * @param x The x-coordinate of the pixel to trace.
 * @param y The y-coordinate of the pixel to trace.
 * @param width The width of the screen in pixels.
 * @param height The height of the screen in pixels.
 * @return The color of that pixel in the final image.
 */
Color3 Raytracer::trace_pixel(size_t x,
                  size_t y,
                  size_t width,
                  size_t height)
{
    assert(x < width);
    assert(y < height);

    real_t dx = real_t(1)/width;
    real_t dy = real_t(1)/height;

    Color3 res = Color3::Black();
    unsigned int iter;
    for (iter = 0; iter < num_samples; iter++)
    {
        // pick a point within the pixel boundaries to fire our
        // ray through.
        real_t i = real_t(2)*(real_t(x)+random_uniform())*dx - real_t(1);
        real_t j = real_t(2)*(real_t(y) + random_uniform())*dy - real_t(1);

        Ray r = Ray(scene->camera.get_position(), projector.get_pixel_dir(i, j));
    
        res += trace_ray(r, 4,false, Vector3(0.0,0.0,0.0), Vector3(0.0,0.0,0.0));
        // TODO return the color of the given pixel
        // you don't have to use this stub function if you prefer to
        // write your own version of Raytracer::raytrace.

    }
    return res*(real_t(1)/num_samples);
}

/**
 * Raytraces some portion of the scene. Should raytrace for about
 * max_time duration and then return, even if the raytrace is not copmlete.
 * The results should be placed in the given buffer.
 * @param buffer The buffer into which to place the color data. It is
 *  32-bit RGBA (4 bytes per pixel), in row-major order.
 * @param max_time, If non-null, the maximum suggested time this
 *  function raytrace before returning, in seconds. If null, the raytrace
 *  should run to completion.
 * @return true if the raytrace is complete, false if there is more
 *  work to be done.
 */
bool Raytracer::raytrace(unsigned char* buffer, real_t* max_time)
{
    
    static const size_t PRINT_INTERVAL = 64;

    // the time in milliseconds that we should stop
    unsigned int end_time = 0;
    bool is_done;



    if (max_time)
    {
        // convert duration to milliseconds
        unsigned int duration = (unsigned int) (*max_time * 1000);
        end_time = SDL_GetTicks() + duration;
    }

    // until time is up, run the raytrace. we render an entire group of
    // rows at once for simplicity and efficiency.
    for (; !max_time || end_time > SDL_GetTicks(); current_row += STEP_SIZE)
    {
        // we're done if we finish the last row
        is_done = current_row >= height;
        // break if we finish
        if (is_done) break;

        int loop_upper = std::min(current_row + STEP_SIZE, height);

        for (int c_row = current_row; c_row < loop_upper; c_row++)
        {
            /*
             * This defines a critical region of code that should be
             * executed sequentially.
             */
#pragma omp critical
            {
                if (c_row % PRINT_INTERVAL == 0)
                    printf("Raytracing (Row %d)\n", c_row);
            }
            
        // This tells OpenMP that this loop can be parallelized.
#pragma omp parallel for schedule(dynamic, CHUNK_SIZE)
            for (int x = 0; x < width; x++)
            {
                // trace a pixel
                Color3 color = trace_pixel(x, c_row, width, height);
                // write the result to the buffer, always use 1.0 as the alpha
                color.to_array4(&buffer[4 * (c_row * width + x)]);
            }
#pragma omp barrier

        }
    }

    if (is_done) printf("Done raytracing!\n");

    return is_done;
}

} /* _462 */
