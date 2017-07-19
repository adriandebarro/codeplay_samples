#pragma once 

#include "Checkpoints/Checkpoints.h"

#include <vector>


using std::vector;
using Engine::Paths::Checkpoints::Checkpoint;
namespace Engine
{
	namespace Paths
	{
		///-------------------------------------------------------------------------------------------------
		/// <summary>	A path: Consiting of a list of checkpoints which can be chosen </summary>
		///
		/// <remarks>	Adrian, 10/10/2015. </remarks>
		///-------------------------------------------------------------------------------------------------
		class Path
		{
		public:
			Path()
			{}

			Path(vector<Checkpoint*> p_pathPoints)
			{
				m_checkpoints = p_pathPoints;
			}

			vector<Checkpoint*> GetCheckpoints()
			{
				return m_checkpoints;
			}

			virtual vector<Checkpoint*> GetRandomPath(string p_path)
			{
				return vector<Checkpoint*>();
			}

		protected:
			vector<Checkpoint*> m_checkpoints;

		};
	}
}