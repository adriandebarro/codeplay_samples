#pragma once

#include "Sampler.h"
#include "..\Point\Point2D\Point2D.h"

namespace Engine
{
	namespace Sampling
	{
		class RandomSampler : public Engine::Sampler
		{
		public:
			RandomSampler(int p_amountSamples, int p_amountSets, float p_cosineWeighting) : Sampler(p_amountSamples, p_amountSets, p_cosineWeighting)
			{}
			~RandomSampler();

			using Sampler::GenerateSamples;
			virtual void GenerateSamples()
			{
				_ASSERT(numSets != 0 && numSamples != 0);

				for (int sampleSet = 0; sampleSet < numSets; ++sampleSet)
				{
					for (int currentSample = 0; currentSample < numSamples; currentSample++)
					{
						Point2D sample;
						sample.x = twister.Rand();
						sample.y = twister.Rand();
						samples.push_back(sample);
					}
				}
			}
		private:

		};
	}
}
