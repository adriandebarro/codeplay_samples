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

#include "..\BVH.h"
#include "..\..\Integrators\RadianceDetails\RadiancePoint.h"
#include "..\..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\..\Scene\Scene.h"
#include "..\..\CoreClasses\UtilClasses\Common.h"
#include "..\..\CoreClasses\BoundingShapes\BoundingBox.h"
#include "..\..\CoreClasses\Logger\Logger.h"

#include "../../Scene/Scene.h"

#define OCT_BUCKET_LIMIT 20
#define AMOUNT_OF_POINTS_FOR_ESTIMATE 0
#define OCT_MAXIMUM_LEVEL 8

namespace Engine
{
	class LockingOctree;
}

using Engine::Scene;



namespace Engine
{
	using std::queue;

			typedef struct
		{
			int index;
			RadiancePoint* radiancePoint;
		}NodeEntry;


	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: OctNode
	//	 Class Description: Node representation for the 
	//----------------------------------------------------------------------------------------------------------------
	class OctNode
	{
		friend class Octree;
		friend class LockingOctree;
	private:
		BoundingBox *m_boundingBos;
		OctNode* children[8];
		vector<int> m_storedPoints;
		bool childNode;
		bool parentNode;
		int level;
		std::mutex m_mutex;

	public:



		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: IsParentNode
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool IsParentNode()
		{
			return parentNode;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: minPoint, maxPoint, level
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		OctNode(Point3D minPoint, Point3D maxPoint, int p_level)
		{
			childNode = true;
			m_boundingBos = new BoundingBox(minPoint, maxPoint);
			parentNode = false;
			level = p_level;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: BoundingBoxPointer
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		OctNode(BoundingBox* boundingBox)
		{
			childNode = true;
			m_boundingBos = boundingBox;
			parentNode = false;
			level = 0;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		OctNode()
		{
			m_boundingBos = new BoundingBox(0);
			level = 0;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: IsParentNode
		//		Return Value: VOID
		//		  Parameters: RadiancePoint
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool CheckIntersectionWithPoint(RadiancePoint radiancePoint)
		{
			return (this->m_boundingBos->CheckIntersection(radiancePoint.m_position, radiancePoint.m_rayLength));
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CheckIntersectionWithPoint
		//		Return Value: VOID
		//		  Parameters: Point3D Position
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool CheckIntersectionWithPoint(Point3D point)
		{
			return (this->m_boundingBos->CheckPointInsideBox(point));
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetPoints
		//		Return Value: A list of RadiancePoint which have a weight greater than 0
		//		  Parameters: Position in 3D Space
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		vector<int> GetPoints(Point3D radiancePoint)
		{
			return m_storedPoints;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SplitBox
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SplitBox()
		{
			//cout << " Box Being Split" << endl;
			Point3D centoridPoint = m_boundingBos->GetCentroid();
			Point3D tempMinPoint = Point3D(0), tempMaxPoint = Point3D(0);

			Point3D minPoint = this->m_boundingBos->GetMinPoint();
			Point3D maxPoint = this->m_boundingBos->GetMaxPoint();

			//min - centroid 
	 		tempMinPoint = minPoint;
			tempMaxPoint = centoridPoint;
			children[0] = new OctNode(tempMinPoint, tempMaxPoint, ++level);

			//xmin ymin zc-- xc yc zmax
			tempMinPoint = Point3D(minPoint.x, minPoint.y, centoridPoint.z);
			tempMaxPoint = Point3D(centoridPoint.x, centoridPoint.y, maxPoint.z);

			children[1] = new OctNode(tempMinPoint, tempMaxPoint, ++level);

			//xc ymin zc  xmax yc zmax
			tempMinPoint = Point3D(centoridPoint.x, minPoint.y, centoridPoint.z);
			tempMaxPoint = Point3D(maxPoint.x, centoridPoint.y, maxPoint.z);

			children[2] = new OctNode(tempMinPoint, tempMaxPoint, ++level);

			//xc ymin zmin - xmax yc zc
			tempMinPoint = Point3D(centoridPoint.x, minPoint.y, minPoint.z);
			tempMaxPoint = Point3D(maxPoint.x, centoridPoint.y, centoridPoint.z);

			children[3] = new OctNode(tempMinPoint, tempMaxPoint, ++level);

			//xc yc zmin -- xmax ymax zc
			tempMinPoint = Point3D(centoridPoint.x, centoridPoint.y, minPoint.z);
			tempMaxPoint = Point3D(maxPoint.x, maxPoint.y, centoridPoint.z);

			children[4] = new OctNode(tempMinPoint, tempMaxPoint, ++level);

			//xmin yc zmin -- xc ymax zc
			tempMinPoint = Point3D(minPoint.x, centoridPoint.y, minPoint.z);
			tempMaxPoint = Point3D(centoridPoint.x, maxPoint.y, centoridPoint.z);

			children[5] = new OctNode(tempMinPoint, tempMaxPoint, ++level);


			//xmin yc zc -- xc ymax zmax
			tempMinPoint = Point3D(minPoint.x, centoridPoint.y, centoridPoint.z);
			tempMaxPoint = Point3D(centoridPoint.x, maxPoint.y, maxPoint.z);

			children[6] = new OctNode(tempMinPoint, tempMaxPoint, ++level);

			//centroid - Max
			children[7] = new OctNode(centoridPoint, maxPoint, ++level);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: IsChildNode
		//		Return Value: bool
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool IsChildNode()
		{
			return childNode;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ToString
		//		Return Value: string representation of a node
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		string ToString()
		{
			return m_boundingBos->ToString();
		}
	};
}
//----------------------------------------------------------------------------------------------------------------
