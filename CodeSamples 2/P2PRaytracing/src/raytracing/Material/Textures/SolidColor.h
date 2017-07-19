#ifndef __SOLID_COLOR_H__
#define __SOLID_COLOR_H__

#include "Texture.h"
#include "..\..\CoreClasses\RGBObject\RGBColor.h"

namespace Engine
{
	class SolidColor : public Texture
	{
	public:

		SolidColor(RGBColor p_color) : Texture()
		{
			m_color = p_color;
			m_containsTexture = false;
		}
		
		using Texture::GetColor;
		virtual RGBColor GetColor(float p_x, float p_y)
		{
			return m_color;
		}

	private:

		RGBColor m_color;
	};
}

#endif // !__SOLID_COLOR_H__
