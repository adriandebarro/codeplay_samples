#pragma once 

#include <string>
#include <thread>
#include <vector>
#include <iostream>
#include <atomic>
#include <chrono>
#include <vector>

#include "../../CoreClasses/BoundingShapes/BoundingBox.h"
#include "../../Threads/NonLockingList.h"
#include "../../CoreClasses/UtilClasses/Common.h"

using namespace Engine::Threads::NonLocking;
using namespace Engine::Common;
using std::vector;
namespace Engine
{
	namespace AccelerationStructures
	{
		namespace OctTree
		{
			///-------------------------------------------------------------------------------------------------
			/// <summary>	An octal node. </summary>
			///
			/// <remarks>	Adrian, 19/07/2017. </remarks>
			///
			/// <typeparam name="T">				  	Generic type parameter. </typeparam>
			/// <typeparam name="depth_limit">		  	Type of the depth limit. </typeparam>
			/// <typeparam name="min_spacing_samples">	Type of the minimum spacing samples. </typeparam>
			///-------------------------------------------------------------------------------------------------

			template<typename T, size_t depth_limit, size_t min_spacing_samples = 0.5f>
			struct OctNode
			{
				std::atomic<OctNode<T, depth_limit>*>* m_childrenNodes;
				BoundingBox* m_boundingBox;
				NonLockingList<int, -1>* m_nodesContained;
				std::atomic<int> nodeCount = ATOMIC_VAR_INIT(0);
				int m_nodeLevel;
				bool m_utilized = true;
				
				//---------------------------------------------------------------------
				// Constructors
				//---------------------------------------------------------------------
				OctNode()
				{
					m_childrenNodes = new std::atomic<OctNode<T, depth_limit>>[8];
					InitNode();
				}

				OctNode(const OctNode <T, depth_limit>& p_copyableObject)
				{
					
				}

				OctNode(Point3D p_min, Point3D p_max, int p_level)
				{
					m_childrenNodes = new std::atomic<OctNode<T, depth_limit>*>[8];
					m_boundingBox = new BoundingBox(p_min, p_max);
					m_nodeLevel = p_level;
					InitNode();
					m_utilized = true;
				}
				//---------------------------------------------------------------------

				//---------------------------------------------------------------------
				// SetNodeInfo: Set he bounding box information
				//---------------------------------------------------------------------
				void SetNodeInfo(Point3D p_min, Point3D p_max, int p_level)
				{
					if(m_boundingBox == nullptr)
					{
						m_boundingBox = new BoundingBox(p_min, p_max);
						m_nodeLevel = p_level;
					}
				}
				//---------------------------------------------------------------------

