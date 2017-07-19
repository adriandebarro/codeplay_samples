#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Oct.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <vector>
#include <queue>
#include <mutex>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "..\BVH.h"
#include "..\..\Integrators\RadianceDetails\RadiancePoint.h"
#include "..\..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\..\Scene\Scene.h"
#include "..\..\CoreClasses\UtilClasses\Common.h"
#include "..\..\CoreClasses\BoundingShapes\BoundingBox.h"
#include "..\..\CoreClasses\Logger\Logger.h"
#include "Octree.h"

#include "../../Scene/Scene.h"

typedef boost::shared_mutex Lock;
typedef boost::unique_lock< Lock > WriteLock;
typedef boost::shared_lock< Lock > ReadLock;
typedef boost::upgrade_lock<boost::shared_mutex> upgradableLock;
typedef boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock;

namespace Engine
{

	typedef struct
	{
		int index;
		RadiancePoint* representation;
	}InsertionWrapper;


	///-------------------------------------------------------------------------------------------------
	/// <summary>	
	///    A multhithreaded octree implementation that stores in a centralised 
	///    data structure while a reference are stored in the octant
	///  </summary>
	///
	/// <remarks>	Adrian, 27/08/2015. </remarks>
	///-------------------------------------------------------------------------------------------------
	class LockingOctree
	{
	public:
		Lock m_myLock;
		std::mutex myMutex;
		
		
		LockingOctree(Engine::Scene* p_scene)
		{
			BVH * tempBvh = new BVH(p_scene->m_sceneObjects);
			m_rootNode = new OctNode(tempBvh->rootNode->boundingBox);
		}
	
		LockingOctree(BVH* p_sceneRepresentation)
		{
			m_rootNode = new OctNode(p_sceneRepresentation->rootNode->boundingBox);
		}

