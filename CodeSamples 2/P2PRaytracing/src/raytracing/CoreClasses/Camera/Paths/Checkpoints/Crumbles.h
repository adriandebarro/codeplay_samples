#pragma once

#include "Checkpoints.h"
#include "../../Point/Point3D/Point3D.h"

namespace Engine
{
	namespace Paths
	{
		namespace Checkpoints
		{
			namespace Crumbles
			{
				class Crumble : public Checkpoint
				{
				public:
					Crumble(Point3D p_position, float p_rotationAngle) : Checkpoint(p_position, p_rotationAngle)
					{	}

					Crumble(Point3D p_position, Point3D p_direction) : Checkpoint(p_position, p_direction)
					{	}
				};
			}
		}
	}
}