				//---------------------------------------------------------------------
				// InsertPoint: Recursive insertion
				//---------------------------------------------------------------------
				void InsertPoint(T* p_currentPoint, int p_index)
				{
					//if base case, store in current node
					if(m_boundingBox->GetLengthOfCubeSide() <= p_currentPoint->m_rayLength || m_nodeLevel == depth_limit)
					{
						std::atomic_fetch_add(&nodeCount, 1);
						m_nodesContained->InsertValue(new int(p_index));
					}
					else
					{
						//check with 
						bool intersectedNodes[8]{false};
						bool createdNodes[8]{ false };
						//split box and store in a temp array
						vector<OctNode < T, depth_limit>*> tempArray;
						SplitBox(p_currentPoint, tempArray, intersectedNodes, createdNodes);

						int vectorIndex = 0;
						for (int childNodeIndex = 0; childNodeIndex < 8; childNodeIndex ++)
						{
							if(intersectedNodes[childNodeIndex])
							{
								//if the child of the current node is still equalt nullptr
								OctNode<T, depth_limit>* tempPointer = nullptr;
								bool swapProcedure = false;

								if (createdNodes[childNodeIndex])
									swapProcedure = std::atomic_compare_exchange_strong(&m_childrenNodes[childNodeIndex], &tempPointer, tempArray[vectorIndex]);
								
								if (!swapProcedure && createdNodes[childNodeIndex])
								{
									OctNode<T, depth_limit>* tempPtr = tempArray[vectorIndex];
									//tempArray.erase(tempArray.begin() + vectorIndex);
									tempArray[vectorIndex] = nullptr;
									delete tempPtr;
									//tempPtr = nullptr;
									//cout << "--------------------------CAS failed--------------------------" << endl;
								}
								
								OctNode<T, depth_limit>* childd = std::atomic_load(&m_childrenNodes[childNodeIndex]);
								//OctNode<T, depth_limit>* childd = m_childrenNodes[childNodeIndex];
								childd->InsertPoint(p_currentPoint, p_index);
								if (createdNodes[childNodeIndex])
									vectorIndex++;
							}
						}
					}
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	
				/// 	Filtering of insertions as not to insert beyond a minimum 
				/// 	distance from all the already stored samples in the current location 
				/// </summary>
				///
				/// <remarks>	Adrian, 19/07/2017. </remarks>
				///
				/// <param name="p_allSamples">  	[in,out] If non-null, all samples. </param>
				/// <param name="p_currentPoint">	[in,out] If non-null, the current point. </param>
				/// <param name="p_index">		 	The index. </param>
				///-------------------------------------------------------------------------------------------------
				void PoissonInsertion(NonLockingList<RadiancePoint, NULL>* p_allSamples, T* p_currentPoint, int p_index)
				{
					//if base case, store in current node
					if (m_boundingBox->GetLengthOfCubeSide() <= p_currentPoint->m_rayLength || m_nodeLevel == depth_limit)
					{
						bool validInsertion = true;
						
						//counter to loop through all the stored points in the current node.
						int sampleLocalIndex = 0;
						int amountStoredsamples = std::atomic_load(&nodeCount);
						//while the current array iteration is not equal to defaqult value
						int sampleIndex = 0;
						int* tempPtrValue = nullptr;

						while (sampleLocalIndex  <  amountStoredsamples)
						{
							RadiancePoint* currentRadiancePoint = nullptr;
							bool pointFound = false;
							int* sampleIndexPtr = m_nodesContained->GetAtIndex(sampleLocalIndex, pointFound);
							if (pointFound)
							{
								int sampleIndex = *sampleIndexPtr;
								currentRadiancePoint = p_allSamples->GetAtIndex(sampleIndex);
								float seperationDistance = RadiancePoint::DistanceBetweenTwoSamples(p_currentPoint, currentRadiancePoint);
								if (seperationDistance < min_spacing_samples)
								{
									validInsertion = false;
									break;
								}
									
							}
							else
								break;

							sampleLocalIndex++;
						}

						if (validInsertion)
						{
							std::atomic_fetch_add(&nodeCount, 1);
							m_nodesContained->InsertValue(new int(p_index));
						}
					}
					else
					{
						//check with 
						bool intersectedNodes[8]{ false };
						bool createdNodes[8]{ false };
						//split box and store in a temp array
						vector<OctNode < T, depth_limit>*> tempArray;
						SplitBox(p_currentPoint, tempArray, intersectedNodes, createdNodes);

						int vectorIndex = 0;
						for (int childNodeIndex = 0; childNodeIndex < 8; childNodeIndex++)
						{
							if (intersectedNodes[childNodeIndex])
							{
								//if the child of the current node is still equalt nullptr
								OctNode<T, depth_limit>* tempPointer = nullptr;
								bool swapProcedure = false;

								if (createdNodes[childNodeIndex])
									swapProcedure = std::atomic_compare_exchange_strong(&m_childrenNodes[childNodeIndex], &tempPointer, tempArray[vectorIndex]);

								if (!swapProcedure && createdNodes[childNodeIndex])
								{
									OctNode<T, depth_limit>* tempPtr = tempArray[vectorIndex];
									//tempArray.erase(tempArray.begin() + vectorIndex);
									tempArray[vectorIndex] = nullptr;
									delete tempPtr;
									//tempPtr = nullptr;
									//cout << "--------------------------CAS failed--------------------------" << endl;
								}

								OctNode<T, depth_limit>* childd = std::atomic_load(&m_childrenNodes[childNodeIndex]);
								//OctNode<T, depth_limit>* childd = m_childrenNodes[childNodeIndex];
								childd->PoissonInsertion(p_allSamples,p_currentPoint, p_index);
								if (createdNodes[childNodeIndex])
									vectorIndex++;
							}
						}
					}
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary> 
				/// 	Checks for an intersection between a
				///		the node's bounding box and the current
				///		sample 
				///	</summary>
				///
				/// <remarks>	Adrian, 19/07/2017. </remarks>
				///
				/// <param name="radiancePoint">	The radiance point. </param>
				///
				/// <returns>	true if it succeeds, false if it fails. </returns>
				///-------------------------------------------------------------------------------------------------

				bool CheckIntersectionWithPoint(const T* radiancePoint)
				{
					return (this->m_boundingBox->CheckIntersection(radiancePoint->m_position, radiancePoint->m_rayLength));
				}
				//---------------------------------------------------------------------

				///-------------------------------------------------------------------------------------------------
				///<summary>	
				///		Splits the current bounding box for new children
				///		if the currentsample can be inserted in the current child
				///		node and only if a child node has not been created 
				///</summary>
				///
				/// <remarks>	Adrian, 19/07/2017. </remarks>
				///
				/// <param name="p_currentSample">	  	The current sample. </param>
				/// <param name="p_tempArray">		  	[in,out] [in,out] If non-null, array of temporaries. </param>
				/// <param name="p_intersectedNodes)">	[in,out] The intersected nodes) </param>
				/// <param name="p_createdNodes)">	  	[in,out] The created nodes) </param>
				///-------------------------------------------------------------------------------------------------
				void SplitBox(const T* p_currentSample, std::vector<OctNode<T, depth_limit>*>& p_tempArray, bool (&p_intersectedNodes)[8], bool(&p_createdNodes)[8])
				{
					//cout << " Box Being Split" << endl;
					Point3D centoridPoint = m_boundingBox->GetCentroid();
					Point3D tempMinPoint = Point3D(0), tempMaxPoint = Point3D(0);

					Point3D minPoint = m_boundingBox->GetMinPoint();
					Point3D maxPoint = m_boundingBox->GetMaxPoint();
				
					//min - centroid 
					tempMinPoint = minPoint;
					tempMaxPoint = centoridPoint;
					OctNode<T, depth_limit>* nullPtrValue = nullptr;

					p_intersectedNodes[0] = BoundingBox::CheckIntersection(tempMinPoint, tempMaxPoint,p_currentSample->m_position, p_currentSample->m_rayLength);
					if (std::atomic_load(&m_childrenNodes[0]) == nullPtrValue && p_intersectedNodes[0])
					{	
						p_createdNodes[0] = true;
						p_tempArray.push_back(new OctNode<T, depth_limit>(tempMinPoint, tempMaxPoint, m_nodeLevel + 1));
					}

					//xmin ymin zc-- xc yc zmax
					tempMinPoint = Point3D(minPoint.x, minPoint.y, centoridPoint.z);
					tempMaxPoint = Point3D(centoridPoint.x, centoridPoint.y, maxPoint.z);
					p_intersectedNodes[1] = BoundingBox::CheckIntersection(tempMinPoint, tempMaxPoint, p_currentSample->m_position, p_currentSample->m_rayLength);
					if (std::atomic_load(&m_childrenNodes[1]) == nullPtrValue && p_intersectedNodes[1])
					{
						p_createdNodes[1] = true;
						p_tempArray.push_back(new OctNode<T, depth_limit>(tempMinPoint, tempMaxPoint, m_nodeLevel + 1));
					}

					//xc ymin zc  xmax yc zmax
					tempMinPoint = Point3D(centoridPoint.x, minPoint.y, centoridPoint.z);
					tempMaxPoint = Point3D(maxPoint.x, centoridPoint.y, maxPoint.z);
					p_intersectedNodes[2] = BoundingBox::CheckIntersection(tempMinPoint, tempMaxPoint, p_currentSample->m_position, p_currentSample->m_rayLength);
					if(std::atomic_load(&m_childrenNodes[2]) == nullPtrValue && p_intersectedNodes[2])
					{
						p_createdNodes[2] = true;
						p_tempArray.push_back(new OctNode<T, depth_limit>(tempMinPoint, tempMaxPoint, m_nodeLevel + 1));
					}
					
					//xc ymin zmin - xmax yc zc
					tempMinPoint = Point3D(centoridPoint.x, minPoint.y, minPoint.z);
					tempMaxPoint = Point3D(maxPoint.x, centoridPoint.y, centoridPoint.z);
					p_intersectedNodes[3] = BoundingBox::CheckIntersection(tempMinPoint, tempMaxPoint, p_currentSample->m_position, p_currentSample->m_rayLength);
					if (std::atomic_load(&m_childrenNodes[3]) == nullPtrValue && p_intersectedNodes[3])
					{
						p_createdNodes[3] = true;
						p_tempArray.push_back(new OctNode<T, depth_limit>(tempMinPoint, tempMaxPoint, m_nodeLevel+1));
					}

					//xc yc zmin -- xmax ymax zc
					tempMinPoint = Point3D(centoridPoint.x, centoridPoint.y, minPoint.z);
					tempMaxPoint = Point3D(maxPoint.x, maxPoint.y, centoridPoint.z);
					p_intersectedNodes[4] = BoundingBox::CheckIntersection(tempMinPoint, tempMaxPoint, p_currentSample->m_position, p_currentSample->m_rayLength);
					if (std::atomic_load(&m_childrenNodes[4]) == nullPtrValue && p_intersectedNodes[4])
					{
						p_createdNodes[4] = true;
						p_tempArray.push_back(new OctNode<T, depth_limit>(tempMinPoint, tempMaxPoint, m_nodeLevel + 1));
					}

					//xmin yc zmin -- xc ymax zc
					tempMinPoint = Point3D(minPoint.x, centoridPoint.y, minPoint.z);
					tempMaxPoint = Point3D(centoridPoint.x, maxPoint.y, centoridPoint.z);
					p_intersectedNodes[5] = BoundingBox::CheckIntersection(tempMinPoint, tempMaxPoint, p_currentSample->m_position, p_currentSample->m_rayLength);
					if (std::atomic_load(&m_childrenNodes[5]) == nullPtrValue && p_intersectedNodes[5])
					{
						p_createdNodes[5] = true;
						p_tempArray.push_back(new OctNode<T, depth_limit>(tempMinPoint, tempMaxPoint, m_nodeLevel + 1));
					}

					//xmin yc zc -- xc ymax zmax
					tempMinPoint = Point3D(minPoint.x, centoridPoint.y, centoridPoint.z);
					tempMaxPoint = Point3D(centoridPoint.x, maxPoint.y, maxPoint.z);
					p_intersectedNodes[6] = BoundingBox::CheckIntersection(tempMinPoint, tempMaxPoint, p_currentSample->m_position, p_currentSample->m_rayLength);
					if (std::atomic_load(&m_childrenNodes[6]) == nullPtrValue && p_intersectedNodes[6])
					{
						p_createdNodes[6] = true;
						p_tempArray.push_back(new OctNode<T, depth_limit>(tempMinPoint, tempMaxPoint, m_nodeLevel + 1));
					}

					//centroid - Max
					p_intersectedNodes[7] = BoundingBox::CheckIntersection(centoridPoint, maxPoint, p_currentSample->m_position, p_currentSample->m_rayLength);
					if (std::atomic_load(&m_childrenNodes[7]) == nullPtrValue && p_intersectedNodes[7] )
					{
						p_createdNodes[7] = true;
						p_tempArray.push_back(new OctNode<T, depth_limit>(centoridPoint, maxPoint, m_nodeLevel + 1));
					}
					
				}
				//---------------------------------------------------------------------

				///-------------------------------------------------------------------------------------------------
				/// <summary>	
				/// 	Initialisation of an octant. 
				/// </summary>
				///
				/// <remarks>	Adrian, 19/07/2017. </remarks>
				///-------------------------------------------------------------------------------------------------
				void InitNode()
				{
					m_nodesContained = new NonLockingList<int, -1>();
					for (int index = 0; index < 8; index++)
					{
						OctNode<T, depth_limit>* currentPointer = std::atomic_load(&m_childrenNodes[index]);
						//OctNode<T, node_amount, depth_limit> currentObject = &m_childrenNodes[index];
						//TODO solve this issue
						m_childrenNodes[index] = ATOMIC_VAR_INIT(nullptr);
					}
				}
				//---------------------------------------------------------------------


				void InvalidateNode()
				{
					std::atomic_store(&nodeCount, 0);
					m_nodesContained->InvalidateNode();
				}
				//---------------------------------------------------------------------


				//---------------------------------------------------------------------
				// InitNode: Initializes the class lists
				//---------------------------------------------------------------------
				void SetOutdated()
				{
					m_utilized = false;
				}
				//---------------------------------------------------------------------
			};
		}
	}
}