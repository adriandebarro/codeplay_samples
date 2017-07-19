#ifndef __NROOKS__
#define __NROOKS__

#include "..\Sampler\Sampler.h"
#include "..\UtilClasses\Common.h"

namespace Engine
{
	class NRooks : public Sampler
	{
	public:
		void ShuffleXCoordinates()
		{
			for (int p = 0; p < numSets; p++)
			{
				for (int i = 0; i < numSamples - 1; i++)
				{
					int target = GenerateRandomInt() % numSamples + p * numSamples;
					float temp = samples[i + p * numSamples + 1].x;
					samples[i + p * numSamples + 1].x = samples[target].x;
					samples[target].x = temp;
				}
			}
		}

		void ShuffleYCoordinates()
		{
			for (int p = 0; p < numSets; p++)
			{
				for (int i = 0; i < numSamples - 1; i++)
				{
					int target = GenerateRandomInt() % numSamples + p * numSamples;
					float temp = samples[i + p * numSamples + 1].y;
					samples[i + p * numSamples + 1].y = samples[target].y;
					samples[target].y = temp;
				}
			}
		}

		using Sampler::GenerateSamples;
		virtual void GenerateSamples(void)
		{
			for (int p = 0; p < numSets; p++)
			{
				for (int j = 0; j < numSamples; j++)
				{
					Point2D pv;
					pv.x = (j + GenerateRandomFloat()) / numSamples;
					pv.y = (j + GenerateRandomFloat()) / numSamples;

					samples.push_back(pv);
				}
			}

			ShuffleXCoordinates();
			ShuffleYCoordinates();
		}
	};
}

#endif // !__NROOKS__
