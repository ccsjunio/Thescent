#ifndef _iDebugRenderer_HG_
#define _iDebugRenderer_HG_

// This is the interface that EVERYTHING can use.
// Adds the ability to add debug items to draw

#include <string>
#include <map>
#include <GLCommon.h>
#include <MathLibrary.h>
#define GLM_ENABLE_EXPERIMENTAL		// Needed to use glm quaternions, now
#include <glm/gtx/quaternion.hpp>
#include <vector>

class iDebugRenderer
{
public:
	virtual ~iDebugRenderer() {};

	struct sDebugTri
	{
		sDebugTri();
		sDebugTri(Vector3D v1, Vector3D v2, Vector3D v3);	// colour = 1,1,1	lifeTime=0.0f
		sDebugTri(Vector3D v1, Vector3D v2, Vector3D v3, Vector3D colour, float lifeTime=0.0f);
		sDebugTri(Vector3D v[3], Vector3D colour, float lifeTime=0.0f);
		Vector3D v[3];		
		Vector3D colour;
		float lifeTime;	
		bool bIgnorDepthBufferOnDraw;
	};

	struct sDebugLine
	{
		sDebugLine();
		sDebugLine(Vector3D start, Vector3D end);		// colour = 1,1,1	lifeTime=0.0f
		sDebugLine(Vector3D start, Vector3D end, Vector3D colour, float lifeTime=0.0f);
		sDebugLine(Vector3D points[2], Vector3D colour, float lifeTime=0.0f);
		Vector3D points[2];		
		Vector3D colour;
		float lifeTime;
		bool bIgnorDepthBufferOnDraw;
	};

	struct sDebugPoint
	{
		sDebugPoint();
		sDebugPoint(Vector3D xyz);		// colour=1,1,1		lifeTime=0.0f	pointSize=1.0f
		sDebugPoint(Vector3D xyz, Vector3D colour, float lifeTime=0.0f, float pointSize=1.0f);
		Vector3D xyz;		
		Vector3D colour;	
		float pointSize;
		float lifeTime;
		bool bIgnorDepthBufferOnDraw;
	};

	struct sDebugMesh
	{
		sDebugMesh();
		std::string name;
		sDebugMesh(std::string name);	// Assumes a size of 1, colour white
		sDebugMesh(std::string name, Vector3D xyz, float lifeTime=0.0f);
		sDebugMesh(std::string name, Vector3D xyz, Vector3D colour, float scale, float lifeTime=0.0f);
		sDebugMesh(std::string name, Vector3D xyz, Vector3D EulerOrientation, Vector3D colour, float scale, float lifeTime=0.0f);
		sDebugMesh(std::string name, Vector3D xyz, glm::quat qOrientation, Vector3D colour, float scale, float lifeTime=0.0f);
		Vector3D xyz; 		
		glm::quat qOrientation;		
		float scale; 
		Vector3D colour;	
		float lifeTime;
		bool bIgnorDepthBufferOnDraw;
	};

	virtual void addTriangle(Vector3D v1XYZ, Vector3D v2XYZ, Vector3D v3XYZ, Vector3D colour, float lifeTime=0.0f) = 0;
	virtual void addTriangle(sDebugTri &tri) = 0;
	virtual void addLine(Vector3D startXYZ, Vector3D endXYZ, Vector3D colour, float lifeTime=0.0f) = 0;
	virtual void addLine(sDebugLine &line) = 0;
	virtual void addPoint(Vector3D xyz, Vector3D colour, float lifeTime=0.0f, float pointSize=1.0f) = 0;
	virtual void addPoint(sDebugPoint &point) = 0;

	//virtual void addDebugMesh(sDebugMesh &mesh) = 0;
	//virtual void addDebugMesh(std::string name, Vector3D xyz, float lifeTime=0.0f) = 0;
	//virtual void addDebugMesh(std::string name, Vector3D xyz, Vector3D colour, float scale, float lifeTime=0.0f) = 0;

	// Draws a sphere to the scene
	virtual void addDebugSphere(Vector3D xyz, Vector3D colour, float scale, float lifeTime=0.0f) = 0;
	static const std::string DEFAULT_DEBUG_SPHERE_MESH_NAME;// = "DSPHERE";
	virtual void addDebugMesh(std::string meshName, Vector3D xyz, Vector3D orientXYZ_Euler, Vector3D colour, float scale, float lifeTime=0.0f) = 0;
	virtual void addDebugMesh(std::string meshName, Vector3D xyz, glm::quat orientXYZ_Quaternion, Vector3D colour, float scale, float lifeTime=0.0f) = 0;

	// Used to draw an arbitrary mesh. If 
//	virtual void addDebugMesh(std::string name, Vector3D xyz, Vector3D colour, float scale, float lifeTime=0.0f) = 0;
//	virtual void addDebugMesh(std::string name, Vector3D xyz, Vector3D colour, Vector3D EulerOrientation, float scale, float lifeTime=0.0f) = 0;
//	virtual void addDebugMesh(std::string name, Vector3D xyz, Vector3D colour, glm::quat qOrientation, float scale, float lifeTime=0.0f) = 0;

	// Various meshes that you could load and draw (are drawn with static meshes)
	// Note: the mesh is ONLY triangles, so not indexed
	virtual void loadDebugMesh(std::string friendlyName, std::vector<sDebugTri> &vecTris) = 0;
	// Generates a isosphere. 
	// numIterations = 1 gives 12 points and 20 triangles
	// 2 or more is number of recursive subdivides, so 2 = 20x4 = 80 triangles
	// More than 4 or 5 is pretty crazy for a debug shape.
	virtual void generateIsoSphere( unsigned int numIterations, float radius, std::vector<sDebugTri> &vecTris) = 0;
	//// Uses simulated annealing to distribute the points around the sphere
	//struct sAnnealingSphereSettings
	//{
	//	static const unsigned int ANNEALING_SPHERE_SETTINGS_DEFAULT_NUMBER_OF_POINTS = 1000;
	//	// These are because you can't do static const float in header
	//	#define ANNEALING_SPHERE_SETTINGS_DEFAULT_TIMEOUT_IN_SECONDS 1.0f
	//	#define ANNEALING_SPHERE_SETTINGS_DEFAULT_EPSILON  (2.0f*FLT_EPSILON)

	//	sAnnealingSphereSettings() : 
	//		numPoints(ANNEALING_SPHERE_SETTINGS_DEFAULT_NUMBER_OF_POINTS), 
	//		timeoutInSeconds(ANNEALING_SPHERE_SETTINGS_DEFAULT_TIMEOUT_IN_SECONDS), 
	//		epsilon(ANNEALING_SPHERE_SETTINGS_DEFAULT_EPSILON) {};
	//	unsigned int numPoints;	// 1 to whatever (default is 1000)
	//	float timeoutInSeconds;	// time limit for annealing
	//	float epsilon;			// Error limit (if points move less than this for 2 steps,
	//							//  then the points are 'in position'. Default is 2x FLT_EPSILON 

	//};
	//virtual void generateSphereAnnealing(sAnnealingSphereSettings settings, float radius, std::vector<sDebugTri> &vecTris) = 0;

};

typedef iDebugRenderer::sDebugTri	drTri;	
typedef iDebugRenderer::sDebugLine	drLine;
typedef iDebugRenderer::sDebugPoint drPoint;
typedef iDebugRenderer::sDebugMesh  drMesh;


#endif
