#pragma once 

#include <iostream>
#include <omp.h>
#include <intrin.h>
#include <mmintrin.h>
#include <emmintrin.h>

#include <System/Application.h>
#include <Common/Common.h>
#include <System/Util.h>
#include <Maths/Vector2.h>

using namespace Core::Common;
using namespace Core::Maths::Vector;

namespace NBody
{
	namespace Application
	{
		class OpenMPApplication : public Core::Application::Application
		{
			private:

				virtual void ExecuteApplication();
				void ConvertToStructureOfArrays();
				void MergeResults();
				void ComputeForces();
				
				a16_flt *m_xCoor, *m_yCoor;
				a16_flt *m_xVel, *m_yVel;
				a16_flt *m_bodyMass;
				a16_flt *m_forceX, *m_forceY;
				a16_int **m_mask;
				Body *m_allBodies;
				int m_bodyCount;

			public:

				OpenMPApplication(std::string, float, int);

				virtual bool Initialise();
				virtual void Execute();
				virtual void Terminate();
		};
	}

}
