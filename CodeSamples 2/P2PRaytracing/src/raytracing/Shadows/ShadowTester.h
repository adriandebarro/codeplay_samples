#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		ShadowTester.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <vector>

#include "..\Tracer\Tracer.h"
#include "..\Light\Light.h"
#include "..\CoreClasses\Ray\ShadowRay\ShadowRay.h"
#include "..\CollisionDetails\ShadeRec.h"
#include "..\CoreClasses\Ray\Ray.h"
#include "..\Scene\Scene.h"

using std::vector;

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: ShadowTester
	//		   Description: Represent a TRIS. Concrete implementation of GeometricObjects
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: 
	//						TODO: apply in code
	//----------------------------------------------------------------------------------------------------------------
	class ShadowTester
	{
	public:
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		ShadowTester()
		{}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Tracer*, vector<Light*> all Lights
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		ShadowTester(Tracer* p_currentTracer,Scene* p_scene)
		{
			m_tracerPtr = p_currentTracer;
			m_scene = p_scene;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TestForShadow
		//		Return Value: VOID
		//		  Parameters: ShadeRec 
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool TestForShadow(ShadeRec& p_currentDetails)
		{
			for (int lightIndex = 0; lightIndex < m_scene->m_sceneLight.size(); lightIndex ++ )
			{
				Light* currentLight = m_scene->m_sceneLight[lightIndex];

				Point3D epsillonCollisionPoint = p_currentDetails.GetkEpsillonCollisionPoint();
				Point3D direction = currentLight->GetDirection(p_currentDetails);
				direction.normalize();

				ShadowRay currentShadowRay(epsillonCollisionPoint, direction);

				ShadeRec currentDetails;
				currentLight->TestIntersection(currentShadowRay, currentDetails);

				/*if (TestShadowRay(currentShadowRay, currentLight, currentDetails.tmin))
				{
					return true;
				}*/	
			}
			return false;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TestShadowRay
		//		Return Value: VOID
		//		  Parameters: ShadowRay, Light* Current Light, float Smallest Tmin
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		static bool TestShadowRay(ShadowRay p_shadowRay, double p_distanceToLight, Tracer* p_tracer )
		{
			return p_tracer->TestShadow(p_shadowRay);
		}
	private:
		Tracer* m_tracerPtr;
		Scene* m_scene;
	};
}

