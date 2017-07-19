#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		BRDF.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: READY
//----------------------------------------------------------------------------------------------------------------
#include "..\CoreClasses\Sampler\Sampler.h"
#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "..\CollisionDetails\ShadeRec.h"
#include "..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\Material\Textures\Texture.h"
#include "..\Material\Textures\SolidColor.h"

using Engine::Primitives::Triangle;

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: BRDF
	//		   Description: Abstract BRDF class
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: READY
	//----------------------------------------------------------------------------------------------------------------
	class BRDF
	{
	public:

		BRDF(string p_currentTexturePath)
		{
			SetTexture(p_currentTexturePath);
		}

		BRDF(RGBColor p_solidColor)
		{
			SetTexture(p_solidColor);
		}
		
		BRDF(string p_imagePath, RGBColor p_secondaryColor)
		{
			SetTexture(p_imagePath, p_secondaryColor);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: f
		//		Return Value: VOID
		//		  Parameters: Collision Details, Input direction, output direction, Use texture
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual RGBColor f(ShadeRec& sr, Point3D wi, Point3D w0, bool usesTexture) = 0;

		virtual RGBColor f(ShadeRec& sr, Point3D wi, Point3D w0, RGBColor &p_colorAtPoint, bool p_containsTexture = false) = 0;

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: sampleF
		//		Return Value: VOID
		//		  Parameters: Collision Details, Input direction, output direction
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual RGBColor sampleF(ShadeRec& sr, Point3D wi, Point3D w0) = 0;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: rho
		//		Return Value: VOID
		//		  Parameters: Collision Details, output direction
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual RGBColor rho(ShadeRec& sr, Point3D w0) = 0;
		
		void GetAlbedo(ShadeRec& sr, RGBColor& p_colorAtPoint, bool usesTexture)
		{
			if (usesTexture)
			{
				Triangle* currentTriangle = (Triangle*)sr.m_hitTriangle;
				ShadeRec testInterm;
				Point2D barCoor = currentTriangle->CalcBarycentric(sr.ray, testInterm);
				RGBColor textureBaseColor = m_usedTexture->GetColor(barCoor.x, barCoor.y);
				p_colorAtPoint = textureBaseColor;
			}
			else
			{

				float x = 0, y = 0;
				p_colorAtPoint = m_usedTexture->GetColor(x, y);
			}
		}

		RGBColor GetAlbedo(ShadeRec& sr, bool usesTexture)
		{
			RGBColor toReturn(0);
			GetAlbedo(sr, toReturn, usesTexture);
			return toReturn;
		}

		void SetTexture(string p_texturePath)
		{
			m_usedTexture = new ImageTexture(p_texturePath);
		}

		void SetTexture(RGBColor p_solidColor)
		{
			m_usedTexture = new SolidColor(p_solidColor);
		}

		void SetTexture(string p_imagePath, RGBColor p_secondaryColor)
		{
			m_usedTexture = new ImageTexture(p_imagePath, p_secondaryColor);
		}

		Texture* GetTexture()
		{
			return m_usedTexture;
		}
		

	protected:
		Sampler* m_samplePtr;
		Texture* m_usedTexture;
	};
}
