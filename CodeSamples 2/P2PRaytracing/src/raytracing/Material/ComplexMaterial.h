//=======ComplexMaterial.h====================================
#pragma once 

#include <map>

#include "Material.h"

using std::map;


namespace Engine
{
	namespace Materials
	{
		class ComplexMaterial : public Material
		{
		public:
			map<string, Material*> m_materialCollection;

			RGBColor Shade(ShadeRec& sr, std::vector<Light*>& sceneLights, Tracer* p_tracerPtr, RGBColor& p_colorAtPoint, Light* ambient = NULL)
			{
				return RGBColor(0);
			}

			Material* GetMaterialFromMap(string p_materialName)
			{
				map<string, Material*>::iterator materialFound = m_materialCollection.find(p_materialName);

				if (materialFound != m_materialCollection.end())
				{
					return (materialFound->second);
				}
				else
				{
					return NULL;
				}
			}

			ComplexMaterial(map<string, Material*> p_materialMap)
			{
				m_materialCollection = p_materialMap;
			}


		public:
		protected:
		};
	}
}

//=======ComplexMaterial.h====================================