#pragma once

#include <vector>
#include <assert.h>
#include <stdio.h>

#include <ObservableEvents/ObservableEventsList/ObservaleEventsList.h>
#include <ObservableEvents/ExternalEventMonitor/ExternalEventMonitor.h>

#include "../../Integrators/IrradianceCache.h"
#include "../ObservableEvents/InsertEvent.h"
#include "../PeerInfo/Update.h"
#include "../../AccelerationStructures/CachingStructure/LockingOctree.h"

#include "../ConreteNeighborManagers/StatePeer.h"


using Engine::LockingOctree;
using Network::ExternalEventMonitor::ExternalEventMonitor;
using Engine::Networking::Neighbours::StatePeer;

namespace Engine
{
	namespace Networking
	{
		namespace Monitor
		{
			template<typename T = RadiancePoint, typename S = IrradianceCache, typename U = Update, typename AS = LockingOctree, typename N = StatePeer>
			class UpdateMonitor : public Network::ExternalEventMonitor::ExternalEventMonitor<T, S, U, AS, N>
			{
			public:
				using ExternalEventMonitor<T, S, U,AS, N>::PushInternalEvent;
				using ExternalEventMonitor<T, S, U, AS, N>::PushInternalEvents;
				using ExternalEventMonitor<T, S, U, AS, N>::QueryForInternalEvent;
				using ExternalEventMonitor<T, S, U, AS, N>::CanGenerateExternalEvent;
				using ExternalEventMonitor<T, S, U, AS, N>::m_localUpdates;
				using ExternalEventMonitor<T, S, U, AS, N>::GenerateExternalEvent;
				using ExternalEventMonitor<T, S, U, AS, N>::CheckForUpdates;
				using ExternalEventMonitor<T, S, U, AS, N>::StartMonitoring;
				using ExternalEventMonitor<T, S, U, AS, N>::m_sleepTime;

				const int MAX_NUMBER_LOCAL_UPDATES = 100;
				
				UpdateMonitor(AbstarctPeer<RadiancePoint,IrradianceCache,Update, AS, N>* p_peer, IrradianceCache* p_IC, ObservableEventsList<T,S,AS>* p_updatesList, int p_testPerAmount = 200, bool p_mode = false) : ExternalEventMonitor(p_peer,p_testPerAmount, p_mode )
				{
					m_updatesList = p_updatesList;
					m_IC = p_IC;
					StartMonitoring();
				}


				virtual void PushInternalEvents(vector<RadiancePoint*> p_points)
				{
					for (auto const &iter : p_points)
					{
						m_localUpdates.push_back(iter);

						if (CanGenerateExternalEvent())
						{
							cout << "Generating observable event" << endl;
							GenerateExternalEvent();
						}
					}
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Pushes an internal event. </summary>
				///
				/// <remarks>	Adrian, 23/09/2015. </remarks>
				///
				/// <param name="p_internalEvent">	[in,out] If non-null, the internal event. </param>
				///-------------------------------------------------------------------------------------------------

				virtual void PushInternalEvent(RadiancePoint* p_internalEvent)
				{
					m_localUpdates.push_back(p_internalEvent);

					if (m_localUpdates.size() >= MAX_NUMBER_LOCAL_UPDATES)
					{
						GenerateExternalEvent();
					}
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Queries for internal event. </summary>
				///
				/// <remarks>	Adrian, 23/09/2015. </remarks>
				///-------------------------------------------------------------------------------------------------

				virtual void QueryForInternalEvent()
				{
					while (m_monitoringMode)
					{
						if (CheckForUpdates())
						{
							vector<RadiancePoint*> updatesVector = m_IC->GetLocalUpdates();
							for (int index = 0; index < updatesVector.size(); index++)
							{
								PushInternalEvent(updatesVector[index]);
							}
						}

						this_thread::sleep_for(std::chrono::milliseconds(m_sleepTime));
					}
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Determine if we can generate external event. </summary>
				///
				/// <remarks>	Adrian, 23/09/2015. </remarks>
				///
				/// <returns>	true if we can generate external event, false if not. </returns>
				///-------------------------------------------------------------------------------------------------

				virtual bool CanGenerateExternalEvent()
				{
					if (m_localUpdates.size() >= MAX_NUMBER_LOCAL_UPDATES)
						return true;
					else
						return false;
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Generates an external event. </summary>
				///
				/// <remarks>	Adrian, 24/09/2015. </remarks>
				///-------------------------------------------------------------------------------------------------

				virtual void GenerateExternalEvent()
				{
					
					ObservableEvent<RadiancePoint, IrradianceCache, AS>* currentUpdate = new ObservableEvents::InsertEvent<RadiancePoint, IrradianceCache, AS>(m_localUpdates, m_IC->GetOctree(), OBSERVABLE_PRODUCED_TYPE::INTERNAL_UPDATE);
					m_updatesList->AddEventToList(currentUpdate);
					m_localUpdates.clear();

					int amountLocalUpdates = m_localUpdates.size();

					#ifdef _DEBUG
						assert(amountLocalUpdates == 0);
					#endif
				}

				bool CheckForUpdates()
				{
					return (GetIC()->CheckForUpdates());
				}

				Engine::IrradianceCache* GetIC()
				{
					return this->m_IC;
				}

				~UpdateMonitor()
				{}

			private:
				ObservableEventsList<RadiancePoint, IrradianceCache, AS>* m_updatesList;
				int m_CheckTime;
				Engine::IrradianceCache* m_IC;
			};
					
		}
	}
}
