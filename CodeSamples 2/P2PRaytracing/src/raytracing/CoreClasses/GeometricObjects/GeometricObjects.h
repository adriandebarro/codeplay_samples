#pragma once 
//----------------------------------------------------------------------------------------------------------------
//			Filename:		GeometricObjects.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <string>

#include "..\Ray\Ray.h"
#include "..\..\CollisionDetails\ShadeRec.h"
#include "..\RGBObject\RGBColor.h"
#include "..\BoundingShapes\BoundingBox.h"
#include "..\Point\Point3D\Point3D.h"
#include "..\Logger\Logger.h"

namespace Engine
{
	namespace Materials
	{
		class Material;
	}
}


namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: GeometricObjects
	//		   Description: Abstract parent Class representing a scene object
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class GeometricObjects
	{
	public:

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Hits. </summary>
		///
		/// <remarks>	Adrian, 27/08/2015. </remarks>
		///
		/// <param name="o">   	The Point3D to process. </param>
		/// <param name="d">   	The Point3D to process. </param>
		/// <param name="tmin">	[in,out] The tmin. </param>
		/// <param name="sr">  	[in,out] The sr. </param>
		///
		/// <returns>	true if it succeeds, false if it fails. </returns>
		///-------------------------------------------------------------------------------------------------
		virtual	bool hit(Point3D o, Point3D d, double &tmin, ShadeRec &sr) = 0;
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Hits. </summary>
		///
		/// <remarks>	Adrian, 27/08/2015. </remarks>
		///
		/// <param name="p_ray">	The ray. </param>
		/// <param name="tmin"> 	[in,out] The tmin. </param>
		/// <param name="sr">   	[in,out] The sr. </param>
		///
		/// <returns>	true if it succeeds, false if it fails. </returns>
		///-------------------------------------------------------------------------------------------------
		virtual	bool hit(Ray p_ray, double &tmin, ShadeRec &sr) = 0;
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Hits. </summary>
		///
		/// <remarks>	Adrian, 27/08/2015. </remarks>
		///
		/// <param name="p_ray">	The ray. </param>
		/// <param name="tmin"> 	[in,out] The tmin. </param>
		/// <param name="sr">   	[in,out] The sr. </param>
		///
		/// <returns>	true if it succeeds, false if it fails. </returns>
		///-------------------------------------------------------------------------------------------------
		virtual bool hit(Ray p_ray, Intersection &p_intersection_out) = 0;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetBoundingBox
		//		Return Value: BoundingBox*
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Engine::BoundingBox* GetBoundingBox()
		{
			return box;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: hit
		//		Return Value: VOID
		//		  Parameters: Ray Origin, Ray Direction, Shortest Distance to intersection, Collision Details
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual void MakeBoundingBox() = 0;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		GeometricObjects(){}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetColor
		//		Return Value: RGBColor
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor GetColor()
		{
			return color;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetMaterial
		//		Return Value: Material*
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Engine::Materials::Material * GetMaterial()
		{
			return m_material;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetMaterial
		//		Return Value: VOID
		//		  Parameters: Material*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetMaterial(Engine::Materials::Material *p_material)
		{
			m_material = p_material;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetNormal
		//		Return Value: Point3D
		//		  Parameters: Point3D
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual Point3D GetNormal(Point3D) = 0;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: PrintObjectName
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual void PrintObjectName() = 0;

	protected:
		RGBColor color;
		Engine::Materials::Material *m_material;
		Engine::BoundingBox* box;
		string m_objectName;
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------


