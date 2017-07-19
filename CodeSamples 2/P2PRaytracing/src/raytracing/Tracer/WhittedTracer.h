#ifndef __WHITTEDTRACER__
#define __WHITTEDTRACER__

#include <vector>

#include "..\Tracer\Tracer.h"
#include "..\CoreClasses\GeometricObjects\GeometricObjects.h"
#include "..\Light\Light.h"
#include "..\Scene\Scene.h"
#include "..\AccelerationStructures\BVH.h"


namespace Engine
{
	class WhittedTracer : public Engine::Tracer
	{
	private:
		Scene* m_ScenePtr;
		BVH* m_accelerationStructre;
	public:

		WhittedTracer(Scene* p_ScenePtr)
		{
			m_ScenePtr = p_ScenePtr;
			m_accelerationStructre = new BVH(p_ScenePtr->m_sceneObjects);
			
		}

		WhittedTracer()
		{}

		using Engine::Tracer::TraceRay;
		virtual RGBColor TraceRay(Ray& ray)
		{

			int amountHits = 0;
			return TraceRay(ray.o, ray.d, amountHits);
		}


		RGBColor TraceRay(Point3D& o, Point3D& d, int& depth) const
		{
			return RGBColor(1);
		}


		using Engine::Tracer::TraceRay;
		RGBColor TraceRay(Ray& ray, ShadeRec& sr, int depth)
		{
			bool intersection = m_accelerationStructre->TestIntersection(ray, sr);

			if (intersection)
			{
				GeometricObjects* currentObject = sr.object;
				Material* materialPtr = currentObject->GetMaterial();
				RGBColor outputColor(0);
				
				
			}
		}

		~WhittedTracer()
		{
			
		}
	};
}

#endif // !__WHITTEDTRACER__
