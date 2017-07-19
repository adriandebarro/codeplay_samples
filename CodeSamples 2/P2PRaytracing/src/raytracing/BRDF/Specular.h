#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		BRDF.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: READY
//----------------------------------------------------------------------------------------------------------------
#include <math.h>

#include "..\CoreClasses\GeometricObjects\GeometricObjects.h"
#include "BRDF.h"

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: Specular
	//		   Description: Concrete implementation of the abstract class BRDF, simulates specularity of a material
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class Specular : public Engine::BRDF
	{
	public:
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Specular constant, RGBColor color
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Specular(float p_Rs, RGBColor p_color) : BRDF(p_color)
		{
			m_Ks = p_Rs;
			m_Kd = p_color;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Specular constant, lobe power, RGBColor color
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Specular(float p_Rs, float p_lobePower, RGBColor p_color) : BRDF(p_color)
		{
			m_Ks = p_Rs;
			m_lobePower = p_lobePower;
			m_Kd = p_color;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: f
		//		Return Value: RGBColor
		//		  Parameters: Specular constant, input direction, output direction, boolean texture
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using Engine::BRDF::f;
		virtual RGBColor f(ShadeRec& sr, Point3D wi, Point3D w0, RGBColor& p_colorAtPoint, bool usesTexture = false)
		{
			RGBColor l(0);
			Point3D N = sr.object->GetNormal(sr.localHitPoint);
			float rayD_DotR = w0.dot(sr.ray.d);
			float lightDotN = N.dot(wi);

			float specComponent = m_Ks * (pow(rayD_DotR, 10) / lightDotN);
			return  m_Kd * specComponent;
		}

		virtual RGBColor f(ShadeRec& sr, Point3D wi, Point3D w0, bool usesTexture)
		{
			return RGBColor(0);
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: sampleF
		//		Return Value: RGBColor
		//		  Parameters: Collision Details , input direction, output direction
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual RGBColor sampleF(ShadeRec& sr, Point3D wi, Point3D w0)
		{
			return RGBColor(0);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: rho
		//		Return Value: VOID
		//		  Parameters: Collision Details , output direction
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual RGBColor rho(ShadeRec& sr, Point3D w0)
		{
			return RGBColor(0);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetKs
		//		Return Value: VOID
		//		  Parameters: float specular constant
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetKs(float p_Ks)
		{
			m_Ks = p_Ks;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetKs
		//		Return Value: float
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		float GetKs(void)
		{
			return m_Ks;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetKs
		//		Return Value: float
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetColor(RGBColor& p_Color)
		{
			m_Kd = p_Color;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetColor
		//		Return Value: RGBColor
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor GetColor(void)
		{
			return m_Kd;
		}

	private:
	protected:
		//cant be greater than 0
		float m_Ks;
		float m_lobePower;
		RGBColor m_Kd ;
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------
