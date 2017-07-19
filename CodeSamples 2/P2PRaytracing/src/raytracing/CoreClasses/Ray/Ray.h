#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Ray.h
//			  Author:		Adrian De Barro
//			    Date:		13/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include "..\Point\Point3D\Point3D.h"
#include <limits>

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: Ray
	//		   Description: Represents a ray of light starting at an origin O and direction D
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	struct Ray
	{
		Point3D o;
		Point3D d;
		Point3D invd;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Ray(void)
		{
			o = Point3D(0);
			d = Point3D(0);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Point3D origin, Point3D Direction
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Ray(Point3D origin, Point3D dir)
		{
			o = origin;
			d = dir;
			invd = Point3D( 1/ dir.x, 1/ dir.y, 1/dir.z);

			m_tmax = -500000;
			m_tmin = 500000;
		}
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Constructor. </summary>
		///
		/// <remarks>	Adrian, 27/08/2015. </remarks>
		///
		/// <param name="p_origin">   	The origin. </param>
		/// <param name="p_direction">	The direction. </param>
		/// <param name="p_tmin">	  	The tmin. </param>
		/// <param name="p_tmax">	  	The tmax. </param>
		///-------------------------------------------------------------------------------------------------
		Ray(Point3D p_origin, Point3D p_direction, double p_tmin, double p_tmax) : o(p_origin), d(p_direction), m_tmin(p_tmin), m_tmax(p_tmax)
		{
			invd = Point3D(1 / p_direction.x, 1 / p_direction.y, 1 / p_direction.z);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Ray(Ray& currentRay)
		{
			o = currentRay.o;
			d = currentRay.d;
			invd = Point3D(1 / d.x, 1 / d.y, 1 / d.z);
			m_tmin = currentRay.GetTmin();
			m_tmax = currentRay.GetTmax();
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: operator
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void operator = (const Ray rhs)
		{
			o = rhs.o;
			d = rhs.d;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: TestRayData
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool TestRayData()
		{
			
			if (isnan(o.x) || isnan(o.y) || isnan(o.z) || isnan(d.x) || isnan(d.y) || isnan(d.z))
			{
				return true;
			}
			return false;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: DESTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		~Ray(void)
		{}
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets the tmax. </summary>
		///
		/// <remarks>	Adrian, 27/08/2015. </remarks>
		///
		/// <returns>	The tmax. </returns>
		///-------------------------------------------------------------------------------------------------
		double GetTmax()
		{
			return m_tmax;
		}
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets the tmin. </summary>
		///
		/// <remarks>	Adrian, 27/08/2015. </remarks>
		///
		/// <returns>	The tmin. </returns>
		///-------------------------------------------------------------------------------------------------

		double GetTmin()
		{
			return m_tmin;
		}



		/// <summary>	Minimum limit of the ray segment. </summary>
		double m_tmin;
		/// <summary>	Maximum limit of the ray segment. </summary>
		double m_tmax;

	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------
