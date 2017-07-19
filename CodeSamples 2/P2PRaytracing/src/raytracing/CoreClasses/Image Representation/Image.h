#pragma once 
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Image.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <math.h>

#include "..\RGBObject\RGBColor.h"

#define USE_NEAREST_NEIGHBOUR

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: Image
	//		   Description: Wrapper used for Image textures and procedural textures. Stores a series of colors of type RGBColor
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class Image
	{
	public:
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Image()
		{}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Width, Height of image
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Image(int p_width, int p_height)
		{
			m_imagePixels = new RGBColor[p_width * p_height];
			m_width = p_width;
			m_height = p_height;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TruncateValue
		//		Return Value: VOID
		//		  Parameters: float vlaue
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		inline float TruncateValue(float p_inputValue)
		{
			float interm = (int)p_inputValue;
			float secInterm = p_inputValue - interm;
			return secInterm < 0 ? 1 + secInterm : secInterm;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CreateDither
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void CreateDither(void)
		{
			int currentX = 0;
			int currentY = 0;

			for (int index = 0; index < m_width * m_height; index++)
			{
				if ((currentX % 2 != 0 && currentY % 2 != 0) || (currentX == currentY))
					m_imagePixels[index] = RGBColor(1, 0, 0);
				else
					m_imagePixels[index] = RGBColor(0, 0, 1);

				currentX++;
				if (currentX == m_width)
				{
					currentX = 0;
					currentY++;
				}
			}
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetColor
		//		Return Value: RGBColor
		//		  Parameters: U, V
		//			  Status: 
		//					  TODO //bilinear filtering being implemented https://en.wikipedia.org/wiki/Bilinear_filtering
		//----------------------------------------------------------------------------------------------------------------
		RGBColor GetColor(float p_xCoor, float p_yCoor)
		{
			//nearest neighbour
			float u = TruncateValue(p_xCoor);
			float v = TruncateValue(p_yCoor);

			int _x = u * m_width;
			int _y = v * m_height;

			int index = _y * m_width + _x;
			return m_imagePixels[index];

			float u1 = u * m_width;
			float v1 = v * m_height;

			int location = u1 + v1 * m_width;

			if (location >= m_width*m_height)
			{
				location = m_width * m_height - 1;
			}

			RGBColor outputColor = m_imagePixels[location];

			return outputColor;
		}

		RGBColor BilinearInterpolation(float p_x, float p_y)
		{
			_ASSERT( p_x > 0 && p_y > 0);
		
			float u = TruncateValue(p_x);
			float v = TruncateValue(p_y);

			int _x = u * m_width;
			int _y = v * m_height;

			int index = _y * m_width + _x;

			float uPrime = p_x * m_width - std::floorf(p_x * m_width);
			float vPrime = p_y * m_height - std::floorf(p_y * m_height);



			RGBColor outputColor(0);

			RGBColor temp1 =  m_imagePixels[index] * ((1 - uPrime)*(1 - vPrime));

			int currentIndex = (_x + 1) + (_y * m_width);
			RGBColor temp2 =  m_imagePixels[currentIndex] * (uPrime * (1 - vPrime));
			currentIndex = _x + (_y + 1)*m_width;
			RGBColor temp3 = m_imagePixels[currentIndex] * (1 - uPrime)*vPrime;
			currentIndex = (_x + 1) + (_y + 1)* m_width;
			RGBColor temp4 = m_imagePixels[currentIndex] * (uPrime * vPrime);

			outputColor = outputColor + temp1 + temp2 + temp3 + temp4;
			
			return outputColor;
		}

		RGBColor HermiteInterpolation(float p_x, float p_y)
		{
			_ASSERT(p_x > 0 && p_y > 0);

			float u = TruncateValue(p_x);
			float v = TruncateValue(p_y);

			int _x = u * m_width;
			int _y = v * m_height;

			int index = _y * m_width + _x;

			float uPrime = p_x * m_width - std::floorf(p_x * m_width);
			float vPrime = p_y * m_height - std::floorf(p_y * m_height);

			float uDPrime = 3 * (std::pow(uPrime, 2)) - (2 * std::powf(uPrime, 3));
			float vDPrime = 3 * (std::pow(vPrime, 2)) - (2 * std::powf(vPrime, 3));

			RGBColor outputColor(0);

			RGBColor temp1 = m_imagePixels[index] * ((1 - uDPrime)*(1 - vDPrime));

			int currentIndex = (_x + 1) + (_y * m_width);
			RGBColor temp2 = m_imagePixels[currentIndex] * (uDPrime * (1 - vDPrime));
			currentIndex = _x + (_y + 1)*m_width;
			RGBColor temp3 = m_imagePixels[currentIndex] * (1 - uDPrime)*vDPrime;
			currentIndex = (_x + 1) + (_y + 1)* m_width;
			RGBColor temp4 = m_imagePixels[currentIndex] * (uDPrime * vDPrime);

			outputColor = outputColor + temp1 + temp2 + temp3 + temp4;

			return outputColor;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetColor
		//		Return Value: RGBColor
		//		  Parameters: X coordinate, Y coordinate
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor GetColor(int p_x, int p_y)
		{
			int index = p_x + p_y * m_width;
			return m_imagePixels[index];
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetArea
		//		Return Value: Int
		//		  Parameters: void
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		int GetArea(void)
		{
			return (m_width * m_height);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetColorBilinear
		//		Return Value: RGBColor
		//		  Parameters: X coordinate, Y coordinate
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor GetColorBilinear(int p_x, int p_y)
		{
			return m_imagePixels[p_y + p_x];
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetNearestNeighbour
		//		Return Value: RGBColor
		//		  Parameters: X coordinate, Y coordinate
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor GetNearestNeighbour(int p_x, int p_y)
		{
			return m_imagePixels[p_y + p_x];
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetPixel
		//		Return Value: VOID
		//		  Parameters: Array Index, color
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetPixel(int p_Index, RGBColor p_pixelColor)
		{
			m_imagePixels[p_Index] = p_pixelColor;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetPixel
		//		Return Value: RGBColor
		//		  Parameters: X coordinate, Y coordinate, Color
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetPixel(int p_xCoor, int p_yCoor, RGBColor p_pixelColor)
		{
			int x = p_xCoor;
			int y = p_yCoor * m_width;
			m_imagePixels[x + y] = p_pixelColor;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetHeight
		//		Return Value: int
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		int GetHeight(void)
		{
			return m_height;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetWidth
		//		Return Value: int
		//		  Parameters: void
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		int GetWidth(void)
		{
			return m_width;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: MakeTestCard
		//		Return Value: VOID
		//		  Parameters: void
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void MakeTestCard(void)
		{
			RGBColor cardHues[9] = { RGBColor(1), RGBColor(1, 0, 1), RGBColor(0, 1, 0),
				RGBColor(0, 0, 1), RGBColor(1, 0, 0), RGBColor(0, 0, 1),
				RGBColor(0, 1, 0), RGBColor(0, 1, 1), RGBColor(1) };

			for (int y = 0; y < this->GetHeight(); ++y)
			{
				for (int x = 0; x < this->GetWidth(); ++x)
				{
					RGBColor pixel = cardHues[(int)((((float)x) / (this->GetWidth() + 1)) * 8.f)];
					this->SetPixel(x, y, pixel);
				}
			}
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: WriteToFile
		//		Return Value: RGBColor
		//		  Parameters: String path
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void WriteToFile(string p_path)
		{
			try
			{
				int total = m_width * m_height;
				const char * charPath = p_path.c_str();

				ofstream fp(charPath, ios::out | ios::binary);
				fp << "P6\n" << m_width << " " << m_height << "\n255\n";
				for (int index = 0; index < total; index++)
				{
					RGBColor currentColor = m_imagePixels[index];

					//writing each pixel ot file 
					fp << (unsigned char)(min(float(1), currentColor.r) * 255) <<
						(unsigned char)(min(float(1), currentColor.g) * 255) <<
						(unsigned char)(min(float(1), currentColor.b) * 255);
				}

				fp.close();
			}
			catch (exception e)
			{
				std::cerr << e.what() << endl;
			}
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: DESTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		~Image(void)
		{
			delete[] m_imagePixels;
		}
	private:
		int m_width;
		int m_height;
		RGBColor* m_imagePixels;
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------
