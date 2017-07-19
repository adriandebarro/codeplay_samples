#pragma once 
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Matte.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <vector>
#include <string>
#include <random>

#include "..\CoreClasses\Ray\ShadowRay\ShadowRay.h"
#include "..\BRDF\Lambertian.h"
#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "Material.h"

namespace Engine
{

	namespace Materials
	{

		//----------------------------------------------------------------------------------------------------------------
		//			Class Name: Matte
		//		   Description: Concrete implementation of Materials abstract class
		//----------------------------------------------------------------------------------------------------------------
		//			    Status: REFACTORING
		//----------------------------------------------------------------------------------------------------------------
		class Matte : public Engine::Materials::Material
		{
		public:
			////----------------------------------------------------------------------------------------------------------------
			////		 Method Name: CONSTRUCTOR
			////		Return Value: VOID
			////		  Parameters: VOID
			////			  Status: READY
			////----------------------------------------------------------------------------------------------------------------
			//Matte() : diffuseBrdf(new Lambertian)
			//{}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			Matte(float p_kd, RGBColor p_color) : diffuseBrdf(new Lambertian(p_kd, p_color))
			{
				SetCd(p_color);
				SetKd(p_kd);
				m_containsTexture = false;
				/*m_materialTexture = new SolidColor(p_color);*/
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: string materialName, float constant, RGBColor color
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			Matte(std::string p_materialName, float p_kd, RGBColor p_color) : diffuseBrdf(new Lambertian(p_kd, p_color))
			{
				SetCd(p_color);
				m_containsTexture = false;
				SetKd(p_kd);
				SetMaterialName(p_materialName);
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: string materialName, float constant, RGBColor color
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			Matte(std::string p_materialName, float p_kd, std::string p_texturePath) : diffuseBrdf(new Lambertian(p_kd, p_texturePath))
			{
				m_containsTexture = true;
				SetKd(p_kd);
				SetMaterialName(p_materialName);
				SetType(DIFF);
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: string materialName, float constant, string texturePath, RGBColor
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			Matte(std::string p_materialName, float p_kd, std::string p_texturePath, RGBColor p_secondaryColor) : diffuseBrdf(new Lambertian(p_kd, p_texturePath, p_secondaryColor))
			{

				m_containsTexture = true;
				SetKd(p_kd);
				SetMaterialName(p_materialName);
				SetType(DIFF);
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: SetKd
			//		Return Value: VOID
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			void SetKd(float p_kD)
			{
				diffuseBrdf->SetKd(p_kD);
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: SetCd
			//		Return Value: VOID
			//		  Parameters: RGBColor color
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			void SetCd(RGBColor p_color)
			{
				diffuseBrdf->SetColor(p_color);
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: GetMatteColor
			//		Return Value: VOID
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			RGBColor GetMatteColor(void)
			{
				return diffuseBrdf->GetColor();
			}


			virtual RGBColor GetAlbedo(ShadeRec& p_sr, bool p_containsTexture = false)
			{
				return diffuseBrdf->GetAlbedo(p_sr, this->m_containsTexture);
			}

			virtual RGBColor GetBrdf(ShadeRec& p_sr, Point3D p_incomingDirection, Point3D p_outgoingDirection, RGBColor& p_albedo, bool p_containsTexture = false)
			{
				return diffuseBrdf->f(p_sr, p_incomingDirection, p_outgoingDirection, p_albedo, p_containsTexture);
			}

			virtual RGBColor GetBrdf(ShadeRec& p_sr, Point3D p_incomingDirection, Point3D p_outgoingDirection, bool p_co0ntainsTexture = false)
			{
				return diffuseBrdf->f(p_sr, p_incomingDirection, p_outgoingDirection, false);
			}


		private:
			float m_kA;
			float m_kD;
			Lambertian *diffuseBrdf;
		};

	}
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------
