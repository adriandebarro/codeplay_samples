#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		MatlabUtilityDebuger.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>

#include "../../CollisionDetails\ShadeRec.h"
#include "..\Point\Point3D\Point3D.h"
#include "..\GeometricObjects\Objects.h"
#include "..\GeometricObjects\GeometricObjects.h"
#include "..\GeometricObjects\Triangle.h"

using std::vector;
using Engine::Primitives::Triangle;
namespace Engine
{

	namespace Debugging
	{
		//----------------------------------------------------------------------------------------------------------------
		//			Class Name: MatlabUtilityDebuger
		//		   Description: Used to visualize vector data 
		//----------------------------------------------------------------------------------------------------------------
		//			    Status: REFACTORING
		//----------------------------------------------------------------------------------------------------------------
		class MatlabUtilityDebuger
		{
		public:
			/**
			 * Gets the matlab code representation of the samples being shot from a specific point
			 */
			static string CreateVisualizer(vector<ShadeRec*> p_samples, Point3D p_rayOrigin, GeometricObjects* p_currentObject, Point3D p_Normal)
			{

				stringstream ouptutResult;
				Triangle* currentTriangle = (Triangle*)p_currentObject;

				ouptutResult << "%these is the trinagle coordinates" << "\n";
				ouptutResult << "x = [" << currentTriangle->point1.vertex.x << "," << currentTriangle->point2.vertex.x << "," << currentTriangle->point3.vertex.x << "]; \n";
				ouptutResult << "y = [" << currentTriangle->point1.vertex.y << "," << currentTriangle->point2.vertex.y << "," << currentTriangle->point3.vertex.y << "]; \n";
				ouptutResult << "z  = [" << currentTriangle->point1.vertex.z << "," << currentTriangle->point2.vertex.z << "," << currentTriangle->point3.vertex.z << "]; \n";

				ouptutResult << "normalDirection = [" << p_Normal.x << " " << p_Normal.y << " " << p_Normal.z << "]; \n";

				ouptutResult << "fill3( x, y ,z, 'r'); \n";

				ouptutResult << "hold all" << "; \n";

				ouptutResult << "collisionPoint = [" << p_rayOrigin.x << " " << p_rayOrigin.y << " " << p_rayOrigin.z << "]; \n";

				for (int sampleIndex = 0; sampleIndex < p_samples.size(); sampleIndex++)
				{
					//Point3D currentHitPoint = p_samples[sampleIndex]->localHitPoint.x == 0 && p_samples[sampleIndex]->localHitPoint.y == 0 && p_samples[sampleIndex]->localHitPoint.z == 0? p_samples[sampleIndex]->ray.d : p_samples[sampleIndex]->localHitPoint;
					Point3D currentHitPoint = p_samples[sampleIndex]->ray.d ;
					ouptutResult << "sample" << sampleIndex << " = [" << currentHitPoint.x << " " << currentHitPoint.y << " " << currentHitPoint.z << "]; \n";
				}

				ouptutResult << "starts = [";

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

				ouptutResult << "quiver3(collisionPoint(1), collisionPoint(2), collisionPoint(3), normalDirection(1), normalDirection(2), normalDirection(3), 'color', [1 0 0]); \n";
				ouptutResult << "xlabel('x'); ylabel('y'); zlabel('z')";

				return ouptutResult.str();
			}

			static string CreateVisualizer(Ray p_currentRay, GeometricObjects* p_currentObject)
			{

				stringstream ouptutResult;
				Triangle* currentTriangle = (Triangle*)p_currentObject;

				ouptutResult << "//this is the trinagle coordinates" << "\n";
				ouptutResult << "x = [" << currentTriangle->point1.vertex.x << "," << currentTriangle->point2.vertex.x << "," << currentTriangle->point3.vertex.x << "]; \n";
				ouptutResult << "y = [" << currentTriangle->point1.vertex.y << "," << currentTriangle->point2.vertex.y << "," << currentTriangle->point3.vertex.y << "]; \n";
				ouptutResult << "z  = [" << currentTriangle->point1.vertex.z << "," << currentTriangle->point2.vertex.z << "," << currentTriangle->point3.vertex.z << "]; \n";

				ouptutResult << "fill3( x, y ,z, 'r'); \n";

				ouptutResult << "collisionPoint = [" << p_currentRay.o.x << " " << p_currentRay.o.y << " " << p_currentRay.o.z << "]; \n";


				ouptutResult << "sample1" << " = [" << p_currentRay.d.x << " " << p_currentRay.d.y << " " << p_currentRay.d.z << "]; \n";


				ouptutResult << "start = [";


				ouptutResult << "collisionPoint" << ";";

				ouptutResult << "]; \n";
				ouptutResult << "ends = [";

				ouptutResult << "sample1;";


				ouptutResult << "]; \n";

				ouptutResult << "quiver3(starts(:,1), starts(:,2), starts(:,3), ends(:,1), ends(:,2), ends(:,3), 'color', [1 0 0]);\n";
				ouptutResult << "xlabel('x'); ylabel('y'); zlabel('z')";

				return ouptutResult.str();
			}

