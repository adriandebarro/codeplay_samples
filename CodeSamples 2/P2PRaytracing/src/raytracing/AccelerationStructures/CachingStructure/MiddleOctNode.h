
#include "./SlimOctNode.h"
#include "./LeafNode.h"

#include <atomic>

#include "./SlimOctNode.h"


namespace Engine
{
	namespace AccelerationStructures
	{
		namespace OctTree
		{
			template<typename T, size_t node_amount, size_t depth_limit>
			struct MiddleOctNode : OctNode<T, node_amount, depth_limit>
			{
				typedef OctNode<T, node_amount, depth_limit> ParentOctNode;

				using OctNode<T, node_amount, depth_limit>::m_nodeLevel;
				using OctNode<T, node_amount, depth_limit>::nodeCount;
				using OctNode<T, node_amount, depth_limit>::m_boundingBox;
				using OctNode<T, node_amount, depth_limit>::m_leafNode;


				int m_storedPoints[depth_limit];
				std::atomic<OctNode<T, node_amount, depth_limit>*> m_childrenNodes = ATOMIC_VAR_INIT(NULL);

				MiddleOctNode(Point3D p_min, Point3D p_max, int p_level) : OctNode(p_min, p_max, p_level)
				{}

				using OctNode<T, node_amount, depth_limit>::InsertPoint;
				virtual void InsertPoint(T& p_currentPoint, int p_index)
				{
					int previousIndex = std::atomic_fetch_add(&(nodeCount), 1);

					if (previousIndex < node_amount)
					{
						m_storedPoints[previousIndex] = p_index;
					}
					else
					{
						OctNode<T, node_amount, depth_limit>* tempArray;

						if (m_nodeLevel < depth_limit)
						{
							//tempArray = new MiddleOctNode<T, node_amount, depth_limit>[8];
							SplitBox(0, tempArray);
						}
						else
						{
							//tempArray = new LeafNode<T, node_amount, depth_limit>[8];
							SplitBox(1, tempArray);
						}

						vector<int> samples;
						//agglomorate all the other samples
						samples.insert(samples.begin(), m_storedPoints, m_storedPoints + node_amount);
						samples.push_back(p_index);

						//loop through all the samples and add them to the respective children 
						for (int currentSampleIndex = 0; currentSampleIndex < node_amount + 1; currentSampleIndex++)
						{
							for (int index = 0; index < 8; index++)
							{
								OctNode<T, node_amount, depth_limit>* currentNode = tempArray[index];
								if (currentNode->CheckIntersectionWithPoint(p_currentPoint))
								{
									currentNode->InsertPoint(p_currentPoint, p_index);
								}
							}
						}


						//check that it is till null
						bool result = m_childrenNodes.atomic_compare_exchange_strong(NULL, tempArray);

						//if did not manage retract
						if (!result)
							delete[] tempArray;
						else
							std::atomic_compare_exchange_strong(&m_leafNode, 1, 0);

						//check node insertion in the children 

					}
				}

				using OctNode<T, node_amount, depth_limit>::SplitBox;
				virtual void SplitBox(int p_type, OctNode<T, node_amount, depth_limit>* p_tempArray)
				{
					//cout << " Box Being Split" << endl;
					Point3D centoridPoint = m_boundingBox->GetCentroid();
					Point3D tempMinPoint = Point3D(0), tempMaxPoint = Point3D(0);

					Point3D minPoint = m_boundingBox->GetMinPoint();
					Point3D maxPoint = m_boundingBox->GetMaxPoint();
					if (p_type == 0)
					{
						//min - centroid 
						tempMinPoint = minPoint;
						tempMaxPoint = centoridPoint;
						p_tempArray[0] = new MiddleOctNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);

						//xmin ymin zc-- xc yc zmax
						tempMinPoint = Point3D(minPoint.x, minPoint.y, centoridPoint.z);
						tempMaxPoint = Point3D(centoridPoint.x, centoridPoint.y, maxPoint.z);

						p_tempArray[1] = new MiddleOctNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);

