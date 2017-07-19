#ifndef __MATRIX_4__
#define __MATRIX_4__

#include "..\Point\Point3D\Point3D.h"

namespace Engine
{
	class Matrix4
	{
	public:
		Matrix4(bool p_setIdenity = false)
		{
			if (p_setIdenity)
				SetIdentity();
		}

		Matrix4(double p_matrixNumbers[])
		{
		}

		Matrix4& GetIdentity()
		{
			return Matrix4(true);
		}

		void SetIdentity()
		{
			int index = 0;
			for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
			{
				if (x == y)
					m_matrix[index] = 1;
				else
					m_matrix[index] = 0;

				index++;
			}
		}

		void SetIdentity(double& p_constant)
		{
			int index = 0;
			for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
			{
				if (x == y)
					m_matrix[index] = p_constant;
				else
					m_matrix[index] = 0;

				index++;
			}
		}

		void Scale(int& p_nu)
		{

		}


		inline Matrix4 operator * (Matrix4& rhs)
		{
			//TODO fill this
		}

		inline Point3D& operator *(Point3D& rhs)
		{
			Point3D output;

			for (int indexY = 0; indexY < 4; indexY++)
			{
				for (int indexX = 0; indexX < 4; indexX++)
				{

				}
			}

		}

		inline Matrix4& operator* (double& p_constant)
		{
			for (int index = 0; index < 4 * 4; index++)
			{
				m_matrix[index] *= p_constant;
			}
		}

		double& GetNumberAt(int& x, int& y)
		{
			int location = y * 4;
			location += x;
			return m_matrix[location];
		}

		Matrix4& GetIdenitytMatrix()
		{
			Matrix4 identity;

		}

		//rotating on an arbitrary axis 
		void RotateByAngleTheta(double& p_angle, Point3D& p_arbitraryAxis)
		{

		}

	private:
		const static int MATRIX_4_SIZE = 4;

		double m_matrix[4 * 4];
	};
}

#endif // !__MATRIX_4__
