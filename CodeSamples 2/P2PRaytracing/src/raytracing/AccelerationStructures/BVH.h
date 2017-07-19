#pragma once 
#define BVH_BUCKET_SIZE 1

#include "AccelerationStructure.h"

#include <queue>

///-------------------------------------------------------------------------------------------------
/// <summary>	A macro that defines bvh depth size. </summary>
///
/// <remarks>	Adrian, 18/08/2015. </remarks>
///-------------------------------------------------------------------------------------------------

#define BVH_DEPTH_SIZE 12

using namespace Engine::AccelerationStructures;

namespace Engine
{

	namespace AccelerationStructures
	{
		using Engine::BoundingBox;
		using std::queue;
		using std::vector;
		//----------------------------------------------------------------------------------------------------------------
		//			Enum Name: PartitioningStyle
		//	 Enum Description: The kind of partitioning to be used for the splitting of the primitives
		//----------------------------------------------------------------------------------------------------------------

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Wraps around geometric objects before sorted out in Acceleration Structure </summary>
		/// <
		/// <remarks>	Adrian, 18/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		struct PrimitiveBvhRepresentation
		{
		public:
			/// <summary>	The primitive bounding box. </summary>
			BoundingBox* primitiveBoundingBox;
			/// <summary>	Zero-based index of the. </summary>
			int index;
			/// <summary>	The bounding box centroid. </summary>
			Point3D boundingBoxCentroid;
			/// <summary>	The primitive pointer. </summary>
			GeometricObjects* primitivePointer;
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			PrimitiveBvhRepresentation()
			{
				primitiveBoundingBox = new BoundingBox();
				index = 0;
				boundingBoxCentroid = Point3D(0);
				primitivePointer = NULL;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: BoundingBox pointer, CurrentIndex, ObjectToWrapAround
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			PrimitiveBvhRepresentation(BoundingBox* p_Box, int& p_index, GeometricObjects* p_PrimitivePointer) : primitiveBoundingBox(p_Box), index(p_index), primitivePointer(p_PrimitivePointer)
			{
				boundingBoxCentroid = p_Box->GetCentroid();
			}
		};
		//----------------------------------------------------------------------------------------------------------------
		//			Class Name: PrimitiveBvhRepresentation
		//		   Description: A BVH Node, may contain a list of PrimitiveBvhRepresentation. A 
		//						BVH is built entirley from BVH nodes
		//----------------------------------------------------------------------------------------------------------------
		//			    Status: REFACTORING
		//----------------------------------------------------------------------------------------------------------------

		///-------------------------------------------------------------------------------------------------
		/// <summary>	A bvh node. </summary>
		///
		/// <remarks>	Adrian, 18/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		class BVHNode
		{
		public:

			/// <summary>	true to child node. </summary>
			bool childNode;
			/// <summary>	The bounding box. </summary>
			BoundingBox* boundingBox;
			/// <summary>	The splitting axis. </summary>
			int splittingAxis;
			/// <summary>	The splitting axis value. </summary>
			double splittingAxisValue;
			/// <summary>	The objects. </summary>
			vector<GeometricObjects*> objects;
			/// <summary>	The left child. </summary>
			Engine::AccelerationStructures::BVHNode *leftChild;
			/// <summary>	The right child. </summary>
			Engine::AccelerationStructures::BVHNode *rightChild;
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			BVHNode()
			{
				childNode = false;
				boundingBox = new BoundingBox();
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: minPoint and maxPoint of the BoundingBox to build
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			BVHNode(Point3D p_minPoint, Point3D p_maxPoint)
			{
				boundingBox = new BoundingBox(p_minPoint, p_maxPoint);
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: Iterator. Iterator, Type of splitting, currentDepth
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			BVHNode(vector<PrimitiveBvhRepresentation>::iterator p_first, vector<PrimitiveBvhRepresentation>::iterator p_last, PartitioningStyle p_style, int depth)
			{
				childNode = false;
				boundingBox = new BoundingBox();
				int amountOfObjects = p_last - p_first;

				//Logger::LogInfo("current node "+to_string(amountOfObjects));

				InitNode(p_first, p_last, p_style, depth);
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: InitNode
			//		Return Value: VOID
			//		  Parameters: Iterator. Iterator, Type of splitting, currentDepth
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			void InitNode(vector<PrimitiveBvhRepresentation>::iterator first, vector<PrimitiveBvhRepresentation>::iterator last, PartitioningStyle style, int depth)
			{
				int size = last - first;
				childNode = size <= BVH_BUCKET_SIZE || depth >= BVH_DEPTH_SIZE;

				Point3D minTemp(INT_MAX);
				Point3D maxTemp(INT_MIN);

				for (vector<PrimitiveBvhRepresentation>::iterator iter = first; iter != last; ++iter)
				{
					boundingBox->Union((*iter).primitiveBoundingBox);

					minTemp.Min((*iter).boundingBoxCentroid);
					maxTemp.Max((*iter).boundingBoxCentroid);

					if (childNode)
					{
						objects.push_back((*iter).primitivePointer);
					}
				}

				if (!childNode)
				{
					//splittingAxis = GetLongestAxis();
					splittingAxis = GetLongestAxis(minTemp, maxTemp);
					splittingAxisValue = GetSplitOfAxis(splittingAxis, minTemp, maxTemp);

					if (style == AccelerationStructures::PartitioningStyle::MIDDLE_POINT)
					{
						auto lastFirst = std::partition(first, last,
							[this](PrimitiveBvhRepresentation const& x) {return MiddlePointPartition(x); });

						leftChild = new BVHNode(first, lastFirst, style, ++depth);
						rightChild = new BVHNode(lastFirst, last, style, ++depth);
					}
				}
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: GetLongestAxis
			//		Return Value: int
			//		  Parameters: MinPoint, MaxPoint
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			int GetLongestAxis(Point3D minPoint, Point3D maxPoint)
				//int GetLongestAxis()
			{
				Point3D diagVector = maxPoint - minPoint;

				if (diagVector.x > diagVector.y && diagVector.x > diagVector.z)
				{
					return 0;
				}
				else if (diagVector.y > diagVector.z)
				{
					return 1;
				}
				else
				{
					return 2;
				}
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: GetSplitOfAxis
			//		Return Value: float
			//		  Parameters: SideToSplit, minBounds, maxBound
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			float GetSplitOfAxis(int side, Point3D minTemp, Point3D maxTemp)
			{
				Point3D temp = (maxTemp - minTemp) / 2;
				Point3D diagVector = temp + minTemp;
				switch (side)
				{
				case 0:
					return diagVector.x;
				case 1:
					return diagVector.y;
				case 2:
					return diagVector.z;
				default:
					cout << "ERROR" << endl;
					return 0;
				}
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: MiddlePointPartition
			//		Return Value: Boolean 
			//		  Parameters: Current PrimitiveBvhRepresentation
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			bool MiddlePointPartition(PrimitiveBvhRepresentation current)
			{
				if (splittingAxis == 0)
				{
					return current.boundingBoxCentroid.x <= splittingAxisValue;
				}
				else if (splittingAxis == 1)
				{
					return current.boundingBoxCentroid.y <= splittingAxisValue;
				}
				else
				{
					return current.boundingBoxCentroid.z <= splittingAxisValue;
				}
			}

		private:
		};


		///-------------------------------------------------------------------------------------------------
		/// <summary>	A wrapper for the BVHNodes make them suitable for flattening. </summary>
		///
		/// <remarks>	Adrian, 18/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------
		class FlatBvhNode
		{
		public:
			/// <summary>	The bounding box. </summary>
			BoundingBox * bbox;
			/// <summary>	Zero-based index of the left child. </summary>
			int leftChildIndex;
			/// <summary>	Zero-based index of the right child. </summary>
			int rightChildIndex;
			/// <summary>	Zero-based index of my. </summary>
			int myIndex;
			/// <summary>	The representing node. </summary>
			BVHNode *representingNode;
			/// <summary>	true to child node. </summary>
			bool childNode;

			FlatBvhNode(int index, BVHNode* currentNode)
			{
				bbox = currentNode->boundingBox;
				myIndex = index;
				representingNode = currentNode;
				if (currentNode->childNode)
				{
					childNode = true;

				}
				else
				{
					childNode = false;
				}
			}

		};

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	An object containing the BVH main methods </summary>
		///
		/// <remarks>	Adrian, 07/07/2017. </remarks>
		////////////////////////////////////////////////////////////////////////////////////////////////////
		class BVH : public AccelerationStructure
		{
		public:
			/// <summary>	The root node. </summary>
			BVHNode *rootNode;
			/// <summary>	The flat nodes[ 100]. </summary>
			FlatBvhNode* flatNodes[4096];
			/// <summary>	true to flat bvh. </summary>
			bool m_flatBvh;
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			BVH()
			{}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: minPoint, maxPoint of boudningbox
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			BVH(Point3D p_minPoint, Point3D p_maxPoint)
			{
				rootNode = new BVHNode(p_minPoint, p_maxPoint);
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: List of SceneObjects, Style of Splitting in the BVH, Boolean representing if to flatten bvh
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			BVH(vector<GeometricObjects*> sceneObjects, PartitioningStyle style = MIDDLE_POINT, bool p_flatBvh = false)
			{
				TranslateObjects(sceneObjects);
				BuildTree(style);
				int index = 0;
				//if (p_flatBvh) 
				if (p_flatBvh = true)
				{
					m_flatBvh = p_flatBvh;

					FlattenBvh(rootNode, index);
				}
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: TranslateObjects
			//		Return Value: VOID
			//		  Parameters: SceneObjects 
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			void TranslateObjects(vector<GeometricObjects*> sceneObjects)
			{
				for (int index = 0; index < sceneObjects.size(); index++)
				{
					objectsRepresentation.push_back(PrimitiveBvhRepresentation(sceneObjects[index]->GetBoundingBox(), index, sceneObjects[index]));
				}
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: BuildTree
			//		Return Value: VOID
			//		  Parameters: PartitioningStyle
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			using AccelerationStructure::BuildTree;
			virtual void BuildTree(PartitioningStyle style)
			{
				int depth = 0;
				rootNode = new BVHNode(objectsRepresentation.begin(), objectsRepresentation.end(), style, depth);
				m_boundingBox = rootNode->boundingBox;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: TestIntersection
			//		Return Value: bool
			//		  Parameters: Ray, Collision Details, Smallest collision point, boolean for shadow test
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			using AccelerationStructure::TestIntersection;
			virtual bool TestIntersection(Ray ray, ShadeRec& rec, double& tmin, bool p_shadowTest = false)
			{
				if (m_flatBvh)
				{
					return TestIntersectionFlatBvh(ray, rec, tmin, p_shadowTest);
				}
				else
				{
					return TestIntersectionNormal(ray, rec, p_shadowTest);
				}
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: TestIntersectionFlatBvh
			//		Return Value: bool
			//		  Parameters: Ray, Collision Details, Smallest collision point, boolean for shadow test
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			bool TestIntersectionFlatBvh(Ray ray, Engine::ShadeRec& sr, double& tmin, bool p_shadowTest = false)
			{
				double tmax = INT_MIN;
				bool testIntersection = false;

				queue<int> indexesToVisit;
				indexesToVisit.push(0);
				while (indexesToVisit.size() > 0)
				{
					int currentIndex = indexesToVisit.front();
					indexesToVisit.pop();

					FlatBvhNode* currentNode = flatNodes[currentIndex];

					if (currentNode->childNode)
					{
						if (currentNode->representingNode != NULL)
						{
							for (int index = 0; index < currentNode->representingNode->objects.size(); index++)
							{
								bool tempIntersection = false;

								if (p_shadowTest)
								{
									tempIntersection = currentNode->representingNode->objects[index]->hit(ray.o, ray.d, tmin, sr) && sr.tmin < (ray.GetTmax());
								}
								else
								{
									tempIntersection = currentNode->representingNode->objects[index]->hit(ray.o, ray.d, tmin, sr);
								}

								if (p_shadowTest &&  tempIntersection)
								{
									//cout << "the intersection distance is " << sr.tmin << endl;
									return true;
								}
								else if (tempIntersection && !testIntersection)
									testIntersection = true;
							}
						}
					}
					else
					{
						double tempMin = INT_MAX;
						double tempMax = INT_MIN;

						if (flatNodes[currentNode->leftChildIndex]->bbox->CheckIntersection(ray, tempMin, tempMax))
						{
							indexesToVisit.push(currentNode->leftChildIndex);
						}
						tempMin = INT_MAX;
						tempMax = INT_MIN;
						if (flatNodes[currentNode->rightChildIndex]->bbox->CheckIntersection(ray, tempMin, tempMax))
						{
							indexesToVisit.push(currentNode->rightChildIndex);
						}
					}
				}

				if (p_shadowTest)
					return false;
				else
					return testIntersection;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: TestIntersectionNormal
			//		Return Value: VOID
			//		  Parameters: Ray, Collision Details, boolean for shadow testing
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			bool TestIntersectionNormal(Ray ray, ShadeRec& rec, bool p_shadowTest = false)
			{
				double tmin = INT_MAX;
				double tmax = INT_MIN;
				queue<BVHNode*> toVisit;

				toVisit.push(rootNode);
				bool testIntersection = false;
				while (toVisit.size() > 0)
				{
					double tempTmin(INT_MAX);
					double tempTmax(INT_MIN);
					BVHNode * currentNode = toVisit.front();
					toVisit.pop();

					if (currentNode->childNode)
					{
						for (int objectIndex = 0; objectIndex < currentNode->objects.size(); objectIndex++)
						{
							bool temp = currentNode->objects[objectIndex]->hit(ray.o, ray.d, tmin, rec);
							if (p_shadowTest && temp)
							{
								cout << "found shadow" << tmin << endl;
								return true;
							}
							else if (temp)
								testIntersection = temp;
						}
					}
					else
					{
						if (currentNode->leftChild != NULL && currentNode->leftChild->boundingBox->CheckIntersection(ray, tempTmin, tempTmax))
						{
							toVisit.push(currentNode->leftChild);
						}

						tempTmin = INT_MAX;
						tempTmax = INT_MIN;

						if (currentNode->rightChild != NULL && currentNode->rightChild->boundingBox->CheckIntersection(ray, tempTmin, tempTmax))
						{
							toVisit.push(currentNode->rightChild);
						}
					}
				}

				return testIntersection;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: FlattenBvh
			//		Return Value: VOID
			//		  Parameters: BVHNode, current index
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			void FlattenBvh(BVHNode* p_currentNode, int& p_currentIndex)
			{
				if (p_currentNode->childNode)
				{
					flatNodes[p_currentIndex] = new FlatBvhNode(p_currentIndex, p_currentNode);
				}
				else
				{
					int currentNodeIndex = p_currentIndex;
					flatNodes[currentNodeIndex] = new FlatBvhNode(p_currentIndex, p_currentNode);
					flatNodes[currentNodeIndex]->leftChildIndex = ++p_currentIndex;
					FlattenBvh(p_currentNode->leftChild, p_currentIndex);
					flatNodes[currentNodeIndex]->rightChildIndex = ++p_currentIndex;
					FlattenBvh(p_currentNode->rightChild, p_currentIndex);
				}
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: DESTRCUTOR
			//		Return Value: VOID
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			~BVH()
			{
				/*if (m_flatBvh)
				{
					delete[] flatNodes;
				}*/
			}

		private:
			/// <summary>	The objects representation. </summary>
			vector<PrimitiveBvhRepresentation> objectsRepresentation;
		};
	}
}

