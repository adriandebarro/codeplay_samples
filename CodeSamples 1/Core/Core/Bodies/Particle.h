#pragma once 

#include <random>
#include <iostream>
#include "../Maths/Vector2.h"

namespace Core 
{
	namespace Bodies 
	{
		template<typename T>
		struct Particle
		{
			float Mass;
			T Velocity;
			T Position;
			
			Particle():
				Mass(0),
				Position(T(0)),
				Velocity(T(0))
			{}

			Particle(float p_mass, T p_velocity, T p_position):
				Mass(p_mass),
				Velocity(p_velocity),
				Position(p_position)
			{}

			Particle(float p_mass, T p_position):
				Mass(p_mass),
				Position(p_position),
				Velocity(T(0))
			{}
		};

	}

}
