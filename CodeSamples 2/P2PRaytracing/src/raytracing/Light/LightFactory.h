#pragma once 

#include <string>

#include "Light.h"
#include "PointLight.h"
#include "Directional.h"

using std::string;

namespace Engine
{
	class LightFactory
	{
	public:
		//need to check what owuld be required 
		static Light* CreateLight(string p_type,string p_lightName, float p_luminance, RGBColor p_color, Point3D p_position, Point3D p_direction = Point3D(0))
		{
			int type = GetLightType(p_type);
			if(type != 0)
			{
				Light* toReturn = nullptr;

				switch (type)
				{
					case 1:
						toReturn = new PointLight(p_position, p_color, 1, p_luminance);
						toReturn->SetLightName(p_lightName);
						break;

					case 2:
						break;

					case 3:
						toReturn = new Directional(p_direction, p_color, 1, p_luminance);
						toReturn->SetLightName(p_lightName); 
						break;
				}
				toReturn->SetLightStatus(LIGHT_STATE::LIGHT_ON);

				return toReturn;
			} else {
				return nullptr;
			}
		}
	private:
		static int GetLightType(string p_stringType)
		{
			if (p_stringType == "PointLight")
				return 1;
			else if (p_stringType == "AreaLight")
				return 2;
			else if (p_stringType == "DirectionalLight")
				return 3;
			else
				return 0;
		}


	public:
	};
}
