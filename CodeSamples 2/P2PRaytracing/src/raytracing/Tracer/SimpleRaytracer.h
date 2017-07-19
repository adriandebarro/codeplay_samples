#ifndef __SIMPLERAYTRACER__
#define __SIMPLERAYTRACER__

#include <vector>

#include "..\Tracer\Tracer.h"
#include "..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\CoreClasses\GeometricObjects\GeometricObjects.h"
#include "..\CoreClasses\Ray\Ray.h"

/*==========================================================================
Implementation of a Z-Buffer
============================================================================*/

namespace Engine
{
	using std::vector;
	class SimpleRayTracer : public Tracer
	{
	private:
		vector<GeometricObjects*> sceneObjects;

	public:

		SimpleRayTracer(vector<GeometricObjects*>& myObjects)
		{
			for (int index = 0; index < myObjects.size(); index++)
			{
				sceneObjects.push_back(myObjects[index]);
			}
		}

		SimpleRayTracer()
		{}

		using Tracer::TraceRay;
		virtual RGBColor TraceRay(const Ray& ray, bool p_shadowTest = false)
		{
			return TraceRay(ray.o, ray.d);
		}


		RGBColor TraceRay(const Point3D o, const Point3D d)
		{
			//double tmin = INT_MAX;
			//RGBColor ouptutColor;
			//ShadeRec rec;
			//for (int index = 0; index < sceneObjects.size(); index++)
			//{
			//	GeometricObjects *currentGeometricObject = sceneObjects[index];

			//	if (currentGeometricObject->hit(o, d, tmin, rec))
			//	{
			//		ouptutColor = currentGeometricObject->GetColor();
			//	}
			//}

			////retrun constant z 
			//return ouptutColor;
			return RGBColor(0);

			//once we have a hit we can follow to use 
		}
	};

}

#endif // !__SIMPLERAYTRACER__
