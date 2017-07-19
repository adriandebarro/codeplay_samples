#pragma once 

#include "./Octree.h"
#include "./SlimOctNode.h"
#include "../../Threads/NonLockingList.h"
#include "../../Scene/Scene.h"
#include "../AccelerationStructure.h"

#include <atomic>

using namespace Engine::AccelerationStructures::OctTree;
using namespace Engine::Threads::NonLocking;
namespace Engine
{
	namespace AccelerationStructures
	{
		namespace OctTree
		{
			
			const int SAMPLES_PER_NODE = 50;
			const int DEPTH_LIMIT_TREE = 14;

			///-------------------------------------------------------------------------------------------------
			/// <summary>
			///    A wait free implementation of the oct-tree.
			///   Makes use of wait free linked list for the storage of data
			/// </summary>
			///
			/// <remarks>	Adrian, 19/07/2017. </remarks>
			///
			/// <typeparam name="T">			  	Generic type parameter. </typeparam>
			/// <typeparam name="depth_limit">	  	Type of the depth limit. </typeparam>
			/// <typeparam name="main_array_size">	Type of the main array size. </typeparam>
			///-------------------------------------------------------------------------------------------------
			template<typename T, size_t depth_limit, size_t main_array_size>
			class WaitFreeOctTree
			{
			public:
				typedef OctNode<RadiancePoint, DEPTH_LIMIT_TREE> Node;

				NonLockingList<RadiancePoint, NULL>* m_storedPoints;
				std::atomic<int> nodeCount = ATOMIC_VAR_INIT(0);
				Node* m_rootNode;

				//-----------------------------------------------------------------------------------------------------------
				//	Constructors
				//-----------------------------------------------------------------------------------------------------------
				WaitFreeOctTree()
				{
					m_storedPoints = new NonLockingList<RadiancePoint, NULL>();
					m_rootNode = new Node();
				}

				WaitFreeOctTree(AccelerationStructure* p_accelrationStructure)
				{
					BoundingBox* tempBox = p_accelrationStructure->m_boundingBox;
					m_rootNode = new Node(tempBox->minPoint, tempBox->maxPoint, 0);
					m_storedPoints = new NonLockingList<RadiancePoint, NULL>();
				}

				WaitFreeOctTree(Point3D p_min, Point3D p_max)
				{
					m_rootNode = new Node(p_min, p_max, 0);
					m_storedPoints = new NonLockingList<RadiancePoint, NULL>();
				}
				//-----------------------------------------------------------------------------------------------------------

				//-----------------------------------------------------------------------------------------------------------
				// InsertPoint:
				//-----------------------------------------------------------------------------------------------------------
				void InsertPoint(T p_currentSample)
				{
					T* sample = new T(p_currentSample);
					std::atomic_fetch_add(&nodeCount, 1);
					int index = m_storedPoints->InsertValue(sample);
					m_rootNode->InsertPoint(sample, index);
				}
				//-----------------------------------------------------------------------------------------------------------

				//-----------------------------------------------------------------------------------------------------------
				// PoissonInsertion:
				//-----------------------------------------------------------------------------------------------------------
				void PoissonInsertion(T* p_currentSample)
				{
					std::atomic_fetch_add(&nodeCount, 1);
					int index = m_storedPoints->InsertValue(p_currentSample);
					m_rootNode->PoissonInsertion(m_storedPoints, p_currentSample, index);
				}
				//-----------------------------------------------------------------------------------------------------------

				//-----------------------------------------------------------------------------------------------------------
				//  InsertPoint:
				//-----------------------------------------------------------------------------------------------------------
				void InsertPoint(T* p_currentSample)
				{
					int index = m_storedPoints->InsertValue(p_currentSample);
					m_rootNode->InsertPoint(p_currentSample, index);
					std::atomic_fetch_add(&nodeCount, 1);
				}
				//-----------------------------------------------------------------------------------------------------------

				//-----------------------------------------------------------------------------------------------------------
				//	IncrementArIndex: Increment used counter at the smaple index
				//-----------------------------------------------------------------------------------------------------------
				RadiancePoint* GetPointByIndex(int p_index)
				{
					if (p_index < std::atomic_load(&nodeCount))
					{
						int localIndex = p_index % ELEMENTS_PER_LINKED_LIST_NODE;
						int blockNumber = p_index / ELEMENTS_PER_LINKED_LIST_NODE;

						int currentBlockIndex = 0;
						return m_storedPoints->GetAtIndex(p_index);
					}
					else
						return nullptr;
				}
				//-----------------------------------------------------------------------------------------------------------

				//-----------------------------------------------------------------------------------------------------------
				//	IncrementArIndex: Increment used counter at the smaple index
				//-----------------------------------------------------------------------------------------------------------
				void IncrementAtIndex(int p_index)
				{
					RadiancePoint* currentPoint = m_storedPoints->GetAtIndex(p_index);
					currentPoint->m_usedCounter++;
				}
				//-----------------------------------------------------------------------------------------------------------

