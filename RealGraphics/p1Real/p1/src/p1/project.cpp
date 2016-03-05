

/**
* @file project.cpp
* @brief OpenGL project
*
* @author H. Q. Bovik (hqbovik)
* @bug Unimplemented
*/

#include "p1/project.hpp"

// use this header to include the OpenGL headers
// DO NOT include gl.h or glu.h directly; it will not compile correctly.
#include "application/opengl.hpp"

// A namespace declaration. All proejct files use this namespace.
// Add this declration (and its closing) to all source/headers you create.
// Note that all #includes should be BEFORE the namespace declaration.
namespace _462 {

    // definitions of functions for the OpenglProject class

    // constructor, invoked when object is created
OpenglProject::OpenglProject()
{
// TODO any basic construction or initialization of members
// Warning: Although members' constructors are automatically called,
// ints, floats, pointers, and classes with empty contructors all
// will have uninitialized data!


   //NUMVERTS is the number of vertices per column/row for the heightmap
   NUMVERTS = 64;
   numHeightVerts = NUMVERTS * NUMVERTS;
   numHeightIndicies = 6 * (NUMVERTS - 1) * (NUMVERTS - 1);


   //helper arrays to help determine the normal of heightmap mesh data
   masterHNormTotal = (Vector3 *)calloc(numHeightVerts, sizeof(Vector3));


   //Arrays that hold the vertices,indicies, and normal of the heightmap mesh
   masterHeightMap = (Vector3 *)malloc(numHeightVerts * sizeof(Vector3));
   masterHIndex = (unsigned int *)malloc(
                           numHeightIndicies * sizeof(unsigned int));
   masterHeightNorm = (Vector3 *)calloc(numHeightVerts, sizeof(Vector3));

}

// destructor, invoked when object is destroyed
OpenglProject::~OpenglProject()
{
  // TODO any final cleanup of members
  // Warning: Do not throw exceptions or call virtual functions from 
  // deconstructors!
  // They will cause undefined behavior (probably a crash, but perhaps worse).


}



/*
 *
 *initialize_triangle - It is the function to call to render
                        scene mesh data
 */
void OpenglProject::initialize_triangle()
{
   list = glGenLists(1);
   glNewList(list, GL_COMPILE); // starts the list



   GLfloat green[] = { 0.1, 0.7, 0.0, 1.0 };
   GLfloat mat_specular[] = { 0.5, 1.0, 0.0, 1.0 };
   GLfloat low_shininess[] = { 5.0 };

   glColor3f(0.0, 1.0, 0.0);

   glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);



   glVertexPointer(3, GL_DOUBLE, 0, scene.mesh.vertices);
   glNormalPointer(GL_DOUBLE, 0, masterNormal);


  glDrawElements(GL_TRIANGLES, 3 * scene.mesh.num_triangles, 
                 GL_UNSIGNED_INT, scene.mesh.triangles);



  glEndList(); // finishes the list
}





/*
 *
 *   display_callback() - the draw function
 *
 */
void OpenglProject::display_callback()
{
   glCallList(list); // renders the compiled list
}





