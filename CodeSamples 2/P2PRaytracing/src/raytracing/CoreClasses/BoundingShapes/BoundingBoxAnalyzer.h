#pragma once 

#include <string>
#include <vector>
#include <iostream>

#include "./BoundingBox.h"
#include "../UtilClasses/Common.h"
#include "../Point/Point3D/Point3D.h"

using Engine::BoundingBox;
using std::vector;
namespace Engine
{
	namespace BoundingShapes
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	
		/// 	A bounding box analyzer. 
		/// 	Used for the bounding box context, 
		/// 	to process the bounding box details
		///  </summary>
		///
		/// <remarks>	Adrian, 07/07/2017. </remarks>
		////////////////////////////////////////////////////////////////////////////////////////////////////
		class BoundingBoxAnalyzer
		{

		public:
			BoundingBoxAnalyzer(std::string p_filePath, BoundingBox* p_mapLimits)
			{
				m_filePath = p_filePath;
				m_mapLimits = p_mapLimits;
				LoadBoundingBoxes();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Loads bounding boxes. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			void LoadBoundingBoxes()
			{
				std::ifstream boundingBoxDefinitionFile(m_filePath);
				string splittingString1 = " ";
				string splittingString2 = "-";
				string splittingString3 = ",";

				if (boundingBoxDefinitionFile.is_open())
				{
					string currentLine = "";
					while (std::getline(boundingBoxDefinitionFile, currentLine))
					{
						char* charCurrentLine = const_cast<char*>(currentLine.c_str());

						if (charCurrentLine[0] == '#' || charCurrentLine[0] == 'O' || currentLine == " ")
							continue;
						else
						{
							char zero = currentLine.at(0);
							char one = currentLine.at(1);
							//parse 
							if (charCurrentLine[0] == 'B' && charCurrentLine[1] == 'B')
							{
								vector<string> allParts;
								vector<string> allParts2;
								vector<string> minDetails;
								vector<string> maxDetails;

								Common::Tokenizer::ParseStringSentence(currentLine, splittingString1, allParts);
								Common::Tokenizer::ParseStringSentence(allParts[1], splittingString2, allParts2);
								Common::Tokenizer::ParseStringSentence(allParts2[0], splittingString3, minDetails);
								Common::Tokenizer::ParseStringSentence(allParts2[1], splittingString3, maxDetails);
								_ASSERT(maxDetails.size() > 0 && minDetails.size() > 0);
								ParseCurrentBoundingBox(minDetails, maxDetails);
							}
						}
					}
				}
				else
				{
					std::cout << "PROBLEM: path not found bbounding boxes parser" << std::endl;
				}

			}

			void ParseCurrentBoundingBox(vector<string>& p_minDetails, vector<string>& p_maxDetails)
			{
				//parse mindata
				float normMinX = stof(p_minDetails[0]);
				float normMinZ = stof(p_minDetails[2]);
				//parse max details
				float normMaxX = stof(p_maxDetails[0]);
				float normMaxZ = stof(p_maxDetails[2]);

				float remaningChnageX = m_mapLimits->maxPoint.x - m_mapLimits->minPoint.x;
				float remaningChnageZ = m_mapLimits->maxPoint.z - m_mapLimits->minPoint.z;
				//shift from normalized to world space coordinates
				Point3D tempMinPosition(m_mapLimits->minPoint.x + normMinX*remaningChnageX, m_mapLimits->minPoint.y, m_mapLimits->minPoint.z + remaningChnageZ*normMinZ);
				Point3D tempMaxPosition(m_mapLimits->minPoint.x + normMaxX*remaningChnageX, m_mapLimits->maxPoint.y, m_mapLimits->minPoint.z + remaningChnageZ*normMaxZ);

				//rotate by 90 degrees clockwise 90 degrees = 1.57 radians
				float minPosX = tempMinPosition.x * std::cos(1.5708) - tempMinPosition.z * std::sin(1.5708);
				float minPosZ = tempMinPosition.z * std::cos(1.5708) + tempMinPosition.x * std::sin(1.5708);

				float maxPosX = tempMaxPosition.x * std::cos(1.5708) - tempMaxPosition.z * std::sin(1.5708);
				float maxPosZ = tempMaxPosition.z * std::cos(1.5708) + tempMaxPosition.x * std::sin(1.5708);

				Point3D maxPoint = Point3D(maxPosX, m_mapLimits->maxPoint.y, maxPosZ);
				Point3D minPoint = Point3D(minPosX, m_mapLimits->minPoint.y, minPosZ);

				BoundingBox* currentBoundingBox = new BoundingBox(minPoint, maxPoint);
				m_allBoundingBoxes.push_back(currentBoundingBox);
			}


			int CheckContainingBoundingBox(Point3D p_currentPosition)
			{
				if (Point3D::DistanceBetweenPoints(p_currentPosition, m_lastCheckedLocation) > 0.5)
				{
					for (int boundingBoxesIndex = 0; boundingBoxesIndex < m_allBoundingBoxes.size(); boundingBoxesIndex++)
					{
						if (m_allBoundingBoxes[boundingBoxesIndex]->CheckPointInsideBox(p_currentPosition))
						{
							m_lastCheckedLocation = p_currentPosition;
							return boundingBoxesIndex;
						}
					}
					return -1;
				}

			}

			float GetdistanceBetweenBoundBoxes(int p_lhs, int p_rhs)
			{
				if (p_lhs >= m_allBoundingBoxes.size() || p_rhs >= m_allBoundingBoxes.size())
				{
					#undef max
					return std::numeric_limits<float>::max();
				}
				else
				{
					Point3D leftCentroid = m_allBoundingBoxes[p_lhs]->GetCentroid();
					Point3D rightCentroid = m_allBoundingBoxes[p_rhs]->GetCentroid();

					return Point3D::DistanceBetweenPoints(leftCentroid, rightCentroid);
				}
			}

			BoundingBox* GetBoundingBox(int index)
			{
				return m_allBoundingBoxes[index];
			}

			int CheckIntersectionIndex(Point3D p_currentPosition)
			{
				for (int index = 0; index < m_allBoundingBoxes.size(); index++)
				{
					BoundingBox* currentBoundingBox = m_allBoundingBoxes[index];
					if (currentBoundingBox->CheckPointInsideBox(p_currentPosition))
					{
						return index;
					}
				}

				return -1;
			}

			vector<BoundingBox*> m_allBoundingBoxes;
		private:
			std::string m_filePath;
			Point3D m_lastCheckedLocation;
			
			BoundingBox* m_mapLimits;
		};
	}
}