#pragma once 

#include "../PeerInfo/Update.h"

#include <boost/math/distributions/exponential.hpp>

namespace Engine
{
	namespace Networking
	{
		namespace Metrics
		{
			///-------------------------------------------------------------------------------------------------
			/// <summary>	A metric interface. </summary>
			///
			/// <remarks>	Adrian, 08/12/2015. </remarks>
			///-------------------------------------------------------------------------------------------------
			template<typename T>
			class Metric
			{
			public:

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Process the updates received. </summary>
				///
				/// <remarks>	Adrian, 08/12/2015. </remarks>
				///
				/// <param name="p_updatesToProcess">	[in,out] The updates to process. </param>
				///-------------------------------------------------------------------------------------------------

				virtual void ProcessUpdates(vector<T*> &p_clusteringPoints, vector<Update*> &p_updatesToProcess, string p_localLocation) = 0;
				virtual void WeightUpdates(vector<T*> &p_clusteringPoints) = 0;

				//virtual vector<T*> GetClusters(vector<T*> &p_clusteringPoints) = 0;

			protected:

				float GetWeight(float p_interval)
				{
					auto const lambda = 1.0;
					auto d = exponential_distribution<>{lambda};
					return pdf(d, 0.1);
				}


			};
		}
	}
}