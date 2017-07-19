//----------Quad.h--------------------------
#pragma once

#include "..\GeometricObjects\GeometricObjects.h"
#include "..\BoundingShapes\BoundingBox.h"
#include "..\Point\TriangleVertex\TriangleVertex.h"
#include "..\UtilClasses\Common.h"
#include "..\Logger\Logger.h"
#include "..\GeometricObjects\Triangle.h"

using Engine::Materials::Material;

namespace Engine
{
	class Quad : public GeometricObjects
	{
	public:
		friend class MatlabUtilityDebuger;
		Quad()
		{}

		Quad(Engine::TriangleVertex p_vertices[4])
		{
			for (int index = 0; index < 4; index++)
			{
				m_vertices[index] = p_vertices[index];
			}
			MakeNormal();
			MakeBoundingBox();
		}

		Quad(Engine::TriangleVertex p_vertices[4], Material* p_material)
		{
			for (int index = 0; index < 4; index++)
			{
				m_vertices[index] = p_vertices[index];
			}
			MakeNormal();
			m_material = p_material;
			MakeBoundingBox();
		}

		using GeometricObjects::MakeBoundingBox;
		void MakeBoundingBox()
		{
			BuildTrinagles();
			this->box = new BoundingBox();
			this->box->Union(m_firstTriangle->GetBoundingBox());
			this->box->Union(m_secondTrinagle->GetBoundingBox());
		}

		void BuildTrinagles()
		{
			//trinagle 1 0-1-2
			m_firstTriangle = new Primitives::Triangle(m_vertices[0], m_vertices[1], m_vertices[2]);
			//triangle 2 2-3-0
			m_secondTrinagle = new Primitives::Triangle(m_vertices[2], m_vertices[3], m_vertices[0]);
		}
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Hits. </summary>
		///
		/// <remarks>	Adrian, 27/08/2015. </remarks>
		///
		/// <param name="p_ray"> 	The ray. </param>
		/// <param name="p_tmin">	[in,out] The tmin. </param>
		/// <param name="sr">	 	[in,out] The sr. </param>
		///
		/// <returns>	true if it succeeds, false if it fails. </returns>
		///-------------------------------------------------------------------------------------------------
		using GeometricObjects::hit;
		virtual bool hit(Ray p_ray, double& p_tmin, ShadeRec& sr)
		{
			return hit(p_ray.o, p_ray.d, p_tmin, sr);
		}
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Hits. </summary>
		///
		/// <remarks>	Adrian, 27/08/2015. </remarks>
		///
		/// <param name="o">   	The Point3D to process. </param>
		/// <param name="d">   	The Point3D to process. </param>
		/// <param name="tmin">	[in,out] The tmin. </param>
		/// <param name="sr">  	[in,out] The sr. </param>
		///
		/// <returns>	true if it succeeds, false if it fails. </returns>
		///-------------------------------------------------------------------------------------------------
		virtual bool hit(Point3D o, Point3D  d, double& tmin, Engine::ShadeRec& sr)
		{
			double tempTmax = INT_MIN;
			double tempTmin = INT_MAX;
			Ray currentRay(o, d);
			bool intersection = box->CheckIntersection(currentRay, tempTmin, tempTmax);

			if (tempTmin < tmin && tempTmin > kEpsilon)
			{
				Point3D intersectionPoint = currentRay.o + currentRay.d * tempTmin;
				int trianglePoint = m_firstTriangle->GetBoundingBox()->CheckPointInsideBox(intersectionPoint) ? 0 : m_secondTrinagle->GetBoundingBox()->CheckPointInsideBox(intersectionPoint) ? 1 : 0;


				if (intersection && trianglePoint < 2)
				{
					sr.normal = GetNormal(Point3D());
					sr.object = this;
					sr.hitAnObject = true;
					sr.localHitPoint = intersectionPoint;
					sr.object = this;
					tmin = tempTmin;
					sr.tmin = tmin;
					sr.ray = currentRay;
					return true;
				}
			}

			return false;
		}

		void MakeNormal()
		{
			/*Point3D  p0 = m_vertices[1].vertex - m_vertices[0].vertex;
			Point3D p1 = m_vertices[2].vertex - m_vertices[0].vertex;

			Point3D faceNormal = p0.CrossProduct(p1);*/
			Point3D vertexNormal = m_vertices[0].normal;

			/*faceNormal.normalize();
			float dotProduct = faceNormal.dot(vertexNormal);

			m_normal = (dotProduct < 0.0f) ? -faceNormal : faceNormal;*/
			m_normal = vertexNormal;
		}
		virtual void PrintObjectName()
		{
			Logger::LogInfo("Quad " + m_material->GetMaterialName() + "---------");
		}


		virtual Point3D GetNormal(Point3D p_location)
		{
			return m_normal;
		}

		~Quad()
		{
			/*for (int index = 0; index < 4; index++)
			{
			delete m_vertices[index];
			}*/
		}

	private:
		Engine::TriangleVertex m_vertices[4];
		//a quad will be split into two triangles
		Primitives::Triangle* m_firstTriangle;
		Primitives::Triangle* m_secondTrinagle;
		Point3D m_normal;
	};
}
//--------Quad.h----------------------------