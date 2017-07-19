///-------------------------------------------------------------------------------------------------
// file:	Peer\Peer.h
//
// summary:	Declares the peer class
///-------------------------------------------------------------------------------------------------

#pragma once 

#include <string>
#include <hash_map>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <tuple>

///For ThreadPool
//#include <boost/asio/io_service.hpp>
//#include <boost/bind.hpp>
//#include <boost/thread/thread.hpp>


#include <boost\tokenizer.hpp>
#include <boost\lexical_cast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

#include "..\UtilityClasses\Common.h"
#include "..\Peer\HelperClass.h"
#include "..\Updater\PeerSimulator.h"
#include "../Peer/NeighbourManager/NeighboursManager.h"
#include "../ObservableEvents/ObservableEventsList/ObservaleEventsList.h"

///-------------------------------------------------------------------------------------------------
/// <summary>	A macro that defines thread pool amount threads. </summary>
///
/// <remarks>	Adrian, 31/08/2015. </remarks>
///-------------------------------------------------------------------------------------------------

#define THREAD_POOL_AMOUNT_THREADS 1
using std::hash_map;
using std::cout;
using std::endl;
using std::vector;

/// <summary>	Identifier for the boot strap peer. </summary>
//const static std::string BOOT_STRAP_PEER_ID = "bootStrap";

///-------------------------------------------------------------------------------------------------
/// <summary>	Defines an alias representing the tokenizer. </summary>
///
/// <remarks>	Adrian, 31/08/2015. </remarks>
///-------------------------------------------------------------------------------------------------

typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
typedef boost::asio::ip::tcp::socket Socket;
typedef boost::asio::io_service IoSrevice;
typedef boost::system::error_code BoostSystemErrorCode;

namespace Network
{
	namespace Peer
	{
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Abstract peer </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------
		template<typename T, typename S, typename U, typename AS, typename N = Neighbour>
		class AbstarctPeer
		{
		public:

