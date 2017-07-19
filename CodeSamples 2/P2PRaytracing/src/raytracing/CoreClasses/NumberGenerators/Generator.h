#pragma once 


#include <boost/math/distributions/exponential.hpp>
#include <math.h>


namespace Engine
{
	namespace NumberGenerators
	{
		namespace ExponentialDistibution
		{
			const float MIN_BOUNDS = 0.1f;
			const float MAX_BOUNDS = 3.0f;


			static float GetWeightAtInterval(float p_interval)
			{
				auto const lambda = 1.0;
				auto d = boost::math::exponential_distribution<>{ lambda };
				return static_cast<float>(boost::math::pdf(d, p_interval));
			}

			static float ComputeIntervalSize(float p_max, float p_min, int p_amount)
			{
				float acceptedRange = p_max - p_min;
				return acceptedRange / (float)p_amount;
			}

			static float GetIntervalSize(int p_amountIntervals)
			{
				return ComputeIntervalSize(MAX_BOUNDS, MIN_BOUNDS, p_amountIntervals);
			}
		}
	}
}