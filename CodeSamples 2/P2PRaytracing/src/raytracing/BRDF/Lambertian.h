#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Lambertian.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: READY
//----------------------------------------------------------------------------------------------------------------
#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "BRDF.h"
#include "..\CoreClasses\UtilClasses\Common.h"
#include "..\Material\Textures\Texture.h"
#include "..\CoreClasses\GeometricObjects\Triangle.h"
#include "..\CoreClasses\Point\Point2D\Point2D.h"

using Engine::Primitives::Triangle;

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: Lambertian surface (Diffuse)
	//		   Description: Concrete implementation of BRDF abstract class, 
	//						implements the lambertian function for lambertian surfaces
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class Lambertian : public Engine::BRDF
	{
	public:
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: D
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			Lambertian(float p_kd, RGBColor p_cd) : BRDF(p_cd), m_kd(p_kd), m_cd(p_cd)
			{}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: Diffuse Constant, Surface Color
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			Lambertian(float p_kD, string p_texturePath) : BRDF(p_texturePath), m_cd(0), m_kd(p_kD)
			{}

			Lambertian(float p_Kd, string p_imagePath, RGBColor p_secondaryColor) : BRDF(p_imagePath, p_secondaryColor), m_cd(0), m_kd(p_Kd)
			{}

			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: f
			//		Return Value: RGBColor
			//		  Parameters: Collision Details, input direction, output direction, boolean contains texture
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			virtual RGBColor f(ShadeRec& sr, Point3D wi, Point3D w0, RGBColor& p_colorAtPoint, bool p_containsTexture = false)
			{
				GetAlbedo(sr, p_colorAtPoint, p_containsTexture);

				return ((p_colorAtPoint*  m_kd) * (1 / PI_NUM));
			}

			virtual RGBColor f(ShadeRec& sr, Point3D wi, Point3D w0, bool p_containsTexture = false)
			{
				RGBColor baseColor = GetAlbedo(sr, p_containsTexture);
				return (baseColor*  m_kd * 1 / PI_NUM);
			}


			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: sampleF
			//		Return Value: RGBColor
			//		  Parameters: Collision Details, input direction, output direction
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			virtual RGBColor sampleF(ShadeRec& sr, Point3D wi, Point3D w0)
			{
				return RGBColor(0);
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: rho
			//		Return Value: RGBColor
			//		  Parameters: Collision Details, output direction
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			virtual RGBColor rho(ShadeRec& sr, Point3D w0)
			{
				return (m_cd * m_kd );
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: SetColor
			//		Return Value: RGBColor
			//		  Parameters: RGBColor 
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			void SetColor(RGBColor p_color)
			{
				m_cd = p_color;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: GetColor
			//		Return Value: RGBColor
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			RGBColor GetColor()
			{
				return m_cd;
			}

		
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: GetKd
			//		Return Value: float
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			float GetKd()
			{
				return m_kd;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: SetKd
			//		Return Value: VOID
			//		  Parameters: float diffuse constant
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			void SetKd(float kD)
			{
				m_kd = kD;
			}

		private:
			float m_kd;
			RGBColor m_cd;
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------