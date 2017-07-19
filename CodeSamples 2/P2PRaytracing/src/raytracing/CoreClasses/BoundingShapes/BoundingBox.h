#pragma once 
//----------------------------------------------------------------------------------------------------------------
//			Filename:		BoundingBox.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <math.h>
#include <algorithm>
#include <vector>
#include <string>
#include <limits>
#include "..\Ray\Ray.h"
#include "..\Point\Point3D\Point3D.h"

namespace Engine
{
	using std::min;
	using std::max;
	using std::vector;
	using std::string;

	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: BoundingBox
	//		   Description: Shape that wraps around scene objects for coarse grained intersection tests
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: 
	//						TODO: Create Bounding shape Abstract Parent Class, for multiple different bounding shapes 
	//----------------------------------------------------------------------------------------------------------------
	class BoundingBox
	{
	public:
		Point3D minPoint, maxPoint;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		BoundingBox()
		{
			#undef max
			#undef  min
			minPoint = Point3D((std::numeric_limits<float>::max()));
			maxPoint = Point3D(std::numeric_limits<float>::min());

			#ifndef max
			#define max(a,b)            (((a) > (b)) ? (a) : (b))
			#endif

			#ifndef min
			#define min(a,b)            (((a) < (b)) ? (a) : (b))
			#endif
		
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: center
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		BoundingBox(Point3D point)
		{
			minPoint = Point3D(point.x, point.y, point.z);
			maxPoint = Point3D(point.x, point.y, point.z);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: MinPoint, MaxPoint
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		BoundingBox(Point3D p_minPoint, Point3D p_maxPoint)
		{
			minPoint = Point3D(p_minPoint.x, p_minPoint.y, p_minPoint.z);
			maxPoint = Point3D(p_maxPoint.x, p_maxPoint.y, p_maxPoint.z);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetMinPoint
		//		Return Value: Point3D minPoint
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D GetMinPoint()
		{
			return minPoint;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: Point3D MaxPoint
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D GetMaxPoint()
		{
			return maxPoint;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Point3D
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void Expand(Point3D currentPoint)
		{
			minPoint.x = min(minPoint.x, currentPoint.x);
			minPoint.y = min(minPoint.y, currentPoint.y);
			minPoint.z = min(minPoint.z, currentPoint.z);

			bool test = maxPoint.x > currentPoint.x;


			maxPoint.x = max(maxPoint.x, currentPoint.x);
			maxPoint.y = max(maxPoint.y, currentPoint.y);
			maxPoint.z = max(maxPoint.z, currentPoint.z);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: Bool 
		//		  Parameters: BoundingBox
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool Overlaps(BoundingBox b)
		{
			bool x = b.maxPoint.x >= minPoint.x && b.minPoint.x <= maxPoint.x;
			bool y = b.maxPoint.y >= minPoint.y && b.minPoint.y <= maxPoint.y;
			bool z = b.maxPoint.z >= minPoint.z && b.minPoint.z <= maxPoint.z;

			return x && y && z;
		}

		Point3D GetExtent()
		{
			return maxPoint - minPoint;
		}

		Real GetExtent(Axis p_axis)
		{
			return maxPoint.At[p_axis] - minPoint.At[p_axis];
		}


		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: bool
		//		  Parameters: Point3D
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool CheckPointInsideBox(Point3D pont)
		{
			bool x = pont.x >= minPoint.x && pont.x <= maxPoint.x;
			bool y = pont.y >= minPoint.y && pont.y <= maxPoint.y;
			bool z = pont.z >= minPoint.z && pont.z <= maxPoint.z;

			return x && y && z;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: BoundingBox*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void Union(BoundingBox* b)
		{
			minPoint.x = min(minPoint.x, b->minPoint.x);
			minPoint.y = min(minPoint.y, b->minPoint.y);
			minPoint.z = min(minPoint.z, b->minPoint.z);

			maxPoint.x = max(maxPoint.x, b->maxPoint.x);
			maxPoint.y = max(maxPoint.y, b->maxPoint.y);
			maxPoint.z = max(maxPoint.z, b->maxPoint.z);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: Union
		//		Return Value: VOID
		//		  Parameters: BoundingBox
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void Union(BoundingBox b)
		{
			minPoint.x = min(minPoint.x, b.minPoint.x);
			minPoint.y = min(minPoint.y, b.minPoint.y);
			minPoint.z = min(minPoint.z, b.minPoint.z);

			maxPoint.x = max(maxPoint.x, b.maxPoint.x);
			maxPoint.y = max(maxPoint.y, b.maxPoint.y);
			maxPoint.z = max(maxPoint.z, b.maxPoint.z);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetLongestAxis
		//		Return Value: Int
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		int GetLongestAxis()
		{
			Point3D diagVector = maxPoint - minPoint;

			if (diagVector.x > diagVector.y && diagVector.x > diagVector.z)
			{
				return 0;
			}
			else if (diagVector.y > diagVector.z)
			{
				return 1;
			}
			else
			{
				return 2;
			}
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetLongestLengthAxis
		//		Return Value: double
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		float GetLongestLengthAxis()
		{
			Point3D diagVector = maxPoint - minPoint;

			if (diagVector.x > diagVector.y && diagVector.x > diagVector.z)
			{
				return diagVector.x;
			}
			else if (diagVector.y > diagVector.z)
			{
				return diagVector.y;
			}
			else
			{
				return diagVector.z;
			}
		}

		inline float GetLengthOfCubeSide()
		{
			Point3D diagVector = maxPoint - minPoint;
			return diagVector.x;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetMiddleOfSide
		//		Return Value: float
		//		  Parameters: int
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		float GetMiddleOfSide(int side)
		{
			Point3D temp =( maxPoint - minPoint)/2;
			Point3D diagVector = temp + minPoint;
			switch (side)
			{

			case 0:
				return diagVector.x;
			case 1:
				return diagVector.y ;
			case 2:
				return diagVector.z;
			default:
				break;
			}
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CheckIntersection
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------v
		bool CheckIntersection(Ray p_ray, double& tmin, double& tmax)
		{
			return CheckIntersection(p_ray.o, p_ray.d, tmin, tmax);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CheckIntersection
		//		Return Value: Bool
		//		  Parameters: Point3D, float
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool CheckIntersection(Point3D radiancePoint, float radius)
		{
			float dMin = 0;

			if (radiancePoint.x < minPoint.x)
				dMin += pow(radiancePoint.x - minPoint.x, 2);
			else if (radiancePoint.x > maxPoint.x)
				dMin += pow(radiancePoint.x - maxPoint.x, 2);

			if (radiancePoint.y < minPoint.y)
				dMin += pow(radiancePoint.y - minPoint.y, 2);
			else if (radiancePoint.y > maxPoint.y)
				dMin += pow(radiancePoint.y - maxPoint.y, 2);

			if (radiancePoint.z < minPoint.z)
				dMin += pow(radiancePoint.z - minPoint.z, 2);
			else if (radiancePoint.z > maxPoint.z)
				dMin += pow(radiancePoint.z - maxPoint.z, 2);

			return dMin < (radius * radius);
		}


		static bool CheckIntersection(Point3D p_minPoint, Point3D p_maxPoint, Point3D radiancePoint, float radius)
		{
			float dMin = 0;

			if (radiancePoint.x < p_minPoint.x)
				dMin += pow(radiancePoint.x - p_minPoint.x, 2);
			else if (radiancePoint.x > p_maxPoint.x)
			{
				//float res = radiancePoint.x - p_maxPoint.x;
				dMin += pow(radiancePoint.x - p_maxPoint.x, 2);
				//dMin += res * res;
			}
			if (radiancePoint.y < p_minPoint.y)
				dMin += pow(radiancePoint.y - p_minPoint.y, 2);
			else if (radiancePoint.y > p_maxPoint.y)
				dMin += pow(radiancePoint.y - p_maxPoint.y, 2);

			if (radiancePoint.z < p_minPoint.z)
				dMin += pow(radiancePoint.z - p_minPoint.z, 2);
			else if (radiancePoint.z > p_maxPoint.z)
				dMin += pow(radiancePoint.z - p_maxPoint.z, 2);

			return dMin < (radius * radius);
		}

		float Check(float p_x1, float p_x2, float p_x3)
		{
			float out = 0;
			float v = p_x1;

			if(v < p_x2)
			{
				float val = (p_x2 - v);
				out += val * val;
			}

			if(v > p_x2)
			{
				float val = (v - p_x2);
				out += val * val;
			}

			return out;
		}

		float SquareDistancePoint(Point3D p_position, float p_radius)
		{
			float sq = 0;

			sq += Check(p_position.x, minPoint.x, maxPoint.x);
			sq += Check(p_position.y, minPoint.y, maxPoint.y);
			sq += Check(p_position.z, minPoint.z, maxPoint.z);

			return sq;
		}

		bool TestSphereBoxIntersectoion(Point3D p_currentPosition,  float p_radius)
		{
			float squareDistance = SquareDistancePoint(p_currentPosition, p_radius);
			return squareDistance <= (p_radius * p_radius);
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetCentroid
		//		Return Value: VOID
		//		  Parameters: Point3D
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D GetCentroid()
		{
			Point3D middle = maxPoint - minPoint;
			return minPoint + (middle / 2);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CheckIntersection
		//		Return Value: bool
		//		  Parameters: Origin, Direction, minDistance, maxDistance
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool CheckIntersection(Point3D o, Point3D d, double& tmin, double& tmax)
		{
			Point3D dirfrac(0);

			dirfrac.x = 1.0f / d.x;
			dirfrac.y = 1.0f / d.y;
			dirfrac.z = 1.0f / d.z;
			// lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
			// r.org is origin of ray
			float t1 = (minPoint.x - o.x)*dirfrac.x;
			float t2 = (maxPoint.x - o.x)*dirfrac.x;
			float t3 = (minPoint.y - o.y)*dirfrac.y;
			float t4 = (maxPoint.y - o.y)*dirfrac.y;
			float t5 = (minPoint.z - o.z)*dirfrac.z;
			float t6 = (maxPoint.z - o.z)*dirfrac.z;

			tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
			tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

			// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
			if (tmax < 0)
			{
				tmin = tmax;
				return false;
			}

			// if tmin > tmax, ray doesn't intersect AABB
			if (tmin > tmax)
			{
				float temp = tmax;
				tmin = tmax;
				tmax = temp;
				return false;
			}
			return true;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ToString
		//		Return Value: string
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		string ToString()
		{
			string output = minPoint.toString() + "--" + maxPoint.toString();
			return output;
		}

	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------
