#pragma once 

#include "..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\CollisionDetails\ShadeRec.h"
#include "..\Light\Light.h"

namespace Engine
{
	class Ambient : public Engine::Light
	{
	public:
		virtual Point3D GetDirection(ShadeRec& sr)
		{
			return Point3D(0.0);
		}

		virtual RGBColor L(ShadeRec& sr)
		{
			return (m_color * m_ls);
		}

		Ambient() : Engine::Light(), m_ls(100), m_color(1.0)
		{
			SetLightType(AMBIENT_LIGHTING);
		}

		Ambient(RGBColor p_color, float p_ls) : Engine::Light(), m_ls(p_ls), m_color(p_color)
		{}

		virtual void TestIntersection(Ray& testRay, ShadeRec& sr)
		{}

	private:
		float m_ls;
		RGBColor m_color;
	};


}//Engine