			//Manages the neighbours of the peer
			NeighbourManager<N>* neighbors;
			AbstarctPeer()
			{
				m_sockerNumber = LISTENING_PORT;
				newToNetwork = false;
	
				neighbors = new NeighbourManager<N>();
				cout << "Server established" << endl;
				running = true;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Constructor. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <param name="ipAddress">   	The IP address. </param>
			/// <param name="socketNumber">	The socket number. </param>
			///-------------------------------------------------------------------------------------------------
			AbstarctPeer(std::string ipAddress, int socketNumber)
			{
				m_errorLogger = Logger(1);
				m_sockerNumber = LISTENING_PORT;
				bootStrapAddress = ipAddress;
				newToNetwork = true;
				neighbors = new NeighbourManager<N>(new N(ipAddress, to_string(m_sockerNumber)));
				//m_updatesList = new ObservableEvents::ObservableEventsList<T>(socketNumber);
				running = true;

				cout << "Peer established" << endl;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Constructor. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <param name="ipAddress">   	The IP address. </param>
			/// <param name="socketNumber">	The socket number. </param>
			/// <param name="nameAssigned">	The name assigned. </param>
			///-------------------------------------------------------------------------------------------------
			AbstarctPeer(std::string ipAddress, int socketNumber, std::string nameAssigned)
			{
				m_errorLogger = Logger(1);
				m_sockerNumber = LISTENING_PORT
				bootStrapAddress = ipAddress;
				newToNetwork = true;
				neighbors = new NeighbourManager(new N(ipAddress, to_string(m_sockerNumber)));
				running = true;
			}

			AbstarctPeer(std::string ipAddress, int socketNumber, std::string nameAssigned, ObservableEvents::ObservableEventsList<T, S, AS>* p_observableEventsList)
			{
				m_sockerNumber = LISTENING_PORT;
				bootStrapAddress = ipAddress;
				m_updatesList = p_observableEventsList;
				running = true;
				cout << "Peer " << nameAssigned << " established" << endl;
			}


			AbstarctPeer(std::string ipAddress, int socketNumber, std::string nameAssigned, ObservableEvents::ObservableEventsList<T, S, AS>* p_observableEventsList, NeighbourManager<N>* p_neighborManager)
			{
				m_sockerNumber = LISTENING_PORT;
				bootStrapAddress = ipAddress;
				newToNetwork = true;
				N* tempNeighbor = new N("123.1.2.3");
				neighbors = p_neighborManager;
				m_updatesList = p_observableEventsList;
				running = true;
				cout << "Peer " << nameAssigned << " established" << endl;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Constructor. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <param name="socketNumber">	The socket number. </param>
			///-------------------------------------------------------------------------------------------------

			AbstarctPeer(int socketNumber)
			{
				m_errorLogger = Logger(1);
				m_sockerNumber = LISTENING_PORT;
				newToNetwork = true;
				string ipAddress = "127.0.0.1";
				neighbors = new NeighbourManager<N>(new N(ipAddress.c_str(), to_string(m_sockerNumber).c_str()));
				cout << "Server Peer established" << endl;
				running = true;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Destructor. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///-------------------------------------------------------------------------------------------------
			~AbstarctPeer()
			{}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Listen on network. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///-------------------------------------------------------------------------------------------------
			virtual void ListenOnNetwork() = 0;

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Sends the update. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///-------------------------------------------------------------------------------------------------
			virtual void UpdateFunction() = 0;

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Boot sequence. </summary>
			///
			/// <remarks>	Adrian, 04/09/2015. </remarks>
			///-------------------------------------------------------------------------------------------------
			virtual void BootUpSequence() = 0;

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Use update. </summary>
			///
			/// <remarks>	Adrian, 05/09/2015. </remarks>
			///-------------------------------------------------------------------------------------------------
			virtual  void AddUpdate(T*) = 0;

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Sends a data. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <param name="currentNeighbour">	[in,out] The current neighbour. </param>
			/// <param name="stringToUse">	   	[in,out] The string to use. </param>
			/// <param name="id">			   	The identifier. </param>
			///-------------------------------------------------------------------------------------------------
			void SendData(std::string stringToUse, boost::asio::ip::tcp::socket &socket)
			{
				boost::asio::write(socket, boost::asio::buffer(stringToUse));
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Sends byte array. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_dataToSend">	The data to send. </param>
			/// <param name="socket">	   	[in,out] The socket. </param>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			void SendData(vector<unsigned char> p_dataToSend, boost::asio::ip::tcp::socket &socket)
			{
				//boost::exception ec;
				vector<unsigned char> amountToSend;

				int size = p_dataToSend.size();

				unsigned char* amountBytes = (unsigned char*)&size;
				//parse the amount of bytes sent on network
				for (int index = 0; index < 4; index++)
				{
					amountToSend.push_back(amountBytes[index]);
				}

				boost::asio::write(socket, boost::asio::buffer(amountToSend), boost::asio::transfer_all());
				boost::asio::write(socket, boost::asio::buffer(p_dataToSend), boost::asio::transfer_all());
			}

			bool CheckContent(char* myArray, int length)
			{
				for (int index = 0; index < length; index++)
				{
					if (myArray[index] == '|')
						return true;
				}

				return false;
			}

			string ReceiveData(boost::asio::ip::tcp::socket &socket)
			{
				stringstream stream;

				for (;;)
				{
					std::array <char, 2048> buf;
					boost::system::error_code error;

					size_t len = socket.read_some(boost::asio::buffer(buf), error);

					if (error == boost::asio::error::eof)
						break;
					else if (error)
						throw boost::system::system_error(error);

					if (CheckContent(buf.data(), len))
					{
						stream.write(buf.data(), len - 1);
						break;
					}
					else
						stream.write(buf.data(), len);
						
				}

				return stream.str();
			}


			void AddDataToVector(vector<unsigned char>& p_outputArray, std::array<unsigned char, 50000>& p_buf, size_t len, int& amount)
			{
				for (int index = 0; index < len; index++)
				{
					p_outputArray.push_back(p_buf[index]);
					amount--;
				}
			}

			void ReceiveData(boost::asio::ip::tcp::socket &socket, vector<unsigned char>& p_toOutput)
			{
				boost::system::error_code error;
				std::array<unsigned char, 4> numberArray;

				int amount = 0;

				unsigned char* amountTest = (unsigned char*)&amount;

				size_t len = socket.read_some(boost::asio::buffer(numberArray,4), error);

				for (int index = 0; index < 4; index++)
				{
					amountTest[index] = numberArray[index];
				}

				for (;;)
				{
					std::array <unsigned char, 50000> buf;
					
					size_t len = socket.read_some(boost::asio::buffer(buf), error);

					if (error == boost::asio::error::eof)
					{
						break;
					}
					else if (error)
						throw boost::system::system_error(error);
					else
					{
						AddDataToVector(p_toOutput, buf, len, amount);
					}

					if (amount <= 0)
					{
						break;
					}
				}
			}


			void ReceiveDataTester(boost::asio::ip::tcp::socket &socket, vector<unsigned char>& p_toOutput)
			{
				clock_t overallStartingPRocess = clock();

				clock_t prepWorkStart = clock();

				boost::system::error_code error;
				std::array<unsigned char, 4> numberArray;

				int amount = 0;

				unsigned char* amountTest = (unsigned char*)&amount;

				size_t len = socket.read_some(boost::asio::buffer(numberArray,4), error);

				for (int index = 0; index < 4; index++)
				{
					amountTest[index] = numberArray[index];
				}

				clock_t prepWorkEnd = clock_t();

				cout << " time taken before receving and parsing is " << ((double)prepWorkEnd - (double)prepWorkStart) / ((double)CLOCKS_PER_SEC) << endl;

				clock_t startintProcesstime = clock();

				for (;;)
				{
					std::array <unsigned char, 50000> buf;

					clock_t startingTimeReadBuf = clock();
					size_t len = socket.read_some(boost::asio::buffer(buf), error);
					clock_t endingTimeReadBuf = clock();

					cout << "read buffer took " << ((double)endingTimeReadBuf - startingTimeReadBuf) / (double)CLOCKS_PER_SEC << endl;;


					if (error == boost::asio::error::eof)
					{
						break;
					}
					else if (error)
						throw boost::system::system_error(error);
					else
					{
						clock_t startingAddingDataToBuffer = clock();
						AddDataToVector(p_toOutput, buf, len, amount);
						clock_t endingAddingDataToBuffer = clock();

						cout << "transfered buffer data in  " << ((double)endingAddingDataToBuffer - startingAddingDataToBuffer) / (double)CLOCKS_PER_SEC << endl;;
					}

					if (amount <= 0)
					{
						break;
					}
				}

				clock_t endingReadingProcess = clock();
				clock_t overallEndingTimew = clock();

				cout << "reading and parsing process took " << ((double)endingReadingProcess - startintProcesstime) / (double)CLOCKS_PER_SEC << endl;
				cout << "overall process internally took " << ((double)overallEndingTimew - overallStartingPRocess) / (double)CLOCKS_PER_SEC << endl;
				
			}


			void ReceiveDataTimeout(boost::asio::ip::tcp::socket &socket, vector<unsigned char>& p_toOutput)
			{
				
				// Set a deadline for the asynchronous operation. Since this function uses
				// a composed operation (async_read_until), the deadline applies to the
				// entire operation, rather than individual reads from the socket.
				deadline_.expires_from_now(timeout);

				// Set up the variable that receives the result of the asynchronous
				// operation. The error code is set to would_block to signal that the
				// operation is incomplete. Asio guarantees that its asynchronous
				// operations will never fail with would_block, so any other value in
				// ec indicates completion.
				boost::system::error_code ec = boost::asio::error::would_block;

				// Start the asynchronous operation itself. The boost::lambda function
				// object is used as a callback and will update the ec variable when the
				// operation completes. The blocking_udp_client.cpp example shows how you
				// can use boost::bind rather than boost::lambda.
				boost::asio::async_read_until(socket_, input_buffer_, '\n', var(ec) = _1);

				// Block until the asynchronous operation has completed.
				do io_service_.run_one(); while (ec == boost::asio::error::would_block);

				if (ec)
					throw boost::system::system_error(ec);

				std::string line;
				std::istream is(&input_buffer_);
				std::getline(is, line);
				return line;
			}


			///-------------------------------------------------------------------------------------------------
			/// <summary>	Increment logical time. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///-------------------------------------------------------------------------------------------------
			void IncrementLogicalTime()
			{
				logicalTimer++;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets current logical time. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <returns>	The current logical time. </returns>
			///-------------------------------------------------------------------------------------------------

			int GetCurrentLogicalTime()
			{
				return logicalTimer;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Closes a connection to another peer. </summary>
			///
			/// <remarks>	Adrian </remarks>
			///
			/// <param name="p_packet">	[in,out] If non-null, the socket. </param>
			///-------------------------------------------------------------------------------------------------
			bool CloseConnection(Socket &socket)
			{
				try
				{
					BoostSystemErrorCode ec;
					socket.close(ec);

					if (ec)
						return false;
					else
						return true;
				}
				catch (std::exception ex)
				{
					cout << " Error occured" << ex.what() << endl;
					return false;
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Opens a connection. </summary>
			///
			/// <remarks>	Adrian </remarks>
			///
			/// <param name="p_host">  	[in,out] If non-null, the host. </param>
			/// <param name="p_port">  	[in,out] If non-null, the port. </param>
			/// <param name="p_socket">	[in,out] The socket. </param>
			/// <param name="aios">	   	[in,out] The aios. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			bool OpenConnection(char* p_host, char* p_port, Socket &p_socket, IoSrevice &aios)
			{
				/*boost::asio::io_service aios;
				boost::asio::ip::tcp::socket sockett(aios);*/
				try
				{
					cout << "connecting to " << p_host << ":::" << p_port << endl;
					boost::system::error_code ec;
					boost::asio::ip::tcp::resolver resolver(aios);
					boost::asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(boost::asio::ip::tcp::resolver::query(p_host, p_port));
					boost::asio::connect(p_socket, endpoint, ec);
					if (ec)
					{
						cout << ec.message() << endl;
						return false;
					}
						
					return true;
				}
				catch (exception e)
				{
					cout << e.what() << endl;
					return false;
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Opens a connection to a peer </summary>
			///
			/// <remarks>	Adrian </remarks>
			///
			/// <param name="p_host">  	The host. </param>
			/// <param name="p_port">  	The port. </param>
			/// <param name="p_socket">	[in,out] The socket. </param>
			/// <param name="aios">	   	[in,out] The aios. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			bool OpenConnection(const char* p_host, const char* p_port, Socket &p_socket, IoSrevice &aios)
			{
				try
				{
					boost::system::error_code ec;
					boost::asio::ip::tcp::resolver resolver(aios);
					boost::asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(boost::asio::ip::tcp::resolver::query(p_host, p_port));
					boost::asio::connect(p_socket, endpoint, ec);
					if (ec)
					{
						cout << ec.message() << endl;
						return false;
					}

					return true;
				}
				catch (exception e)
				{
					cout << e.what() << endl;
					return false;
				}
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets updates list. </summary>
			///
			/// <remarks>	Adrian, 24/09/2015. </remarks>
			///
			/// <returns>	null if it fails, else the updates list. </returns>
			///-------------------------------------------------------------------------------------------------
			ObservableEvents::ObservableEventsList<T,S, AS>* GetUpdatesList()
			{
				return m_updatesList;
			}


			void Exitlistening()
			{
				m_canExit = true;
			}

			string GetIpAddress()
			{
				return m_ipAddress;
			}

			bool IsIpSet()
			{
				return ipSet;
			}

			void SetNeighborManager(NeighbourManager<N>* p_neighborManager)
			{
				neighbors = p_neighborManager;
			}

			NeighbourManager<N>* GetNeighborManager()
			{
				return neighbors;
			}


			protected:
			/// <summary>	The logical timer. </summary>
			int logicalTimer;
			/// <summary>	The boot strap address. </summary>
			std::string bootStrapAddress;
			/// <summary>	true to updates exist. </summary>
			bool updatesExist;
			/// <summary>	true to new to network. </summary>
			bool newToNetwork;
			/// <summary>	The update thread. </summary>
			std::thread m_updateThread;
			/// <summary>	The listening thread. </summary>
			std::thread m_listeningThread;
			/// <summary>	The updater. </summary>
			/// <summary>	Zero-based index of the peer. </summary>
			int peerIndex;
			int m_sockerNumber;
			/// <summary>	my mutex. </summary>
			std::mutex myMutex;
			/// <summary>	true to running. </summary>
			bool running;
			/// <summary>	The neighbors. </summary>
			ObservableEvents::ObservableEventsList<T,S, AS>* m_updatesList;
			bool m_canExit = false;
			bool ipSet = false;
			string m_ipAddress = "";
			

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Writes to peer. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <param name="peerId">	  	Identifier for the peer. </param>
			/// <param name="toWrite">	  	to write. </param>
			/// <param name="peerMessage">	Message describing the peer. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </retuthe rns>
			   
			bool WriteToPeer(Socket &p_socket,std::string p_message)
			{
				try
				{
					SendData(p_socket, p_message);
				}
				catch (std::exception e)
				{
					return false;
				}
			}

			virtual void CloseGateKeeper()
			{}
		};
	}
}