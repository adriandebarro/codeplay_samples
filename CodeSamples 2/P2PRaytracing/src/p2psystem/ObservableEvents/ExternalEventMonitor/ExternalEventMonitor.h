///-------------------------------------------------------------------------------------------------
// file:	ObservableEvents\ExternalEventMonitor\ExternalEventMonitor.h
//
// summary:	Declares the external event monitor class
///-------------------------------------------------------------------------------------------------

#pragma once 

#include <vector>
#include <thread>
#include <iostream>

#include "../../Peer/AbstractPeer.h"
#include "../ObservableEventsList/ObservaleEventsList.h"

using std::vector;
using std::thread;
using std::cout;
using std::endl;

using namespace Network::Peer;

namespace Network
{
	namespace ExternalEventMonitor
	{
		///-------------------------------------------------------------------------------------------------
		/// <summary>	An external event monitor. </summary>
		///
		/// <remarks>	Adrian, 23/09/2015. </remarks>
		///
		/// <typeparam name="T">	Generic type parameter. </typeparam>
		///-------------------------------------------------------------------------------------------------

		template<typename T, typename S, typename U, typename AS, typename N>
		class ExternalEventMonitor
		{
		public:

			static ObservableEvents::ObservableEventsList<T,S,AS>* m_ObservableEventsList;

			ExternalEventMonitor()
			{
				
			}

			ExternalEventMonitor(AbstarctPeer<T,S,U,AS,N>* p_peer, int p_testPerAmount = 200, bool p_monitoringMode = false)
			{
				//m_updateList = p_observableList;
				m_peer = p_peer;
				m_monitoringMode = p_monitoringMode;
				m_sleepTime = p_testPerAmount;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Pushes an internal event. </summary>
			///
			/// <remarks>	Adrian, 23/09/2015. </remarks>
			///
			/// <param name="p_internalEvent">	[in,out] If non-null, the internal event. </param>
			///-------------------------------------------------------------------------------------------------

			virtual void PushInternalEvent(T* p_internalEvent) = 0;

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Pushes an internal events. </summary>
			///
			/// <remarks>	Adrian, 24/09/2015. </remarks>
			///
			/// <param name="p_internalEvents">	[in,out] If non-null, the internal events. </param>
			///-------------------------------------------------------------------------------------------------

			virtual void PushInternalEvents(vector<T*> p_internalEvents) = 0;

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Queries for internal event. </summary>
			///
			/// <remarks>	Adrian, 23/09/2015. </remarks>
			///-------------------------------------------------------------------------------------------------

			virtual void QueryForInternalEvent() = 0;

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Determine if we can generate external event. </summary>
			///
			/// <remarks>	Adrian, 23/09/2015. </remarks>
			///
			/// <returns>	true if we can generate external event, false if not. </returns>
			///-------------------------------------------------------------------------------------------------

			virtual bool CanGenerateExternalEvent() = 0;

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Generates an external event. </summary>
			///
			/// <remarks>	Adrian, 24/09/2015. </remarks>
			///-------------------------------------------------------------------------------------------------

			virtual void GenerateExternalEvent() = 0;

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Enables the monitoring mode. </summary>
			///
			/// <remarks>	Adrian, 23/09/2015. </remarks>
			///-------------------------------------------------------------------------------------------------

			void EnableMonitoringMode()
			{
				m_monitoringMode = true;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Query if this object is monitoring. </summary>
			///
			/// <remarks>	Adrian, 23/09/2015. </remarks>
			///
			/// <returns>	true if monitoring, false if not. </returns>
			///-------------------------------------------------------------------------------------------------

			bool IsMonitoring()
			{
				return m_monitoringMode;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Starts a monitoring. </summary>
			///
			/// <remarks>	Adrian, 23/09/2015. </remarks>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			///-------------------------------------------------------------------------------------------------

			bool StartMonitoring()
			{
				if (m_monitoringMode)
				{
					cout << "starting to monitor" << endl;
					m_monitoringThread = new thread(&ExternalEventMonitor::QueryForInternalEvent, this);
				}
				else
				{
					cout << "Monitor cannot be initiated" << endl;
				}
				return true;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Stops a monitoring. </summary>
			///
			/// <remarks>	Adrian, 23/09/2015. </remarks>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			///-------------------------------------------------------------------------------------------------

			void StopMonitoring()
			{
				m_monitoringMode = false;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Determines if we can check for updates. </summary>
			///
			/// <remarks>	Adrian, 24/09/2015. </remarks>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			///-------------------------------------------------------------------------------------------------

			virtual bool CheckForUpdates() = 0;


		protected:
			/// <summary>	true to enable monitoring mode, false to disable it. </summary>
			bool m_monitoringMode;
			/// <summary>	The peer. </summary>
			AbstarctPeer<T,S,U,AS,N>* m_peer;
			/// <summary>	List of updates. </summary>
			ObservableEvents::ObservableEventsList<T,S,AS>* m_updateList;
			/// <summary>	The local updates. </summary>
			vector<T*> m_localUpdates;
			/// <summary>	The monitoring thread. </summary>
			thread* m_monitoringThread;

			int m_sleepTime;
			
		};
	}
}