		LockingOctree()
		{
			m_rootNode = new OctNode(Point3D(0), Point3D(100), 0);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: InsertPoint
		//		Return Value: VOID
		//		  Parameters: RadiancePoint*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void InsertPoint(RadiancePoint p_toInst)
		{
			////----------------------------------------------------------Writer Lock ---------------------------------------------
			upgradableLock w_lock(m_myLock);
			uniqueLock uniqueLock(w_lock);
			m_allPoints.push_back(p_toInst);
			InsertPoint(m_allPoints.size() - 1, m_rootNode);
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetPoints
		//		Return Value: VOID
		//		  Parameters: RadiancePoint*, vector<RadiancePoints*>&
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool GetPoints(ShadeRec p_collisionDetails, Point3D p_normal, vector<int>& p_availablePoints, float p_alphaConstant)
		{
			GetRadiancePoints(p_collisionDetails, p_normal, p_availablePoints, p_alphaConstant);
			return p_availablePoints.size() > AMOUNT_OF_POINTS_FOR_ESTIMATE;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Invalidate octal tree. </summary>
		///
		/// <remarks>	Adrian, 03/02/2016. </remarks>
		///-------------------------------------------------------------------------------------------------

		void InvalidateOctTree()
		{
			InvalidateOctTree(m_rootNode);
		}

		RadiancePoint* GetPointByIndex(int p_index)
		{
			m_allPoints[p_index].m_usedCounter++;
			return &m_allPoints[p_index];
		}

		void IncrementAtIndex(int p_index)
		{
			m_allPoints[p_index].m_usedCounter++;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetRadiancePoints
		//		Return Value: VOID
		//		  Parameters: RadiancePoint*, vector<RadiancePoints*>&
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void GetRadiancePoints(ShadeRec p_sr, Point3D p_normalVector, vector<int>& p_availablePoints, float p_alphaConstant)
		{
			OctNode* currentNode = m_rootNode;
			bool foundChildNode = false;
			while (!foundChildNode)
			{
				if (currentNode->childNode)
				{
					//------------------------------------READER LOCK HERE ----------------------------
					ReadLock r_lock(m_myLock);
					if (!currentNode->IsChildNode())
						goto BACK_TO_SEARCH;
					for (int pointIndex = 0; pointIndex <currentNode->m_storedPoints.size(); pointIndex++)
					{
						RadiancePoint currentPoint = m_allPoints[currentNode->m_storedPoints[pointIndex]];
						
						double weight = currentPoint.GetWeighting(p_sr.localHitPoint, p_normalVector, p_alphaConstant);
						if (weight > 0)
						{
							p_availablePoints.push_back(currentNode->m_storedPoints[pointIndex]);
						}
						
					}

					foundChildNode = true;
				}
				else
				{
					BACK_TO_SEARCH:
					bool nodeFound = false;
					for (int index = 0; index < 8; index++)
					{
						OctNode* currentSubNode = currentNode->children[index];

						if (currentSubNode->m_boundingBos->CheckPointInsideBox(p_sr.localHitPoint))
						{
							currentNode = currentSubNode;
							nodeFound = true;
							break;
						}
					}

					if (!nodeFound)
						foundChildNode = true;
				}
			}
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CheckIfExist
		//		Return Value: VOID
		//		  Parameters: Point3D position
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool CheckIfExist(Point3D p_postion)
		{
			RadiancePoint* toFind;
			return GetRadiancePoint(p_postion, toFind);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetRadiancePoint
		//		Return Value: VOID
		//		  Parameters: Point3D position
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool GetRadiancePoint(Point3D p_position, RadiancePoint* p_outputRadiancPoint_out)
		{
			RadiancePoint* toFind = nullptr;
			OctNode * currentNode = m_rootNode;
			bool childNodeFound = false;
			bool pointFound = false;
			while (!childNodeFound)
			{
				if (currentNode->IsChildNode())
				{
					//------------------------------------READER LOCK HERE ----------------------------
					ReadLock r_lock(m_myLock);
					if (!currentNode->IsChildNode())
						goto BACK_TO_SEARCH;
					for (int index = 0; index < currentNode->m_storedPoints.size(); index++)
					{
						RadiancePoint currentRadiancePoint = m_allPoints[currentNode->m_storedPoints[index]];
						Point3D radiancePosition;
						
						radiancePosition = currentRadiancePoint.m_position;
						if (radiancePosition == p_position)
						{
							pointFound = true;
							p_outputRadiancPoint_out = &m_allPoints[currentNode->m_storedPoints[index]];
						}
						
					}
					childNodeFound = true;
				}
				else
				{
					BACK_TO_SEARCH:
					bool nextNode = false;

					for (int nodeIndex = 0; nodeIndex < 8; nodeIndex++)
					{
						if (currentNode->children[nodeIndex]->CheckIntersectionWithPoint(p_position))
						{
							currentNode = currentNode->children[nodeIndex];
							nextNode = true;
							break;
						}
					}

					if (!nextNode)
						childNodeFound = true;
				}
			}

			return pointFound;
		}

		int GetAmountRadiancePoint()
		{
			return GetAmountRadiancePoint(this->m_rootNode);
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	
		/// 	Gets the total amount of radiance points in the octree. 
		/// </summary>
		///
		/// <remarks>	Adrian, 19/07/2017. </remarks>
		///
		/// <param name="p_CurrentNode">	[in,out] If non-null, the current node. </param>
		///
		/// <returns>	The amount radiance point. </returns>
		///-------------------------------------------------------------------------------------------------
		int GetAmountRadiancePoint(OctNode* p_CurrentNode)
		{
			int total = 0;
			if (p_CurrentNode->IsChildNode())
			{
				for (int index = 0; index < p_CurrentNode->m_storedPoints.size(); index++)
				{
					RadiancePoint currentRadiancePoint = m_allPoints[p_CurrentNode->m_storedPoints[index]];
					if (currentRadiancePoint.GotFrom() == 1 && currentRadiancePoint.m_usedCounter > 0 && currentRadiancePoint.m_alreadyChecked == false)
					{
						//cout << "found one!" << endl;
						m_allPoints[p_CurrentNode->m_storedPoints[index]].SetChecked();
						total += 1;
					}
						
				}
			}
			else
			{
				for (int index = 0; index < 8; index++)
				{
					total += GetAmountRadiancePoint(p_CurrentNode->children[index]);
				}
			}

			return total;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Traversal of the octree</summary>
		///
		/// <remarks>	Adrian, 19/07/2017. </remarks>
		///
		/// <param name="p_position">	The position. </param>
		///
		/// <returns>	The radiance point. </returns>
		///-------------------------------------------------------------------------------------------------
		RadiancePoint GetRadiancePoint(Point3D p_position)
		{
			RadiancePoint toFind();
			OctNode* currentNode = m_rootNode;
			bool childNodeFound = false;
			
			while (!childNodeFound)
			{
				if (currentNode->IsChildNode())
				{
					//------------------------------------READER LOCK HERE ----------------------------
					ReadLock r_lock(m_myLock);
					if (!currentNode->IsChildNode())
						goto BACK_TO_SEARCH;
					for (int index = 0; index < currentNode->m_storedPoints.size(); index++)
					{
						RadiancePoint currentRadiancePoint = m_allPoints[currentNode->m_storedPoints[index]];
						if (p_position == currentRadiancePoint.m_position)
						{
							return m_allPoints[currentNode->m_storedPoints[index]];
						}
					}
					childNodeFound = true;
				}
				else
				{
				BACK_TO_SEARCH:
					bool nextNode = false;

					for (int nodeIndex = 0; nodeIndex < 8; nodeIndex++)
					{
						if (currentNode->children[nodeIndex]->CheckIntersectionWithPoint(p_position))
						{
							currentNode = currentNode->children[nodeIndex];
							nextNode = true;
							break;
						}
					}

					if (!nextNode)
						childNodeFound = true;
				}
			}
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	
		/// 	Gets readers lock object. 
		/// </summary>
		///
		/// <remarks>	Adrian, 19/07/2017. </remarks>
		///
		/// <returns>	The readers lock object. </returns>
		///-------------------------------------------------------------------------------------------------
		Lock& GetReadersLockObject()
		{
			return m_myLock;
		}

		vector<RadiancePoint>& GetRadiancePointsArray()
		{
			return m_allPoints;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: DESTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		~LockingOctree()
		{
			delete m_rootNode;
		}

	private:
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Invalidation of data contained in a specific octant </summary>
		///
		/// <remarks>	Adrian, 03/02/2016. </remarks>
		///
		/// <param name="p_currentNode">	[in,out] If non-null, the current node. </param>
		///-------------------------------------------------------------------------------------------------
		void InvalidateOctTree(OctNode* p_currentNode)
		{
			///----------------------------------------------------------Writer Lock ---------------------------------------------
			upgradableLock w_lock(m_myLock);
			uniqueLock uniqueLock(w_lock);
			for (int index = 0; index < m_allOctreeNodes.size(); index++)
			{
				OctNode* currentChildNode = m_allOctreeNodes[index];

				if (currentChildNode != nullptr && currentChildNode->childNode && currentChildNode->m_storedPoints.size() > 0)
					InvalidateOctTreeNode(currentChildNode);
			}

			p_currentNode->m_storedPoints.clear();
		}

		void InvalidateOctTreeNode(OctNode* p_currentNode)
		{
			if (p_currentNode->childNode)
			{
				p_currentNode->m_storedPoints.erase(p_currentNode->m_storedPoints.begin(), p_currentNode->m_storedPoints.end());
			}
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Insertion of a point in the octree structure </summary>
		///
		/// <remarks>	Adrian, 19/07/2017. </remarks>
		///
		/// <param name="p_toInstIndex">	Zero-based index of to instance. </param>
		/// <param name="p_currentNode">	[in,out] If non-null, the current node. </param>
		///-------------------------------------------------------------------------------------------------
		void InsertPoint(int p_toInstIndex, OctNode* p_currentNode)
		{
			if (p_currentNode->IsChildNode())
			{
				InsertPointInNode(p_toInstIndex, p_currentNode);
			}
			else
			{
				try
				{
					bool outsideBounderies = false;
					
					for (int childIndex = 0; childIndex < 8; childIndex++)
					{
						OctNode* currentChildNode = p_currentNode->children[childIndex];
						if (currentChildNode->CheckIntersectionWithPoint(m_allPoints[p_toInstIndex]))
						{
							outsideBounderies = true;
							InsertPoint(p_toInstIndex, currentChildNode);
						}
					}
					
				}
				catch (const std::system_error& e)
				{
					cout << "problem with " << e.what() << endl;
				}
			}

		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Inserts of a point in a specific octant</summary>
		///
		/// <remarks>	Adrian, 19/07/2017. </remarks>
		///
		/// <param name="p_pointIndex"> 	Zero-based index of the point. </param>
		/// <param name="p_currentNode">	[in,out] If non-null, the current node. </param>
		///-------------------------------------------------------------------------------------------------
		void InsertPointInNode(int p_pointIndex, OctNode* p_currentNode)
		{
			if ((p_currentNode->m_storedPoints.size() + 1 <= OCT_BUCKET_LIMIT && !p_currentNode->parentNode) || p_currentNode->level > OCT_MAXIMUM_LEVEL)
			{
				std::lock_guard<std::mutex> m(p_currentNode->m_mutex);
				p_currentNode->m_storedPoints.push_back(p_pointIndex);
			}
			else
			{
				std::lock_guard<std::mutex> m(p_currentNode->m_mutex);
				p_currentNode->SplitBox();
				Point3D previousPoint;
				//add the currently being added node 
				p_currentNode->m_storedPoints.push_back(p_pointIndex);
				for (int index = 0; index < p_currentNode->m_storedPoints.size(); index++)
				{
					
					//get currentStoredIndex
					int currentPointIndex = p_currentNode->m_storedPoints[index];
					RadiancePoint currentPoint = m_allPoints[currentPointIndex];

					for (int boudningBoxSize = 0; boudningBoxSize < 8; boudningBoxSize++)
					{
						OctNode* currentChild = p_currentNode->children[boudningBoxSize];

						if (currentChild->m_boundingBos->CheckIntersection(currentPoint.m_position, currentPoint.m_rayLength))
						{
							InsertPointInNode(currentPointIndex, currentChild);
						}
					}
				}

				for (int currentNode = 0; currentNode < 8; currentNode++)
				{
					m_allOctreeNodes.push_back(p_currentNode->children[currentNode]);
				}

				p_currentNode->m_storedPoints.clear();
				p_currentNode->childNode = false;
				p_currentNode->parentNode = true;
			}
		}

		OctNode *m_rootNode;	/// The root node
		int level; 
		vector<RadiancePoint> m_allPoints;  /// 
		vector<OctNode*> m_allOctreeNodes; /// all created octants are stored in this vector

	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------
