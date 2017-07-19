#pragma once 

#include <numeric>

#include "AccelerationStructure.h"
#include "../CoreClasses/UtilClasses/Common.h"

using std::cout;
using std::endl;


using namespace Engine::Common::GeomerticPrimitives;

//FIXHERE places that still need to be fixxed
//DIRTY places that need to be optimized 

namespace Engine
{
	namespace AccelerationStructures
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	A KD-Tree implementation ported from Illumina (Keith Bugeja). </summary>
		///
		/// <remarks>	Adrian, 07/07/2017. </remarks>
		///
		/// <typeparam name="T">	Generic type parameter. </typeparam>
		////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename T>
		struct Accelerator_KDT : public AccelerationStructure
		{

			// KDT Node 
			struct ALIGN_8 Node
			{
				union {
					struct {
						unsigned int Leaf : 1;
						unsigned int Axis : 2;
						unsigned int FirstChildNodeId : 29;
					};

					struct {
						unsigned int _Leaf : 1;
						unsigned int _Axis : 2;
						unsigned int TriangleListId : 29;
					};
				};

				float Partition;

				bool IsLeaf(void) { return Leaf != 0; }
			};

			static const int MaxTrisPerLeaf = 15;
			static const int MaxDepth = 30;
			static const int MaxBins = 33;

			const Real CostTraverse = 2.0;
			const Real CostIntersect = 1.0;

			std::vector<Node> m_nodeList;
			std::vector<std::vector<int>> m_nodeTriangleList;

			Accelerator_KDT(vector<GeometricObjects*> p_allPrimitives)
			{
				_ASSERT(p_allPrimitives.size() > 0);
				m_allPrimitives = p_allPrimitives;
				m_boundingBox = new BoundingBox();
				BuildTree(PartitioningStyle::SAH);
			}

			virtual void BuildTree(PartitioningStyle p_partitioningStyle)
			{
				m_nodeList.clear();
				m_nodeList.push_back(Node());

				// Generate triangle index list
				std::vector<int> triangleIndexList(m_allPrimitives.size());
				std::iota(triangleIndexList.begin(), triangleIndexList.end(), 0);

				// Subdivide
				Axis bestAxis = Axis::Y; 
				Real bestPartition;
				//FindBestSplit(triangleIndexList, Axis::None, bestAxis, bestPartition);
				Subdivide(0, triangleIndexList, bestAxis, 0);
				// Subdivide(0, triangleIndexList, Axis::None, 0);

				// Compute scene bounds
				ComputeBounds(triangleIndexList, m_boundingBox);

				OptimiseTree();
			}

			void OptimiseTree(void)
			{
			}

			void Subdivide(const int p_nodeIndex, const std::vector<int> &p_root, const Axis p_axis, const int p_depth)
			{
				// Choose partitioning plane
				Real partition;

				//if (p_depth < MaxDepth && FindBestSplit(p_root, p_axis, axis, partition)) 
				if (p_depth < MaxDepth && PartitionSweep(p_root, p_axis, partition))
				{
					std::vector<int> leftList, rightList;
					//Distribute(p_root, axis, partition, leftList, rightList);
					Distribute(p_root, p_axis, partition, leftList, rightList);

					m_nodeList[p_nodeIndex].Leaf = 0;

					// Axis
					//m_nodeList[p_nodeIndex].Axis = (unsigned int)axis;
					m_nodeList[p_nodeIndex].Axis = (unsigned int)p_axis;

					// Partition
					m_nodeList[p_nodeIndex].Partition = partition;

					// Push children
					m_nodeList.push_back(Node());
					m_nodeList.push_back(Node());

					int rightNode = (int)(m_nodeList.size() - 1),
						leftNode = m_nodeList[p_nodeIndex].FirstChildNodeId = rightNode - 1;

					// Subdivide node
					//Subdivide(leftNode, leftList, axis, p_depth + 1);
					//Subdivide(rightNode, rightList, axis, p_depth + 1);
					Subdivide(leftNode, leftList, (Axis)((p_axis + 1) % 3), p_depth + 1);
					Subdivide(rightNode, rightList, (Axis)((p_axis + 1) % 3), p_depth + 1);
				}
				else
				{
					//LoggerManager::Debug() << "Pushing [" << p_root.size() << "] triangles in node" << Logger::endl;

					m_nodeTriangleList.push_back(p_root);

					m_nodeList[p_nodeIndex].TriangleListId = (unsigned int)(m_nodeTriangleList.size() - 1);
					m_nodeList[p_nodeIndex].Leaf = 1;
				}
			}

