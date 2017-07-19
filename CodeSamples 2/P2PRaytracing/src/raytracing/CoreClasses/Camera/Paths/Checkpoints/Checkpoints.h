#pragma once 

#include "../../../Point/Point3D/Point3D.h"

namespace Engine
{
	namespace Paths
	{
		namespace Checkpoints
		{
			class Checkpoint
			{
			public:
				float m_angle;
				Point3D m_position;
				Point3D m_direction;

				Checkpoint(const Checkpoint* p_current)
				{
					m_position = p_current->m_position;
					m_angle = p_current->m_angle;
				}


				Checkpoint(Point3D p_position, float p_angle = 0)
				{
					m_position = p_position;
					m_angle = p_angle;
				}

				Checkpoint(Point3D p_position,  Point3D p_direction)
				{
					m_position = p_position;
					m_direction = p_direction;
				}

				Checkpoint()
				{
					m_position = Point3D(0);
					m_direction = Point3D(0);
				}

				void AddIndex(int p_index)
				{
					m_linkedCheclPointsIndex.push_back(p_index);
				}

				int GetAMountCheckpoints()
				{
					return ((int) m_linkedCheclPointsIndex.size());
				}

				int GetIndex(int p_index)
				{
					return m_linkedCheclPointsIndex[p_index];
				}

			private:
				vector<int> m_linkedCheclPointsIndex;

			};
		}// Checkpoint
	}// Camera
}// Engine