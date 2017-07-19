#ifndef VECTOR3D_H
#define VECTOR3D_H

#include <math.h>
#include "Point3D.h"
#include "Normal.h"




class Vector3D
{
//public:
//	
//
//	Vector3D()
//	{
//		x = 0;
//		y = 0;
//		z = 0;
//	}
//
//	Vector3D(const Vector3D& currentVector)
//	{
//		x = currentVector.x;
//		y = currentVector.y;
//		z = currentVector.z;
//	}
//
//	Vector3D(const double xx, const double yy, const double zz)
//	{
//		x = xx;
//		y = yy;
//		z = zz;
//	}
//
//	Vector3D(const double constant)
//	{
//		x = constant;
//		y = constant;
//		z = constant;
//	}
//
//	Vector3D& 												// assignment operator
//		operator= (const Vector3D& rhs)
//	{
//			return Vector3D(rhs.x, rhs.y, rhs.z);
//	}
//
//	Vector3D& 												// assign a Normal to a vector
//		operator= (const Normal& rhs)
//	{
//		return Vector3D(rhs.x, rhs.y, rhs.z);
//	}
//
//	Vector3D& 												// assign a Point3D to a vector
//		operator= (const Point3D& rhs)
//	{
//			return Vector3D(rhs.x, rhs.y, rhs.z);
//		}
//
//	Vector3D												// unary minus
//		operator- (void) const
//	{
//			return Vector3D(-x, -y, -z);
//	}
//
//	double													// length
//		length(void)
//	{
//		return 0;
//	}
//
//	double													// square of the length
//		len_squared(void);
//
//	Vector3D												// multiplication by a double on the right
//		operator* (const double a) const
//	{
//		return Vector3D(a * x, a * y, a * z);
//	}
//
//	Vector3D												// division by a double
//		operator/ (const double a) const
//	{
//		return Vector3D(x / a, y / a, z / a);
//	}
//
//	Vector3D												// addition
//		operator+ (const Vector3D& v)
//	{
//		return Vector3D(x * v.x, y * v.y, z * v.z);
//	}
//
//	Vector3D& 												// compound addition
//		operator+= (const Vector3D& v)
//	{
//		return Vector3D(x + v.x, y + v.y, z + v.z);
//	}
//	
//	Vector3D												// subtraction
//		operator- (const Vector3D& v)
//	{
//			return Vector3D(x - v.x, y - v.y, z - v.z);
//	}
//
//	double 													// dot product							
//		operator* (const Vector3D& b)
//	{
//			return (x * b.x + y*b.y + z*b.z);
//	}
//
//	Vector3D 												// cross product				
//		operator^ (const Vector3D& v) const;
//
//	void 													// convert vector to a unit vector
//		normalize(void)
//	{
//			float l = 1 / sqrtf(x*x + y*y + z*z);
//			x *= l;
//			y *= l; 
//			z *= l;
//	}
//
//	//Vector3D& 												// return a unit vector, and normalize the vector												
//	//	hat(void);
//
};

#endif // !VECTOR3D_H
