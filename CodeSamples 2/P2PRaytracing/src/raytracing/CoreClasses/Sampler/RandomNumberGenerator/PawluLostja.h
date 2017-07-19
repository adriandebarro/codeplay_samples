#pragma once 

#include <random>

namespace Engine
{
	class Twister
	{
		public:
			
		void Seed(unsigned long seed);
		Twister(unsigned long seed)
		{
			if (seed) { Seed(seed); }
			else { Seed((unsigned long)0xf2710812); }
		}
		Twister() { Seed((unsigned long)0xf2710812); }
		float Rand();
		
	};

	void Twister::Seed(unsigned long seed)
	{
		std::srand(seed);
	}

	float Twister::Rand()
	{
		return ((float)std::rand())/ RAND_MAX;
	}


} // namespace Raytracer
