#ifndef __PHONG__
#define __PHONG__

#include "..\Material\Material.h"
#include "..\BRDF\Specular.h"
#include "..\BRDF\Lambertian.h"
#include "..\Light\Light.h"
#include "..\CoreClasses\GeometricObjects\GeometricObjects.h"

namespace Engine
{
	class Phong : public Engine::Materials::Material
	{

	public:
		//Phong() : diffuseBRDF(new Engine::Lambertian), specularBRDF(new Engine::Specular)
		//{}

		//Phong(float p_kd, float p_ks, RGBColor p_color) : diffuseBRDF(new Engine::Lambertian), specularBRDF(new Engine::Specular)
		//{
		//	SetCd(p_color);
		//	SetCs(p_color);
		//	SetKd(p_kd);
		//	SetKs(p_ks);
		//	m_type = Materials::GLOSSY;
		//}

		//Phong(float p_kd, float p_ks, RGBColor p_DiffuseColor, RGBColor p_ReflecColor) : diffuseBRDF(new Engine::Lambertian), specularBRDF(new Engine::Specular)
		//{
		//	SetCd(p_DiffuseColor);
		//	SetCs(p_ReflecColor);
		//	//diffuse constant 
		//	SetKd(p_kd);
		//	//reflective constant
		//	SetKs(p_ks);
		//	m_type = Materials::GLOSSY;
		//}

		


		void SetCd(RGBColor& p_color)
		{
			diffuseBRDF->SetColor(p_color);
		}

		void SetCs(RGBColor& p_color)
		{
			specularBRDF->SetColor(p_color);
		}

		RGBColor GetMatteColor()
		{
			return diffuseBRDF->GetColor();
		}

		void SetKd(float p_Kd)
		{
			diffuseBRDF->SetKd(p_Kd);
		}

		void SetKs(float p_Ks)
		{
			specularBRDF->SetKs(p_Ks);
		}
		/*sing Engine::Material::areaLightShade;
		RGBColor AreaLightShade(ShadeRec& sr)
		{
		return RGBColor(0);
		}

		using Engine::Material::PathShader;
		RGBColor PathShader(ShadeRec& sr)
		{
		return RGBColor(0);
		}*/


	private:
		Engine::Lambertian* diffuseBRDF;
		Engine::Specular* specularBRDF;

	};

}



#endif // !__PHONG__
