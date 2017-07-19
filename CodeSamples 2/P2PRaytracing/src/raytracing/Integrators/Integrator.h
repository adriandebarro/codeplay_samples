#pragma once 
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Integrator.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: BUILDING
//----------------------------------------------------------------------------------------------------------------

#include "..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "RadianceDetails\RadiancePoint.h"

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: Integrator
	//		   Description: Abstract Class for integrators
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: BUILDING
	//----------------------------------------------------------------------------------------------------------------
	class Integrator
	{
	private:

	public:
		virtual RGBColor GetIndirectLightingAtPoint(ShadeRec& p_collisionDetails) = 0;
		virtual void InvalidateCache()
		{}

		virtual void PartialInvalidate(BoundingBox* p_boundingBox)
		{

		}
	};
}