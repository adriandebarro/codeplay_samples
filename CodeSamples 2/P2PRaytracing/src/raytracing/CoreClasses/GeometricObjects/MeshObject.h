#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		MeshObject.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <string>
#include <iostream>

#include "..\..\AccelerationStructures\BVH.h"
#include "..\..\AccelerationStructures\KdTree.h"
#include "..\GeometricObjects\GeometricObjects.h"
#include "..\..\Loaders\ObjLoader.h"

using std::string;
using std::cout;
using std::endl;

using namespace Engine::AccelerationStructures;

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: ObjectMesh
	//		   Description: Represent a TRIS. Concrete implementation of GeometricObjects
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class ObjectMesh : public GeometricObjects
	{
	public:
		vector<GeometricObjects*> m_objects;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		ObjectMesh()
		{}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Obj File path, Translation Coordinates, Boolean if it has textures
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		ObjectMesh(string p_path, Point3D centorid = Point3D(0), bool p_useTexture = false)
		{
			clock_t startingTimer = clock(), endingTimer;
			m_objects = ObjLoader().LoadFile(p_path, centorid, p_useTexture);
			endingTimer = clock();
			double timeInSeconds = ((double)endingTimer - startingTimer) / (double)CLOCKS_PER_SEC;
			Logger().LogTrace(to_string(timeInSeconds));

			/*for (int index = 0; index < m_objects.size(); index ++)
			{
				if (m_objects[index]->GetMaterial()->GetMaterialName() != "kamen_zid")
				{
					cout << "error here" << endl;
				}
			}*/
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: MakeBoundingBox
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using GeometricObjects::MakeBoundingBox;
		void MakeBoundingBox()
		{
			box = accelerationStructure->m_boundingBox;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: hit
		//		Return Value: bool
		//		  Parameters: Ray Origin, Ray Direction, Shortest Intersection distance, Collision Details
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using GeometricObjects::hit;
		bool hit(Point3D o, Point3D  d, double& tmin, ShadeRec& sr)
		{
			double tmax = INT_MIN;
			Ray currentRay(o, d);
			bool hitObject = accelerationStructure->TestIntersection(currentRay, sr, tmin);
			return hitObject;
		}

		bool hit(Ray& p_currentRay, Intersection& sr)
		{
			double tmin = INT_MAX;
			double tmax = INT_MIN;
			ShadeRec rec;
			bool hitObject = accelerationStructure->TestIntersection(p_currentRay, rec, tmin, true);
			return hitObject;
		}
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Hits. </summary>
		///
		/// <remarks>	Adrian, 27/08/2015. </remarks>
		///
		/// <param name="p_ray">	The ray. </param>
		/// <param name="tmin"> 	[in,out] The tmin. </param>
		/// <param name="sr">   	[in,out] The sr. </param>
		///
		/// <returns>	true if it succeeds, false if it fails. </returns>
		///-------------------------------------------------------------------------------------------------
		virtual	bool hit(Ray p_ray, double &tmin, ShadeRec &sr)
		{
			double tempTmin = INT_MAX;
			return hit(p_ray.o, p_ray.d, tmin, sr);
		}


		virtual bool hit(Ray p_ray, Intersection &p_intersection_out)
		{
			double tempTmin = INT_MAX;
			ShadeRec tempRec;
			bool hitResult = hit(p_ray, tempTmin, tempRec);
			p_intersection_out.t = tempRec.tmin;
			return hitResult;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetNormal
		//		Return Value: VOID
		//		  Parameters: Collision Point on Object Mesh
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using GeometricObjects::GetNormal;
		virtual Point3D GetNormal(Point3D p_location)
		{
			//TODO fix this 
			return Point3D();
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: BuildScene
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void BuildScene(void)
		{
			accelerationStructure = new BVH(m_objects, MIDDLE_POINT, true);
			MakeBoundingBox();
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: BuildScene
		//		Return Value: VOID
		//		  Parameters: bool flatten bvh
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void BuildScene(bool p_flatten, int p_accelerationType = 0)
		{
			if (p_accelerationType == 0)
			{ 
				accelerationStructure = new BVH(m_objects, MIDDLE_POINT, p_flatten);
			}
			else
			{
				accelerationStructure = new Accelerator_KDT<Primitives::Triangle>(m_objects);
			}

			MakeBoundingBox();
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: PrintObjectName
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		~ObjectMesh(void)
		{}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: PrintObjectName
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void PrintObjectName(void)
		{
			Logger::LogInfo("Triangle soup " + GetMaterial()->GetMaterialName() + "---------");
		}
	private:
		AccelerationStructure* accelerationStructure;
		
	protected:
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------

