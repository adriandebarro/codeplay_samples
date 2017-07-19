#pragma once 

#include <boost/asio.hpp>
#include <boost/uuid/uuid.hpp>
#include <atomic>

#include "../SuperPeer.h"
#include "../SubscribersManager.h"
#include "../../Peer/Neighbors/Neighbour.h"
#include "../../TCP/AsyncTcpServer.h"
#include "../Communicator.h"
#include "../ContextStatus.h"
#include "./StateRepresentation.h"

#include "../../UtilityClasses/Common.h"

//using Network::AsynComm::ContextCommunication::tcp_connection_context;
//using Network::AsynComm::tcp_server_context;
using Network::SuperPeers::Subscribers::SubscriberManager;
using Network::SuperPeers::Subscribers::Subscriber;
namespace Network
{
	namespace Context
	{
		//The template type T refers to the 
		template<typename T>
		class Context
		{
		public:
			//-------------------------------------------------------------
			//
			//-------------------------------------------------------------
			Context(int p_contextNumber, int p_broadcastPeriod, char* p_selfIp, int p_subscriberLimit)
			{
				m_contextUuid = boost::uuids::random_generator()();
				m_contextNumber = p_contextNumber;
				m_broadCastPeriod = p_broadcastPeriod;
				m_subscribers = new SubscriberManager<T>(p_subscriberLimit);
				m_subscribers->AddSubscriber(p_selfIp, 1);
				std::atomic_store(&m_updateRound, false);
				m_contextFailedAttempts = 0;
				m_failedAttemptsServer = 0;
				m_selfIp = p_selfIp;
				InitContextListener();
				InitContextUpdater();
			}
			//-------------------------------------------------------------

			void InitContextListener()
			{
				m_listeningThread = std::thread(boost::bind(&Context::Listen, this));
			}

			void InitContextUpdater()
			{
				m_senderThread = std::thread(boost::bind(&Context::UpdateSubscribers, this));
			}

			//-------------------------------------------------------------
			//
			//-------------------------------------------------------------
			virtual void Listen()
			{
				try
				{
					boost::asio::io_service io_service;
					Network::AsynComm::tcp_server_context<T> server(io_service, m_subscribers, m_contextNumber, &m_contextStatus, &m_updateRound, &m_editSubscribers, m_selfIp);
					io_service.run();
				}
				catch (std::exception& e)
				{
					std::cerr << e.what() << std::endl;
				}
			}
			//-------------------------------------------------------------

			//-------------------------------------------------------------
			//
			//-------------------------------------------------------------
			void BroadCast(IoSrevice& p_asio, Socket& p_socket)
			{
				//how am i going to save the state?
				vector<Subscriber<T>*> allNeighbors;
				m_subscribers->GetAllSubscribersWLock(allNeighbors, m_contextIter);

				if (m_subscribers->m_neighbours.size() > 0)
				{
					//get all current data 
					vector<unsigned char> allData;
					Network::HelperClass::AddIntBytes(allData, m_contextNumber);
					m_subscribers->GetAllUpdates(allData);
					for (int peerIndex = 0; peerIndex < allNeighbors.size(); peerIndex++)
					{
						int amountFails = std::atomic_load(&(allNeighbors[peerIndex]->m_failCounter));
						if (amountFails == 5)
						{
							m_subscribers->RemoveSubscriber(allNeighbors[peerIndex]);
						}
						else
						{
							SendUpdateToPeer(allNeighbors[peerIndex], allData, p_asio, p_socket);
						}
					}
				}
				else if (m_failedAttemptsServer == 8)
				{
					//take down server 
					m_contextStatus.SetContextOffline();
				}
				else
				{
					m_failedAttemptsServer++;
				}
			}
			//-------------------------------------------------------------

			//-------------------------------------------------------------
			// Peer - Superpeer communication counter part
			//-------------------------------------------------------------
			void UpdateSubscribers()
			{
				IoSrevice aios;
				Socket socket(aios);


				while (m_contextStatus.GetContextStatus())
				{
					bool toUpdate = std::atomic_load(&m_updateRound);

					if (toUpdate && m_contextNumber >= 0 || m_contextNumber > 10)
					{
						m_contextIter++;
						Network::DebugCout("BEFORE :: Sending a context update "+m_contextNumber);
						BroadCast(aios, socket);
						std::atomic_store(&m_updateRound, false);
						m_failedAttemptsServer = 0;
						Network::DebugCout("AFTER :: Sending a context update " + m_contextNumber);
					}
					else if(m_failedAttemptsServer < 5)
					{
						m_failedAttemptsServer++;
					}
					else
					{
						std::stringstream temp;
						temp << "CONTEXT :: context " << m_contextNumber << " is decomissioned after " << m_failedAttemptsServer;
						std::string tempString = temp.str();
						Network::DebugCout(tempString);
						m_contextStatus.SetContextOffline();
					}
					std::this_thread::sleep_for(std::chrono::seconds(m_broadCastPeriod));
				}
			}

			void SendUpdateToPeer(Network::SuperPeers::Subscribers::Subscriber<T>* p_currentSubscriber, vector<unsigned char>& p_updateToSend, IoSrevice& p_aios, Socket& p_socket)
			{
				try
				{
					Network::HelperClass::OpenConnection(p_currentSubscriber->m_Ip, (std::to_string(SUPER_PEER_COMMUNNICATION_TO_PEER)).c_str(), p_socket, p_aios);
					Network::HelperClass::SendData(p_updateToSend, p_socket);
					Network::HelperClass::CloseConnection(p_socket);
				}
				catch (std::exception ex)
				{
					Network::DebugCout("EXCEPTION :: Issue communicating with current subscriber");
					std::atomic_fetch_add(&p_currentSubscriber->m_failCounter, 1);
				}
			}


			void DecommiosionThreads()
			{
				m_listeningThread.detach();
				m_senderThread.detach();
				//delete m_subscribers;
			}

			ContextStatus m_contextStatus;

			//void 
		private:
			Network::SuperPeers::Subscribers::SubscriberManager<T>* m_subscribers;
			std::thread m_listeningThread;
			std::thread m_senderThread;
			int m_contextNumber;
			
			int m_broadCastPeriod;
			boost::uuids::uuid m_contextUuid;
			std::atomic<bool> m_updateRound;
			std::mutex m_editSubscribers;
			int m_contextIter = 0;
			int m_contextFailedAttempts = 0;
			int m_failedAttemptsServer = 0;
			std::string m_selfIp;
			//add the tcp server 
		};
	}

}