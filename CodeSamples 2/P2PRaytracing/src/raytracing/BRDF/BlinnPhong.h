#pragma once 

#include "./BRDF.h"

namespace Engine
{
	namespace Materials
	{
		namespace Brdf
		{
			///-------------------------------------------------------------------------------------------------
			/// <summary>	A blinn phong shader model</summary>
			///
			/// <remarks>	Adrian, 19/07/2017. </remarks>
			///-------------------------------------------------------------------------------------------------
			class BlinnPhong : public BRDF
			{
			public:
				
				BlinnPhong(RGBColor p_ks, RGBColor p_kd) : BRDF(p_kd)
				{
					m_Ks = p_ks;
					m_Kd = p_kd;
				}


				BlinnPhong(RGBColor p_ks, RGBColor p_kd, int p_specularHardness = 1) : BRDF(p_kd)
				{
					m_Ks = p_ks;
					m_Kd = p_ks;
					m_specularHardness = p_specularHardness;

				}

				using BRDF::f;
				virtual RGBColor f(ShadeRec& p_sr, Point3D wi, Point3D w0, RGBColor& p_colorAtPoint, bool p_containsTexture = false)
				{
					p_colorAtPoint = m_Kd;

					Point3D h = (wi + w0) / 2;
					h.Normalize();

					Point3D normal = p_sr.GetNormal();

					float upper = std::pow(normal.dot(h), 1);
					float denominator = normal.dot(wi);

					RGBColor outputColor = m_Ks * (upper / denominator) + m_Kd;

					return outputColor;
				}

				virtual RGBColor f(ShadeRec& p_sr, Point3D wi, Point3D w0, bool p_containsTexture = false)
				{
					Point3D h = (wi + w0) / 2;
					h.Normalize();

					Point3D normal = p_sr.GetNormal();

					float upper = std::pow(normal.dot(h), 1);
					float denominator = normal.dot(wi);

					RGBColor outputColor = m_Ks * (upper / denominator) + m_Kd;

					return outputColor;
				}

				virtual RGBColor sampleF(ShadeRec& sr, Point3D wi, Point3D w0)
				{
					return RGBColor(0);
				}

				
				using BRDF::rho;
				virtual RGBColor rho(ShadeRec& sr, Point3D w0)
				{
					return RGBColor(0);
				}


			private:
				RGBColor m_Ks;
				RGBColor m_Kd;
				float m_specularHardness = 1;
			};
		}
	}
}