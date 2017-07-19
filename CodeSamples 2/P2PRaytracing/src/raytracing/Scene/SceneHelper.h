#ifndef __SCENE_HELPER_H__
#define __SCENE_HELPER_H__

#include <vector>

#include "..\Light\Light.h"
#include "..\Light\PointLight.h"

using std::vector;

namespace Engine
{
	class SceneHelper
	{
	public:
		static void CreateLineLight(vector<Engine::Light*>& p_lightContainer, Point3D p_startingPoistion, int p_amount, int p_direction, float p_spacing, int p_lightType)
		{
			Point3D currentPosition = p_startingPoistion;

			for (int index = 0; index < p_amount; index ++)
			{
				PointLight* currentLight = new PointLight(currentPosition, RGBColor(1), 2, 15);
				p_lightContainer.push_back(currentLight);

				// x  direction
				if (p_direction == 0)
				{
					currentPosition.x += p_spacing;
				}
				//y direction
				else if (p_direction == 1)
				{
					currentPosition.y += p_spacing;
				}
				//in z directiom
				else
				{
					currentPosition.z += p_spacing;
				}
			}
		}

	private:
	};
}

#endif // !__SCENE_HELPER_H__