			static string PlotSamplePoints(Point3D p_normal, Point3D p_origin, vector<Point3D>& p_samplePoints)
			{
				stringstream outputCode;

				outputCode << "origin = [" << (std::abs(p_origin.x) < kEpsilon? 0 : p_origin.x) << " " << (std::abs(p_origin.y) < kEpsilon ? 0 : p_origin.y) << " " << (std::abs(p_origin.z) < kEpsilon ? 0 : p_origin.z)  <<  "];" << "\n";
				outputCode << "normalDir = [" << (std::abs(p_normal.x) < kEpsilon ? 0 : p_normal.x) << " " << (std::abs(p_normal.y) < kEpsilon ? 0 : p_normal.y) << " " << (std::abs(p_normal.z) < kEpsilon ? 0 : p_normal.z) << "];" << "\n";
				
				for (int sampleIndex = 0; sampleIndex < p_samplePoints.size(); sampleIndex++)
				{
					outputCode << "sample" << sampleIndex << " = [" << p_samplePoints[sampleIndex].x << " " << p_samplePoints[sampleIndex].y << " " << p_samplePoints[sampleIndex].z << " ];" << "\n";
				}

				outputCode << "ends = [";

				for (int index = 0; index < p_samplePoints.size(); index++)
				{
					outputCode << "sample" << index << ";";
				}

				outputCode << "]; \n";


				outputCode << "starts = [";

				for (int index = 0; index < p_samplePoints.size(); index++)
				{
					outputCode << "origin;";
				}

				outputCode << "]; \n";

				outputCode << "quiver3(starts(:,1), starts(:,2), starts(:,3), ends(:,1), ends(:,2), ends(:,3));" << endl;

				outputCode << "hold all;" << endl;

				outputCode << "quiver3(origin(1), origin(2), origin(3), normalDir(1), normalDir(2), normalDir(3), 'color', [1 0 0]); " << endl;

				return outputCode.str();
			}


			static void VisualizeHemisphericalSamplesSamples(vector<Point3D>& p_allSamples, int p_amountSamples, int p_amountsets)
			{
				stringstream ss;
				int counter = 0;

				for (int setIndex = 0; setIndex < p_amountsets; setIndex++)
				{
					counter = 0;

					ss << "% start of set --------------------------------------------------------------------------" << setIndex << endl;

					ss << "startingPoint = [0 0 0];" << endl;
					ss << "normalDirection = [0 0 1];" << endl;

					ss << "starts = [";

					for (int sampleIndex = 0; sampleIndex < p_amountSamples; sampleIndex ++)
					{
						ss << "startingPoint;";
					}

					ss << "];" << endl;

					for (int sampleIndex = setIndex * setIndex; sampleIndex < p_amountSamples; sampleIndex ++)
					{
						//set the points coordinates 
						//normal is always 0 0 1
						ss << "sample" << counter << " = [" << p_allSamples[sampleIndex].x << " " << p_allSamples[sampleIndex].y << " " << p_allSamples[sampleIndex].z << "]; " << endl;
						counter++;
					}

					ss << "ends = [";

					for (int index = 0; index < p_amountSamples; index ++)
					{
						ss << "sample" << index << ";";
					}

					ss << "];" << endl;

					ss << "quiver3(starts(:,1), starts(:,2), starts(:,3), ends(:,1), ends(:,2), ends(:,3));" << endl;

					ss << "hold all;" << endl;
					
					ss << "quiver3(startingPoint(1), startingPoint(2), startingPoint(3), normalDirection(1), normalDirection(2), normalDirection(3), 'color', [1 0 0]); " << endl;

					ss << "% end of current set --------------------------------------------------------------------------" << setIndex<< endl;

					std::cout << ss.str() << endl;

					ss = stringstream();
				}
			}


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
}