				//-----------------------------------------------------------------------------------------------------------
				//	GetRadiancePoints:
				//-----------------------------------------------------------------------------------------------------------
				bool GetPoints(ShadeRec p_collisionDetails, Point3D p_normal, vector<int>& p_availablePoints, float p_alphaConstant)
				{
					vector<int> foundPoint;
					GetRadiancePoints(p_collisionDetails, p_normal, p_availablePoints, p_alphaConstant, m_rootNode);

					/*bool failed = p_availablePoints.size() > 0;
					if (!failed)
						cout << "interpolation failed "  << endl;
					return failed;*/
					return p_availablePoints.size() > AMOUNT_OF_POINTS_FOR_ESTIMATE;
				}

				//-----------------------------------------------------------------------------------------------------------
				//	GetRadiancePoints: Gets all stored samples that have a weight greater than 0
				//-----------------------------------------------------------------------------------------------------------
				void GetRadiancePoints(ShadeRec p_sr, Point3D p_normalVector, vector<int>& p_availablePoints, float p_alphaConstant, Node* p_currentNode)
				{
					//counter to loop through all the stored points in the current node.
					int sampleLocalIndex = 0;
					int amountStoredsamples = std::atomic_load(&p_currentNode->nodeCount);
					//while the current array iteration is not equal to defaqult value
					int sampleIndex = 0;
					int* tempPtrValue = nullptr;
					
					while(sampleLocalIndex  <  amountStoredsamples)
					{
						RadiancePoint* currentRadiancePoint = nullptr;
						bool pointFound = false;
						int* sampleIndex = p_currentNode->m_nodesContained->GetAtIndex(sampleLocalIndex, pointFound);
						if (pointFound && sampleIndex != nullptr)
							currentRadiancePoint = m_storedPoints->GetAtIndex(*sampleIndex);
						else
							break;

						double weight = currentRadiancePoint->GetWeighting(p_sr.localHitPoint, p_normalVector, p_alphaConstant);
						if (weight > 0)
						{
							p_availablePoints.push_back(*sampleIndex);
						}

						sampleLocalIndex++;
					}

					if (p_currentNode->m_nodeLevel < depth_limit)
					{
						//loop and test for intersection with the child nodes.
						for (int childIndex = 0; childIndex < 8; childIndex++)
						{
							Node* currentChildNode = std::atomic_load(&p_currentNode->m_childrenNodes[childIndex]);
							Node* nullPtrValue = nullptr;
							if (currentChildNode != nullPtrValue && currentChildNode->m_boundingBox->CheckPointInsideBox(p_sr.localHitPoint))
							{
								GetRadiancePoints(p_sr, p_normalVector, p_availablePoints, p_alphaConstant, currentChildNode);
								break;
							}

						}
					}

				}
				//-----------------------------------------------------------------------------------------------------------

				void GetRadiancePointsArray(vector<RadiancePoint>& p_samples)
				{
					int amountSamples = std::atomic_load(&nodeCount);

					for (int index = 0; index < amountSamples; index++)
					{
						p_samples.push_back(*GetPointByIndex(index));
					}
				}


				virtual void InvalidateOctTree()
				{
					InvalidateOctreeStructure(m_rootNode);
				}


				void InvalidateOctreeStructure(Node* p_currentNode)
				{
					std::cout << "invalidated node " << p_currentNode->m_nodeLevel << p_currentNode->m_boundingBox->ToString() << std::endl;
					p_currentNode->InvalidateNode();

					for (int childIndex = 0; childIndex < 8; childIndex++)
					{
						Node* currentChildNode = std::atomic_load(&p_currentNode->m_childrenNodes[childIndex]);
						Node* nullPtrValue = nullptr;
						if (currentChildNode != nullPtrValue)
						{
							InvalidateOctreeStructure(currentChildNode);
						}
					}
				}


				virtual void PartiallyInvalidateOctree(BoundingBox* p_invalidationBounds)
				{
					if (m_rootNode->m_boundingBox->Overlaps(*p_invalidationBounds))
					{
						PartialOctreeInvalidation(p_invalidationBounds, m_rootNode);
					}
				}

				void PartialOctreeInvalidation(BoundingBox* p_invalidationBounds, Node* p_currentNode)
				{

					std::cout << "invalidated node " << p_currentNode->m_nodeLevel << p_currentNode->m_boundingBox->ToString()<< std::endl;
					p_currentNode->InvalidateNode();
					
					for (int childIndex = 0; childIndex < 8; childIndex++)
					{
						Node* currentChildNode = std::atomic_load(&p_currentNode->m_childrenNodes[childIndex]);
						Node* nullPtrValue = nullptr;
						if (currentChildNode != nullPtrValue &&  currentChildNode->m_boundingBox->Overlaps(*(p_invalidationBounds)))
						{
							
							PartialOctreeInvalidation(p_invalidationBounds, currentChildNode);
						}
					}
				}

				int GetAmountRadiancePoint()
				{
					return GetAmountRadiancePoint(m_rootNode);
				}

				int GetAmountRadiancePoint(Node* p_CurrentNode)
				{
					int total = 0;

					int amountStoredPoints = m_storedPoints->GetTotalSize();
					for (int index = 0; index < amountStoredPoints; index++)
					{
						RadiancePoint* currentRadiancePoint = m_storedPoints->GetAtIndex(index);
						if (currentRadiancePoint->GotFrom() == 1 && currentRadiancePoint->m_usedCounter > 0 && currentRadiancePoint->m_alreadyChecked == false)
						{
							currentRadiancePoint->SetChecked();
							total += 1;
						}
					}

					return total;
				}
			};
		}
	}
}