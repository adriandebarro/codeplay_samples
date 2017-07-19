#pragma once 
//----------------------------------------------------------------------------------------------------------------
//			Filename:		BVH.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <string>

#include ".\GeometricObjects.h"
#include "..\Point\TriangleVertex\TriangleVertex.h"
#include "..\..\CollisionDetails\ShadeRec.h"
#include "..\Point\Point2D\Point2D.h"
#include "..\Point\Point3D\Point3D.h"
#include "..\..\Material\ComplexMaterial.h"
#include "..\UtilClasses\Common.h"

using std::string;

namespace Engine
{
	namespace Primitives
	{
		//----------------------------------------------------------------------------------------------------------------
		//			Class Name: Triangle
		//		   Description: Wraps around geometric objects before sorted out in Acceleration Structure
		//----------------------------------------------------------------------------------------------------------------
		//			    Status: REFACTORING
		//----------------------------------------------------------------------------------------------------------------
		class Triangle : public GeometricObjects
		{
		public:
			TriangleVertex point1;
			TriangleVertex point2;
			TriangleVertex point3;

			string m_materialName;

			Point3D minPoint;
			Point3D maxPoint;
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: TriangleVertex, TriangleVertex, TriangleVertex
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			Triangle(TriangleVertex m_point1, TriangleVertex m_point2, TriangleVertex m_point3)
				: point1(m_point1)
				, point2(m_point2)
				, point3(m_point3)
			{
				MakeBoundingBox();
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: TriangleVertex, TriangleVertex, TriangleVertex, Material, MaterialName
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			Triangle(TriangleVertex m_point1, TriangleVertex m_point2, TriangleVertex m_point3, Materials::Material* p_triangleMaterial, string p_materialName)
				: point1(m_point1)
				, point2(m_point2)
				, point3(m_point3)
			{
				m_materialName = p_materialName;
				m_material = p_triangleMaterial;

				MakeBoundingBox();
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: TriangleVertex, TriangleVertex, TriangleVertex, Material, MaterialName, ParentObject
			//			  Status: 
			//					  TODO gotoparentformaterial 
			//					  TODO Link 
			//----------------------------------------------------------------------------------------------------------------
			Triangle(TriangleVertex m_point1, TriangleVertex m_point2, TriangleVertex m_point3, Materials::Material* p_triangleMaterial, string p_materialName, GeometricObjects* p_parentObject)
				: point1(m_point1)
				, point2(m_point2)
				, point3(m_point3)
			{
				m_materialName = p_materialName;
				m_material = p_triangleMaterial;
				/*if (m_material->GetMaterialName() != "kamen_zid")
				{
				cout << "error" << endl;
				}*/

				MakeBoundingBox();
			}

			#define __EPSILON 1e-5

			#define __CROSS(dest, v1, v2) \
			dest[0] = v1[1] * v2[2] - v1[2] * v2[1]; \
			dest[1] = v1[2] * v2[0] - v1[0] * v2[2]; \
			dest[2] = v1[0] * v2[1] - v1[1] * v2[0];

			#define __DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

			#define __SUB(dest, v1, v2) \
			dest[0] = v1[0] - v2[0]; \
			dest[1] = v1[1] - v2[1]; \
			dest[2] = v1[2] - v2[2];

			#define __DET(c1, c2, c3) \
			(c1[0] * (c2[1] * c3[2] - c3[1] * c2[2]) - \
			c1[1] * (c2[0] * c3[2] - c3[0] * c2[2]) + \
			c1[2] * (c2[0] * c3[1] - c3[0] * c2[1]))
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CalcBarycebtric
			//		Return Value: Point2D(u,v)
			//		  Parameters: Ray, Collision Details
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			Point2D CalcBarycentric(Ray p_ray, ShadeRec& p_sr)
			{
				//----------------------------------------------------------------------------------------------
				// Note that p_surface is not modified unless there has been an intersection
				//----------------------------------------------------------------------------------------------

				//----------------------------------------------------------------------------------------------
				// Shirley's 
				//----------------------------------------------------------------------------------------------
				float te1xte2[3],
					edge2[3],
					interm[3];

				Point3D p0 = point2.vertex - point1.vertex,
					p1 = point3.vertex - point1.vertex;

				float v0[3] = { (float)point1.vertex.x, (float)point1.vertex.y, (float)point1.vertex.z };

				float _te1[3] = { (float)p0.x, (float)p0.y, (float)p0.z },
					_te2[3] = { (float)p1.x, (float)p1.y, (float)p1.z };

				float _ro[3] = { (float)p_ray.o.x, (float)p_ray.o.y, (float)p_ray.o.z },
					_rd[3] = { (float)p_ray.d.x, (float)p_ray.d.y, (float)p_ray.d.z };

				const float *te1 = _te1;
				const float *te2 = _te2;

				__CROSS(te1xte2, te1, te2);

				const float rcp = 1.0f / __DOT(te1xte2, _rd);
				__SUB(edge2, v0, _ro);

				const float t = __DOT(te1xte2, edge2) * rcp;

				//if (t > p_ray.Max - 1e-5 || t < 1e-5)
				if (t < 1e-5) return false;

				__CROSS(interm, _rd, edge2);

				const float beta = __DOT(interm, te2) * -rcp;
				if (beta < 0.0f)
					return false;

				const float gamma = __DOT(interm, te1) * rcp;
				if (beta + gamma > 1.0f || gamma < 0.0f)
					return false;

				const float alpha = 1.f - beta - gamma;

				float u = point1.textureCoordinate.x * alpha +
					point2.textureCoordinate.x * beta +
					point3.textureCoordinate.x * gamma;

				float v = point1.textureCoordinate.y * alpha +
					point2.textureCoordinate.y * beta +
					point3.textureCoordinate.y * gamma;

				p_sr.tmin = t;
				p_sr.depth = t;
				p_sr.localHitPoint = (Point3D)p_ray.o + (t * (Point3D)p_ray.d);
				p_sr.hitAnObject = true;
				p_sr.textureCoordinates.x = u;
				p_sr.textureCoordinates.y = v;
				p_sr.m_hitTriangle = this;
				p_sr.normal = p0.CrossProduct(p1).normalize();

				Point2D barycentricCoor(u, v);
				return barycentricCoor;
			}


			static void GetBounds(Triangle *p_tri, const Axis p_axis, Real &p_boundsLow_out, Real &p_boundsHigh_out)
			{
				_ASSERT(p_axis >= -1 && p_axis <= 2);


//DIRTY

				Real v0 = p_axis == 0? p_tri->point1.vertex.x : p_axis == 1? p_tri->point1.vertex.y : p_tri->point1.vertex.z ,
					v1 = p_axis == 0 ? p_tri->point2.vertex.x : p_axis == 1 ? p_tri->point2.vertex.y : p_tri->point2.vertex.z,
					v2 = p_axis == 0 ? p_tri->point3.vertex.x : p_axis == 1 ? p_tri->point3.vertex.y : p_tri->point3.vertex.z;

				p_boundsLow_out = min(min(v0, v1), v2);
				p_boundsHigh_out = max(max(v0, v1), v2);
			}


			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: TriangleVertex, TriangleVertex, TriangleVertex
			//			  Status: 
			//					  TODO replace current hit method
			//----------------------------------------------------------------------------------------------------------------
			virtual bool hit(Ray p_ray, double& tmin, ShadeRec& p_sr)
			{
				//----------------------------------------------------------------------------------------------
				// Note that p_surface is not modified unless there has been an intersection
				//----------------------------------------------------------------------------------------------

				//----------------------------------------------------------------------------------------------
				// Shirley's 
				//----------------------------------------------------------------------------------------------
				float te1xte2[3],
					edge2[3],
					interm[3];

				Point3D p0 = point2.vertex - point1.vertex,
					p1 = point3.vertex - point1.vertex;

				float v0[3] = { (float)point1.vertex.x, (float)point1.vertex.y, (float)point1.vertex.z };

				float _te1[3] = { (float)p0.x, (float)p0.y, (float)p0.z },
					_te2[3] = { (float)p1.x, (float)p1.y, (float)p1.z };

				float _ro[3] = { (float)p_ray.o.x, (float)p_ray.o.y, (float)p_ray.o.z },
					_rd[3] = { (float)p_ray.d.x, (float)p_ray.d.y, (float)p_ray.d.z };

				const float *te1 = _te1;
				const float *te2 = _te2;

				__CROSS(te1xte2, te1, te2);

				const float rcp = 1.0f / __DOT(te1xte2, _rd);
				__SUB(edge2, v0, _ro);

				const float t = __DOT(te1xte2, edge2) * rcp;

				//if (t > p_ray.Max - 1e-5 || t < 1e-5)
				if (t < 1e-5) return false;

				__CROSS(interm, _rd, edge2);

				const float beta = __DOT(interm, te2) * -rcp;
				if (beta < 0.0f)
					return false;

				const float gamma = __DOT(interm, te1) * rcp;
				if (beta + gamma > 1.0f || gamma < 0.0f)
					return false;

				const float alpha = 1.f - beta - gamma;

				float u = point1.textureCoordinate.x * alpha +
					point2.textureCoordinate.x * beta +
					point3.textureCoordinate.x * gamma;

				float v = point1.textureCoordinate.y * alpha +
					point2.textureCoordinate.y * beta +
					point3.textureCoordinate.y * gamma;

				p_sr.tmin = t;
				p_sr.depth = t;
				p_sr.localHitPoint = (Point3D)p_ray.o + (t * (Point3D)p_ray.d);
				p_sr.hitAnObject = true;
				p_sr.textureCoordinates.x = u;
				p_sr.textureCoordinates.y = v;
				p_sr.m_hitTriangle = this;
				p_sr.normal = p0.CrossProduct(p1).normalize();

				p_sr.ray = p_ray;

				return true;
			}



			virtual bool hit(Ray p_ray, Intersection &p_intersection_out)
			{
				//----------------------------------------------------------------------------------------------
				// Note that p_surface is not modified unless there has been an intersection
				//----------------------------------------------------------------------------------------------

				//----------------------------------------------------------------------------------------------
				// Shirley's 
				//----------------------------------------------------------------------------------------------
				float te1xte2[3],
					edge2[3],
					interm[3];

				Point3D p0 = point2.vertex - point1.vertex,
					p1 = point3.vertex - point1.vertex;

				float v0[3] = { (float)point1.vertex.x, (float)point1.vertex.y, (float)point1.vertex.z };

				float _te1[3] = { (float)p0.x, (float)p0.y, (float)p0.z },
					_te2[3] = { (float)p1.x, (float)p1.y, (float)p1.z };

				float _ro[3] = { (float)p_ray.o.x, (float)p_ray.o.y, (float)p_ray.o.z },
					_rd[3] = { (float)p_ray.d.x, (float)p_ray.d.y, (float)p_ray.d.z };

				const float *te1 = _te1;
				const float *te2 = _te2;

				__CROSS(te1xte2, te1, te2);

				const float rcp = 1.0f / __DOT(te1xte2, _rd);
				__SUB(edge2, v0, _ro);

				const float t = __DOT(te1xte2, edge2) * rcp;

				//if (t > p_ray.Max - 1e-5 || t < 1e-5)
				if (t < 1e-5) return false;

				__CROSS(interm, _rd, edge2);

				const float beta = __DOT(interm, te2) * -rcp;
				if (beta < 0.0f)
					return false;

				const float gamma = __DOT(interm, te1) * rcp;
				if (beta + gamma > 1.0f || gamma < 0.0f)
					return false;

				p_intersection_out.beta = beta;
				p_intersection_out.gamma = gamma;
				p_intersection_out.t = t;
				return true;
			}


#define CROSS(rez,v1,v2) \
		rez.x = v1.y*v2.z - v1.z*v2.y; \
		rez.y = v1.z*v2.x - v1.x*v2.z; \
		rez.z = v1.x*v2.y - v1.y*v2.x;
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: hit
			//		Return Value: VOID
			//		  Parameters: Ray origin, Ray Direction, shortest distance, Collision Details
			//			  Status: 
			//					  TODO replace with keiths method
			//----------------------------------------------------------------------------------------------------------------
			using GeometricObjects::hit;
			bool hit(Point3D o, Point3D d, double &tmin, ShadeRec &sr)
			{
				Point3D e1, e2, q, r, s;

				e1 = point2.vertex - point1.vertex;
				e2 = point3.vertex - point1.vertex;

				q = d.CrossProduct(e2);

				d.normalize();
				//NORMALIZE(pvec);
				float a = DOT(q, e1);
#ifdef TEST_CULL
				if (det < EPSILON)
				{
					return false;
				}
				SUB(tvec, orig, v1);
				float u = DOT(tvec, pvec);
				if (u < 0.0 || u > det)
				{

					return false;
				}
				CROSS(qvec, tvec, e1);
				float v = DOT(dir, qvec);
				if (v < 0.0f || v + u > det)
				{

					return false;
				}
#else
				if (a < kEpsilon && a > -kEpsilon)
				{
					return false;
				}

				float f = 1.0f / a;
				s = o - point1.vertex;
				// NORMALIZE(tvec);
				float u = f * DOT(s, q);
				if (u <0.0f || u > 1.0f)
				{

					return false;
				}

				r = s.CrossProduct(e1);

				// NORMALIZE(qvec);
				float v = f * DOT(r, d);
				if (v < 0.0f || u + v > 1.0f)
				{

					return false;
				}
#endif
				float t = f * e2.dot(r);

				if (t < tmin && t > kEpsilon)
				{
					tmin = t;
					sr.localHitPoint = o + d * tmin;
					sr.normal = e1.CrossProduct(e2);
					sr.normal.normalize();
					sr.tmin = tmin;
					sr.m_hitTriangle = this;
					sr.hitAnObject = true;
					sr.ray.o = o;
					sr.ray.d = d;
				}
				else
				{
					return false;
				}

				return true;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: TakeCenterPoint
			//		Return Value: VOID
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			Point3D TakeCenterPoint(void)
			{
				return (point1.vertex + point2.vertex + point3.vertex) / 3;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: GetNormal
			//		Return Value: Point3D
			//		  Parameters: CollisonPoint
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			using GeometricObjects::GetNormal;
			Point3D GetNormal(Point3D p_hitPoint)
			{
				Point3D e1 = point2.vertex - point1.vertex;
				Point3D e2 = point3.vertex - point1.vertex;

				return e1.CrossProduct(e2);
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: GetBoundingBox
			//		Return Value: BoundingBox
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			using GeometricObjects::GetBoundingBox;
			Engine::BoundingBox* GetBoundingBox(void)
			{
				return box;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: MakeBoundingBox
			//		Return Value: VOID
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			void MakeBoundingBox(void)
			{
				BoundingBox *bbox = new BoundingBox();
				bbox->Expand(point1.vertex);
				bbox->Expand(point2.vertex);
				bbox->Expand(point3.vertex);
				box = bbox;
			}

			void ExpandBoundingBox(BoundingBox* p_bbox)
			{
				p_bbox->Expand(point1.vertex);
				p_bbox->Expand(point2.vertex);
				p_bbox->Expand(point3.vertex);
			}

			void static AddCollisionDerailsFromIntersection(Common::GeomerticPrimitives::Intersection &p_intersection,Ray &p_ray, double &p_tmin, ShadeRec & p_shaderec_out, Triangle* p_triangle)
			{
				Real beta = p_intersection.beta,
					gamma = p_intersection.gamma,
					t = p_intersection.t;

				int triangleId = p_intersection.triangleId;

				const float alpha = 1.f - beta - gamma;

				float u = p_triangle->point1.textureCoordinate.x * alpha +
					p_triangle->point2.textureCoordinate.x * beta +
					p_triangle->point3.textureCoordinate.x * gamma;

				float v = p_triangle->point1.textureCoordinate.y * alpha +
					p_triangle->point2.textureCoordinate.y * beta +
					p_triangle->point3.textureCoordinate.y * gamma;

				
				p_shaderec_out.m_barycetric_constants.alpha = alpha;
				p_shaderec_out.m_barycetric_constants.beta = beta;
				p_shaderec_out.m_barycetric_constants.gamma = gamma;
				p_shaderec_out.tmin = t;
				p_shaderec_out.depth = t;
				p_shaderec_out.localHitPoint = (Point3D)p_ray.o + (t * (Point3D)p_ray.d);
				p_shaderec_out.hitAnObject = true;
				p_shaderec_out.textureCoordinates.x = u;
				p_shaderec_out.textureCoordinates.y = v;
				p_shaderec_out.m_hitTriangle =  p_triangle;

				Triangle* intersectedTriangle = (Triangle*)p_shaderec_out.m_hitTriangle;

				/*float x = alpha*intersectedTriangle->point1.normal.x + beta* intersectedTriangle->point2.normal.x + gamma * intersectedTriangle->point3.normal.x;
				float y = alpha*intersectedTriangle->point1.normal.y + beta* intersectedTriangle->point2.normal.y + gamma * intersectedTriangle->point3.normal.y;
				float z = alpha*intersectedTriangle->point1.normal.z + beta* intersectedTriangle->point2.normal.z + gamma * intersectedTriangle->point3.normal.z;

				Point3D resultingNormal(x, y, z);
				resultingNormal.Normalize();

				p_shaderec_out.normal = resultingNormal;
*/
				Point3D normal_0 = intersectedTriangle->point1.normal;
				Point3D normal_1 = intersectedTriangle->point2.normal;
				Point3D normal_2 = intersectedTriangle->point3.normal;

				Point3D vertexNormal = alpha * normal_0 + beta * normal_1 + gamma * normal_2;
				vertexNormal.Normalize();
				//both normals are are written to the collision details 
				p_shaderec_out.m_shadingNormal = vertexNormal;				
				p_shaderec_out.normal = p_triangle->GetNormal(p_shaderec_out.localHitPoint).normalize();
				
				p_shaderec_out.ray = p_ray;

			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: GetMaterial
			//		Return Value: VOID
			//		  Parameters: TriangleVertex, TriangleVertex, TriangleVertex
			//			  Status: TODO Link with Parent
			//----------------------------------------------------------------------------------------------------------------
			using GeometricObjects::GetMaterial;
			Engine::Materials::Material* GetMaterial()
			{
				return m_material;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: PrintObjectName
			//		Return Value: VOID
			//		  Parameters: TriangleVertex, TriangleVertex, TriangleVertex
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			virtual void PrintObjectName(void)
			{
				cout << "Triangle " << m_material->GetMaterialName() << "---------" << endl;
			}
		};
	}
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------

