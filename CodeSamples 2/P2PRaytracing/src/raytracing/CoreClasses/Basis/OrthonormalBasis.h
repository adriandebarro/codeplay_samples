#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		OrthonomalBasis.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <math.h>

#include "..\Point\Point3D\Point3D.h"

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: OrthonomalBasis
	//		   Description: Orthonomal basiis represent three vectors which are perpendicular to each other
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class OrthonomalBasis
	{
	public:
		//----------------------------------------------------------------------------------------------------------------
		//			w == x|up == y|v == z
		//----------------------------------------------------------------------------------------------------------------
		Point3D up, v, w;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		OrthonomalBasis()
		{
			up = Point3D(0, 1, 0);
			v = Point3D(1, 0, 0);
			w = Point3D(0, 0, 1);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ProduceOrthonomalBasis
		//		Return Value: New Position
		//		  Parameters: New Normal, point to translate
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		static Point3D ProduceOrthonomalBasis(Point3D p_Normal, Point3D p_pointToTransalte)
		{
			Point3D w = p_Normal.normalize();
			Point3D v(0);
			Point3D u(0);

			if (std::fabs(w.x) >  std::fabs(w.y))
			{
				float invLen = 1.f / std::sqrt(w.x * w.x + w.z * w.z);
				v = Point3D(-w.z * invLen, 0.f, w.x * invLen);
			}
			else
			{
				float invLen = 1.f / std::sqrt(w.x * w.x + w.z * w.z);
				v = Point3D(0.f, w.z * invLen, -w.y * invLen);
			}

			u = w.CrossProduct(v);
			
			return Point3D(u.dot(p_pointToTransalte), v.dot(p_pointToTransalte), w.dot(p_pointToTransalte));
		
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ProduceOrthonomalBasis 
		//		Return Value: VOID
		//		  Parameters: Direction
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		 void ProduceOrthonomalBasis(Point3D p_w)
		 {
			 Point3D currentUpVector(0,1 ,0);
			 float resultDotProduct = currentUpVector.dot(p_w);

			 /*if (resultDotProduct < 0)
			 {
				 currentUpVector = Engine::Point3D(1, 0, 0);
			 }*/

			 w = p_w;
			 w.normalize();

			 v = w.CrossProduct(currentUpVector);
			 v.normalize();

			 up = v.CrossProduct(w);
		 }


		 void ProduceOrthonomalBasis(Point3D p_w, Point3D p_v, Point3D p_Up)
		 {
			 up = p_Up;
			 w = p_w;
			 v = p_v;

			 up.normalize();
			 v.normalize();
			 w.normalize();
		 }

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: BuildOrthonomalFromNormal 
		//		Return Value: VOID
		//		  Parameters: Normal 
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void BuildOrthonomalFromNormal(Point3D p_normal)
		{
			p_normal.normalize();
			v = Point3D(1 - pow(p_normal.x, 2)/(1 + p_normal.z), -p_normal.x * p_normal.y/(1 + p_normal.z), -p_normal.x);
			w = Point3D((-p_normal.x*p_normal.y)/(1 + p_normal.z), 1 - pow(p_normal.y, 2)/ (1 + p_normal.z), -p_normal.y);
			up = p_normal;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: BuildOrthonomalFromNormal 
		//		Return Value: VOID
		//		  Parameters: Normal 
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D RotatePointRoundOrthonomal(Point3D p_currentPoint)
		{
			if (up != NULL && v != NULL && w != NULL)
			{
				Point3D newOutput = p_currentPoint.x * v + p_currentPoint.y * w + p_currentPoint.z * up;
				newOutput.normalize();
				return newOutput;
			}
			else
				return p_currentPoint;
		}

		string ToString()
		{
			stringstream ss;
			ss << "up: " << up.toString() << "w: " << w.toString() << "v: " << v.toString();
			return ss.str();
		}


	private:
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------
