#pragma once 

#include "./Material.h"
#include "../BRDF/BlinnPhong.h"

using Engine::Materials::Brdf::BlinnPhong;

namespace Engine
{
	namespace Materials
	{
		class Glossy : public Material
		{
		public:

			Glossy(RGBColor p_ks, RGBColor p_kd, int p_specularExponent) : Material()
			{
				m_type = GLOSSY;
				m_blinnPhong = new BlinnPhong(p_ks, p_kd, p_specularExponent);
			}

			//to implement
			virtual RGBColor GetBrdf(ShadeRec& p_sr, Point3D p_icomingDirection, Point3D p_outgoingDirection, RGBColor& p_albedo, bool p_co0ntainsTexture = false)
			{
				return m_blinnPhong->f(p_sr, p_icomingDirection, p_outgoingDirection, p_albedo, p_co0ntainsTexture);
			}

			virtual RGBColor GetBrdf(ShadeRec& p_sr, Point3D p_icomingDirection, Point3D p_outgoingDirection, bool p_co0ntainsTexture = false)
			{
				return m_blinnPhong->f(p_sr, p_icomingDirection, p_outgoingDirection, p_co0ntainsTexture);
			}

			virtual RGBColor GetAlbedo(ShadeRec& p_sr, bool p_containsTexture = false)
			{
				return m_blinnPhong->GetAlbedo(p_sr, p_containsTexture);
			}

		private:
			BlinnPhong * m_blinnPhong;
		};
	}
}