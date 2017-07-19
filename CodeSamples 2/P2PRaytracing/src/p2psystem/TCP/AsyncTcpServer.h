#pragma once 

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <string>
#include <iostream>
#include <hash_map>
#include <atomic>

#include "../SuperPeer/SubscribersManager.h"
#include "../SuperPeer/Subscriber.h"
#include "../SuperPeer/ContextStatus.h"
#include "../UtilityClasses/HelperClass.h"
#include "../UtilityClasses/Common.h"

const int PORT_NUMBER_CONTEXT = 20200;

using namespace boost::asio::ip;
using namespace Network::SuperPeers::Subscribers;
namespace Network
{
	namespace AsynComm
	{
		namespace connections
		{

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	A TCP connection. </summary>
			///
			/// <remarks>	Adrian. </remarks> 
			/// 
			/// <remark> Test async TCP communication</remark>
			///
			/// <typeparam name="REF">	Type of the reference. </typeparam>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			template<typename REF>
			class tcp_connection : public boost::enable_shared_from_this<tcp_connection<REF>>
			{
			public:
				typedef boost::shared_ptr<tcp_connection<REF>> pointer;

				static pointer create(boost::asio::io_service& p_io_service)
				{
					return pointer(new tcp_connection<REF>(p_io_service));
				}

				tcp::socket& socket()
				{
					return m_socket;
				}

				void start(int& p_counter)
				{}


				void handle_write(const boost::system::error_code& /*error*/,
					size_t /*bytes_transferred*/)
				{
				}

			private:
				tcp_connection(boost::asio::io_service& io_service) : m_socket(io_service)
				{}

				tcp::socket m_socket;
				std::string m_message;
			};


