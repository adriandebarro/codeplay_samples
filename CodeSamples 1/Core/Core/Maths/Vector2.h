#pragma once 

#include <iostream>
#include <assert.h>

#include "../Common/Common.h"

namespace Core
{	
	namespace Maths
	{
		namespace Vector
		{
			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	A 2D representation of a vector. </summary>
			///	
			/// <remarks>	Adrian, 08/07/2017. </remarks>
			////////////////////////////////////////////////////////////////////////////////////////////////////

			///-------------------------------------------------------------------------------------------------
			/// <summary>	A vector 2. </summary>
			///
			/// <remarks>	Adrian, 13/07/2017. </remarks>
			///-------------------------------------------------------------------------------------------------

			class Vector2
			{
			public:
					
				union 
				{
					float element[2];
					struct { float m_x, m_y; };
					struct { float X, Y; };
				};

				CUDA_CALLABLE_MEMBER Vector2():
					m_x(0.f), m_y(0.f) {}

				CUDA_CALLABLE_MEMBER Vector2(float m_val) :
					m_x(m_val), m_y(m_val) {}


				CUDA_CALLABLE_MEMBER Vector2(float p_x, float p_y)
				{
					m_x = p_x;
					m_y = p_y;
				}

				CUDA_CALLABLE_MEMBER Vector2(const Vector2 &p_vector):
					m_x(p_vector.X), m_y(p_vector.Y) {}

				CUDA_CALLABLE_MEMBER Vector2& operator = (const Vector2& p_vector)
				{
					m_x = p_vector.X;
					m_y = p_vector.Y;
					return *this;
				}

				CUDA_CALLABLE_MEMBER inline Vector2 operator + (const Vector2& p_vector) const
				{
					return Vector2(m_x + p_vector.X, m_y + p_vector.Y);
				}

				CUDA_CALLABLE_MEMBER inline Vector2 operator - (const Vector2& p_vector) const
				{
					return Vector2(m_x - p_vector.X, m_y - p_vector.Y);
				}

				CUDA_CALLABLE_MEMBER inline Vector2 operator * (const float p_value) const
				{
					_ASSERT(p_value > KEpsillon);
					return Vector2(m_x * p_value, m_y * p_value);
				}

				CUDA_CALLABLE_MEMBER inline Vector2 operator / (const float p_value) const
				{
					_ASSERT(p_value > KEpsillon);
					return Vector2(m_x / p_value, m_y / p_value);
				}

				CUDA_CALLABLE_MEMBER inline bool operator!=(const Vector2& p_vector)const
				{
					return !(*this == p_vector);
				}

				CUDA_CALLABLE_MEMBER inline bool operator==(const Vector2& p_vector) const
				{	
					return Equals(p_vector);
				}

				CUDA_CALLABLE_MEMBER inline bool Equals(const Vector2& p_vector, float p_kEpsillon = 1e-5f) const
				{
					if (std::fabsf(m_x - p_vector.X) < p_kEpsillon) 
						return false;
					if (std::fabsf(m_y - p_vector.Y) < p_kEpsillon)
						return false;
					return true;
				}

				CUDA_CALLABLE_MEMBER inline float DoubleLength()
				{
					return m_x * m_x + m_y * m_y;
				}

				CUDA_CALLABLE_MEMBER inline float Length()
				{
					return std::sqrt(m_x * m_x + m_y * m_y);
				}

				CUDA_CALLABLE_MEMBER inline Vector2& operator += (const Vector2 p_vector)
				{
					return *this = *this + p_vector;
				}

			};
		}
	}
}
