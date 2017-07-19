#pragma once

//------MatlabDebugerUtil.h--------------------------------------------------


#include "ShadeRec.h"
#include "Point3D.h"
#include "Objects.h"
#include "GeometricObjects.h"
#include <string>
#include <vector>



using std::vector;
namespace Engine
{
	class MatlabUtilityDebuger
	{
	public:
		/**
		 * Gets the matlab string representation of the samples being shot froma specific point
		 */
		static string CreateVisualizer(vector<Point3D> p_samples, ShadeRec& startingPoint)
		{
			stringstream ouptutResult;
			ouptutResult << "collisionPoint = [" << startingPoint.localHitPoint.x << " " << startingPoint.localHitPoint.y << " " << startingPoint.localHitPoint.z << "]; \n";

			for (int sampleIndex = 0; sampleIndex < p_samples.size(); sampleIndex++)
			{
				ouptutResult << "sample" << sampleIndex << " = [" << p_samples[sampleIndex].x << " " << p_samples[sampleIndex].y << " " << p_samples[sampleIndex].z << "]; \n";
			}

			ouptutResult << "start = [";

			for (int index = 0; index < p_samples.size(); index++)
			{
				ouptutResult << "collisionPoint" << ";";
			}

			ouptutResult << "]; \n";
			ouptutResult << "ends = [";

			for (int index = 0; index < p_samples.size() ; index++)
			{
				
				ouptutResult << "sample" + to_string(index) << ";";
			}

			ouptutResult << "]; \n";

			ouptutResult << "quiver3(starts(:,1), starts(:,2), starts(:,3), ends(:,1), ends(:,2), ends(:,3));\n";
			ouptutResult << "xlabel('x'); ylabel('y'); zlabel('z')";

			return ouptutResult.str();
		}


		/**
		 * Gets the matlab string representation with the quad 
		 */

		/*static string CreateVisualiser(vector<Point3D> p_samples, ShadeRec& startingPoint, Quad* p_currentObject)
		{
			stringstream ouptutResult;
			ouptutResult << "collisionPoint = [" << startingPoint.localHitPoint.x << " " << startingPoint.localHitPoint.y << " " << startingPoint.localHitPoint.z << "]; \n";

			for (int sampleIndex = 0; sampleIndex < p_samples.size(); sampleIndex++)
			{
				ouptutResult << "sample" << sampleIndex << " = [" << p_samples[sampleIndex].x << " " << p_samples[sampleIndex].y << " " << p_samples[sampleIndex].z << "]; \n";
			}

			ouptutResult << "start = [";

			for (int index = 0; index < p_samples.size(); index++)
			{
				ouptutResult << "collisionPoint" << ";";
			}

			ouptutResult << "]; \n";
			ouptutResult << "ends = [";

			for (int index = 0; index < p_samples.size(); index++)
			{
				ouptutResult << "sample" + to_string(index) << ";";
			}

			ouptutResult << "]; \n";

			ouptutResult << "quiver3(starts(:,1), starts(:,2), starts(:,3), ends(:,1), ends(:,2), ends(:,3));\n";
			ouptutResult << "xlabel('x'); ylabel('y'); zlabel('z')";

			ouptutResult << "x = [" << p_currentObject->m_firstTriangle->point1.vertex.x << " " << p_currentObject->m_firstTriangle->point2.vertex.x << " " << p_currentObject->m_firstTriangle->point3.vertex.x << " " << p_currentObject->m_secondTrinagle->point3.vertex.x << "]; \n";
			ouptutResult << "y = [" << p_currentObject->m_firstTriangle->point1.vertex.y << " " << p_currentObject->m_firstTriangle->point2.vertex.y << " " << p_currentObject->m_firstTriangle->point3.vertex.y << " " << p_currentObject->m_secondTrinagle->point3.vertex.y << "]; \n";
			ouptutResult << "z = [" << p_currentObject->m_firstTriangle->point1.vertex.z << " " << p_currentObject->m_firstTriangle->point2.vertex.z << " " << p_currentObject->m_firstTriangle->point3.vertex.z << " " << p_currentObject->m_secondTrinagle->point3.vertex.z << "]; \n";

			ouptutResult << "fill3(x,y,z,'r'); \n";
			ouptutResult << "axis equal \n";

			return ouptutResult.str();
		}*/


		/**
		 * Starts matlab and inputs the samples to the engine 
		 */
		
		static void VisualizeInMatlab()
		{
			//start the engine

			/*engine *m_Engine;
			m_Engine = engOpen("null");

			
			if (m_Engine == NULL)
			{
				std::cout << "Error matlab was not found" << std::endl;
			}*/
		}

	private:

	};
}
//------MatlabDebugerUtil.h--------------------------------------------------