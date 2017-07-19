#pragma once 

#include <string>
#include <Peer/Peer2Peer-Peer/P2pPeer.h>
#include <Peer/AbstractPeer.h>
#include <ObservableEvents/ObservableEventsList/ObservaleEventsList.h>
#include <SuperPeer\SuperPeerManager.h>


#include "../ObservableEventsMonitor/ICUpdatesMonitor.h"
#include "../ObservableEventsList/ICObservableEventsList.h"
#include "../../Integrators/RadianceDetails/RadiancePoint.h"
#include "../ConreteNeighborManagers/PdfNeighborManager.h"

#include "../Contexts/ContextUpdater.h"
#include "../Contexts/ContextInitializer.h"
#include "../Contexts/ContextStrategy.h"
#include "../ConreteNeighborManagers/StatePeer.h"
#include "../Contexts/ContextDataParser.h"

using namespace Network::Peer;
using namespace Engine::Networking::Monitor;
using std::string;

using Engine::Networking::Neighbors::PdfNeighborManager;
using Engine::Networking::Contexts::ContextInitializer;
using Engine::Networking::Contexts::ContextUpdater;
using Network::SuperPeers::SuperPeerManager;
using Engine::Networking::Contexts::ContextStrategy;
using Engine::Networking::Neighbours::StatePeer;

namespace Engine
{
	namespace Networking
	{

		enum PEER_TYPE
		{
			TESTING = 0
		};
		using Engine::Networking::Contexts::NetworkPropertiesParser;

		///-------------------------------------------------------------------------------------------------
		/// <summary>	A peer initializer. </summary>
		///
		/// <remarks>	Adrian, 24/09/2015. </remarks>
		///-------------------------------------------------------------------------------------------------
		template<typename T, typename S, typename U, typename AS>
		class PeerInitializer
		{
		public:

			PeerInitializer()
			{}


			//TODO need to specify the time periods for the updater and the initializer and the period for checking 
			
			AbstarctPeer<T,S, U, AS, StatePeer>* InitializePeer(int p_amountPeers, int p_cacheSizePeer ,string p_ipAddress, int p_socketNumber, int p_sequenceNumber, 
													string p_nameAssigned, int p_peerIndex, int p_integratorType, Integrator* p_Ic,  Camera* p_camera,
													SceneManager* p_sceneManager, BoundingBox* p_limitsBoundingBox , string p_boundingBoxesPath, clock_t* p_startingTime ,float p_fltSleepAfterComm = 2.0f, float p_fltSleepAfterFailedComm = 2.0f, 
													bool p_monitoringMode = true, bool p_oracleInitializer = false, 
													bool p_queuedEvents = false, bool p_logToServer = false, string p_hostPath = "", int p_connectionTier = 0, string p_contextParamDetails = "")
			{

			
				//might have to fix this for invalidations 
				if (p_oracleInitializer)
				{
					std::cout << "\t\t\t context mode \t\t\t" << std::endl;

					m_contextProperties = new NetworkPropertiesParser(p_contextParamDetails);
					m_neighborManager = new PdfNeighborManager<StatePeer>(m_contextProperties, p_cacheSizePeer);
					std::atomic_store(&m_latency, -1);
					m_superPeer = new SuperPeerManager(p_connectionTier, &m_latency);
					m_contextStrategy = new ContextStrategy(m_contextProperties);

					//need the 
					m_contextUpdater = new ContextUpdater(m_superPeer, (PdfNeighborManager<StatePeer>*)m_neighborManager, m_contextProperties->m_peerUpdatePeriod, p_startingTime, p_boundingBoxesPath, p_limitsBoundingBox, p_connectionTier, p_camera, &m_latency, m_contextProperties);
				}

				if (p_integratorType == 0)
				{
					m_observableEventsList = new Engine::Networking::ObservableEvents::ICObservableEventsList<T, S, AS>(
						p_peerIndex,
						p_amountPeers,
						(IrradianceCache*)p_Ic,
						p_sceneManager, 
						p_hostPath,
						p_queuedEvents);
				}
				else
				{
					m_observableEventsList = new Engine::Networking::ObservableEvents::ICObservableEventsList<T, S, AS>(
						p_peerIndex,
						p_amountPeers,
						((TwoLayerIrradianceCache*)p_Ic)->GetGlobalIC(),
						((TwoLayerIrradianceCache*)p_Ic)->GetLocalIC(),
						p_sceneManager,
						p_queuedEvents,
						p_logToServer,
						p_hostPath);
				}
			


				if (p_oracleInitializer)
				{
					((ICObservableEventsList<T, S, AS>*) m_observableEventsList)->SetBoundingBoxAnalyser(m_contextUpdater->GetBoundingBoxAnalyser());
					m_peer = new P2pPeer<T, S, U, AS, StatePeer>(
						p_ipAddress,
						p_socketNumber,
						p_sequenceNumber,
						p_nameAssigned,
						m_observableEventsList,
						p_amountPeers,
						p_fltSleepAfterComm,
						p_fltSleepAfterFailedComm, (NeighbourManager<StatePeer>*)m_neighborManager, m_superPeer, true);
				}
				else
				{
					std::cout << "\t\t\t peer to peer mode  \t\t\t" << std::endl;
					m_neighborManager = new NeighbourManager<StatePeer>(p_cacheSizePeer);
					m_peer = new P2pPeer<T, S, U, AS, StatePeer>(
						p_ipAddress,
						p_socketNumber,
						p_sequenceNumber,
						p_nameAssigned,
						m_observableEventsList,
						p_amountPeers,
						p_fltSleepAfterComm,
						p_fltSleepAfterFailedComm, m_neighborManager, nullptr, false);
				}


				if (p_oracleInitializer)
				{
					m_contextInitializer = new ContextInitializer(m_contextStrategy, m_superPeer, &m_contextUpdater->m_continueUpdating, 
						m_contextProperties->m_contextInitializerPeriod, ((P2pPeer<T, S, U, AS, StatePeer>*)m_peer)->GetCurrentVectorTimer(), 
							m_contextProperties->m_enabledContexts, p_peerIndex, p_hostPath);
				}

				//m_peer->SetNeighborManager((NeighbourManager<StatePeer>*)m_neighborManager);

				m_monitor = new UpdateMonitor<T, S ,U, AS>(m_peer, p_integratorType == 0 ? (IrradianceCache*)p_Ic : ((TwoLayerIrradianceCache*)p_Ic)->GetGlobalIC(), m_observableEventsList, 200, true);
				
				return m_peer;
			}