/**
* Initialize the project, doing any necessary opengl initialization.
* @param camera An already-initialized camera.
* @param scene The scene to render.
* @return true on success, false on error.
*/
bool OpenglProject::initialize(Camera* camera, Scene* scene)
{
   // copy scene
   this->scene = *scene;

   // TODO opengl initialization code and precomputation of mesh/heightmap


   //Enabling features of opengl
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_NORMALIZE);
   glEnable(GL_LIGHTING); // Enables lighting
   glEnable(GL_LIGHT0); // Enables light 0







   //i,j to help with looping
   int i = 0;
   int j = 0;

   //Array to hold the normals of the mesh data
   masterNormal = (Vector3 *)malloc(this->scene.mesh.num_vertices 
                                                 * sizeof(Vector3));

   //Arrays to help create the calcuation of the normals
   Vector3 *masterNormalTotal = (Vector3 *)calloc(
                                      this->scene.mesh.num_vertices
                                    , sizeof(Vector3));


   //Some temporary variables to help with organzing the calcuation
   Triangle triTemp;
   Vector3 temp;
   Vector3 tempNormal;

   Vector3 a, b, c;
   Vector3 cMinusb,aMinusb;


   //Loop to calculate normals for mesh data
   for (i = 0; i < this->scene.mesh.num_triangles; i++)
   {
       triTemp = this->scene.mesh.triangles[i];

       a = this->scene.mesh.vertices[triTemp.vertices[0]];
       b = this->scene.mesh.vertices[triTemp.vertices[1]];
       c = this->scene.mesh.vertices[triTemp.vertices[2]];

       cMinusb = c - b;
       aMinusb = a - b;

       tempNormal = cross(cMinusb, aMinusb);
       tempNormal = normalize(tempNormal);

       for (j = 0; j < 3; j++)
       {
           masterNormalTotal[triTemp.vertices[j]] = 
                       masterNormalTotal[triTemp.vertices[j]] + tempNormal;
       }

   }



   for (i = 0; i < this->scene.mesh.num_vertices; i++)
   {
       masterNormal[i] = normalize(masterNormalTotal[i]);
   }


   //Freeing the arrays that helped calculate 
   //the normals no need for them anymore
   free(masterNormalTotal);




   //creation of helper variables to help with incrementing and looping
   double increment = 2.0 / (NUMVERTS-1);
   double x = -1.0;
   double z = -1.0;
   double y = 0;
   unsigned int index = 0;
   Vector3 tempHolder;



   //Creation of the inital heightmap mesh
   for (i = 0; i < NUMVERTS; i++)
   {
       for (j = 0; j < NUMVERTS; j++)
       {

           y = this->scene.heightmap->compute_height(Vector2(x, z));
           tempHolder = Vector3(x, y, z);
           masterHeightMap[index] = tempHolder;
           x = x + increment;
           index++;
       }
       z = z + increment;
       x = -1;
   }

		


   //resetn index;
   index = 0;

   //creation of the array that holds the order of indicies of the
   //heightmap mesh
   for (i = 0; i < (NUMVERTS - 1); i++)
   {
       for (j = 0; j < (NUMVERTS - 1); j++)
       {

           //First triangle
           masterHIndex[index] = (i * NUMVERTS) + j;
           masterHIndex[index + 1] = ((i + 1) * NUMVERTS) + (j + 1);
           masterHIndex[index + 2] = ((i + 1) * NUMVERTS) + j;

           //Second Triangle
           masterHIndex[index + 3] = (i * NUMVERTS) + j;
           masterHIndex[index + 4] = (i * NUMVERTS) + (j + 1);
           masterHIndex[index + 5] = ((i + 1) * NUMVERTS) + (j + 1);
           index = index + 6;
      }
   }

   //This is the function call to calculate normal for the heightmap
   calculateNormalH();

   GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_position[] = { 5.0, 5.0, 5.0, 0.0 };



   glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
   glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(camera->get_fov_degrees(), camera->get_aspect_ratio(), 
                  camera->get_near_clip(), camera->get_far_clip());
   glMatrixMode(GL_MODELVIEW);







   initialize_triangle();


  return true;
}

/**
* Clean up the project. Free any memory, etc.
*/
void OpenglProject::destroy()
{
   // TODO any cleanup code, e.g., freeing memory
   free(masterNormal);
   free(masterHNormTotal);
   free(masterHeightMap);
   free(masterHIndex);
   free(masterHeightNorm);
}

/**
* Perform an update step. This happens on a regular interval.
* @param dt The time difference from the previous frame to the current.
*/
void OpenglProject::update(real_t dt)
{
   // update our heightmap
   scene.heightmap->update(dt);

   // TODO any update code, e.g. commputing heightmap mesh positions and normals
   updateHeightMesh();
   calculateNormalH();
}

