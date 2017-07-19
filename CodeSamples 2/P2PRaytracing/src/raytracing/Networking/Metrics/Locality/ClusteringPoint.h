#pragma once 

#include <vector>

#include "../../../CoreClasses/Point/Point3D/Point3D.h"
#include "../../PeerInfo/Update.h"

using std::vector;
using Engine::Point3D;
namespace Engine
{
	namespace Networking
	{
		namespace Metrics
		{
			namespace Locality
			{
				class ClusteringPoint
				{
					
				private:

				public:
					vector<Update*> clusteredPoints;
					Point3D ClusterCenter;

					float m_weightAssignedLocality;
					float m_weightAssignedBw;
					float m_weightAssignedCpu;
					

					int m_ClusteringType;

					float m_distanceFromLocalPoint = 0;

					ClusteringPoint(Point3D p_location, int type = 0)
					{
						ClusterCenter = p_location;
					}

					void GenerateCenter()
					{
						Point3D tempPoint = (0, 0);

						for (Update* currentPoint : clusteredPoints)
						{
							tempPoint = currentPoint->GetLocation() + tempPoint;
						}

						if (clusteredPoints.size() > 0)
						{
							tempPoint = tempPoint / clusteredPoints.size();
							ClusterCenter = tempPoint;
						}

						
					}
					
					void UpdateWeightForChildren()
					{
						for (Update* currentUpdate : clusteredPoints)
						{
							currentUpdate->SetLocalityrobability(m_weightAssignedLocality);
						}
					}

					void SetLocalityWeight(float p_assignedWeight)
					{
						m_weightAssignedLocality = p_assignedWeight;
					}

					void SetWeightBandwidth(float p_assignedWeight)
					{
						m_weightAssignedBw = p_assignedWeight;
					}

					void SetWeightCpu(float p_assignedWeight)
					{
						m_weightAssignedCpu = p_assignedWeight;
					}

					bool FindUpdate(Update* p_currentUpdate)
					{
						if (std::find(clusteredPoints.begin(), clusteredPoints.end(), p_currentUpdate) != clusteredPoints.end())
						{
							return true;
						}
						
						return false;
					}

					bool operator == (const ClusteringPoint &p_currentPoint)
					{
						return ClusterCenter == p_currentPoint.ClusterCenter;
					}

					void SetDistance(ClusteringPoint* p_referencePoint)
					{
						m_distanceFromLocalPoint = std::abs((ClusterCenter - p_referencePoint->ClusterCenter).length());
					}

					void SetDistance(float p_distance)
					{
						m_distanceFromLocalPoint = p_distance;
					}

					static bool SortFunction(const ClusteringPoint* p_first, const ClusteringPoint* p_second)
					{
						return p_first->m_distanceFromLocalPoint < p_second->m_distanceFromLocalPoint;
					}

				};
			}
		}
	}
}