			bool FindBestSplit(const std::vector<int> &p_root, const Axis p_excludeAxis, Axis &p_axis_out, Real &p_partition_out)
			{
				if (p_root.size() < MaxTrisPerLeaf)
					return false;

				// Compute node bounds
				//fixhere
				BoundingBox* aabb = new BoundingBox();
				ComputeBounds(p_root, aabb);

				Point3D binWidth = (aabb->GetExtent() + Point3D((Real)1e-5)) / (Real)MaxBins;

				// Compute SAH costs
				Axis bestAxis = Axis::None;

				Real boundsLo, boundsHi, thisCost,
					bestCost = CostIntersect * p_root.size();

				int bestEvent = -1;

				for (Axis axis : {Axis::X, Axis::Y, Axis::Z})
				{
					if (axis == p_excludeAxis)
						continue;

					int binsLo[MaxBins] = { 0 },
						binsHi[MaxBins] = { 0 },
						suffix[MaxBins] = { 0 },
						prefix[MaxBins] = { 0 };

					// Compute binning
					for (auto index : p_root)
					{
						T::GetBounds((T*)m_allPrimitives[index], axis, boundsLo, boundsHi);

						int leftBin = (int)((boundsLo - aabb->minPoint.At[axis]) / binWidth.At[axis]),
							rightBin = (int)((boundsHi - aabb->minPoint.At[axis]) / binWidth.At[axis]);

						binsLo[leftBin]++;
						binsHi[rightBin]++;
					}

					int prefixBin,
						suffixSum = 0,
						prefixSum = 0;

					// Compute prefix/suffix sums
					for (int index = 0; index < MaxBins; ++index)
					{
						prefixBin = MaxBins - (index + 1);

						prefixSum += binsLo[prefixBin];
						suffixSum += binsHi[index];

						prefix[prefixBin] = prefixSum;
						suffix[index] = suffixSum;
					}

					// Find best split point for axis
					for (int index = 0; index < (MaxBins - 1); ++index)
					{
						thisCost = ComputeSplitCost((Real)index, suffix[index], (Real)(MaxBins - index), prefix[index + 1], (Real)MaxBins);

						//LoggerManager::Debug() << "Axis : [" << axis << "], Cost : [" << thisCost << "]" << Logger::endl;

						if (thisCost < bestCost)
						{
							bestCost = thisCost;
							bestEvent = index;
							bestAxis = axis;
						}
					}
				}

				//LoggerManager::Debug() << "Best cost : [" << bestCost << "]" << Logger::endl;

				if (bestEvent == -1)
					return false;

				//LoggerManager::Debug() << "Split accepted!" << Logger::endl;

				p_partition_out = aabb->minPoint.At[bestAxis] + bestEvent * binWidth.At[bestAxis];
				p_axis_out = bestAxis;

				return true;
			}

			bool PartitionSweep(const std::vector<int> &p_root, const Axis &p_axis, Real &p_partition_out)
			{
				//return PartitionSweep_Median(p_root, p_axis, p_partition_out);
				return PartitionSweep_BinnedSAH(p_root, p_axis, p_partition_out);
			}

			bool PartitionSweep_BinnedSAH(const std::vector<int> &p_root, const Axis &p_axis, Real &p_partition_out)
			{
				if (p_root.size() < MaxTrisPerLeaf)
					return false;

				int binsLo[MaxBins] = { 0 },
					binsHi[MaxBins] = { 0 },
					suffix[MaxBins] = { 0 },
					prefix[MaxBins] = { 0 };

				//FIXHERE
				BoundingBox* aabb = new BoundingBox();
				ComputeBounds(p_root, aabb);

				Real binWidth = (Real)((1e-3 + aabb->GetExtent(p_axis)) / MaxBins),
					boundsLo, boundsHi;

				// Compute binning
				for (auto index : p_root)
				{
					T::GetBounds((T*)m_allPrimitives[index], p_axis, boundsLo, boundsHi);

					int leftBin = (int)((boundsLo - aabb->minPoint.At[p_axis]) / binWidth),
						rightBin = (int)((boundsHi - aabb->minPoint.At[p_axis]) / binWidth);

					binsLo[leftBin]++;
					binsHi[rightBin]++;
				}

				int prefixBin,
					suffixSum = 0,
					prefixSum = 0;

				// Compute prefix/suffix sums
				for (int index = 0; index < MaxBins; ++index)
				{
					prefixBin = MaxBins - (index + 1);

					prefixSum += binsLo[prefixBin];
					suffixSum += binsHi[index];

					prefix[prefixBin] = prefixSum;
					suffix[index] = suffixSum;
				}

				// Compute SAH costs
				const Real Taabb = 1.0, Ttri = 1.5;
				Real bestCost = Ttri * p_root.size(), thisCost;
				int bestEvent = -1;

				for (int index = 0; index < (MaxBins - 1); ++index)
				{
					thisCost = ComputeSplitCost((Real)index, suffix[index], (Real)(MaxBins - index), prefix[index + 1], (Real)MaxBins);

					if (thisCost < bestCost)
					{
						bestCost = thisCost;
						bestEvent = index;
					}
				}

				if (bestEvent == -1)
					return false;

				p_partition_out = aabb->minPoint.At[p_axis] + bestEvent * binWidth;

				return true;
			}

