#pragma once

#include <string>
#include <atomic>

#include "./PdfNeighborManager.h"
#include "./StatePeer.h"
#include "../ConcreteContexts/ContextStrategy.h"
#include "../../CoreClasses/Point/Point3D/Point3D.h"
#include "../Contexts/TriangleRepresentation.h"

#include <boost/math/distributions/exponential.hpp>

#include "../../CoreClasses/BoundingShapes/BoundingBoxAnalyzer.h"
#include "../../CoreClasses/Camera/Camera.h"

using namespace Engine::Networking::Neighbors;
using namespace Engine::Networking::Contexts;
using namespace Engine::Networking::Contexts::ViewPointContext;
using namespace Engine::BoundingShapes;

namespace Engine
{
	namespace Networking
	{
		namespace Neighbors
		{
			namespace Metrics
			{
				template<typename T>
				struct TempRefVariable
				{
					T amount;
					int index;
				};

				class WeightingMetrics
				{
				private:
					PdfNeighborManager<Neighbours::StatePeer>* m_neighbourManager;

					clock_t* m_startingTime;
					Camera* m_camera;
					float m_viewPointDistance;
					BoundingBoxAnalyzer* m_boundingBoxAnalyzer;

					std::atomic<int>* m_latency;
					float* m_bandwidth;

				public:
					WeightingMetrics(PdfNeighborManager<Neighbours::StatePeer>* p_neighbourManager, BoundingBoxAnalyzer* p_boudningBoxAnalyzer, Camera* p_camera, std::atomic<int>* p_latency)
						: m_neighbourManager(p_neighbourManager),
						  m_camera(p_camera)
					{
						m_boundingBoxAnalyzer = p_boudningBoxAnalyzer;
						m_latency = p_latency;
					}


					void UpdateWeighting(int p_contextType)
					{
						switch ((CONTEXT_ENUM)p_contextType)
						{
							case CONTEXT_ENUM::locality:
								UpdateWeighting(m_camera->GetCameraPosition(), p_contextType);
								break;

							case CONTEXT_ENUM::viewpoint:
								UpdateWeighting(m_camera->GetCameraDirection(), m_camera->GetCameraPosition(), m_camera->m_fov, m_viewPointDistance, p_contextType);
								break;

							case CONTEXT_ENUM::bounds:
								UpdateWeighting(m_boundingBoxAnalyzer->CheckContainingBoundingBox(m_camera->GetCameraPosition()), p_contextType);
								break;

							case CONTEXT_ENUM::frameRate:
								UpdateWeighting(0, p_contextType);
								break;

							case CONTEXT_ENUM::latency:
								UpdateWeighting(0,p_contextType);
								break;

							default:
								break;
						}
					}

