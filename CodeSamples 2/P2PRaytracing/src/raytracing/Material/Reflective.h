#pragma once 

#include "./Material.h"
#include "../BRDF/Specular.h"

namespace Engine
{
	namespace Materials
	{
		//--------------------------------------------------------------------------------------------------------------------------------------------------
		//	Reflective : Represents materials which have reflective properties such as mirrors (Angle of incidence = angle of reflection...thats the motto!)
		//--------------------------------------------------------------------------------------------------------------------------------------------------
		class Reflective : public Material
		{
		public:

			Reflective(RGBColor p_specularColor, RGBColor p_diffuseColor) :Material(SURFACE_TYPE::REFL)
			{
				m_specularColor = p_specularColor;
				m_diffuseComponent = new Matte(0.75, p_diffuseColor);
				m_diffuseColor = p_diffuseColor;
				m_containsTexture = false;
			}

			Reflective(RGBColor p_specularColor, RGBColor p_diffuseColor, string p_path) :Material(SURFACE_TYPE::REFL)
			{
				m_specularColor = p_specularColor;
				m_containsTexture = true;
				m_diffuseComponent = new Matte("subMaterial", 1, p_path);
			}

			Reflective() : Material(SURFACE_TYPE::REFL)
			{}

			virtual RGBColor GetBrdf(ShadeRec& p_sr, Point3D p_icomingDirection, Point3D p_outgoingDirection, RGBColor& p_albedo, bool p_co0ntainsTexture = false)
			{
				return RGBColor(0);
			}

			virtual RGBColor GetBrdf(ShadeRec& p_sr, Point3D p_icomingDirection, Point3D p_outgoingDirection, bool p_co0ntainsTexture = false)
			{
				return RGBColor(0);
			}

			virtual RGBColor GetAlbedo(ShadeRec& p_sr, bool p_containsTexture = false)
			{
				return RGBColor(0);
			}

			RGBColor GetSpecularColor()
			{
				return m_specularColor;
			}
			
			RGBColor GetDiffuseColor()
			{
				return m_diffuseColor;
			}
			
			Matte* GetMatteComponent()
			{
				return m_diffuseComponent;
			}

			void GetComponentRatios(RGBColor& p_diffuseRatio, RGBColor& p_specularRatio)
			{
				p_specularRatio = m_specularColor;
				p_diffuseRatio = m_diffuseColor;
			}

		private:
			bool m_hasColor;
			bool m_containsTexture;
			//temp 
			RGBColor m_diffuseColor;

			RGBColor m_specularColor;
			Specular* m_specularSurface;
			Matte* m_diffuseComponent;
		};
	}
}