						//xc ymin zc  xmax yc zmax
						tempMinPoint = Point3D(centoridPoint.x, minPoint.y, centoridPoint.z);
						tempMaxPoint = Point3D(maxPoint.x, centoridPoint.y, maxPoint.z);

						p_tempArray[2] = new MiddleOctNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);

						//xc ymin zmin - xmax yc zc
						tempMinPoint = Point3D(centoridPoint.x, minPoint.y, minPoint.z);
						tempMaxPoint = Point3D(maxPoint.x, centoridPoint.y, centoridPoint.z);

						p_tempArray[3] = new MiddleOctNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);

						//xc yc zmin -- xmax ymax zc
						tempMinPoint = Point3D(centoridPoint.x, centoridPoint.y, minPoint.z);
						tempMaxPoint = Point3D(maxPoint.x, maxPoint.y, centoridPoint.z);

						p_tempArray[4] = new MiddleOctNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);

						//xmin yc zmin -- xc ymax zc
						tempMinPoint = Point3D(minPoint.x, centoridPoint.y, minPoint.z);
						tempMaxPoint = Point3D(centoridPoint.x, maxPoint.y, centoridPoint.z);

						p_tempArray[5] = new MiddleOctNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);


						//xmin yc zc -- xc ymax zmax
						tempMinPoint = Point3D(minPoint.x, centoridPoint.y, centoridPoint.z);
						tempMaxPoint = Point3D(centoridPoint.x, maxPoint.y, maxPoint.z);

						p_tempArray[6] = new MiddleOctNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);

						//centroid - Max
						p_tempArray[7] = new MiddleOctNode<T, node_amount, depth_limit>(centoridPoint, maxPoint, ++m_nodeLevel);
					}
					else
					{
						//min - centroid 
						tempMinPoint = minPoint;
						tempMaxPoint = centoridPoint;
						m_childrenNodes[0] = new LeafNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);

						//xmin ymin zc-- xc yc zmax
						tempMinPoint = Point3D(minPoint.x, minPoint.y, centoridPoint.z);
						tempMaxPoint = Point3D(centoridPoint.x, centoridPoint.y, maxPoint.z);

						m_childrenNodes[1] = new LeafNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);

						//xc ymin zc  xmax yc zmax
						tempMinPoint = Point3D(centoridPoint.x, minPoint.y, centoridPoint.z);
						tempMaxPoint = Point3D(maxPoint.x, centoridPoint.y, maxPoint.z);

						m_childrenNodes[2] = new LeafNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);

						//xc ymin zmin - xmax yc zc
						tempMinPoint = Point3D(centoridPoint.x, minPoint.y, minPoint.z);
						tempMaxPoint = Point3D(maxPoint.x, centoridPoint.y, centoridPoint.z);

						m_childrenNodes[3] = new LeafNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);

						//xc yc zmin -- xmax ymax zc
						tempMinPoint = Point3D(centoridPoint.x, centoridPoint.y, minPoint.z);
						tempMaxPoint = Point3D(maxPoint.x, maxPoint.y, centoridPoint.z);

						m_childrenNodes[4] = new LeafNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);

						//xmin yc zmin -- xc ymax zc
						tempMinPoint = Point3D(minPoint.x, centoridPoint.y, minPoint.z);
						tempMaxPoint = Point3D(centoridPoint.x, maxPoint.y, centoridPoint.z);

						m_childrenNodes[5] = new LeafNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);


						//xmin yc zc -- xc ymax zmax
						tempMinPoint = Point3D(minPoint.x, centoridPoint.y, centoridPoint.z);
						tempMaxPoint = Point3D(centoridPoint.x, maxPoint.y, maxPoint.z);

						m_childrenNodes[6] = new LeafNode<T, node_amount, depth_limit>(tempMinPoint, tempMaxPoint, ++m_nodeLevel);

						//centroid - Max
						m_childrenNodes[7] = new LeafNode<T, node_amount, depth_limit>(centoridPoint, maxPoint, ++m_nodeLevel);
					}
				}
			};
		}
	}
}