/**
* Clear the screen, then render the mesh using the given camera.
* @param camera The logical camera to use.
* @see math/camera.hpp
*/
void OpenglProject::render(const Camera* camera)
{
   // TODO render code
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




   //Quaternion to calcuate the angle and axis for rotation
   Quaternion rotation = scene.mesh_position.orientation;
   Quaternion rotationHeight = scene.heightmap_position.orientation;


   //Container mesh rotation conversion
   Vector3 rot_axis;
   double angle;
   rotation.to_axis_angle(&rot_axis, &angle);
   angle = angle *(180 / PI);


   //Heightmap mesh rotation conversion
   Vector3 rot_axisHeight;
   double angleHeight;
   rotationHeight.to_axis_angle(&rot_axisHeight, &angleHeight);
   angleHeight = angleHeight * (180 / PI);






   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();



   //Camera set up
   gluLookAt(camera->position.x, camera->position.y, camera->position.z,
             camera->position.x + camera->get_direction().x, 
             camera->position.y + camera->get_direction().y, 
             camera->position.z + camera->get_direction().z,
             camera->get_up().x, camera->get_up().y, camera->get_up().z);
		
		



   //Transformation for the heightmap mesh data
   glPushMatrix();

		
   GLfloat waterColor[] = { 0.0, 0.65, 1.0, 1.0 };
   GLfloat mat_specular[] = { 0.0, 0.65, 1.0, 1.0 };
   GLfloat low_shininess[] = { 6.0 };

   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, waterColor);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
   glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, low_shininess);
		

   glTranslated(
     scene.heightmap_position.position.x, 
     scene.heightmap_position.position.y,
     scene.heightmap_position.position.z);
   glRotated( angleHeight, 
              rot_axisHeight.x, rot_axisHeight.y, rot_axisHeight.z);
   glScaled(scene.heightmap_position.scale.x, 
            scene.heightmap_position.scale.y,
            scene.heightmap_position.scale.z);

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glColor3f(0.0, 1.0, 1.0);
   glVertexPointer(3, GL_DOUBLE, 0, masterHeightMap);
   glNormalPointer(GL_DOUBLE, 0, masterHeightNorm);
   glDrawElements(GL_TRIANGLES, 6 * (NUMVERTS - 1)*(NUMVERTS - 1), 
                  GL_UNSIGNED_INT, masterHIndex);
		
   glPopMatrix();

   //Transformation for the triangle mesh data
   glTranslated(scene.mesh_position.position.x, 
                scene.mesh_position.position.y, 
                scene.mesh_position.position.z);
   glRotated(angle, rot_axis.x, rot_axis.y, rot_axis.z);
   glScaled(scene.mesh_position.scale.x, 
            scene.mesh_position.scale.y, 
            scene.mesh_position.scale.z);
   display_callback();



   glFlush();

}


/*
 *cacluateNormalH - calculate the normal of for the heightmap mesh
 *
 */
void OpenglProject::calculateNormalH()
{

   int i,j;
   Vector3 a, b, c;
   Vector3 cMinusa, bMinusa;
   Vector3 tempNormal;

   for (i = 0; i < numHeightIndicies; i += 3)
   {
       a = masterHeightMap[masterHIndex[i]];
       b = masterHeightMap[masterHIndex[i+1]];
       c = masterHeightMap[masterHIndex[i+2]];

       cMinusa = c - a;
       bMinusa = b - a;

       tempNormal = cross(cMinusa,bMinusa);
       tempNormal = normalize(tempNormal);

       for (j = 0; j < 3; j++)
       {
           masterHNormTotal[masterHIndex[i + j]] = 
                       masterHNormTotal[masterHIndex[i + j]] + tempNormal;
       }

   }

   for (i = 0; i < numHeightVerts; i++)
   {
       masterHeightNorm[i] = 
            normalize(masterHNormTotal[i]);
   }
}

void OpenglProject::updateHeightMesh()
{
   int i;

   double y;

   Vector2 input;
   Vector3 output;

   for (i = 0; i < numHeightVerts; i++)
   {
       input = Vector2(masterHeightMap[i].x, masterHeightMap[i].z);
       y = scene.heightmap->compute_height(input);


       //remember that we gave it a Vector2(x,z) so that means that
       //to reference x we just call input.x and to reference
       //z we just call input.y
       output = Vector3(input.x, y, input.y);
       masterHeightMap[i] = output;

   }
}

} /* _462 */
