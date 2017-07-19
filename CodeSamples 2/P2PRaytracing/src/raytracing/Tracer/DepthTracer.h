#pragma once 
//----------------------------------------------------------------------------------------------------------------
//			Filename:		MeshObject.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <vector>

#include "Tracer.h"
#include "..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\CoreClasses\GeometricObjects\GeometricObjects.h"
#include "..\Scene\Scene.h"
#include "..\CoreClasses\Ray\Ray.h"
#include "..\AccelerationStructures\BVH.h"
#include "..\CoreClasses\Ray\ShadowRay\ShadowRay.h"
#include "CoreClasses/Debugging/MatlabUtilityDebuger.h"

using namespace Engine::AccelerationStructures;

namespace Engine
{
	using std::vector;

	///-------------------------------------------------------------------------------------------------
	/// <summary>	A depth tracer. </summary>
	///
	/// <remarks>	Adrian, 19/07/2017. </remarks>
	///-------------------------------------------------------------------------------------------------
	class DepthTracer : public Engine::Tracer
	{
	public:
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		DepthTracer(Scene * p_scene, bool p_flatten = false, PartitioningStyle p_partitioningStyle = MIDDLE_POINT)
		{
			sceneObjects = p_scene;
			accelerationStructure = new BVH(sceneObjects->m_sceneObjects,p_partitioningStyle, p_flatten);
			accelerated = false;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Default constructor. </summary>
		///
		/// <remarks>	Adrian, 19/07/2017. </remarks>
		///-------------------------------------------------------------------------------------------------

		DepthTracer()
		{}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TraceRay
		//		Return Value: VOID
		//		  Parameters: Ray to test, Collision Details, bool if to shadow test
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using Engine::Tracer::TraceRay;
		virtual bool TraceRay(Ray ray, ShadeRec& rec, bool p_shadowRayTest = false)
		{
			double tmin = INT_MAX;
			rec.ray = ray;

			return TraceRay(ray.o, ray.d, rec);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TestShadow
		//		Return Value: bool
		//		  Parameters: collsiion details of point to test for shadow, light Position
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using Tracer::TestShadow;
		virtual bool TestShadow(ShadowRay& p_shadowRay)
		{
			ShadeRec collisondetails;
			double distanceToHit = INT_MAX;
			bool hit = accelerationStructure->TestIntersection(p_shadowRay, collisondetails, distanceToHit, true);
			return hit;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TraceRay
		//		Return Value: bool
		//		  Parameters: Origin of ray, Direction of ray, collision details
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool TraceRay(Point3D o, Point3D d, ShadeRec& rec)
		{
			double tmin = INT_MAX;
			RGBColor ouptutColor(0);
			bool tempHit = false;
			for (int index = 0; index < sceneObjects->m_sceneObjects.size(); index++)
			{
				GeometricObjects *currentGeometricObject = sceneObjects->m_sceneObjects[index];
				tempHit = currentGeometricObject->hit(o, d, tmin, rec);
			}
			//return constant z 
			return rec.hitAnObject;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetAccelerationStructure
		//		Return Value: bool
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		BVH* GetAccelerationStructure(void)
		{
			return accelerationStructure;
		}
	};

}
