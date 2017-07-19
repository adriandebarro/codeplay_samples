#pragma once 

#include "..\..\Point\Point2D\Point2D.h"

namespace Engine
{
	class Tile
	{
	public:
		int m_width;
		int m_height;
		Point2D m_startingPoint;

		Tile(Point2D p_startingPoint, int p_width, int p_height)
		{
			m_startingPoint = p_startingPoint;
			m_width = p_width;
			m_height = p_height;
		}

		string ToString()
		{
			return m_startingPoint.toString() + "----" + to_string(m_width) + "-----" + to_string(m_height);
		}
	private:
	};
}