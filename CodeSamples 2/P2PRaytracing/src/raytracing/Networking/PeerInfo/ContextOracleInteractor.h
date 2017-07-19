#pragma once 

#include <thread>
#include <string>

#include "Updater/IOracleInteractor.h"
#include "../Metrics/Metrics.h"
#include "../Metrics/MetricsManager.h"

#include <Peer/NeighbourManager/NeighboursManager.h>

using namespace Network;
using Engine::Networking::Metrics::Metric;
using Engine::Networking::Communication::Util::UtilClass;
using std::thread;

namespace Engine
{
	namespace Networking 
	{
		const int LISTENING_PORT_FOR_ORACLE = 25000;
		const int LISTENING_PORT_CLIENT = 15000;

		template<typename U = Update>
		class ContextOracleInteractor : public IOracleInteractor
		{
		public:
			ContextOracleInteractor(string p_ipAddress, NeighbourManager<U>* p_neighborManager)
			{
				m_neighborManager = p_neighborManager;
				m_serverPortNumber = LISTENING_PORT_FOR_ORACLE;
				m_serverIp = p_ipAddress;
				m_metrics = new Metrics::MetricsManager(m_neighborManager);
				m_listeningThread = thread(&ContextOracleInteractor::GetUpdates, this);
			}

			ContextOracleInteractor(string p_ipAddress)
			{
				m_serverPortNumber = LISTENING_PORT_FOR_ORACLE;
				m_serverIp = p_ipAddress;
				m_metrics = new Metrics::MetricsManager();
				m_listeningThread = thread(&ContextOracleInteractor::GetUpdates, this);
			}

			using IOracleInteractor::GetUpdates;
			virtual void GetUpdates()
			{
				boost::asio::io_service aios;
				boost::asio::ip::tcp::acceptor acceptor(aios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), LISTENING_PORT_CLIENT));
				boost::asio::ip::tcp::socket socket(aios);

				while (!mCanExit)
				{
					acceptor.accept(socket);
 					vector<unsigned char> resultArray;
					Network::Communication::CommunicationUtil::ReceiveData(socket, resultArray);
					_ASSERT(resultArray.size() > 0);
					m_metrics->GetUpdates(resultArray);
					Network::Communication::CommunicationUtil::CloseConnection(socket);
				}
			}

			using IOracleInteractor::SendUpdates;
			virtual void SendUpdates(string p_update)
			{
				try
				{
					boost::asio::io_service aios;
					Socket socket(aios);
					Network::Communication::CommunicationUtil::OpenConnection(m_serverIp.c_str(), to_string(LISTENING_PORT_FOR_ORACLE).c_str(), socket, aios);
					Network::Communication::CommunicationUtil::SendData(p_update + "<EOF>", socket);
					Network::Communication::CommunicationUtil::CloseConnection(socket);
				}
				catch (exception ex)
				{
					std::cout << "Failed to communicate Oracle" << std::endl;
					std::this_thread::sleep_for(chrono::milliseconds(5000));
				}
			}

		private:
			thread m_listeningThread;
			bool mCanExit = false;
			string m_peerName = "";
			NeighbourManager<Update>* m_neighborManager;
			Metrics::MetricsManager* m_metrics;
		};
	}//networking
}// Engine