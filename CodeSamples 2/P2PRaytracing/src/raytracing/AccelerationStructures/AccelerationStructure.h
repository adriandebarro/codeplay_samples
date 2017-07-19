#pragma once 


#include <vector>
#include <iostream>
#include <queue>
#include <sstream>

#include "../CoreClasses/UtilClasses/Common.h"
#include "../CoreClasses/BoundingShapes/BoundingBox.h"
#include "../CoreClasses/Point/Point3D/Point3D.h"
#include "../CoreClasses/GeometricObjects/GeometricObjects.h"
#include "../CoreClasses/Ray/Ray.h"
#include "../CoreClasses/GeometricObjects/Triangle.h"
#include "../CoreClasses/UtilClasses/Common.h"


namespace Engine
{
	

	namespace AccelerationStructures
	{

		enum PartitioningStyle
		{
			/// <summary>	An enum constant representing the sah option. </summary>
			SAH,
			/// <summary>	An enum constant representing the middle point option. </summary>
			MIDDLE_POINT,
			/// <summary>	An enum constant representing the half option. </summary>
			HALF
		};

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	
		/// 	Abstract Acceleration Structure.
		/// </summary>
		///
		/// <remarks>	Adrian, 07/07/2017. </remarks>
		////////////////////////////////////////////////////////////////////////////////////////////////////
		struct AccelerationStructure
		{
			vector<GeometricObjects*> m_allPrimitives;
			vector<int> m_TriangleIndexList;
			BoundingBox* m_boundingBox;

			AccelerationStructure()
			{}

			AccelerationStructure(vector<GeometricObjects*> p_allObjects)
			{
				m_allPrimitives = p_allObjects;
			}

			virtual void BuildTree(PartitioningStyle p_partitioningStyle) = 0;
			virtual bool TestIntersection(Ray p_ray, ShadeRec& p_rec, double& p_tmin, bool p_shadowTest = false) = 0;

			GeometricObjects* GetPrimitive(int index)
			{
				_ASSERT(index < m_allPrimitives.size());
				return m_allPrimitives[index];
			}
		};
	}
	
}
