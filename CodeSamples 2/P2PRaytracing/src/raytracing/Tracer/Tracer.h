#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Tracer.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------

#include "../Scene/Scene.h"

#include "..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\CoreClasses\GeometricObjects\GeometricObjects.h"
#include "..\CoreClasses\Ray\Ray.h"
#include "..\CoreClasses\Ray\ShadowRay\ShadowRay.h"

namespace Engine
{
	namespace AccelerationStructures
	{
		class BVH;
	}
}

using Engine::AccelerationStructures::BVH;

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: Tracer
	//		   Description: Represent a TRIS. Concrete implementation of GeometricObjects
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class Tracer
	{
	public:
		friend class IrradianceCache;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TraceRay
		//		Return Value: bool
		//		  Parameters: Ray, CollisionDetails, bool test for shadow
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual bool TraceRay(Ray currentRay, ShadeRec& sr, bool p_shadowTestray = false) = 0;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Tracer()
		{
			accelerated = false;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TraceRay
		//		Return Value: bool
		//		  Parameters: Origin, Direction, Collision Details
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual bool TraceRay(Point3D o,Point3D d, ShadeRec& sr)
		{
			return false;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TraceRay
		//		Return Value: bool
		//		  Parameters: Origin of ray, Direction of ray, depth of bounces, Collision Details
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual bool TraceRay(Point3D o, Point3D d, int& depth, ShadeRec& sr)
		{
			return false;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TraceRay
		//		Return Value: bool 
		//		  Parameters: Origin of Ray, Direction of ray, smallest distance to collision, CollisionDetails
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual bool TraceRay(Point3D o, Point3D d, float& tmin, int& depth, ShadeRec& sr)
		{
			return false;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ShadowTest
		//		Return Value: bool 
		//		  Parameters: Collision Details
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual bool TestShadow(ShadowRay& p_shadowRay)
		{
			return false;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: UseAccelerationStructure
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void UseAccelerationStructure(void)
		{
			accelerated = true;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: DontUseAccelerationSturcture
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void DontUseAccelerationSturcture(void)
		{
			accelerated = false;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetScene
		//		Return Value: Scene*
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Scene* GetScene(void)
		{
			return sceneObjects;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetScene
		//		Return Value: VOID
		//		  Parameters: Scene*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetScene(Scene* p_currentScene)
		{
			sceneObjects = p_currentScene;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetSccelerationStructure
		//		Return Value: BVH*
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		BVH* GetAccelerationStructure(void)
		{
			return accelerationStructure;
		}

	protected:
		bool accelerated;
		Scene *sceneObjects;
		BVH *accelerationStructure;
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------