					void UpdateWeighting(int p_updateType, int p_contextType)
					{
						std::lock_guard<std::mutex> lck(m_neighbourManager->m_mtx);
						vector<Neighbours::StatePeer*> allStoredPeers;
						m_neighbourManager->GetAllNeighbours(allStoredPeers);
						vector<TempRefVariable<float>> refVector;
						//_ASSERT(refVector.size() > 0);
						
						if ((CONTEXT_ENUM)p_contextType == CONTEXT_ENUM::latency)
						{
							//assign the distances between the two points 
							for (int neighborIndex = 0; neighborIndex < allStoredPeers.size(); neighborIndex++)
							{
								TempRefVariable<float> tempRef;
								tempRef.index = neighborIndex;

								if (allStoredPeers[neighborIndex]->m_peerState.m_latency < 0)
								{
									tempRef.amount = numeric_limits<float>::max();
								}
								else
								{
									tempRef.amount = allStoredPeers[neighborIndex]->m_peerState.m_latency;
								}
								refVector.push_back(tempRef);
							}
							//sort them 
							std::sort(refVector.begin(), refVector.end(), LatencySortFunction);

							//calculating the interval length
							float interval = (3.0f - 0.1f) / refVector.size();
							//starting position in tohe exponential graph 
							float exponentialGrtaphPosition = 0.1f;

							//assign the weights
							for (int refIndex = 0; refIndex < refVector.size(); refIndex++)
							{
								allStoredPeers[refVector[refIndex].index]->m_peerState.SetWeighting(GetWeight(exponentialGrtaphPosition), p_contextType);
								exponentialGrtaphPosition += interval;
							}
						}
						else if ((CONTEXT_ENUM)p_contextType == CONTEXT_ENUM::frameRate)
						{
							//assign the distances between the two points 
							for (int neighborIndex = 0; neighborIndex < allStoredPeers.size(); neighborIndex++)
							{
								TempRefVariable<float> tempRef;
								tempRef.index = neighborIndex;

								#undef min

								if (allStoredPeers[neighborIndex]->m_peerState.m_frameRate < 0)
									tempRef.amount = (numeric_limits<float>::min());
								else
								{
									tempRef.amount = allStoredPeers[neighborIndex]->m_peerState.m_frameRate;
								}

								refVector.push_back(tempRef);

							}
							//sort them 
							std::sort(refVector.begin(), refVector.end(), FrameRateSortFunction);

							//calculating the interval length
							float interval = (3.0f - 0.1f) / refVector.size();
							//starting position in tohe exponential graph 
							float exponentialGrtaphPosition = 0.1f;

							//assign the weights
							for (int refIndex = 0; refIndex < refVector.size(); refIndex++)
							{
								allStoredPeers[refVector[refIndex].index]->m_peerState.SetWeighting(GetWeight(exponentialGrtaphPosition), p_contextType);
								exponentialGrtaphPosition += interval;
							}
						}
						else if ((CONTEXT_ENUM)p_contextType == CONTEXT_ENUM::bounds)
						{

							//assign the distances between the two points 
							for (int neighborIndex = 0; neighborIndex < allStoredPeers.size(); neighborIndex++)
							{
								TempRefVariable<float> tempRef;
								tempRef.index = neighborIndex;
								if (allStoredPeers[neighborIndex]->m_peerState.m_boundsPosition < 0)
									tempRef.amount = numeric_limits<float>::max();
								else
									tempRef.amount = m_boundingBoxAnalyzer->GetdistanceBetweenBoundBoxes(p_updateType, allStoredPeers[neighborIndex]->m_peerState.m_boundsPosition);
								refVector.push_back(tempRef);
							}
							//sort them 
							std::sort(refVector.begin(), refVector.end(), BoundSortFunction);

							//calculating the interval length
							float interval = (3.0f - 0.1f) / refVector.size();
							//starting position in tohe exponential graph 
							float exponentialGrtaphPosition = 0.1f;

							//assign the weights
							for (int refIndex = 0; refIndex < refVector.size(); refIndex++)
							{
								allStoredPeers[refVector[refIndex].index]->m_peerState.SetWeighting(GetWeight(exponentialGrtaphPosition), p_contextType);
								exponentialGrtaphPosition += interval;
							}
						}
					}