			void Distribute(const std::vector<int> &p_root, const Axis p_axis, const float p_partition, std::vector<int> & p_leftList_out, std::vector<int> & p_rightList_out)
			{
				Real min = 0, max = 0;

				for (auto index : p_root)
				{
					T::GetBounds((T*)m_allPrimitives[index], p_axis, min, max);

					if (p_partition >= min) p_leftList_out.push_back(index);
					if (p_partition <= max) p_rightList_out.push_back(index);
				}
			}

			inline Real ComputeSplitCost(Real p_leftArea, int p_leftTriangles, Real p_rightArea, int p_rightTriangles, Real p_totalArea)
			{
				return (Real)(((p_leftTriangles == 0 || p_rightTriangles == 0) ? 0.8 : 1.0) *
					CostTraverse + CostIntersect * (p_leftTriangles * p_leftArea + p_rightTriangles * p_rightArea) / p_totalArea);
			}

			void ComputeBounds(const std::vector<int> & p_triangleIndexList, BoundingBox *p_aabb_out)
			{
				for (auto idx : p_triangleIndexList)
				{
					 T* tri = (T*)m_allPrimitives[idx];
					tri->ExpandBoundingBox(p_aabb_out);
				}
			}

			struct StackNode {
				int node;
				float min;
				float max;
			};

			// Traversal
			bool Traverse(int p_node, Ray &p_ray, Intersection &p_intersection_out, bool p_shadowTest = false)
			{
				StackNode stackNodes[64];

				/*Real tmin = p_ray.GetTmin(),
					tmax = p_ray.GetTmax();*/

				Real tmin = -500000,
					tmax = 500000;

				bool intersectionFound = false;

				int currentNode = p_node;
				int stackPosition = 0;

				for (;;)
				{
					if (p_ray.GetTmin() < tmin)
						break;

					if (m_nodeList[currentNode].IsLeaf())
					{
						Intersection intersection;

						#if defined(SURFACE_DEBUG)
							int nodeTriangleCount = (int)m_nodeTriangleList[m_nodeList[currentNode].TriangleListId].size();
						#endif

						for (auto idx : m_nodeTriangleList[m_nodeList[currentNode].TriangleListId])
						{
							if (m_allPrimitives[idx]->hit(p_ray, intersection))
							{
								if (p_intersection_out.t > intersection.t)
								{
									if (p_shadowTest)
										return true;

									p_intersection_out = intersection;
									p_intersection_out.triangleId = idx;
									//FIXHERE
									p_ray.m_tmax = min(p_ray.m_tmax, intersection.t);

									#if defined(SURFACE_DEBUG)
										intersection.hotspot = nodeTriangleCount;
									#endif
								}

								intersectionFound = true;
							}
						}

						if (stackPosition > 0) {
							--stackPosition;
							currentNode = stackNodes[stackPosition].node;
							tmin = stackNodes[stackPosition].min;
							tmax = stackNodes[stackPosition].max;
						}
						else break;
					}
					else {
						int leftChild, rightChild,
							axis = m_nodeList[currentNode].Axis;

						Real tplane = (p_ray.d.At[axis] == 0.0)
							? tmax
							: (m_nodeList[currentNode].Partition - p_ray.o.At[axis]) * p_ray.invd.At[axis];

						int belowFirst = p_ray.o.At[axis] < m_nodeList[currentNode].Partition ||
							(p_ray.o.At[axis] == m_nodeList[currentNode].Partition && p_ray.d.At[axis] <= 0);

						if (belowFirst) {
							leftChild = m_nodeList[currentNode].FirstChildNodeId;
							rightChild = leftChild + 1;
						}
						else {
							rightChild = m_nodeList[currentNode].FirstChildNodeId;
							leftChild = rightChild + 1;
						}

						if (tplane > tmax || tplane <= 0)
							currentNode = leftChild;
						else if (tplane < tmin)
							currentNode = rightChild;
						else
						{
							stackNodes[stackPosition].node = rightChild;
							stackNodes[stackPosition].min = tplane;
							stackNodes[stackPosition].max = tmax;

							++stackPosition;

							currentNode = leftChild;
							tmax = tplane;
						}
					}
				}

				return intersectionFound;
			}

			virtual bool TestIntersection(Ray p_ray, ShadeRec& p_rec, double& p_tmin, bool p_shadowTest = false)
			{
				bool intersectionHappened = false;

				if (!p_shadowTest)
				{
					Intersection outputIntersection;
					intersectionHappened = Traverse(0, p_ray, outputIntersection);
					if(intersectionHappened)
						T::AddCollisionDerailsFromIntersection(outputIntersection, p_ray, p_tmin, p_rec, (T*)m_allPrimitives[outputIntersection.triangleId]);
				}
				else
				{
					Intersection outputIntersection;
					intersectionHappened = Traverse(0, p_ray, outputIntersection, true);
				}
				return intersectionHappened;
			}

			// Test intersection
			inline bool Intersects(Ray &p_ray) {
				return Traverse(0, p_ray);
			}

			// Get nearest intersection
			inline bool Intersects(Ray &p_ray, Intersection &p_intersection_out) {
				return Traverse(0, p_ray, p_intersection_out);
			}
		};
	}
}
