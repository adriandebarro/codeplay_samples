#pragma once 

#include "../../CoreClasses/Point/Point3D/Point3D.h"

namespace Engine
{
	namespace Networking
	{
		namespace Contexts
		{
			struct FrustumRepresentation
			{
				FrustumRepresentation(Point3D p_direction, Point3D p_position, float p_fov)
				{
					dir = p_direction;
					eye = p_position;
					m_fov = p_fov;
				}

				Point3D pos2;
				Point3D pos3;
				int m_fov;
				Point3D dir;
				Point3D eye;
			};
		}
	}
}