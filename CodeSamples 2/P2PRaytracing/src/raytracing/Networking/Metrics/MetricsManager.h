#pragma once 

#include <Peer/NeighbourManager/NeighboursManager.h>
#include <Peer/Neighbors/Neighbour.h>

#include "../PeerInfo/Update.h"
#include "../NetworkingUtil/CommunicationUtil.h"
#include "../../CoreClasses/BoundingShapes/BoundingBox.h"

#include "../ConreteNeighborManagers/StatePeer.h"
#include "../Contexts/StateRepresentation.h"
#include "../../CoreClasses/Logger/ClusteringUpdateLogger.h"


#include <vector>
#include <stdio.h>
#include <limits>
#include <hash_map>

using namespace Network::Peer;
using std::vector;
using namespace Engine::Networking;
using namespace Engine::Networking::Metrics::Locality;
using namespace Engine::Networking::Communication::Util;
using Engine::BoundingBox;
using Engine::Logs::ClusteringUpdateLogger;

namespace Engine
{
	namespace Networking
	{
		namespace Metrics
		{
			class MetricsManager
			{
			public:

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Default constructor. </summary>
				///
				/// <remarks>	Adrian, 21/12/2015. </remarks>
				///-------------------------------------------------------------------------------------------------

				MetricsManager()
				{
					m_LocalityMetric = new KMeansClustering();
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Constructor. </summary>
				///
				/// <remarks>	Adrian, 21/12/2015. </remarks>
				///
				/// <param name="p_neighborManager">	[in,out] If non-null, manager for neighbor. </param>
				///-------------------------------------------------------------------------------------------------

				MetricsManager()
				{
					m_clusterLogger = new ClusteringUpdateLogger();
					m_LocalityMetric = new KMeansClustering();
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Gets the updates. </summary>
				///
				/// <remarks>	Adrian, 21/12/2015. </remarks>
				///
				/// <param name="p_receivedUpdates">	The received updates. </param>
				///-------------------------------------------------------------------------------------------------

				void GetUpdates(vector<unsigned char> p_receivedUpdates)
				{
					//create updates here 
					int randomSeed = 0;
					int amountClusters = 5;
					float localityWeight = 0.0f;
					float bwWeight = 0.0f;
					float cpuWeight = 0.0f;

					vector<Update*> updates = TranslateUpdates(p_receivedUpdates, randomSeed, amountClusters, localityWeight, bwWeight, cpuWeight);
					//deal with reacived updates
					ApplyMetrics(updates, randomSeed, amountClusters, localityWeight, bwWeight, cpuWeight);
					//m_neighborManager->UpdateNeighbors(updates);
				}

				//    4 bytes  | 4 bytes |var |   20 bytes    | --- |   4 bytes   |   4 bytes   |   4 bytes   |   4 bytes   |   4 bytes   |
				//amount peers | size ip | ip | hardware info | --- | Random Seed | Am Clutsers | weight  Loc |  bw weight  | cpu  weight |

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Translate updates. </summary>
				///
				/// <remarks>	Adrian, 21/12/2015. </remarks>
				///
				/// <param name="p_ByteArray">	   	Array of bytes. </param>
				/// <param name="p_receivedSeed">  	[in,out] The received seed. </param>
				/// <param name="p_amountClusters">	[in,out] The amount clusters. </param>
				/// <param name="p_localityWeight">	[in,out] The locality weight. </param>
				/// <param name="p_bwWeight">	   	[in,out] The bw weight. </param>
				/// <param name="p_cpuWeight">	   	[in,out] The CPU weight. </param>
				///
				/// <returns>	null if it fails, else a vector&lt;Update*&gt; </returns>
				///-------------------------------------------------------------------------------------------------

				vector<Update*> TranslateUpdates(vector<unsigned char> p_ByteArray, int &p_receivedSeed, int &p_amountClusters, float &p_localityWeight, float &p_bwWeight, float &p_cpuWeight)
				{
					vector<Update*> toReturn;
					int index = 0;

					int amountPeers = UtilClass::GetNum<int>(p_ByteArray, index);
					
					for (int currentPeer = 0; currentPeer < amountPeers; currentPeer++)
					{
						Update* currentUpdate = new Update();
						
						string ipAddress = UtilClass::GetIpAddress(p_ByteArray, index);
						index += 4;

						float* location = new float[3];
						UtilClass::GetNumArray<float>(p_ByteArray, index, 3, location);
						float bandwidth = UtilClass::GetNum<float>(p_ByteArray, index);
						float cpuProcessor = UtilClass::GetNum<float>(p_ByteArray,index);
						
						currentUpdate->SetBandwidth(bandwidth);
						currentUpdate->SetLocation(location[0], location[1], location[2]);
						currentUpdate->SetIpAddress(ipAddress);
						currentUpdate->SetCpu(cpuProcessor);

						toReturn.push_back(currentUpdate);
					}

					p_receivedSeed = UtilClass::GetNum<int>(p_ByteArray, index);
					p_amountClusters = UtilClass::GetNum<int>(p_ByteArray, index);
					p_localityWeight = UtilClass::GetNum<float>(p_ByteArray, index);
					p_bwWeight = UtilClass::GetNum<float>(p_ByteArray, index);
					p_cpuWeight = UtilClass::GetNum<float>(p_ByteArray, index);

					return toReturn;
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Gets clustering points. </summary>
				///
				/// <remarks>	Adrian, 21/12/2015. </remarks>
				///
				/// <param name="p_allUpdates">	[in,out] If non-null, all updates. </param>
				/// <param name="p_seed">	   	The seed. </param>
				///
				/// <returns>	null if it fails, else the clustering points. </returns>
				///-------------------------------------------------------------------------------------------------

				vector<ClusteringPoint*> GetClusteringPoints(vector<Update*> p_allUpdates, int p_seed)
				{
					int amountClusters = 4;
					return GetClusteringPoints(p_allUpdates, p_seed, amountClusters);
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Gets clustering points. </summary>
				///
				/// <remarks>	Adrian, 21/12/2015. </remarks>
				///
				/// <param name="p_allUpdates">	   	[in,out] If non-null, all updates. </param>
				/// <param name="seed">			   	The seed. </param>
				/// <param name="p_amountClusters">	The amount clusters. </param>
				///
				/// <returns>	null if it fails, else the clustering points. </returns>
				///-------------------------------------------------------------------------------------------------

				vector<ClusteringPoint*> GetClusteringPoints(vector<Update*> p_allUpdates, int seed, int p_amountClusters)
				{
					
					vector<ClusteringPoint*> toReturn;
					
					#undef  max
					#undef min
					
					float min = std::numeric_limits<float>::max();
					float max = std::numeric_limits<float>::min();
					
					BoundingBox  limits;

					for (Update* currentUpdate : p_allUpdates)
					{
						limits.Expand(currentUpdate->GetLocation());
					}

					srand(seed);

					for (int index = 0; index < p_amountClusters; index++)
					{
						float randTempX = limits.GetMinPoint().x + (rand() % (int)(limits.GetMaxPoint().x - limits.GetMinPoint().x + 1));
						float randTempY = limits.GetMinPoint().y + (rand() % (int)(limits.GetMaxPoint().y - limits.GetMinPoint().y + 1));
						float randTempZ = limits.GetMinPoint().z + (rand() % (int)(limits.GetMaxPoint().z - limits.GetMinPoint().z + 1));

						Point3D currentPoint(randTempX, randTempY, randTempZ);

						toReturn.push_back(new ClusteringPoint(currentPoint));
					}

					return toReturn;
				}

				void ReWeighPeers()
				{
					std::vector<Engine::Networking::Neighbours::StatePeer*> allPeers;
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Applies the metrics. </summary>
				///
				/// <remarks>	Adrian, 21/12/2015. </remarks>
				///
				/// <TODO>  Apply weighting to the final outcome  </TODO>
				/// 
				/// <param name="p_allUpdates">	   	[in,out] [in,out] If non-null, all updates. </param>
				/// <param name="p_randomSeed">	   	The random seed. </param>
				/// <param name="p_amountClusters">	The amount clusters. </param>
				/// <param name="p_localityWeight">	The locality weight. </param>
				/// <param name="p_bwWeight">	   	The bw weight. </param>
				/// <param name="p_cpuWeight">	   	The CPU weight. </param>
				///-------------------------------------------------------------------------------------------------

				void ApplyMetrics(vector<Update*> &p_allUpdates, int p_randomSeed, int p_amountClusters, float p_localityWeight, float p_bwWeight, float p_cpuWeight)
				{

					if (p_allUpdates.size() > 0)
					{
						_ASSERT(p_randomSeed != 0 && p_allUpdates.size() > 0);
						vector<ClusteringPoint*> allClusters = GetClusteringPoints(p_allUpdates, p_randomSeed);
						if (m_neighborManager->GetLocalIp() != "0.0.0.0")
						{
							m_LocalityMetric->ProcessUpdates(allClusters, p_allUpdates, m_neighborManager->GetLocalIp());
							ProducePeerWeights(allClusters, p_allUpdates);
						}
					}
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Produce peer weights. </summary>
				///
				/// <remarks>	Adrian, 21/12/2015. </remarks>
				///
				/// <param name="p_allClusters">	[in,out] If non-null, all clusters. </param>
				/// <param name="p_allUpdates"> 	[in,out] If non-null, all updates. </param>
				///-------------------------------------------------------------------------------------------------

				void ProducePeerWeights(vector<ClusteringPoint*> p_allClusters, vector<Update*> p_allUpdates)
				{
					m_LocalityMetric->WeightUpdates(p_allClusters);
					//m_neighborManager->UpdateNeighbors(p_allUpdates);
					
					if (m_clusterLogger != nullptr)
						m_clusterLogger->LogClusters(p_allClusters);
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Gets test updates. </summary>
				///
				/// <remarks>	Adrian, 21/12/2015. </remarks>
				///
				/// <param name="p_clusterPoints">	[in,out] If non-null, the cluster points. </param>
				///
				/// <returns>	null if it fails, else the test updates. </returns>
				///-------------------------------------------------------------------------------------------------

				vector<Update*> GetTestUpdates(vector<ClusteringPoint*> p_clusterPoints)
				{
					vector<Update*> updatesToReturn;
					for (ClusteringPoint* currentCluser : p_clusterPoints)
					{
						updatesToReturn.push_back(currentCluser->clusteredPoints[0]);
					}
					return updatesToReturn;
				}

			private:
				NeighbourManager<Update>* m_neighborManager;
				Metric<ClusteringPoint>* m_LocalityMetric;
				int m_clusteringAmount;
				ClusteringUpdateLogger* m_clusterLogger;
				std::hash_map<string, Engine::Networking::Neighbours::StatePeer>* m_allPeers;

			};
		}
	}
}

