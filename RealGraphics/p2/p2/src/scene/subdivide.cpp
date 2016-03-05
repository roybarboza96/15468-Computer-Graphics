#include "scene/mesh.hpp"
#include<map>
#include<set>

namespace _462 {

	typedef struct edge
	{
		int set;                   //flag for if this is set or not
		unsigned int vertNbrs[2];  //index for the vertices of the endpoints
		unsigned int faceNbrs[2];  //index for the vertices of the non-endpoints
		unsigned int index;        //index of the newly created vertex
		int boundary;              //flag for if this is a boundary point or not


	}
	EdgeInfo;

	typedef struct vert
	{
		std::set<unsigned int> nbrs; //indicies of all the neighbor vertices
	}
	VertInfo;

bool Mesh::subdivide()
{
    /*
      You should implement loop subdivision here.

      Triangles are stored in an std::vector<MeshTriangle> in 'triangles'.
      Vertices are stored in an std::vector<MeshVertex> in 'vertices'.

      Check mesh.hpp for the Mesh class definition.
     */


	//Upper bound of the newly created vertices
	int numEdges = 3 * triangles.size();


	//Adjancy structures of the odd and even vertices
	std::map<unsigned int, EdgeInfo> testmap;

	int originalSize = vertices.size();

	std::vector< std::set< unsigned int > > vset(originalSize);


	//The new list of triangles that we will use
	MeshTriangle *newTriangles = (MeshTriangle*)
                     malloc(4*triangles.size()*sizeof(MeshTriangle));



	//for the for loops
	int i;
	int j;

	//refernces to first,second and third vertices
	//of the current triangle
	unsigned int first;
	unsigned int second;
	unsigned int third;

	int firstInt;
	int secondInt;

	//array to keep track of the index of the vertices
	unsigned int oddVert[3];

	//for loading how many neighbors the
	//even vertices currently has
	int tempIndex;

	//refernce to the current MeshTriangle
	MeshTriangle triTemp;

	//refernces to load all the new triangles
	MeshTriangle newTriUp;
	MeshTriangle newTriLeft;
	MeshTriangle newTriRight;
	MeshTriangle newTriMid;

	//reference to the current edge that we are working with
	EdgeInfo currentEdge;

	//used so that we can calculate the correct index
	unsigned int indexEdge;
	unsigned int indexEdgeTest;

	//indexVert so that we can assign indicies to the newly
	//created vertices
	unsigned int indexVert = vertices.size();


	std::map<unsigned int, EdgeInfo>::iterator tempIt;


	int test;

	for (i = 0; i < triangles.size(); i++)
	{
		triTemp = triangles[i];

		for (j = 0; j < 3; j++)
		{

			first = triTemp.vertices[j];
			second = triTemp.vertices[(j + 1) % 3];
			third = triTemp.vertices[(j + 2) % 3];


			//makes sure that we make do distinction of edges 
			//(first,second) and (second,first)



			if (first < second)
				indexEdge = first + second * numEdges;
			else
				indexEdge = second + first * numEdges;



			tempIt = testmap.find(indexEdge);
			if (testmap.end() == tempIt)
			{
                                //Interior case
                                //
				//fill in the info for adjaceny structure
				currentEdge.vertNbrs[0] = first;
				currentEdge.vertNbrs[1] = second;
				currentEdge.faceNbrs[0] = third;
				currentEdge.set = 1;
				currentEdge.boundary = 1;


				//assign this edge a vertex index
				//then increment the vertex index place ho
				//der
				//essentially adding a vertex to the end of
				//the vertices list
				currentEdge.index = indexVert;
				oddVert[j] = indexVert;
				indexVert++;

				//add element with its key to our map
				testmap.insert(
                                      std::pair<unsigned int,EdgeInfo>
					(indexEdge, currentEdge));


			}
			else
			{
			       //boundary case
				currentEdge = tempIt->second;
				currentEdge.faceNbrs[1] = third;
				currentEdge.boundary = 0;
				oddVert[j] = currentEdge.index;
				testmap[indexEdge] = currentEdge;
			}



			//This is for the filling in the info
			//for the even vertices
			vset[first].insert(second);
			vset[first].insert(third);



		}



		//assigns this newly created vertex to the 
		//the newly created triangle

		//Middle Triangle
		newTriMid.vertices[0] = oddVert[0];
		newTriMid.vertices[1] = oddVert[1];
		newTriMid.vertices[2] = oddVert[2];

		//Left Triangle
		newTriLeft.vertices[0] = triTemp.vertices[0];
		newTriLeft.vertices[1] = oddVert[0];
		newTriLeft.vertices[2] = oddVert[2];

		//Right Triangle
		newTriRight.vertices[0] = oddVert[2];
		newTriRight.vertices[1] = oddVert[1];
		newTriRight.vertices[2] = triTemp.vertices[2];

		//Upper Triangle
		newTriUp.vertices[0] = oddVert[0];
		newTriUp.vertices[1] = triTemp.vertices[1];
		newTriUp.vertices[2] = oddVert[1];

		//add newly made triangles to our list
		newTriangles[4 * i] = newTriUp;
		newTriangles[4 * i + 1] = newTriLeft;
		newTriangles[4 * i + 2] = newTriRight;
		newTriangles[4 * i + 3] = newTriMid;


	}

	

	

	//resize the vertices list to include all of the
	//new vertices that we just added

	vertices.resize(indexVert);


	//helpful for organzing
	Vector3 aplusb;
	Vector3 cplusd;
	Vector2 aplusbtex;
	Vector2 cplusdtex;

	EdgeInfo workingEdge;


	std::map<unsigned int, EdgeInfo>::iterator iter;
	

	for (iter = testmap.begin(); iter != testmap.end(); ++iter)
	{

		workingEdge = iter->second;

		if (workingEdge.boundary)
		{
			//boundary point

			aplusb = vertices[workingEdge.vertNbrs[0]].position
		             + vertices[workingEdge.vertNbrs[1]].position;
			aplusbtex = 
                                vertices[workingEdge.vertNbrs[0]].tex_coord
		             + vertices[workingEdge.vertNbrs[1]].tex_coord;

			vertices[workingEdge.index].position = 
                                                     (1.0 / 2.0)*aplusb;
			vertices[workingEdge.index].tex_coord = 
                                                   (1.0 / 2.0)*aplusbtex;
		}
		else
		{
			//interior point
			aplusb = vertices[workingEdge.vertNbrs[0]].position
			      + vertices[workingEdge.vertNbrs[1]].position;
			aplusbtex = 
                                vertices[workingEdge.vertNbrs[0]].tex_coord
			     + vertices[workingEdge.vertNbrs[1]].tex_coord;

			cplusd = vertices[workingEdge.faceNbrs[0]].position
			      + vertices[workingEdge.faceNbrs[1]].position;
			cplusdtex = 
                                vertices[workingEdge.faceNbrs[0]].tex_coord
			     + vertices[workingEdge.faceNbrs[1]].tex_coord;



			vertices[workingEdge.index].position =
				(3.0 / 8.0)*aplusb + (1.0 / 8.0)*cplusd;
			vertices[workingEdge.index].tex_coord =
			   (3.0 / 8.0)*aplusbtex + (1.0 / 8.0)*cplusdtex;



		}
	}


	
	std::set<unsigned int> currentSet;

	Vector3 v;
	Vector3 vAccum;
	Vector2 vAccumTex;
	Vector2 vtex;
	std::set<unsigned int>::iterator setiter;
	double beta;
	int numNbrs;
	for (i = 0; i < originalSize; i++)
	{
		currentSet = vset[i];
		numNbrs = currentSet.size();
		setiter = currentSet.begin();

		v = vertices[i].position;
		vtex = vertices[i].tex_coord;
		vAccum = vAccum.Zero;
		vAccumTex = vAccumTex.Zero;

		for (; setiter != currentSet.end(); ++setiter)
		{
			vAccum = vAccum + vertices[*setiter].position;
			vAccumTex = vAccumTex + vertices[*setiter].tex_coord;
		}

		//boundary case
		if ( numNbrs == 2)
		{

	              vertices[i].position = 
                                   (1.0 / 8.0)*vAccum + (3.0 / 4.0)*v;
		      vertices[i].tex_coord = 
                                (1.0 / 8.0)*vAccumTex + (3.0 / 4.0)*vtex;

		}
		else
		{

			//Interior Case
			//the value for beta
			beta = (1.0 / numNbrs)
				*
				((5.0 / 8.0)
				- pow(((3.0 / 8.0) + (1.0 / 4.0)*cos(2.0*PI / numNbrs)), 2));

			v = (1 - beta*numNbrs)*v + beta*vAccum;
			vtex = (1 - beta*numNbrs)*vtex + beta*vAccumTex;

			vertices[i].position = v;
			vertices[i].tex_coord = vtex;
		}

	}
	

	//Adding the new triangles 
	triangles.assign(newTriangles, newTriangles + (4*triangles.size()) );








	//calculation of all the normals
	Vector3 *normalTotal = (Vector3 *)calloc(vertices.size(),sizeof(Vector3));

	Vector3 bminusa;
	Vector3 cminusa;
	Vector3 currentNormal;
	MeshTriangle currentTri;
	for (i = 0; i < triangles.size(); i++)
	{
	
		currentTri = triangles[i];
		bminusa = vertices[currentTri.vertices[1]].position 
			- vertices[currentTri.vertices[0]].position;

		cminusa = vertices[currentTri.vertices[2]].position
			- vertices[currentTri.vertices[0]].position;

		currentNormal = normalize(cross(bminusa, cminusa));

		normalTotal[currentTri.vertices[0]] += currentNormal;
		normalTotal[currentTri.vertices[1]] += currentNormal;
		normalTotal[currentTri.vertices[2]] += currentNormal;

	}

	for (i = 0; i < vertices.size(); i++)
		vertices[i].normal = normalize(normalTotal[i]);

	free(normalTotal);
	free(newTriangles);




	create_gl_data();



    std::cout << "Subdivision has been implemented" << std::endl;
    return true;
}

} /* _462 */