					//-----------------------------------------------------------------------------
					// UpdateWeighting for position related criteria
					//-----------------------------------------------------------------------------
					void UpdateWeighting(Point3D p_currentPosition, int p_contextType)
					{
						std::lock_guard<std::mutex> lck(m_neighbourManager->m_mtx);
						vector<Neighbours::StatePeer*> allStoredPeers;
						m_neighbourManager->GetAllNeighbours(allStoredPeers);
						vector<TempRefVariable<float>> refVector;
						//_ASSERT(refVector.size() > 0);
						if ((CONTEXT_ENUM)p_contextType == CONTEXT_ENUM::locality)
						{
							//assign the distances between the two points 
							for (int neighborIndex = 0; neighborIndex < allStoredPeers.size(); neighborIndex++)
							{
								TempRefVariable<float> tempRef;
								tempRef.index = neighborIndex;
								
								if (allStoredPeers[neighborIndex]->m_peerState.m_peerPosition == 0)
								{
									tempRef.amount = numeric_limits<float>::max();
								}
								else
								{
									tempRef.amount = std::abs(Point3D::DistanceBetweenPoints(p_currentPosition, allStoredPeers[neighborIndex]->m_peerState.m_peerPosition));
								}
								refVector.push_back(tempRef);
							}
							//sort them 
							std::sort(refVector.begin(), refVector.end(), PositionSortFunction);

							//calculating the interval length
							float interval = (3.0f - 0.1f) / refVector.size();
							//starting position in tohe exponential graph 
							float exponentialGrtaphPosition = 0.1f;
							
							//assign the weights
							for (int refIndex = 0; refIndex < refVector.size(); refIndex++)
							{
								allStoredPeers[refVector[refIndex].index]->m_peerState.SetWeighting(GetWeight(exponentialGrtaphPosition), p_contextType);
								exponentialGrtaphPosition += interval;
							}
						}
						
					}

					void UpdateWeighting(Point3D p_currentPosition, Point3D p_direction, float p_fov, float p_distance ,int p_contextType)
					{
						std::lock_guard<std::mutex> lck(m_neighbourManager->m_mtx);
						vector<Neighbours::StatePeer*> allStoredPeers;
						m_neighbourManager->GetAllNeighbours(allStoredPeers);
						vector<TempRefVariable<float>> refVector;
						//_ASSERT(refVector.size() > 0);
						if ((CONTEXT_ENUM)p_contextType == CONTEXT_ENUM::viewpoint)
						{
							//add the local view triangle
							TriangleRep localViewPoint(p_currentPosition, p_direction, p_distance, p_fov);
							//assign the distances between the two points 
							for (int neighborIndex = 0; neighborIndex < allStoredPeers.size(); neighborIndex++)
							{
								Neighbours::StatePeer* currentPeer = allStoredPeers[neighborIndex];
								TempRefVariable<float> tempRef;
								tempRef.index = neighborIndex;
								
								if (currentPeer->m_peerState.m_peerPosition == 0 || currentPeer->m_peerState.m_viewingDirection == 0)
								{
									#undef min
									tempRef.amount = numeric_limits<float>::min();
								}
								else
								{
									//build triangel representation for the other peer; 
									TriangleRep otherViewPoint(currentPeer->m_peerState.m_peerPosition, currentPeer->m_peerState.m_viewingDirection, p_distance, p_fov);
									ViewPointMetric tempComparer(localViewPoint, otherViewPoint);
									tempComparer.TestCollision(tempRef.amount);
								}

								refVector.push_back(tempRef);
							}
							//sort them ascending ordser
							std::sort(refVector.begin(), refVector.end(), ViewPointSortFunction);

							//calculating the interval length
							float interval = (3.0f - 0.1f) / refVector.size();
							//starting position in tohe exponential graph 
							float exponentialGrtaphPosition = 0.1f;

							//assign the weights
							for (int refIndex = 0; refIndex < refVector.size(); refIndex++)
							{
								allStoredPeers[refVector[refIndex].index]->m_peerState.SetWeighting(GetWeight(exponentialGrtaphPosition), p_contextType);
								exponentialGrtaphPosition += interval;
							}
						}
						//expand further here 
					}


