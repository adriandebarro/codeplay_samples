#pragma once 

#include "CommunicationLogger.h"
#include "../../../Integrators/RadianceDetails/RadiancePoint.h"
#include "../../../Integrators/IrradianceCache.h"


using Engine::Logger::CommunicationLogger;

using Engine::RadiancePoint;
using Engine::IrradianceCache;

namespace Engine
{
	namespace Logs
	{
		namespace CommunicationLogs
		{
			///-------------------------------------------------------------------------------------------------
			/// <summary>	This . </summary>
			///
			/// <remarks>	Adrian, 30/01/2016. </remarks>
			///-------------------------------------------------------------------------------------------------

			class CommunicationServerLogger : public CommunicationLogger
			{
			public:

				CommunicationServerLogger()
				{

				}

				using CommunicationLogger::AddCommunicationDetails;
				void AddCommunicationDetails(vector<ObservableEvent<RadiancePoint, IrradianceCache, LockingOctree>>& p_allUpdates, Neighbour* p_neighborDetails)
				{
					return;
				}

			private:
			};
		}
	}
}