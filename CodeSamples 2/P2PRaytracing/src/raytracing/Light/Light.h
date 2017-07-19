#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Light.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------

#include <string>

#include "..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\CollisionDetails\ShadeRec.h"
#include "..\CoreClasses\Ray\Ray.h"

namespace Engine
{

	using std::string;
	//----------------------------------------------------------------------------------------------------------------
	//			Enum Name: Light Type
	//		  Description: Types of light
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	enum LIGHT_TYPE
	{
		AMBIENT_LIGHTING,
		POINT_LIGHT,
		DIRECTIONAL_LIGHT,
		AREA_LIGHT
	};

	enum LIGHT_STATE
	{
		LIGHT_OFF = 0,
		LIGHT_ON = 1
	};
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: Light
	//		   Description: This object represents a light source in the scene
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class Light
	{
	public:
		string m_lightName;
		
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetDirection
		//		Return Value: Point3D
		//		  Parameters: CollisionDetails
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual Point3D GetDirection(ShadeRec& sr) = 0;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: L
		//		Return Value: RGBColor
		//		  Parameters: CollisionDetails
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual RGBColor L(ShadeRec& sr) = 0;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetRadiance
		//		Return Value: float
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		float GetRadiance()
		{
			if (m_lightState == LIGHT_ON)
				return m_ls;
			else
				return 0;
		}
			
		void SwitchLight()
		{
			m_lightState = (m_lightState == LIGHT_ON) ? LIGHT_OFF : LIGHT_ON;
		}

		void SetLightStatus(int p_status)
		{
			m_lightState = p_status == 0? LIGHT_OFF : LIGHT_ON;
		}


		///-------------------------------------------------------------------------------------------------
		/// <summary>	Sets a radiance. </summary>
		///
		/// <remarks>	Adrian, 07/10/2015. </remarks>
		///
		/// <param name="p_ls">	The ls. </param>
		///-------------------------------------------------------------------------------------------------

		void SetRadiance(float p_ls)
		{
			m_ls = p_ls;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Sets a location. </summary>
		///
		/// <remarks>	Adrian, 07/10/2015. </remarks>
		///
		/// <param name="p_x">	The x coordinate. </param>
		/// <param name="p_y">	The y coordinate. </param>
		/// <param name="p_z">	The z coordinate. </param>
		///-------------------------------------------------------------------------------------------------

		void SetLocation(float p_x, float p_y, float p_z)
		{
			m_location = Point3D(p_x, p_y, p_z);
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Sets a location. </summary>
		///
		/// <remarks>	Adrian, 07/10/2015. </remarks>
		///
		/// <param name="p_location">	The location. </param>
		///-------------------------------------------------------------------------------------------------

		void SetLocation(Point3D p_location)
		{
			m_location = p_location;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets the location. </summary>
		///
		/// <remarks>	Adrian, 07/10/2015. </remarks>
		///
		/// <returns>	The location. </returns>
		///-------------------------------------------------------------------------------------------------

		Point3D GetLocation()
		{
			return m_location;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Sets a direction. </summary>
		///
		/// <remarks>	Adrian, 07/10/2015. </remarks>
		///
		/// <param name="p_direction">	The direction. </param>
		///-------------------------------------------------------------------------------------------------

		void SetDirection(Point3D p_direction)
		{
			m_location = p_direction;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets the direction. </summary>
		///
		/// <remarks>	Adrian, 07/10/2015. </remarks>
		///
		/// <returns>	The direction. </returns>
		///-------------------------------------------------------------------------------------------------

		Point3D GetDirection()
		{
			return m_location;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Tests intersection. </summary>
		///
		/// <remarks>	Adrian, 07/10/2015. </remarks>
		///
		/// <param name="parameter1">	[in,out] The first parameter. </param>
		/// <param name="parameter2">	[in,out] The second parameter. </param>
		///-------------------------------------------------------------------------------------------------
		virtual void TestIntersection(Ray&, ShadeRec&) = 0;

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets light type. </summary>
		///
		/// <remarks>	Adrian, 07/10/2015. </remarks>
		///
		/// <returns>	The light type. </returns>
		///-------------------------------------------------------------------------------------------------

		LIGHT_TYPE GetLightType()
		{
			return m_lightType;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Sets light type. </summary>
		///
		/// <remarks>	Adrian, 07/10/2015. </remarks>
		///
		/// <param name="p_lightType">	Type of the light. </param>
		///-------------------------------------------------------------------------------------------------

		void SetLightType(LIGHT_TYPE p_lightType)
		{
			m_lightType = p_lightType;
		}

		void SetLightName(string p_lightName)
		{
			m_lightName = p_lightName;
		}

		string GetLightName()
		{
			return m_lightName;
		}


		int GetIndex()
		{
			return m_Index;
		}

		void SetIndex(int p_index)
		{
			m_Index = p_index;
		}

		void SwitchStatus()
		{
			m_lightState = (m_lightState == LIGHT_ON)? LIGHT_OFF : LIGHT_ON;
		}

		LIGHT_STATE GetStatus()
		{
			return m_lightState;
		}

		void SetRange(float p_range)
		{
			m_range = p_range;
		}

		float GetRange()
		{
			return m_range;
		}

	protected:
		float m_ls;
		Point3D m_location;
		GeometricObjects* m_objectRepresentation;
		LIGHT_TYPE m_lightType;
		LIGHT_STATE m_lightState = LIGHT_ON;
		int m_Index = 0;
		float m_range;
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------

