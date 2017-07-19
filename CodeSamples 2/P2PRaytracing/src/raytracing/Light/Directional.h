#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Directional.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------

#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "..\Light\Light.h"
#include <math.h>
#include "..\CoreClasses\GeometricObjects\Objects.h"

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: PointLight
	//		   Description: Concrete implementation of a Point Light
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class Directional : public Light
	{
	public:
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetDirection
		//		Return Value: Point3D
		//		  Parameters: CollisionDetails
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using Engine::Light::GetDirection;
		Point3D GetDirection(ShadeRec& sr)
		{
			return -1*m_location;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: L
		//		Return Value: RGBColor
		//		  Parameters: CollisionDetails
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using Engine::Light::L;
		RGBColor L(ShadeRec& sr)
		{
			//float angle = GetDirection().dot(sr.GetNormal());
			return (color * m_ls);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Directional()
		{
			color = RGBColor(1);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Point3D location, RGBColor LightColor, float Radius, float Luminous Power
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Directional(Point3D p_direction, RGBColor p_color = RGBColor(1), float p_radius = 1, float p_radiance = 1.0f)
		{

			SetLightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
			color = p_color;
			//m_objectRepresentation = new Sphere(p_location, p_radius, p_color);
			SetRadiance(p_radiance);
			
			p_direction.normalize();
			SetDirection(p_direction);

			cout << m_location.toString() << endl;
			
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TestIntersection
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: 
		//					TODO: to be deleted 
		//----------------------------------------------------------------------------------------------------------------
		using Engine::Light::TestIntersection;
		virtual void TestIntersection(Ray& p_currentRay, ShadeRec& p_details)
		{
			double tmin = INT_MAX;
			m_objectRepresentation->hit(p_currentRay.o, p_currentRay.d, tmin, p_details);
		}

	private:
		RGBColor color;
	};
}
