#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Point3D.h
//			  Author:		Adrian De Barro
//			    Date:		13/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <math.h>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "../../UtilClasses/Common.h"
#include "../../UtilClasses/ConvertionHelper.h"

#define DOT(A,B)		(A.x*B.x+A.y*B.y+A.z*B.z)
#define NORMALIZE(A)	{float l=1/sqrtf(A.x*A.x+A.y*A.y+A.z*A.z);A.x*=l;A.y*=l;A.z*=l;}
#define LENGTH(A)		(sqrtf(A.x*A.x+A.y*A.y+A.z*A.z))
#define SQRLENGTH(A)	(A.x*A.x+A.y*A.y+A.z*A.z)
#define SQRDISTANCE(A,B) ((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y)+(A.z-B.z)*(A.z-B.z))

using std::min;
using std::max;
using Engine::Communication::Serialization::ConvertionClass;

namespace Engine
{
	// ----------------------------------------------------------------------------------------------------------------
	//			Class Name: Point3D
	//		   Description: Represents a 3D point.
	//----------------------------------------------------------------------------------------------------------------
	//			    Status:
	//						TODO http://www.flipcode.com/archives/Faster_Vector_Math_Using_Templates.shtml (optimise performance)
	//----------------------------------------------------------------------------------------------------------------
	class Point3D
	{
	public:

