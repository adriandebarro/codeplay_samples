#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Objects.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <iostream>
#include <math.h>

#include "GeometricObjects.h"
#include "..\Point\Point3D\Point3D.h"
#include "..\Normal\Normal.h"
#include "..\Ray\Ray.h"
#include "..\..\CollisionDetails\ShadeRec.h"
#include "..\RGBObject\RGBColor.h"
#include "..\UtilClasses\Common.h"
#include "..\BoundingShapes\BoundingBox.h"
#include "..\..\AccelerationStructures\BVH.h"
#include "..\GeometricObjects\Triangle.h"
#include "..\..\Loaders\ObjLoader.h"

using std::cout;

using namespace Engine::AccelerationStructures;

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: Sphere
	//		   Description: Concrete implementation of GeometricObjects
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class Sphere : public GeometricObjects
	{
	public:
		Point3D centerPoint;
		float radius;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Sphere()
		{}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Spehere center, Sphere radius, Material
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Sphere(Point3D p_center, float p_radius, Engine::Materials::Material* p_mtrPtr) : centerPoint(p_center), radius(p_radius)
		{
			SetMaterial(p_mtrPtr);
			MakeBoundingBox();
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetColor
		//		Return Value: VOID
		//		  Parameters: RgbColor
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetColor(RGBColor _color)
		{
			color = _color;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetColor
		//		Return Value: VOID
		//		  Parameters: float, float, float
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetColor(float _r, float _g, float _b)
		{
			color = RGBColor(_r, _g, _b);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetColor
		//		Return Value: RGBColor
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor GetColor(void)
		{
			return color;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetRadius
		//		Return Value: VOID
		//		  Parameters: float
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetRadius(float _radius)
		{
			radius = _radius;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetCenterPoint
		//		Return Value: VOID
		//		  Parameters: float, float, float
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetCenterPoint(float p_x, float p_y, float p_z)
		{
			centerPoint = Point3D(p_x, p_y, p_z);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetCenterPoint
		//		Return Value: VOID
		//		  Parameters: Point3D
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetCenterPoint(Point3D point)
		{
			centerPoint = point;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetNormal
		//		Return Value: Point3D
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using GeometricObjects::GetNormal;
		Point3D GetNormal(Point3D p_hitPoint)
		{
			return radius * (p_hitPoint - centerPoint);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: hit
		//		Return Value: bool
		//		  Parameters: Origin Point, Direction Point, shortest distance to collision, Collision Details
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using GeometricObjects::hit;
		virtual bool hit(Point3D o, Point3D d, double &tmin, ShadeRec& sr)
		{
			Point3D l = centerPoint - o;
			float tca = l.dot(d);
			if (tca < 0) return false;
			float d2 = l.dot(l) - tca * tca;
			if (d2 > radius) return false;
			float thc = sqrt(radius - d2);
			float t0 = tca - thc;
			float t1 = tca + thc;
			if (t0 > kEpsilon && t1 > kEpsilon && (t0 < tmin || t1 < tmin))
			{

				if (t0 < t1 && t0 > kEpsilon)
				{
					tmin = t0;

				}
				else
				{
					tmin = t1;
				}

				sr.localHitPoint = o + (d * tmin);
				sr.normal = (((double)radius)* (sr.localHitPoint - centerPoint));
				sr.normal.normalize();
				sr.object = ((GeometricObjects*)this);
				sr.hitAnObject = true;
				sr.myColor = GetColor();
				sr.tmin = tmin;
				return (true);
			}

			return false;
		}

		virtual bool hit(Ray p_ray, Intersection &p_intersection_out)
		{
			return false;
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
			return hit(p_ray.o, p_ray.d, tmin, sr);
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
			Point3D minPoint(centerPoint.x - radius, centerPoint.y - radius, centerPoint.z - radius);
			Point3D maxPoint(centerPoint.x + radius, centerPoint.y + radius, centerPoint.z + radius);

			box = new Engine::BoundingBox(minPoint, maxPoint);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: PrintObjectName
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual void PrintObjectName()
		{
			cout << "Sphere at location " + centerPoint.toString() << "---------" << std::endl;
		}
	};
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: Tris
	//		   Description: Depricrated Mesh representer. 
	//					    Concrete implementation of GeometricObjects
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class Tris :public GeometricObjects
	{
	public:
		Engine::AccelerationStructures::BVH *triangleSoup;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: Tris
		//		Return Value: VOID
		//		  Parameters: Path, Material, Transition Center, Flatten Bvh
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Tris(std::string p_path, Engine::Materials::Material* p_material, Point3D centroid = Point3D(0), bool enhance = false)
		{
			vector<GeometricObjects*> soup = centroid != 0 ? ObjLoader().LoadFile(p_path, centroid) : ObjLoader().LoadFile(p_path);
			triangleSoup = new Engine::AccelerationStructures::BVH(soup, PartitioningStyle::MIDDLE_POINT, enhance);
			SetMaterial(p_material);
			MakeBoundingBox();
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: hit
		//		Return Value: VOID
		//		  Parameters: Ray Origin, Ray Direction, Collision Details
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using GeometricObjects::hit;
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Hits. </summary>
		///
		/// <remarks>	Adrian, 27/08/2015. </remarks>
		///
		/// <param name="o">   	The Point3D to process. </param>
		/// <param name="d">   	The Point3D to process. </param>
		/// <param name="tmin">	[in,out] The tmin. </param>
		/// <param name="sr">  	[in,out] The sr. </param>
		///
		/// <returns>	true if it hits the tris, false if it doesnt hit the tris. </returns>
		///-------------------------------------------------------------------------------------------------
		virtual bool hit(Point3D o, Point3D d, double &tmin, ShadeRec &sr)
		{
			Ray currentRay = Ray(o, d);
			bool intersection = triangleSoup->TestIntersection(currentRay, sr, tmin);
			if (intersection)
			{
				sr.object = this;
			}
			return intersection;
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
		virtual bool hit(Ray p_ray, double &tmin, ShadeRec &sr)
		{
			bool intersection = triangleSoup->TestIntersection(p_ray, sr, tmin);
			if (intersection)
			{
				sr.object = this;
			}
			return intersection;
		}

		virtual bool hit(Ray p_ray, Intersection &p_intersection_out)
		{
			return false;
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
			box = triangleSoup->rootNode->boundingBox;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetBoundingBox
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Engine::BoundingBox* GetBoundingBox()
		{
			return box;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetNormal
		//		Return Value: VOID
		//		  Parameters: Collision Point
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using GeometricObjects::GetNormal;
		Point3D GetNormal(Point3D p_hitPoint)
		{
			return Point3D();
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: PrintObjectName
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual void PrintObjectName()
		{
			cout << "Triangle soup " + GetMaterial()->GetMaterialName() << "---------" << endl;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: DESTRUCTOR
		//		Return Value: VOID
		//		  Parameters: BoundingBox pointer, CurrentIndex, ObjectToWrapAround
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		~Tris()
		{
			delete triangleSoup;
		}
	private:
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------
