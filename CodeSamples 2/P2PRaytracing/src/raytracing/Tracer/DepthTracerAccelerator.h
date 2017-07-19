#ifndef __DEPTHTRACERACCELERATOR__
#define __DEPTHTRACERACCELERATOR__

#include <vector>

#include "..\Tracer\Tracer.h"
#include "..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\CoreClasses\GeometricObjects\GeometricObjects.h"
#include "..\CoreClasses\Ray\Ray.h"
#include "..\AccelerationStructures\BVH.h"

/*==========================================================================
Implementation of a Z-Buffer
============================================================================*/

namespace Engine
{
	using std::vector;
	class DepthTracerAccelerator : public Tracer
	{
	private:
		vector<GeometricObjects*> sceneObjects;
		Engine::BVH collisionTree;

	public:

		DepthTracerAccelerator(vector<GeometricObjects*>& myObjects)
		{
			sceneObjects = myObjects;
			collisionTree = Engine::BVH(sceneObjects);
		}

		DepthTracerAccelerator()
		{}

		using Tracer::TraceRay;
		virtual RGBColor TraceRay(const Ray& ray)
		{
			return TraceRay(ray.o, ray.d);
		}


		RGBColor TraceRay(const Point3D& o, const Point3D& d) const
		{
			double tmin = INT_MAX;
			RGBColor ouptutColor;
			ShadeRec rec;
			for (int index = 0; index < sceneObjects.size(); index++)
			{
				GeometricObjects *currentGeometricObject = sceneObjects[index];

				if (currentGeometricObject->hit(o, d, tmin, rec))
				{
					ouptutColor = currentGeometricObject->GetColor();
				}
			}

			//retrun constant z 
			return ouptutColor;
			//once we have a hit we can follow to use 
		}
	};

}
#endif // !__DEFAULTRAYTRACER__
