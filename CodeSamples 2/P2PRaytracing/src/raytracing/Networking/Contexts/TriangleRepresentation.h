#pragma once 

#include <vector>

#include "../../CoreClasses/Point/Point3D/Point3D.h"


using std::vector;
namespace Engine
{
	namespace Networking
	{
		namespace Contexts
		{
			namespace ViewPointContext
			{

				struct TriangleRep
				{
					union
					{
						Point3D vertices[3];
						Point3D point1, point2, point3;
					};

					TriangleRep()
					{}


					void ToString()
					{
						cout << point1.toString() << endl;
						cout << point2.toString() << endl;
						cout << point3.toString() << endl;
					}

					TriangleRep(const TriangleRep& p_toCopy)
					{
						vertices[0] = p_toCopy.vertices[0];
						vertices[1] = p_toCopy.vertices[1];
						vertices[2] = p_toCopy.vertices[2];
					}

					TriangleRep(Point3D p_cameraPosition, Point3D p_viewingDirection, float p_distance, float p_fov)
					{
						BuildTriangleRep(p_cameraPosition, p_viewingDirection, p_distance, p_fov);
					}

					TriangleRep(Point3D pos1, Point3D pos2, Point3D pos3) : point1(pos1), point2(pos2), point3(pos3)
					{
						vertices[0] = pos1;
						vertices[1] = pos2;
						vertices[2] = pos3;
					}

					void BuildTriangleRep(Point3D p_cameraPosition, Point3D p_viewingDirection, float p_distance, float p_fov)
					{
						vertices[0] = p_cameraPosition;
						Point3D tempPos = p_cameraPosition + p_viewingDirection * p_distance;
						float halfFov = p_fov / 2;

						float halfFovRadians = halfFov * (3.14159 / 180);

						vertices[1].x = tempPos.x * cosf(halfFovRadians) - tempPos.z* sinf(halfFovRadians);
						vertices[1].z = tempPos.x * sinf(halfFovRadians) - tempPos.z* cosf(halfFovRadians);

						vertices[2].x = tempPos.x * cosf(-halfFovRadians) - tempPos.z* sinf(-halfFovRadians);
						vertices[2].z = tempPos.x * sinf(-halfFovRadians) - tempPos.z* cosf(-halfFovRadians);
					}

					//--------------------------------------------------------------------
					// GetSeperationAxis: Returns all the seperation axis for a particular
					//					  triangle
					//--------------------------------------------------------------------
					void GetSeperationAxis(vector<Point3D>& p_seperationAxis)
					{
						int length = sizeof(vertices) / sizeof(Point3D);
						for (int vertexIndex = 0; vertexIndex < length; vertexIndex++)
						{
							Point3D currentPoint = vertices[vertexIndex];
							Point3D nextPoint = vertices[vertexIndex + 1 == length ? 0 : vertexIndex + 1];
							Point3D currentEdge = nextPoint - currentPoint;
							currentEdge.Normalize();
							currentEdge.x = currentEdge.x * -1;
							p_seperationAxis.push_back(currentEdge);
						}
					}

					//--------------------------------------------------------------------
					// Project: Project shape on a particular axis 
					//--------------------------------------------------------------------
					Point3D Project(Point3D p_axis)
					{
						float min = p_axis.dot(point1);
						float max = min;

						for (int index = 1; index < 3; index++)
						{
							float currentP = p_axis.dot(vertices[index]);

							if (currentP < min)
							{
								min = currentP;
							}
							else if (max < currentP)
								max = currentP;
						}

						return Point3D(min, 0,max);
					}
				};

				
				//--------------------------------------------------------------------
				// Representation of two viewepoint intersection
				//--------------------------------------------------------------------
				struct ViewPointMetric
				{
					TriangleRep m_triangle1;
					TriangleRep m_triangle2;

					ViewPointMetric()
					{}

					void SetTriangle1(TriangleRep p_triangle1)
					{
						m_triangle1 = p_triangle1;
					}

					void SetTriangle2(TriangleRep p_triangle2)
					{
						m_triangle2 = p_triangle2;
					}

					ViewPointMetric(TriangleRep p_triangle1, TriangleRep p_triangle2)
					{
						m_triangle1 = p_triangle1;
						m_triangle2 = p_triangle2;
					}

					//--------------------------------------------------------------------
					// TestCollision: Tests collision between two triangles
					//--------------------------------------------------------------------
					bool TestCollision(float& p_summation)
					{
						bool output = true;
						vector<Point3D> edges;

						m_triangle1.GetSeperationAxis(edges);
						m_triangle2.GetSeperationAxis(edges);

						for (int axisIndex = 0; axisIndex < edges.size(); axisIndex++)
						{
							Point3D proj1 = m_triangle1.Project(edges[axisIndex]);
							Point3D proj2 = m_triangle2.Project(edges[axisIndex]);
							bool result = TestOverlap(proj1, proj2, p_summation);
							if (!result)
							{
								#undef min
								p_summation = std::numeric_limits<float>::min();
								output = false;
								break;
							}
						}

						return output;
					}

					//--------------------------------------------------------------------
					// TestOverlap: returns if twqo projected shapes are ovcerlapping on 
					//				a particular axis
					//--------------------------------------------------------------------
					bool TestOverlap(Point3D& p_lhs, Point3D& p_rhs, float& p_summation)
					{
						if (p_lhs.At[2] > p_rhs.At[0])
						{
							p_summation += (p_lhs.At[2] - p_rhs.At[0]);
							return true;
						}
						else if (p_lhs.At[0] > p_rhs.At[2])
						{
							p_summation += (p_lhs.At[0] - p_rhs.At[2]);
							return true;
						}
						else
							return false;
					}


				};
			}
		}
	}
}