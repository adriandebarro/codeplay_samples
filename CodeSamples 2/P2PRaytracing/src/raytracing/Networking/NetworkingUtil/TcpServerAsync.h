#pragma once 

#include <UtilityClasses/Common.h>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <UtilityClasses/HelperClass.h>
#include <UtilityClasses/Common.h>

#include <string>
#include <iostream>
#include <hash_map>

#include "../Contexts/ContextStrategy.h"
#include "../ConreteNeighborManagers/PdfNeighborManager.h"
#include "../ConreteNeighborManagers/WeightingMetric.h"
#include "../ConreteNeighborManagers/StatePeer.h"

#include "../../System/SystemHelper.h"

using namespace Engine::Networking::Neighbors;
using namespace Engine::Networking::Contexts;
using namespace Network;

using Engine::BoundingShapes::BoundingBoxAnalyzer;
using Engine::Camera;
using Engine::Networking::Neighbors::Metrics::WeightingMetrics;

using namespace Engine::System;

namespace Engine
{
	namespace Networking
	{
		namespace Contexts
		{
			
			class tcp_connection_context : public boost::enable_shared_from_this<tcp_connection_context>
			{
			public:
				typedef boost::shared_ptr<tcp_connection_context> pointer;

				static pointer create(boost::asio::io_service& p_io_service)
				{
					return pointer(new tcp_connection_context(p_io_service));
				}

				tcp::socket& socket()
				{
					return m_socket;
				}

				void start(PdfNeighborManager<Engine::Networking::Neighbours::StatePeer>* m_peers, WeightingMetrics* p_metricManager)
				{
  					vector<unsigned char> updatesReceived;
					//receive data
					Network::HelperClass::ReceiveData(m_socket, updatesReceived);
					if (updatesReceived.size() > 0)
					{
						int contextTypeUpdate = -1;
						Network::HelperClass::ByteArrayToInt(contextTypeUpdate, updatesReceived);
						int startingPoint = 0;
						m_peers->UpdateNeighbors(updatesReceived, startingPoint);
						_ASSERT(contextTypeUpdate >= 0);
						p_metricManager->UpdateWeighting(contextTypeUpdate);
						m_peers->UpdatePdfDistribution();
					}
				}

				void handle_write(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/)
				{
				}

			private:
				tcp_connection_context(boost::asio::io_service& io_service) : m_socket(io_service)
				{}

				tcp::socket m_socket;
			};

			class tcp_server_peer
			{
			public:
				//---------------------------------------------------
				// Constructor
				//---------------------------------------------------
				tcp_server_peer(boost::asio::io_service& io_service, PdfNeighborManager<Engine::Networking::Neighbours::StatePeer>* p_nnieghborManager, WeightingMetrics* p_weightMetrics, bool* p_continueListening) 
					: m_serverAcceptor(io_service, tcp::endpoint(tcp::v4(), 20700))
				{
					m_neighbourManager = p_nnieghborManager;
					m_weightingMetrics = p_weightMetrics;
					m_continueListening = p_continueListening;
					start_accept();
				}

				//---------------------------------------------------
				// created a socket and initiates an asynchronous 
				// accept operation to wait for a new connection
				//---------------------------------------------------
				void start_accept()
				{
					tcp_connection_context::pointer new_connection =
						tcp_connection_context::create(m_serverAcceptor.get_io_service());
					m_serverAcceptor.async_accept(new_connection->socket(), boost::bind(&tcp_server_peer::handle_accept, this, new_connection, boost::asio::placeholders::error));
				}

				void handle_accept(typename tcp_connection_context::pointer p_new_connection, const boost::system::error_code& p_error)
				{
					//System::SystemUtil::PrintAlertMessage("Handling a context update");
					if (*m_continueListening)
					{
						try
						{
							if (!p_error)
							{
								p_new_connection->start(m_neighbourManager, m_weightingMetrics);
							}

							//std::cout << "Waiting for a new connection" << std::endl;

							start_accept();
						}
						catch (std::exception ex)
						{
							std::cout << "Error while handling a context" << std::endl;
							start_accept();
						}
					}
				}

			private:
				boost::asio::ip::tcp::acceptor m_serverAcceptor;
				PdfNeighborManager<Engine::Networking::Neighbours::StatePeer>* m_neighbourManager;
				WeightingMetrics* m_weightingMetrics;
				bool* m_continueListening = nullptr;

			};
		}
	}
}