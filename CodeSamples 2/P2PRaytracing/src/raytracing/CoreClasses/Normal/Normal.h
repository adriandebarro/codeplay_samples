#ifndef NORMAL_H
#define NORMAL_H

#include <math.h>

#include "..\Point\Point3D\Point3D.h"

namespace Engine
{
	class Normal
	{
	public:

		double x, y, z;

		Normal(void)
		{
			x = 0;
			y = 0;
			z = 0;
		}

		Normal(const double constant)
		{
			x = constant;
			y = constant;
			z = constant;
		}

		Normal(const double xx, const double yy, const double zz)
		{
			x = xx;
			y = yy;
			z = zz;
		}

		Normal(const Point3D& vector)
		{
			x = vector.x;
			y = vector.y;
			z = vector.z;
		}


		Normal(const Normal &normal)
		{
			x = normal.x;
			y = normal.y;
			z = normal.z;
		}

		inline void 											// assignment operator
			operator= (const Normal& rhs)
		{
				x = rhs.x;
				y = rhs.y;
				z = rhs.z;
			}


		inline void 											// assignment of a point to a normal
			operator= (const Point3D& rhs)
		{
				x = rhs.x;
				y = rhs.y;
				z = rhs.z;
			}

		inline Normal 												// unary minus
			operator- (void)
		{
				return (Normal(-x, -y, -z));
			}

		inline Normal 												// addition
			operator+ (const Normal& n) const
		{
				return (Normal(x + n.x, y + n.y, z + n.z));
			}

		inline Point3D 												// addition
			operator + (const Point3D& n) const
		{
				return (Point3D(x + n.x, y + n.y, z + n.z));
			}

		Normal 											// compound addition
			operator+= (const Normal& n)
		{
				return Normal(x + n.x, y + n.y, z + n.z);
			}

		double operator* (const Point3D& v)				// dot product with a vector on the right
		{
			return (x * v.x + y * v.y + z * v.z);
		}

		Normal operator * (const float& v)				// dot product with a vector on the right
		{
			return (x * v + y * v + z * v);
		}
		//Normal												// multiplication by a double on the right
		//	operator* (const double a) const
		//{

		//}

		void normalize(void)
		{
			double length = sqrt(x * x + y * y + z * z);
			x /= length; y /= length; z /= length;
		}
	};

	inline Point3D operator - (Point3D& lhs, Normal& rhs)
	{
		return Point3D(lhs.x * rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}
}
#endif // !NORMAL_H