					//-----------------------------------------------------------------------------
					// UpdateWeighting for position related criteria
					//-----------------------------------------------------------------------------
					//void UpdateWeighting(Point3D p_currentPosition, float p_fov ,int p_contextType)
					//{
					//	std::lock_guard<std::mutex> lck(m_neighbourManager->mtx);
					//	vector<Neighbours::StatePeer*> allStoredPeers;
					//	m_neighbourManager->GetAllNeighbours(allStoredPeers);
					//	vector<TempRefVariable<float>> refVector;
					//	_ASSERT(refVector.size() > 0);
					//	if ((CONTEXT_ENUM)p_contextType == CONTEXT_ENUM::viewpoint)
					//	{
					//		//assign the distances between the two points 
					//		for (int neighborIndex = 0; neighborIndex < allStoredPeers.size(); neighborIndex++)
					//		{
					//			TempRefVariable<float> tempRef;
					//			tempRef.index = neighborIndex;
					//			if (allStoredPeers[neighborIndex]->m_peerState.m_peerPosition == 0 || )
					//			{
					//				
					//			}
					//			else
					//			{
					//				tempRef.amount = std::abs(Point3D::DistanceBetweenPoints(p_currentPosition, allStoredPeers[neighborIndex]->m_peerState.m_peerPosition));
					//			}
					//			
					//			refVector.push_back(tempRef);
					//		}
					//		//sort them 
					//		std::sort(refVector.begin(), refVector.end(), PositionSortFunction);

					//		//calculating the interval length
					//		float interval = (3.0f - 0.1f) / refVector.size();
					//		//starting position in tohe exponential graph 
					//		float exponentialGrtaphPosition = 0.1f;

					//		//assign the weights
					//		for (int refIndex = 0; refIndex < refVector.size(); refIndex++)
					//		{
					//			allStoredPeers[refVector[refIndex].index]->m_peerState.SetWeighting(GetWeight(exponentialGrtaphPosition), p_contextType);
					//			exponentialGrtaphPosition += interval;
					//		}
					//	}
					//	//expand further here 
					//}

					
					//-----------------------------------------------------------------------------
					// Locality sort function
					//-----------------------------------------------------------------------------
					static bool PositionSortFunction(const TempRefVariable<float>& p_lhs, const TempRefVariable<float>& p_rhs)
					{
						return p_lhs.amount < p_rhs.amount;
					}
					
					//-----------------------------------------------------------------------------
					// Bandwidth sort function
					//-----------------------------------------------------------------------------
					static bool LatencySortFunction(const TempRefVariable<float>& p_lhs, const TempRefVariable<float>& p_rhs)
					{
						return p_lhs.amount < p_rhs.amount;
					}

					//-----------------------------------------------------------------------------
					// CPU power sort function
					//-----------------------------------------------------------------------------
					static bool CPUPowerSortFunction(const TempRefVariable<float>& p_lhs, const TempRefVariable<float>& p_rhs)
					{
						return p_lhs.amount > p_rhs.amount;
					}

					//-----------------------------------------------------------------------------
					// ViewPoint sort function
					//-----------------------------------------------------------------------------
					static bool ViewPointSortFunction(const TempRefVariable<float>& p_lhs, const TempRefVariable<float>& p_rhs)
					{
						return p_lhs.amount > p_rhs.amount;
					}

					//-----------------------------------------------------------------------------
					// ViewPoint sort function
					//-----------------------------------------------------------------------------
					static bool FrameRateSortFunction(const TempRefVariable<float>& p_lhs, const TempRefVariable<float>& p_rhs)
					{
						return p_lhs.amount > p_rhs.amount;
					}

					//-----------------------------------------------------------------------------
					// Boundingboxes sort function
					//-----------------------------------------------------------------------------
					static bool BoundSortFunction(const TempRefVariable<float>& p_lhs, const TempRefVariable<float>& p_rhs)
					{
						return p_lhs.amount < p_rhs.amount;
					}

					//-----------------------------------------------------------------------------
					// GetWeight: gets the y-axis value for a given interval given x-axis
					//			  on an exponential function
					//-----------------------------------------------------------------------------
					float GetWeight(float p_intervalAssigned)
					{
						auto const lambda = 1.0;
						auto d = boost::math::exponential_distribution<>{ lambda };
						return static_cast<float>(boost::math::pdf(d, p_intervalAssigned));
					}


					bool GetViewPointIntersection(const TriangleRep& p_lhs, const TriangleRep& p_rhs, float& p_summation)
					{
						ViewPointMetric current(p_lhs, p_rhs);
						return current.TestCollision(p_summation);
					}
				};
			}
		}
	}
}