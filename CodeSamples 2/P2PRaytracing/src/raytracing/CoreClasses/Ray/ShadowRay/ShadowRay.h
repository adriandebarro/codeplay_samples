#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		MeshObject.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include "..\Ray.h"


namespace Engine
{
	///-------------------------------------------------------------------------------------------------
	/// <summary>	A shadow ray. Used to sample for shadows </summary>
	///
	/// <remarks>	Adrian, 27/08/2015. </remarks>
	///-------------------------------------------------------------------------------------------------
	class ShadowRay : public Ray
	{
	public:
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Constructor. </summary>
		///
		/// <remarks>	Adrian, 27/08/2015. </remarks>
		///
		/// <param name="p_origin">   	The origin. </param>
		/// <param name="p_direction">	The direction. </param>
		///-------------------------------------------------------------------------------------------------
		ShadowRay(Point3D p_origin, Point3D p_direction) : Ray(p_origin, p_direction)
		{}
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Constructor. </summary>
		///
		/// <remarks>	Adrian, 27/08/2015. </remarks>
		///
		/// <param name="p_origin">   	The origin. </param>
		/// <param name="p_direction">	The direction. </param>
		/// <param name="p_tmin">	  	Minimum of the ray segment. </param>
		/// <param name="p_tmax">	  	Maximum of the ray segment. </param>
		///-------------------------------------------------------------------------------------------------
		ShadowRay(Point3D p_origin, Point3D p_direction, double p_tmin, double p_tmax) : Ray(p_origin, p_direction, p_tmin, p_tmax)
		{
			
		}
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------

