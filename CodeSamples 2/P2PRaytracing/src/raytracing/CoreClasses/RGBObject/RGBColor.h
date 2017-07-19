#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		RGBColor.h
//			  Author:		Adrian De Barro
//			    Date:		13/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <math.h>
#include <string>
#include <fstream>

#include "UtilityClasses/Common.h"
#include "../UtilClasses/ConvertionHelper.h"

using std::string;
using std::to_string;
using namespace Engine::Communication::Serialization;
namespace Engine
{

	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: RGBColor
	//		   Description: Represents Color and wraps around 3 floats
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class RGBColor
	{
	public:
		float r, g, b;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor()
		{
			r = 0;
			g = 0;
			b = 0;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor(float color)
		{
			r = color;
			g = color;
			b = color;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor(float rr, float gg, float bb) : r(rr), g(gg), b(bb) {}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor(const RGBColor& color) :r(color.r), g(color.g), b(color.b) {}
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Constructor. </summary>
		///
		/// <remarks>	Adrian, 21/09/2015. </remarks>
		///
		/// <param name="p_input">	The input. </param>
		///-------------------------------------------------------------------------------------------------
		RGBColor(string p_input)
		{
			FromString(p_input);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: operators
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		inline RGBColor operator + (RGBColor color)
		{
			return RGBColor(r + color.r, g + color.g, b + color.b);
		}

		inline RGBColor operator * (double constant)
		{
			return RGBColor(r*constant, g * constant, b * constant);
		}

		inline RGBColor operator / (int c)
		{
			return RGBColor(r / c, g / c, b / c);
		}

		inline RGBColor operator / (double constant)
		{
			return RGBColor(r / constant, g / constant, b / constant);
		}

		inline RGBColor operator / (float constant)
		{
			return RGBColor(r / constant, g / constant, b / constant);
		}

		inline RGBColor& operator = (RGBColor color)
		{
			r = color.r;
			g = color.g;
			b = color.b;

			return *this;
		}

		inline RGBColor operator * (RGBColor color)
		{
			return RGBColor(r*color.r, g * color.g, b * color.b);
		}

		inline RGBColor operator += (RGBColor currentColor)
		{
			return RGBColor(r + currentColor.r, g + currentColor.g, b + currentColor.b);
		}
		inline RGBColor operator /= (float p_currentNumber)
		{
			return RGBColor(r /= p_currentNumber, g /= p_currentNumber, b /= p_currentNumber);
		}
		inline bool operator == (int constant)
		{
			return r == constant && g == constant && b == constant;
		}
		inline bool operator != (int constant)
		{
			return r != constant || g != constant || b != constant;
		}
		inline bool operator == (RGBColor p_color)
		{
			return r == p_color.r && g == p_color.g && b == p_color.b;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: Convert
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void Convert()
		{
			double newR = r == 0? 0 : (r / (r + 1));
			double newB = b == 0? 0 : (b / (b + 1));
			double newG = g == 0? 0  : (g / (g + 1));

			int red = newR * 255.99;
			int blue = newB * 255.99;
			int green = newG * 255.99;

			r = red;
			b = blue;
			g = green;

		}


		inline float Summation()
		{
			return r + g + b;
		}
		
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: swap
		//		Return Value: VOID
		//		  Parameters: RGBColor SwapItem
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		inline void swap(RGBColor& p_swapItem)
		{
			std::swap(r, p_swapItem.r);
			std::swap(g, p_swapItem.g);
			std::swap(b, p_swapItem.b);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: toInt
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		int toInt()
		{
			double newR = r / (r + 1);
			double newB = b / (b + 1);
			double newG = g / (g + 1);

			int red = newR * 256;
			int blue = newB * 256;
			int green = newG * 256;

			if (red > 255)
				red = 255;
			if (blue > 255)
				blue = 255;
			if (green > 255)
				green = 255;

			return (red << 16) + (green << 8) + (blue);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: toString
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		string ToString() const 
		{
			return to_string(r) + "," + to_string(g) + "," + to_string(b);
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Initializes this object from the given from string. </summary>
		///
		/// <remarks>	Adrian, 26/09/2015. </remarks>
		///
		/// <param name="p_input">	The input. </param>
		///-------------------------------------------------------------------------------------------------

		void FromString(std::string p_input)
		{
			string data[3];

			Common::Tokenizer::ParseString(p_input, ",", data);

			r = stof(data[0]);
			g = stof(data[1]);
			b = stof(data[2]);
		}


		unsigned char* SerializePoint()
		{
			unsigned char* toReturn = new unsigned char[12];

			ConvertionClass::ConvertFloat(r, 0, toReturn);
			ConvertionClass::ConvertFloat(g, 4, toReturn);
			ConvertionClass::ConvertFloat(b, 8, toReturn);

			return toReturn;
		}

		void SerializePoint(unsigned char p_Array[], int p_startingPosition)
		{
			int incPos = p_startingPosition;
			ConvertionClass::ConvertFloat(r, incPos, p_Array);
			incPos += 4;
			ConvertionClass::ConvertFloat(g, incPos, p_Array);
			incPos += 4;
			ConvertionClass::ConvertFloat(b, incPos, p_Array);
		}

		void SerializePoint(vector<unsigned char>& p_Array)
		{
			ConvertionClass::ConvertFloat(r, p_Array);
			ConvertionClass::ConvertFloat(g, p_Array);
			ConvertionClass::ConvertFloat(b, p_Array);
		}


		void FromSerialize(vector<unsigned char>& p_charArray, int &p_startingPoint)
		{
			ConvertionClass::ByteArrayToFloat(r, p_charArray, p_startingPoint, 4);
			ConvertionClass::ByteArrayToFloat(g, p_charArray, p_startingPoint, 4);
			ConvertionClass::ByteArrayToFloat(b, p_charArray, p_startingPoint, 4);
		}

		void FromSerialize(std::ifstream& p_stream)
		{
			char* buff = new char[12];
			vector<unsigned char> vecBuff(12);
			p_stream.get(buff, 12);
			
			int startingPoint = 0;

			for (int index = 0; index < 12; index++)
			{
				vecBuff[index] = (unsigned char)buff[index];
			}

			ConvertionClass::ByteArrayToFloat(r, vecBuff, startingPoint, 4);
			ConvertionClass::ByteArrayToFloat(g, vecBuff, startingPoint, 4);
			ConvertionClass::ByteArrayToFloat(b, vecBuff, startingPoint, 4);
		}




		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: 
		//					TODO move to common.h as constants
		//----------------------------------------------------------------------------------------------------------------
		static RGBColor GetColor(int colorNumber)
		{
			switch (colorNumber)
			{
			case 0:
				return RGBColor(0, 0, 0);


			case 1:
				return RGBColor(255, 0, 0);


			case 2:
				return RGBColor(0, 255, 0);


			case 3:
				return RGBColor(0, 0, 255);


			case 4:
				return RGBColor(192, 192, 192);

			default:
				break;
			}
		}
	};
	//----------------------------------------------------------------------------------------------------------------

	inline RGBColor operator /(const RGBColor &color, const float constant)
	{
		return RGBColor(color.r / constant, color.b / constant, color.g / constant);
	};
}
//----------------------------------------------------------------------------------------------------------------
