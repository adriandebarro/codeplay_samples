#pragma once 

#include "..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\CoreClasses\Normal\Normal.h"
#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "..\CoreClasses\Ray\Ray.h"
#include "..\CoreClasses\Point\Point2D\Point2D.h"
#include "..\CoreClasses\UtilClasses\Common.h"

using namespace std;
using namespace Engine::Common;
using Engine::Common::GeomerticPrimitives::Intersection;


namespace Engine
{
	class GeometricObjects;
	class ShadeRec
	{
	public:
		bool hitAnObject;
		Point3D localHitPoint;
		Point3D hitPoint;
		Normal normal;
		RGBColor myColor;
		float m_xRandValue;
		float m_yRandValue;
		GeometricObjects *object;
		float tmin;
		int depth;
		Ray ray;
		Point3D m_shadingNormal;
		//in case a meshobject is hit
		GeometricObjects* m_hitTriangle;
		Point2D textureCoordinates;

		union BarycetricConstants
		{
			float coor[3];
			float alpha, beta, gamma;
		}m_barycetric_constants;

		ShadeRec()
		{
			hitAnObject = false;
		}

		/*ShadeRec(ShadeRec& p_currentShadeRec)
		{
			hitAnObject = p_currentShadeRec.hitAnObject;
			localHitPoint = p_currentShadeRec.localHitPoint;
			ray = p_currentShadeRec.ray;
			tmin = p_currentShadeRec.tmin;
		}*/

		ShadeRec(float p_xRandValue, float p_yRandValue)
		{
			hitAnObject = false;
			m_xRandValue = p_xRandValue;
			m_yRandValue = p_yRandValue;
		}

		ShadeRec(const ShadeRec& sr) : hitAnObject(sr.hitAnObject), localHitPoint(sr.localHitPoint)
		{
			hitAnObject = false;
			normal = Normal();
		}

		Point3D GetSpecularReflection()
		{
			//reflection equation 
			Point3D reflDir = ray.d - Normal((normal * 2 * DOT(normal, ray.d)));
			reflDir.normalize();
			return reflDir;
		}

		//void operator = (const ShadeRec& rhs)
		//{
		//	hitAnObject = rhs.hitAnObject;
		//	localHitPoint = rhs.localHitPoint;
		//	ray = rhs.ray;
		//	tmin = rhs.tmin;
		//}

		ShadeRec operator = (const ShadeRec& rhs)
		{
			return ShadeRec(rhs);
		}

		//p = o + dt where t = kEpsillon
		Point3D GetkEpsillonCollisionPoint()
		{
			Point3D output;
			normal.normalize();
			output = localHitPoint + kEpsilon * GetNormal();
			return output;
		}

		Point3D GetkEpsillonCollisionPoint(Point3D p_direction)
		{
			p_direction.Normalize();
			Point3D output = localHitPoint + kEpsilon* p_direction;
			return output;
		}

		Point3D GetNormal()
		{
			return Point3D(normal.x, normal.y, normal.z);
		}

		void SetHitAnObject()
		{
			hitAnObject = true;
		}

		void SetLocalHitPoint(bool isHit)
		{
			hitAnObject = isHit;
		}

		~ShadeRec()
		{
			//delete m_hitTriangle;
		}
	};
}
