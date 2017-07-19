#pragma once 
//----------------------------------------------------------------------------------------------------------------
//			Filename:		ImageTexture.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <string>
#include <iostream>
#include <fstream>

#include "Texture.h"
#include "..\..\CoreClasses\Image Representation\Image.h"
#include "..\..\CoreClasses\RGBObject\RGBColor.h"

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: ImageTexture
	//		   Description: Represent a TRIS. Concrete implementation of GeometricObjects
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class ImageTexture : public Texture
	{
	public:
		Image* m_image;
		int m_width;
		int m_height;

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: string path
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		ImageTexture(string p_path)
		{
			GetTexture(p_path);
			m_containsTexture = true;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters string path, RGBColor secondaryColor
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		ImageTexture(string p_path, RGBColor p_secondaryColor)
		{
			GetTexture(p_path);
			m_containsSecondaryColor = true;
			m_secondaryColor = p_secondaryColor;
			m_containsTexture = true;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		ImageTexture()
		{
			GetTexture("", true);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetColor
		//		Return Value: VOID
		//		  Parameters: float x, float y
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using Texture::GetColor;
		virtual RGBColor GetColor(float p_x, float p_y)
		{
			//need to apply vertical and horizontal bullshit
			return m_image->GetColor(p_x, p_y);
			//return RGBColor(0.3);
		}

		RGBColor GetBiliear(float p_u, float p_v)
		{
			return m_image->BilinearInterpolation(p_u, p_v);
		}

		RGBColor GetHermite(float p_u, float p_v)
		{
			return m_image->HermiteInterpolation(p_u, p_v);
		}


		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetColor
		//		Return Value: VOID
		//		  Parameters: int x , int y
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual RGBColor GetColor(int p_x, int p_y)
		{
			return m_image->GetColor(p_x, p_y);
		}

		int GetWidth()
		{
			return m_image->GetWidth();
		}

		int GetHeight()
		{
			return m_image->GetHeight();
		}


	protected:
		RGBColor m_secondaryColor;

	private:
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetTexture
		//		Return Value: VOID
		//		  Parameters: String path, bool randomTexture
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		int GetTexture(string p_path, bool randomTexture = false)
		{
			char magicNumber[2], whitespace;
			int	width, height, colours;

			try
			{
				std::ifstream imageFile;

				// Open image file
				imageFile.open(p_path.c_str(), std::ios::binary);

				if (!imageFile.is_open() || randomTexture)
				{
					std::cerr << "ERROR -- Couldn't open file \'" << p_path << "\'" << std::endl;

					m_image = new Image(64, 64);
					m_image->MakeTestCard();
					return 1;
				}

				// Read and parse header
				

				RGBColor colour;

				imageFile.get(magicNumber[0]);
				imageFile.get(magicNumber[1]);
				imageFile.get(whitespace); 
				if (whitespace == '\r') 
					imageFile.get(whitespace);
				imageFile >> std::noskipws >> width;
				//[AD] change due to height not being read correctly
				imageFile.get(whitespace); if (whitespace == '\r') imageFile.get(whitespace);
				imageFile >> std::noskipws >> height;
				imageFile.get(whitespace); if (whitespace == '\r') imageFile.get(whitespace);
				imageFile >> std::noskipws >> colours;
				imageFile.get(whitespace); if (whitespace == '\r') imageFile.get(whitespace);

				// Create image
				_ASSERT(width > 0 && height > 0);
				m_image = new Image(width, height);
				Image &image = *m_image;

				for (int i = 0; i < image.GetArea(); i++)
				{
					float r = (float)imageFile.get();
					float g = (float)imageFile.get();
					float b = (float)imageFile.get();

					RGBColor currentColor(r, g, b);

					currentColor /= 255.0f;
					image.SetPixel(i, currentColor);
				}

				// Close image file
				imageFile.close();

				// Quick fix to generate .mmf files from .ppm ones.
				// Should need to enable once for new scenes.
				/*
				size_t f = p_strImageFile.find_last_of('.');
				std::string nf = p_strImageFile.substr(0, f) + ".mmf";
				std::cout << "MMF : " << nf << std::endl;
				MemoryMappedTexture::MakeLDR(image, nf);
				*/
				return 1;
			}
			catch (exception ex)
			{
				cout << "encountered issue parsing the ppm file " << p_path << "--width - Height " << width << "--" << height << "\n";
			}
		}

	
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------