			void tcp_connection<int>::start(int& p_counter)
			{
				m_message = "Hello there i am an async server for your commodity :) " + std::to_string(p_counter);
				p_counter++;
				boost::asio::ip::address_v4 temp = m_socket.remote_endpoint().address().to_v4();
				unsigned long currentIp = temp.to_ulong();

				std::cout << "current ip " << temp.to_string() << std::endl;

				boost::asio::async_write(m_socket, boost::asio::buffer(m_message),
					boost::bind(&tcp_connection<int>::handle_write, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}

			void tcp_connection<char>::start(int& p_counter)
			{
				m_message = "Hello there i am an async server for your commodity :) " + std::to_string(p_counter);
				p_counter++;
				boost::asio::ip::address_v4 temp = m_socket.remote_endpoint().address().to_v4();
				unsigned long currentIp = temp.to_ulong();

				std::cout << "current ip " << temp.to_string() << std::endl;

				boost::asio::async_write(m_socket, boost::asio::buffer(m_message),
					boost::bind(&tcp_connection<char>::handle_write, shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
			}
		}

		namespace ContextCommunication
		{
			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	A TCP connection context. </summary>
			///
			/// <remarks>	Adrian, 19/07/2017. </remarks>
			///
			/// <typeparam name="REF">	Type of the reference. </typeparam>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			template<typename REF>
			class tcp_connection_context : public boost::enable_shared_from_this<tcp_connection_context<REF>>
			{
			public:
				typedef boost::shared_ptr<tcp_connection_context<REF>> pointer;

				static pointer create(boost::asio::io_service& p_io_service)
				{
					return pointer(new tcp_connection_context<REF>(p_io_service));
				}

				tcp::socket& socket()
				{
					return m_socket;
				}

				void start(SubscriberManager<REF>* p_subscriberManager, std::mutex* p_editSubscribers, std::string p_selfIp)
				{
					vector<unsigned char> reply;
					
					try
					{
						vector<unsigned char> receivedUpdate;
						Network::HelperClass::ReceiveData(m_socket, receivedUpdate);
						int requestType = 0;
						int startingPoint = 0;
						int tier = 0;
						
						HelperClass::ByteArrayToInt(requestType, receivedUpdate, startingPoint);

						if (requestType == 1)
							HelperClass::ByteArrayToInt(tier, receivedUpdate, startingPoint);

						//receivedUpdate.erase(receivedUpdate.begin(), receivedUpdate.end());
						switch (requestType)
						{
						//handshake
						case 1:
						{	
							bool subscriberAdded = p_subscriberManager->AddSubscriber(const_cast<char*>(m_socket.remote_endpoint().address().to_string().c_str()), tier);
							if (subscriberAdded)
							{
								//true: Send confirmation to the receiver
								HelperClass::AddIntBytes(reply, 1);
								HelperClass::SendData(reply, m_socket);
							}
							else
							{
								//false: Send failed confirmation to receiver
								HelperClass::AddIntBytes(reply, -5);
								HelperClass::SendData(reply, m_socket);
							}
							break;
						}
							//update
						case 2:
						{
							//update the peer
							p_subscriberManager->UpdateSubscriberState(inet_addr(m_socket.remote_endpoint().address().to_string().c_str()), receivedUpdate, startingPoint);
							//reply with a confirmation
							Network::HelperClass::AddIntBytes(reply, 1);
							Network::HelperClass::SendData(reply, m_socket);
							std::this_thread::sleep_for(std::chrono::seconds(2));
							break;
						}
						//unsubscribe
						case 3:
						{
							string ip = m_socket.remote_endpoint().address().to_string();
							std::cout << ip << std::endl;
							p_subscriberManager->RemoveNeighbour(m_socket.remote_endpoint().address().to_string().c_str());
							HelperClass::AddIntBytes(reply, 1);
							HelperClass::SendData(reply, m_socket);
							break;
						}

						default:
							break;
						}

						m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
						m_socket.close();

					}
					catch (std::exception ex)
					{
						reply.erase(reply.begin(), reply.end());
						HelperClass::AddIntBytes(reply, -5);
						HelperClass::SendData(reply, m_socket);
					}
					
				}

				void handle_write(const boost::system::error_code& /*error*/,size_t /*bytes_transferred*/)
				{
				}

			private:
				tcp_connection_context(boost::asio::io_service& io_service) : m_socket(io_service)
				{}

				tcp::socket m_socket;
			};
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	A TCP server context. </summary>
		///
		/// <remarks>	Adrian, 19/07/2017. </remarks>
		///
		/// <typeparam name="T">	Generic type parameter. </typeparam>
		////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename T>
		class tcp_server_context
		{
		public:
			//---------------------------------------------------
			// Constructor
			//---------------------------------------------------
			tcp_server_context(boost::asio::io_service& io_service, SubscriberManager<T>* p_subsManager, int p_contextNumber, Context::ContextStatus* p_contextStatus, std::atomic<bool>* p_roundUpdate, std::mutex* p_mutex, std::string p_selfIp) 
				: m_serverAcceptor(io_service, tcp::endpoint(tcp::v4(), (PORT_NUMBER_CONTEXT + p_contextNumber)))
			{
				m_contextStatus = p_contextStatus;
				m_subscriberManager = p_subsManager;
				m_roundUpdate = p_roundUpdate;
				m_editSubscribers = p_mutex;
				m_contextNumber = p_contextNumber;
				m_selfIp = m_selfIp;
				start_accept();
			}

		

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Starts an accept. </summary>
			///
			/// <remarks>	Adrian. </remarks>
			/// 
			/// <remarks> created a socket and initiates an asynchronous
			///			  accept operation to wait for a new connection</remarks>
			/// 
			////////////////////////////////////////////////////////////////////////////////////////////////////
			void start_accept()
			{
				ContextCommunication::tcp_connection_context<T>::pointer new_connection =
					ContextCommunication::tcp_connection_context<T>::create(m_serverAcceptor.get_io_service());

				if (m_contextStatus->GetContextStatus())
				{
					m_serverAcceptor.async_accept(new_connection->socket(), boost::bind(&tcp_server_context<T>::handle_accept, this, new_connection, boost::asio::placeholders::error));
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Handles the accept. </summary>
			///
			/// <remarks>	Adrian, 19/07/2017. </remarks>
			///
			/// <param name="p_new_connection">	The new connection. </param>
			/// <param name="p_error">		   	The error. </param>
			////////////////////////////////////////////////////////////////////////////////////////////////////

			void handle_accept(typename ContextCommunication::tcp_connection_context<T>::pointer p_new_connection, const boost::system::error_code& p_error)
			{
				if (m_contextStatus->GetContextStatus())
				{
					try
					{
						Network::DebugCout("BEFORE :: Received a new connection to context " + m_contextNumber);
						
						//accept connection
						if (!p_error)
						{
							p_new_connection->start(m_subscriberManager, m_editSubscribers, m_selfIp);
							bool* temp = new bool;
							*temp = false;
							std::atomic_compare_exchange_strong(m_roundUpdate, temp, true);
							delete temp;
						}
						Network::DebugCout("AFTER :: Received a new connection to context " + m_contextNumber);
					}
					catch (std::exception ex)
					{
						Network::DebugCout("EXCEPTION :: error during handling of context listener, in context " + m_contextNumber);
					}

					start_accept();
				}
				else
				{
					Network::DebugCout("HANDLE_ACCEPT_CONTEXT :: Exiting now");
				}
			}

			int* counter = 0;
			std::thread m_listeningThread;

		private:
			boost::asio::ip::tcp::acceptor m_serverAcceptor;
			SubscriberManager<T>* m_subscriberManager;
			Network::Context::ContextStatus* m_contextStatus;
			std::atomic<bool>* m_roundUpdate;
			std::mutex* m_editSubscribers;
			int m_contextNumber;
			std::string m_selfIp;
		};

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	A TCP server. </summary>
		///
		/// <remarks>	Adrian </remarks>
		///
		/// <typeparam name="T">	Context type </typeparam>
		////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename T>
		class tcp_server
		{
		public:

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Constructor. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="io_service">	[in,out] The i/o service. </param>
			/// 
			/// <param name="p_counter"> 	
			/// 	[in,out] Counts the amount of times a micro-server had no peers, 
			/// 	if 5 times the context is decomissioned by the context manager. 
			/// </param>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			tcp_server(boost::asio::io_service& io_service, int* p_counter) : m_serverAcceptor(io_service, tcp::endpoint(tcp::v4(), PORT_NUMBER_CONTEXT))
			{
				counter = p_counter;
				start_accept();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Constructor. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="io_service">   	[in,out] The i/o service. </param>
			/// <param name="p_counter">		[in,out] If non-null, the counter. </param>
			/// <param name="p_subsManager">	[in,out] Subscriber manager to contain manage the subscribers of the peer </param>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			tcp_server(boost::asio::io_service& io_service, int* p_counter, SubscriberManager<T>* p_subsManager) : m_serverAcceptor(io_service, tcp::endpoint(tcp::v4(), PORT_NUMBER))
			{
				counter = p_counter;
				m_subscriberManager = p_subsManager;
				start_accept();
			}

			//---------------------------------------------------
			// created a socket and initiates an asynchronous 
			// accept operation to wait for a new connection
			//---------------------------------------------------
			void start_accept()
			{
				connections::tcp_connection<T>::pointer new_connection =
					connections::tcp_connection<T>::create(m_serverAcceptor.get_io_service());
				std::cout << "Going to accept new connections" << std::endl;
				m_serverAcceptor.async_accept(new_connection->socket(), boost::bind(&tcp_server<T>::handle_accept, this, new_connection, boost::asio::placeholders::error));
			}

			void handle_accept(typename connections::tcp_connection<T>::pointer p_new_connection, const boost::system::error_code& p_error)
			{

				std::cout << "Waiting for a connection " << std::endl;
				if (!p_error)
				{
					p_new_connection->start(*counter);
				}

				std::cout << "Waiting for a new connection" << std::endl;

				start_accept();
			}

			int* counter = 0;
			std::thread m_listeningThread;

		private:
			boost::asio::ip::tcp::acceptor m_serverAcceptor;
			SubscriberManager<Subscriber<T>>* m_subscriberManager;
		};
	}
}