		union
		{
			struct { float x, y, z; };
			float At[3];
		};

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D(void)
		{
			x = 0;
			y = 0;
			z = 0;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: float coor
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D(float coor)
		{
			x = coor;
			y = coor;
			z = coor;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: float, float, float
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TestRayData
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		
		Point3D(const Point3D& p_src)
		{
			x = p_src.x;
			y = p_src.y;
			z = p_src.z;
		}
		
		
		bool TestPointData()
		{

			if (isnan(x) || isnan(y) || isnan(z))
			{
				return true;
			}
			return false;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: normlaize
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D& normalize()
		{
			float nor2 = length2();
			if (nor2 > 0) {
				float invNor = 1 / sqrt(nor2);
				x *= invNor, y *= invNor, z *= invNor;
			}
			return *this;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Names: Operator Overloads
		//			   Status: READY
		//----------------------------------------------------------------------------------------------------------------
		inline Point3D operator * (float f) { return Point3D(x * f, y * f, z * f); }
		inline Point3D operator * (Point3D v) { return Point3D(x * v.x, y * v.y, z * v.z); }
		inline Point3D operator - (Point3D v) { return Point3D(x - v.x, y - v.y, z - v.z); }
		inline Point3D operator + (Point3D v) { return Point3D(x + v.x, y + v.y, z + v.z); }
		inline Point3D& operator *= (Point3D &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
		inline Point3D operator - () { return Point3D(-x, -y, -z); }
		inline Point3D operator/ (const float & c)
		{
			return Point3D(x / c, y / c, z / c);
		}
		inline Point3D operator/ (const float & c) const
		{
			return Point3D(x / c, y / c, z / c);
		}
		inline bool operator != (const int& number)
		{
			if (x == number && y == number && z == number)
				return false;
			else
				return true;
		}
		inline float operator [] (int& p_index)
		{
			switch (p_index)
			{
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			default:
				return 0;
			}
		}
		inline bool operator == (Point3D p_point)
		{
			float acceptedDiscrep = 1 * pow(10 , -4);
			
			float changeX = abs(p_point.x - x);
			float changeY = abs(p_point.y - y);
			float changeZ = abs(p_point.z - z);

			return changeX <= acceptedDiscrep &&  changeY <= acceptedDiscrep && changeZ <= acceptedDiscrep;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: dot
		//		Return Value: VOID
		//		  Parameters: Point3D
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		inline float dot(Point3D v) { return x * v.x + y * v.y + z * v.z; }
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: length2 
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		inline float length2()  { return x * x + y * y + z * z; }
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: length
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		inline float length() { return sqrt(length2()); }
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: Min
		//		Return Value: VOID
		//		  Parameters: Point3D
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void Min(Point3D currentPoint)
		{
			x = min(x, currentPoint.x);
			y = min(y, currentPoint.y);
			z = min(z, currentPoint.z);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: Max
		//		Return Value: VOID
		//		  Parameters: Point3D
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void Max(Point3D currentPoint)
		{
			x = max(x, currentPoint.x);
			y = max(y, currentPoint.y);
			z = max(z, currentPoint.z);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CrossProduct
		//		Return Value: Point3D
		//		  Parameters: Point3D
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D CrossProduct(Point3D rhs)
		{
			float xx = y * rhs.z - z * rhs.y;
			float yy = z * rhs.x - x * rhs.z;
			float zz = x * rhs.y - y * rhs.x;

			return Point3D(xx, yy, zz);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: Normalize
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void Normalize()
		{
			float temp = 1 / length();

			float xTemp = x * temp;
			float yTemp = y * temp;
			float zTemp = z * temp;

			x = std::abs(xTemp) < Common::kEpsilon ? 0 : xTemp;
			y = std::abs(yTemp) < Common::kEpsilon ? 0 : yTemp;
			z = std::abs(zTemp) < Common::kEpsilon ? 0 : zTemp;

		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: toString
		//		Return Value: string
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		std::string toString() const 
		{
			std::stringstream  test;
			test << (float)x << "," << (float)y << "," << (float)z;
			return test.str();
		}

		void FromString(std::string p_toParse)
		{
			std::string data[3];
			Common::Tokenizer::ParseString(p_toParse, ",", data);
			 
			x = stod(data[0]);
			y = stod(data[1]);
			z = stod(data[2]);
		}

		unsigned char* SerializePoint()
		{
			unsigned char* toReturn = new unsigned char[12];

			ConvertionClass::ConvertFloat(x, 0, toReturn);
			ConvertionClass::ConvertFloat(y, 4, toReturn);
			ConvertionClass::ConvertFloat(z, 8, toReturn);

			return toReturn;
		}

		void SerializePoint(unsigned char p_Array[], int p_startingPosition)
		{
			int incPos = p_startingPosition;
			ConvertionClass::ConvertFloat(x, incPos, p_Array);
			incPos += 4;
			ConvertionClass::ConvertFloat(y, incPos, p_Array);
			incPos += 4;
			ConvertionClass::ConvertFloat(z, incPos, p_Array);
		}

		void SerializePoint(vector<unsigned char>& p_Array)
		{
			ConvertionClass::ConvertFloat(x, p_Array);
			ConvertionClass::ConvertFloat(y, p_Array);
			ConvertionClass::ConvertFloat(z, p_Array);
		}


		/*void FromSerialize(vector<unsigned char>& p_charArray, int p_startingPoint = 0)
		{
			int tempStartingPoint = p_startingPoint;
			ConvertionClass::ByteArrayToFloat(x, p_charArray, tempStartingPoint, 4);
			ConvertionClass::ByteArrayToFloat(y, p_charArray, tempStartingPoint+4, 4);
			ConvertionClass::ByteArrayToFloat(z, p_charArray, tempStartingPoint+8, 4);
		}*/

		void FromSerialize(vector<unsigned char>& p_charArray, int &p_startingPoint)
		{
			ConvertionClass::ByteArrayToFloat(x, p_charArray, p_startingPoint, 4);
			ConvertionClass::ByteArrayToFloat(y, p_charArray, p_startingPoint, 4);
			ConvertionClass::ByteArrayToFloat(z, p_charArray, p_startingPoint, 4);
		}


		static float DistanceBetweenPoints(Point3D &p_point1, const Point3D &p_point2)
		{
			float total = powf(p_point1.x - p_point2.x, 2) + powf(p_point1.y - p_point2.y, 2) + powf(p_point1.z - p_point2.z, 2);
			float output = std::sqrt(total);
			return output;
		}

		/*void FromSerialize(std::ifstream& p_stream)
		{
			char* buff = new char[12];
			vector<unsigned char> vecBuff(12);
			p_stream.get(buff, 12);

			int startingPoint = 0;

			for (int index = 0; index < 12; index++)
			{
				vecBuff[index] = (unsigned char)buff[index];
			}

			ConvertionClass::ByteArrayToFloat(x, vecBuff, startingPoint, 4);
			ConvertionClass::ByteArrayToFloat(y, vecBuff, startingPoint, 4);
			ConvertionClass::ByteArrayToFloat(z, vecBuff, startingPoint, 4);
		}*/

		#pragma region OldCode
		//Point3D(const Point3D &currentPoint)
		//{
		//	x = currentPoint.x;
		//	y = currentPoint.y;
		//	z = currentPoint.z;
		//}

		//Point3D(const float xx, const float yy, const float zz) : x(xx), y(yy), z(zz)
		//{}

		//Point3D operator = (const Point3D & rhs)
		//{
		//	x = rhs.x; y = rhs.y; z = rhs.z;
		//	return (*this);
		//}

		//inline Point3D operator- (void) const
		//{
		//	return (Point3D(-x, -y, -z));
		//}

		//inline Point3D operator - (const Point3D& p)
		//{
		//	Point3D returnPoint = Point3D(x - p.x, y - p.y, z - p.z);
		//	return returnPoint;
		//}

		//inline Point3D operator - (const Point3D& p) const
		//{
		//	return Point3D(x - p.x, y - p.y, z - p.z);
		//}

		//inline Point3D operator * (const float& c)
		//{
		//	return Point3D(x * c, y * c, z * c);
		//}

		//inline Point3D operator * (const float& c) const
		//{
		//	return Point3D(x * c, y * c, z * c);
		//}

		///*Point3D operator * (const Point3D& point)
		//{
		//	return 
		//}*/

		//inline Point3D operator + (const Point3D& p)
		//{
		//	return Point3D(x + p.x, y + p.y , z + p.z);
		//}

		//inline Point3D operator + (const Point3D& p) const
		//{
		//	return Point3D(x + p.x, y + p.y, z + p.z);
		//}

		//inline Point3D operator/ (const float & c)
		//{
		//	return Point3D(x / c, y / c, z / c);
		//}

		//inline Point3D operator/ (const float & c) const
		//{
		//	return Point3D(x / c, y / c, z / c);
		//}

		//inline bool operator != (const int& number)
		//{
		//	if (x == number && y == number && z == number)
		//		return false;
		//	else
		//		return true;
		//}

		//float Maginitude()
		//{
		//	return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		//}

		//void Normalize()
		//{
		//	float temp = 1 / Maginitude();

		//	x = x * temp;
		//	y = y * temp;
		//	z = z * temp;
		//}

		//Point3D CrossProduct(Point3D& rhs)
		//{
		//	float xx = y * rhs.z - rhs.y * z;
		//	float yy = x * rhs.z - rhs.x * z;
		//	float zz = x * rhs.y - rhs.x * y;

		//	return Point3D(xx, yy, zz);
		//}
#pragma endregion 
	};
	//----------------------------------------------------------------------------------------------------------------
	inline Point3D operator * (float lhs,Point3D p)
	{
		return Point3D(p.x * lhs, p.y * lhs, p.z * lhs);
	}

	inline Point3D operator+ (float lhs, Point3D rhs)
	{
		return Point3D(rhs.x + lhs, rhs.y + lhs, rhs.z + lhs);
	}
}
//----------------------------------------------------------------------------------------------------------------
