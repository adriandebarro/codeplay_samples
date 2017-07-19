#ifndef __MATRIX_3__
#define __MATRIX_3__

#include "..\Point\Point3D\Point3D.h"

namespace Engine
{
	class Matrix3
	{
	public:

		//build rotation matrix 
		Matrix3(Point3D& p_currentNormal, Point3D& p_toNormal)
		{
			BuildRotationMatrix(p_currentNormal, p_toNormal);
		}



		Matrix3(bool p_setIdenity = false)
		{
			if (p_setIdenity)
				SetIdentity();
		}

		Matrix3(double p_matrixNumbers[])
		{
		}

		Matrix3 GetIdentity()
		{
			return Matrix3(true);
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

		//arbitrary axis rotiation throug the cross product
		void BuildRotationMatrix(Point3D& p_currentNormal, Point3D& p_newNormal)
		{
			Point3D& arbitraryAxis = p_currentNormal.CrossProduct(p_newNormal);
			double angleRotation = p_currentNormal.dot(p_newNormal);
			double c = cos(angleRotation);
			double s = sin(angleRotation);

			int index = 0;
			m_matrix[index] = c + (1 - c)*arbitraryAxis.x;
			index++;
			m_matrix[index] = (1 - c)*arbitraryAxis.x*arbitraryAxis.z - s * arbitraryAxis.z;
			index++;
			m_matrix[index] = (1 - c)* arbitraryAxis.x * arbitraryAxis.y - s * arbitraryAxis.z;
			index++;
			m_matrix[index] = (1 - c) * arbitraryAxis.x * arbitraryAxis.y + s * arbitraryAxis.z;
			index++;
			m_matrix[index] = c + (1 - c) * arbitraryAxis.y * arbitraryAxis.y;
			index++;
			m_matrix[index] = (1 - c) * arbitraryAxis.y * arbitraryAxis.z - s * arbitraryAxis.z;
			index++;
			m_matrix[index] = (1 - c) * arbitraryAxis.x * arbitraryAxis.z - s * arbitraryAxis.y;
			index++;
			m_matrix[index] = c + (1 - c) * arbitraryAxis.z * arbitraryAxis.z;
		}

		//constant scaling along all axis
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

		//non constant scaling 
		void SetIdentity(double& x_constant, double& y_constant, double& z_constant)
		{
			int index = 0;
			for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
			{
				if (x == y)
				{
					if (index == 0)
						m_matrix[index] = x_constant;
					else if (index == 1)
						m_matrix[index] = y_constant;
					else
						m_matrix[index] = z_constant;
				}
				else
					m_matrix[index] = 0;

				index++;
			}
		}

		//matrix mulitplication
		inline Matrix3 operator * (Matrix3& rhs)
		{
			//TODO fill this
		}

		//matrix multiplied with a point
		inline Point3D operator *(Point3D& rhs)
		{
			Point3D output;

			for (int indexY = 0; indexY < 4; indexY++)
			{
				for (int indexX = 0; indexX < 4; indexX++)
				{
					switch (indexY)
					{
					case 0:
						output.x += GetNumberAt(indexX, indexY) * rhs[indexX];
						break;
					case 1:
						output.y += GetNumberAt(indexX, indexY) * rhs[indexX];
						break;
					case 2:
						output.z += GetNumberAt(indexX, indexY) * rhs[indexX];
						break;
					}
				}
			}

			return output;

		}

		inline Matrix3& operator* (double& p_constant)
		{
			for (int index = 0; index < 4 * 4; index++)
			{
				m_matrix[index] *= p_constant;
			}
		}

		double& GetNumberAt(int& x, int& y)
		{
			int location = y * MATRIX_3_SIZE;
			location += x;
			return m_matrix[location];
		}

		Matrix3 GetIdenitytMatrix()
		{
			return Matrix3(true);
		}

	private:
		const static int MATRIX_3_SIZE = 3;

		double m_matrix[3 * 3];
	};
}

#endif // !__MATRIX_4__