			ObservableEventsList<T,S,AS>* GetObservableEventsList()
			{
				return m_observableEventsList;
			}

			AbstarctPeer<T,S, U,AS>* GetPeer()
			{
				return m_peer;
			}

			UpdateMonitor<T, S, U, AS,StatePeer>* GetMonitor()
			{
				return m_monitor;
			}

			bool IsIpSet()
			{
				return m_peer->ipSet;
			}

			string GetIpAddress()
			{
				return m_peer->m_ipAddress;
			}

			void StopCommunication()
			{
				m_peer->Exitlistening();
			}

			void PushPendingUpdates()
			{
				m_observableEventsList->PushQueuedEvents();
			}

			void AddInvalidationEvents(vector<InvalidationDetails*> &p_invalidationDetails, int p_invalidationType, int p_currentPosition)
			{
				try
				{
					m_observableEventsList->AddInvalidationEvents(p_invalidationDetails, p_invalidationType, p_currentPosition);
				}
				catch(exception ex)
				{
					cout << ex.what() << endl;
				}
			}

			ContextInitializer* GetContextInitializer()
			{
				return m_contextInitializer;
			}

			ContextUpdater* GetContextUpdater()
			{
				return m_contextUpdater;
			}

			int GetBoudningBoxIndex(Point3D p_position)
			{
				return m_contextUpdater->GetBoundingBoxAnalyser()->CheckIntersectionIndex(p_position);
			}

		private:
			ObservableEventsList<T,S,AS>* m_observableEventsList;
			AbstarctPeer<T,S,U,AS, StatePeer>* m_peer;
			UpdateMonitor<T, S, U, AS,StatePeer>* m_monitor;
			string p_peerData;
			ContextInitializer* m_contextInitializer;
			ContextUpdater* m_contextUpdater;
			SuperPeerManager* m_superPeer;
			ContextStrategy* m_contextStrategy;
			bool* m_continue;
			NeighbourManager<StatePeer>* m_neighborManager;
			std::atomic<int> m_latency;
			NetworkPropertiesParser* m_contextProperties;
		};
	}
}
