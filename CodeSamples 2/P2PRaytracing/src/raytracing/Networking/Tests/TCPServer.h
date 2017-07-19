#pragma once 
#include <boost/asio/deadline_timer.hpp>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/timer.hpp>

#include <UtilityClasses/HelperClass.h>

#include <string>
#include <iostream>
#include <hash_map>
const int PORT_NUMBER = 20200;

using namespace boost::asio::ip;
namespace Engine
{
	namespace Networking
	{
		namespace connections
		{
			class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
			{
			public:
				typedef boost::shared_ptr<tcp_connection> pointer;

				static pointer create(boost::asio::io_service& p_io_service)
				{
					return pointer(new tcp_connection(p_io_service));
				}

				tcp::socket& socket()
				{
					return m_socket;
				}

				void start(int& p_counter)
				{
					m_message = "Hello there i am an async server for you commodity :) " + std::to_string(p_counter) ;
					p_counter++;
					boost::asio::async_write(m_socket, boost::asio::buffer(m_message),
						boost::bind(&tcp_connection::handle_write, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
					Network::HelperClass::SendData(m_message, m_socket);
				}
				tcp::socket m_socket;

			private:
				tcp_connection(boost::asio::io_service& io_service) : m_socket(io_service)
				{}

			

				void handle_write(const boost::system::error_code& /*error*/,
					size_t /*bytes_transferred*/)
				{
				}

				std::string m_message;
			};
		}


		namespace Tests
		{
			class tcp_server
			{
			public:
				//---------------------------------------------------
				// Constructor
				//---------------------------------------------------
				tcp_server(boost::asio::io_service& io_service, bool* p_sendNextUpdate) : m_serverAcceptor(io_service, tcp::endpoint(tcp::v4(), PORT_NUMBER)), m_timer(io_service)
				{
					//counter = p_counter;
					start_accept();
				}

				//---------------------------------------------------
				// created a socket and initiates an asynchronous 
				// accept operation to wait for a new connection
				//---------------------------------------------------
				void start_accept()
				{
					if (accept)
					{

					connections::tcp_connection::pointer new_connection =
						connections::tcp_connection::create(m_serverAcceptor.get_io_service());
					//std::cout << "Going to accept new connections" << std::endl;
					
						m_timer.cancel_one();
						m_timer.expires_from_now(boost::posix_time::seconds(10));
						m_timer.async_wait(boost::bind(&tcp_server::handle_expiration, this, boost::asio::placeholders::error));

						//timer.wait();
						//timer.async_wait(&tcp_server::handle_expiration);

						m_serverAcceptor.async_accept(new_connection->socket(), boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));
					}
					else
					{
						m_serverAcceptor.cancel();
					}
				}

				void handle_accept(connections::tcp_connection::pointer p_new_connection, const boost::system::error_code& p_error)
				{
				
					if (!p_error && accept)
					{
						std::cout << "Connected " << std::endl;
						p_new_connection->start(*counter);
					}

					std::cout << "Waiting for a new connection" << std::endl;

					start_accept();
				}

				void handle_expiration(const boost::system::error_code& error)
				{
					if (!error)
					{
						accept = false;
						std::cout << "timer expired" << std::endl;
						m_serverAcceptor.cancel();
					}
				}


				int* counter = 0;
				//std::thread m_listeningThread;
			private:
				boost::asio::ip::tcp::acceptor m_serverAcceptor;
				boost::asio::deadline_timer m_timer;
				bool accept = true;
			};

			class InitiatorClass
			{
			private:
				int count;


			};
		}
	}
}