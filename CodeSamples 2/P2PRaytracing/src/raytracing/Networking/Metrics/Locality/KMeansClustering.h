#pragma once 

#include"../../../CoreClasses/Point/Point3D/Point3D.h"
#include "../../PeerInfo/Update.h"
#include "ClusteringPoint.h"
#include "../Metrics.h"


#include <vector>
#include <limits>
#include <math.h>
#include <algorithm>

#include <boost/math/distributions/exponential.hpp>

using Engine::Point3D;
using std::vector;
using Engine::Networking::Update;
using std::numeric_limits;
using Engine::Networking::Metrics::Locality::ClusteringPoint;
using namespace boost::math;
namespace Engine
{
	namespace Networking
	{
		namespace Metrics
		{
			namespace Locality
			{
				class KMeansClustering : public Metric<ClusteringPoint>
				{
				private:

					///-------------------------------------------------------------------------------------------------
					/// <summary>	Cluster locations. </summary>
					///
					/// <remarks>	Adrian, 08/12/2015. </remarks>
					///
					/// <param name="p_clusteringPoints">	The clustering points. </param>
					/// <param name="p_allLocations">	 	[in,out] If non-null, all locations. </param>
					/// <param name="p_localLocation">   	The local location. </param>
					///-------------------------------------------------------------------------------------------------

					void ClusterLocations(vector<ClusteringPoint*> &p_clusteringPoints, vector<SuperPeer*> p_allLocations, string p_localLocation)
					{
						_ASSERT(p_clusteringPoints.size() > 0 && p_allLocations.size() > 0);
						
						m_localCluster = nullptr;

						//get smallest and add to set 
						for (int index = 0; index < p_allLocations.size(); index++)
						{
							bool localRepresentation = p_allLocations[index]->GetIpAddress() == p_localLocation;

							//undefining max...Take care if using it from this point onwards
							#undef max
								float distance = numeric_limits<float>::max();
							Point3D currentPoint = p_allLocations[index]->GetLocation();
							ClusteringPoint* chosenCluster = nullptr;

							for (ClusteringPoint* currentCluster : p_clusteringPoints)
							{
								float currentDistance = std::sqrtf(std::pow(currentCluster->ClusterCenter.x - currentPoint.x, 2) + std::pow(currentCluster->ClusterCenter.z - currentPoint.z,2));

								if (currentDistance < distance)
								{
									distance = currentDistance;
									chosenCluster = currentCluster;
								}
							}

							if (localRepresentation)
								m_localCluster = chosenCluster;

							chosenCluster->clusteredPoints.push_back(p_allLocations[index]);
						}

						for (ClusteringPoint* currentPoint : p_clusteringPoints)
						{
							currentPoint->GenerateCenter();
						}
					}

					///-------------------------------------------------------------------------------------------------
					/// <summary>	Process the results described by p_. </summary>
					///
					/// <remarks>	Adrian, 08/12/2015. </remarks>
					///
					/// <param name="p_">	The p. </param>
					///
					/// <returns>	A vector&lt;Update&gt; </returns>
					///-------------------------------------------------------------------------------------------------

					vector<Update> ProcessResults(vector<Update*> p_clusteringPoints,vector<Update*> p_locations)
					{

					}

					ClusteringPoint* GetLocalCluster()
					{
						_ASSERT(m_localCluster != nullptr);
						return m_localCluster;
					}

					ClusteringPoint* m_localCluster = nullptr;

				public:

					float GetWeight(float p_intervalAssigned)
					{
						auto const lambda = 1.0;
						auto d = boost::math::exponential_distribution<>{lambda};
						return static_cast<float>(boost::math::pdf(d, p_intervalAssigned));
					}
					

					///-------------------------------------------------------------------------------------------------
					/// <summary>	Process the updates. </summary>
					///
					/// <remarks>	Adrian, 12/12/2015. </remarks>
					///
					/// <param name="p_clusterPoints">   	[in,out] If non-null, the cluster points. </param>
					/// <param name="p_updatesToProcess">	[in,out] [in,out] If non-null, the updates to process. </param>
					/// <param name="p_localLocation">   	The local location. </param>
					///-------------------------------------------------------------------------------------------------
					using Metric<ClusteringPoint>::ProcessUpdates;
					virtual void ProcessUpdates(vector<ClusteringPoint*> &p_clusterPoints, vector<Update*> &p_updatesToProcess, string p_localLocation)
					{
						ClusterLocations(p_clusterPoints, p_updatesToProcess, p_localLocation);
					}

					///-------------------------------------------------------------------------------------------------
					/// <summary>	Weight updates. </summary>
					///
					/// <remarks>	Adrian, 12/12/2015. </remarks>
					///
					/// <param name="p_clusteringPoints">	[in,out] If non-null, the clustering points. </param>
					/// <param name="p_updatesToProcess">	[in,out] [in,out] If non-null, the updates to process. </param>
					///-------------------------------------------------------------------------------------------------
					using Metric<ClusteringPoint>::WeightUpdates;
					virtual void WeightUpdates(vector<ClusteringPoint*> &p_clusteringPoints)
					{
						_ASSERT(p_clusteringPoints.size() > 0);
						float deltaInterval = 2.9 / (p_clusteringPoints.size() );

						ClusteringPoint* localClusteringPoint = m_localCluster;

						for (ClusteringPoint* currentPoint : p_clusteringPoints)
						{
							if (currentPoint == localClusteringPoint)
							{
								localClusteringPoint->SetDistance(0.0f);
								continue;
							}

							currentPoint->SetDistance(localClusteringPoint);
						}

						std::sort(p_clusteringPoints.begin(), p_clusteringPoints.end(), ClusteringPoint::SortFunction);
						
						vector<float> weights;
						float currentInterval = 0.1f;
						float totalWeight = 0.0f;
						
						while (currentInterval < 3.0f)
						{
							float currentWeight = GetWeight(currentInterval);
							weights.push_back(currentWeight);
							currentInterval += deltaInterval;
						}
						
						assert(weights.size() == p_clusteringPoints.size());


						//for (int index = 0; index < p_clusteringPoints.size(); index++)
						//{
						//	//get all the points here and normalize all the weights 
						//	totalWeight += p_clusteringPoints[index]->clusteredPoints.size() * weights[index];
						//}


						for (int clusterIndex = 0; clusterIndex < p_clusteringPoints.size(); clusterIndex ++)
						{
							ClusteringPoint* currentCluster = p_clusteringPoints[clusterIndex];

							//currentCluster->SetWeight(weights[clusterIndex]/totalWeight);
							currentCluster->SetLocalityWeight(weights[clusterIndex]);
							//currentCluster->SetWeight(0.02);
							currentCluster->UpdateWeightForChildren();
							currentInterval += deltaInterval;
						}
					}
				};
			}
		}
	}
}