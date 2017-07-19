#pragma once 

#include <hash_map>
#include <string>
#include <thread>
#include <mutex>
#include <future>

#include <boost/lexical_cast.hpp>

#include "../AbstractPeer.h"
#include "../PeerTestingFramework\TestingCommands\TestingInstruction.h"
//#include "Peer/PeerTestingFramework/TestingCommands/TestingInstrcution.h"
//#include "../PeerTestingFramework/TestingCommoands/TestingInstrcution.h"
#include "../../ObservableEvents/ObservableEventsList/ObservaleEventsList.h"
#include "../../UtilityClasses/HelperClass.h"
#include "../../SuperPeer/SuperPeerManager.h"
#include "../../ObservableEvents/ObservableEventsConsumer.h"

using std::string;
using Network::ObservableEvents::ObservableEventsConsumer;
namespace Network
{
	namespace Peer
	{

		//communication threads will push received data the ObservableEventsConsumer 
		#define USE_OBSERVABLE_EVENTS_CONSUMER

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	A P2P peer. Can either be a gatekeeper or a peer</summary>
		/// <summary>  GateKeeper used for bootstrapping of the peer to peer network </summary>
		///
		/// <remarks>	Adrian, 31/08/2016. </remarks>
		///
		/// <typeparam name="T"> 	Generic type parameter. </typeparam>
		/// <typeparam name="S"> 	Type of the s. </typeparam>
		/// <typeparam name="U"> 	Generic type parameter // depecrated. </typeparam>
		/// <typeparam name="AS">	Type of as referes to the acceleration structure. </typeparam>
		/// <typeparam name="N"> 	Type of the n refers to the neighbour. </typeparam>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		using namespace Network::SuperPeers;

		template<typename T, typename S, typename U, typename AS, typename N = Neighbour>
		class P2pPeer : public AbstarctPeer<T,S,U, AS, N>
		{
		public:
			using AbstarctPeer<T,S,U, AS,N>::m_updatesList;
			using AbstarctPeer<T,S,U, AS,N>::CloseConnection;
			using AbstarctPeer<T,S,U, AS,N>::OpenConnection;
			using AbstarctPeer<T,S,U, AS,N>::neighbors;
			using AbstarctPeer<T,S,U, AS,N>::SendData;
			using AbstarctPeer<T,S,U, AS,N>::m_updateThread;
			using AbstarctPeer<T,S,U, AS,N>::BootUpSequence;
			using AbstarctPeer<T,S,U, AS,N>::ListenOnNetwork;
			using AbstarctPeer<T,S,U, AS,N>::bootStrapAddress;
			using AbstarctPeer<T,S,U, AS,N>::logicalTimer;
			using AbstarctPeer<T,S,U, AS,N>::UpdateFunction;
			using AbstarctPeer<T,S,U, AS,N>::m_listeningThread;
			using AbstarctPeer<T,S,U, AS,N>::m_canExit;
			using AbstarctPeer<T,S,U, AS,N>::m_sockerNumber;
			using AbstarctPeer<T,S,U, AS,N>::ReceiveData;
			using AbstarctPeer<T,S,U, AS,N>::ipSet;
			
			VectorTimer* m_vectorTimer;
			bool notServer;

			P2pPeer(int p_noReply) 
				:AbstarctPeer()
			{
				m_dontStore = p_noReply;
				notServer = true;
				m_sockerNumber = GATEKEEPER_LISTENING_PORT;
				//BootUpSequence();
				m_updateThread = std::thread(&P2pPeer::ListenNetwork,  this);
			}

			P2pPeer(string p_ipAddress, int p_socketNumber, int p_sequenceNumber, string p_nameAssigned, int p_amountPeers) 
				: AbstarctPeer(p_ipAddress, p_socketNumber, p_nameAssigned)
			{
				notServer = false;
				m_vectorTimer = new FixedVectorTimer(p_sequenceNumber);
				m_updatesList->SetVectorTimer(m_vectorTimer);
				this->m_updatesList = AbstarctPeer<T, S>::m_updatesList;

				#ifdef USE_OBSERVABLE_EVENTS_CONSUMER
					m_observablEventConsumer = new ObservableEventsConsumer<T, S, AS>(m_updatesList);
				#endif

				BootUpSequence();
			}

			P2pPeer(string p_ipAddress, int p_socketNumber, int p_sequenceNumber, string p_nameAssigned, ObservableEvents::ObservableEventsList<T, S, AS>* p_updateList, int p_amountPeers) 
				: AbstarctPeer(p_ipAddress, p_socketNumber, p_nameAssigned, p_updateList)
			{
				notServer = false;
				m_vectorTimer = new FixedVectorTimer(p_sequenceNumber, p_amountPeers);
				m_updatesList->SetVectorTimer(m_vectorTimer);
				#ifdef USE_OBSERVABLE_EVENTS_CONSUMER
					m_observablEventConsumer = new ObservableEventsConsumer<T, S, AS>(m_updatesList);
				#endif
				BootUpSequence();
			}

			P2pPeer(string p_ipAddress, int p_socketNumber, int p_sequenceNumber, string p_nameAssigned, ObservableEvents::ObservableEventsList<T, S, AS>* p_updateList, int p_amountPeers, float p_fltSleepAfterComm, float p_fltSleepAfterFailedAttempt, SuperPeerManager* p_superPeerManager = nullptr, bool p_shareUpdates = false)
				: AbstarctPeer(p_ipAddress, p_socketNumber, p_nameAssigned, p_updateList)
			{
				notServer = false;
				m_fltSleepAfterComm = p_fltSleepAfterComm * 1000;
				m_fltSleepAfterFailedAttempt = p_fltSleepAfterFailedAttempt * 1000;
				cout << to_string(m_fltSleepAfterComm) << "::" << to_string(m_fltSleepAfterFailedAttempt) << endl;
				m_vectorTimer = new FixedVectorTimer(p_sequenceNumber, p_amountPeers);
				m_peerIndex = p_sequenceNumber;
				m_superPeerManager = p_superPeerManager;
				m_exchangeSuperPeers = p_shareUpdates;
				m_updatesList->SetVectorTimer(m_vectorTimer);

				#ifdef USE_OBSERVABLE_EVENTS_CONSUMER
					m_observablEventConsumer = new ObservableEventsConsumer<T, S, AS>(m_updatesList);
				#endif

				BootUpSequence();
			}


			P2pPeer(string p_ipAddress, int p_socketNumber, int p_sequenceNumber, string p_nameAssigned, ObservableEvents::ObservableEventsList<T, S, AS>* p_updateList, int p_amountPeers, float p_fltSleepAfterComm, float p_fltSleepAfterFailedAttempt, NeighbourManager<N>* p_neighborManager , SuperPeerManager* p_superPeerManager = nullptr, bool p_shareUpdates = false) 
				: AbstarctPeer(p_ipAddress, p_socketNumber, p_nameAssigned, p_updateList, p_neighborManager)
			{
				notServer = false;
				m_fltSleepAfterComm = p_fltSleepAfterComm * 1000;
				m_fltSleepAfterFailedAttempt = p_fltSleepAfterFailedAttempt * 1000;
				cout << to_string(m_fltSleepAfterComm) << "::" << to_string(m_fltSleepAfterFailedAttempt) << endl;
				m_vectorTimer = new FixedVectorTimer(p_sequenceNumber, p_amountPeers);
				m_peerIndex = p_sequenceNumber;
				m_superPeerManager = p_superPeerManager;
				m_exchangeSuperPeers = p_shareUpdates;
				m_updatesList->SetVectorTimer(m_vectorTimer);
				neighbors = p_neighborManager;

				#ifdef USE_OBSERVABLE_EVENTS_CONSUMER
					m_observablEventConsumer = new ObservableEventsConsumer<T, S, AS>(m_updatesList);
				#endif
				BootUpSequence();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Boot up sequence for a peer </summary>
			/// <summary>   Each peer is made from two threads </summary>
			///
			/// <remarks>	Adrian </remarks>
			////////////////////////////////////////////////////////////////////////////////////////////////////

			virtual void BootUpSequence()
			{
				logicalTimer = 0;
				cout << bootStrapAddress << endl;
				m_updateThread = std::thread(&P2pPeer::ListenOnNetwork, this);
				m_senderThread = std::thread(&P2pPeer::SenderThread, this);
				//ListenOnNetwork();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary> Closes gate keeper. </summary>
			///
			/// <remarks> Adrian </remarks>
			////////////////////////////////////////////////////////////////////////////////////////////////////

			void CloseGateKeeper()
			{
				m_canExit = true;
				gateKeeperAcceptor->close();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary> Listen network. </summary>
			///	<summary> Gate Keeper behaviour</summary>
			/// <remarks> Adrian </remarks>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			void ListenNetwork()
			{
				if (m_dontStore == 1)
					cout << "no save gatekeeper" << endl;
				
				boost::asio::io_service aios;
				gateKeeperAcceptor = new boost::asio::ip::tcp::acceptor(aios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), GATEKEEPER_LISTENING_PORT));
				boost::asio::ip::tcp::socket socket(aios);

				string previousPeer = "";

				while (!m_canExit)
				{
					gateKeeperAcceptor->accept(socket);
					cout << "waiting for a connection" << socket.remote_endpoint().port() << endl;
					std::string sClientIp = socket.remote_endpoint().address().to_string();
					//remove this 
					string receivedDetails = ReceiveData(socket);

					if (m_dontStore != 1)
						neighbors->AddNeighbor(_strdup(sClientIp.c_str()));

					string allKnownNeighbours = neighbors->GetNeighborRep(previousPeer) + "|";
					previousPeer = sClientIp;
					boost::asio::write(socket, boost::asio::buffer(allKnownNeighbours));
					CloseConnection(socket);
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary> Listen on network. </summary>
			///	<summary> Behaviour of the listener thread </summary>
			/// <remarks> Adrian </remarks>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			virtual void ListenOnNetwork()
			{
				bool firstTimer = true;

				// data dynamic containers
				vector<unsigned char> receivedData;
				vector<unsigned char> replyingData;

				boost::asio::io_service aios;
				
				if (notServer)
				{
					boost::asio::ip::tcp::acceptor acceptor(aios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), GATEKEEPER_LISTENING_PORT));
					boost::asio::ip::tcp::socket socket(aios);

					while (!m_canExit)
					{
						acceptor.accept(socket);

						cout << "accepting new members now " << "\n";

						std::string sClientIp = socket.remote_endpoint().address().to_string();
						
						//remove this 
						string receivedDetails = ReceiveData(socket);
						
						neighbors->AddNeighbor(_strdup(sClientIp.c_str()));
						CloseConnection(socket);
						cout << "communication ended " << endl;
					}
				}
				else
				{
					boost::asio::ip::tcp::acceptor acceptor(aios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), LISTENING_PORT));
					boost::asio::ip::tcp::socket socket(aios);
					string currentPeerIp;
					while (!m_canExit)
					{
						try
						{
							acceptor.accept(socket);
							currentPeerIp = socket.remote_endpoint().address().to_string();

								if (currentPeerIp == "127.0.0.1")
								{
									CloseConnection(socket);
								}
								else
								{

									char* strIp = const_cast<char*>(currentPeerIp.c_str());
									neighbors->AddNeighbor(strIp);
									//neighbors->UpdateNeighborTime(string(strIp));
									cout << "[LISTENER] Connected to " + currentPeerIp + "::" + to_string(m_sockerNumber) << "\n";
									
									std::string localPeers = neighbors->GetRandomNeighbors(currentPeerIp);

									//send peers
									SendData(localPeers + "|", socket);
									string peers = ReceiveData(socket);

									//recieve peers 
									neighbors->AddNewNeighbours(peers);
									//first tick
									m_vectorTimer->IncrementTimer();

									//send peer Id
									receivedData.erase(receivedData.begin(), receivedData.end());
									replyingData.erase(replyingData.begin(), replyingData.end());

									//exchange of super peer information is being exchnaged 
									if (m_exchangeSuperPeers)
									{
										m_superPeerManager->SerializeSuperPeers(replyingData);
										SendData(replyingData, socket);
										cout << "waiting to receive the superpeer details" << endl;
										ReceiveData(socket, receivedData);
										m_superPeerManager->MatchSuperPeers(receivedData);
										cout << "matched peers and ready" << endl;
									}

									receivedData.erase(receivedData.begin(), receivedData.end());
									replyingData.erase(replyingData.begin(), replyingData.end());

									HelperClass::AddIntBytes(replyingData, m_peerIndex);
									SendData(replyingData, socket);

									//[AD] Receive for syncing receive
									ReceiveData(socket, receivedData);

									//clear containers 
									receivedData.erase(receivedData.begin(), receivedData.end());
									replyingData.erase(replyingData.begin(), replyingData.end());

									//send available events
									m_updatesList->SerializeAllUuids(replyingData);
									SendData(replyingData, socket);
									cout << "[LISTENER] uuids sent" << "\n";
									
									//clear containers 
									replyingData.erase(replyingData.begin(), replyingData.end());
									receivedData.erase(receivedData.begin(), receivedData.end());

									//receive requested OE
									ReceiveData(socket, receivedData);
									m_vectorTimer->Serialize(replyingData);
									m_updatesList->GetNeededOE(receivedData, replyingData, 0, 0);
									cout << "[LISTENER] Sending missing events size of vector " << replyingData.size() << "\n";
									SendData(replyingData, socket);

									//clear containers 
									receivedData.erase(receivedData.begin(), receivedData.end());
									replyingData.erase(replyingData.begin(), replyingData.end());

									ReceiveData(socket, receivedData);
									if (receivedData.size() >= 4)
										m_updatesList->FindMissingOE(receivedData, replyingData);
									else
										HelperClass::AddIntBytes(replyingData, 0);

									cout << "[LISTENER] requesting missing events" << endl;
									SendData(replyingData, socket);

									receivedData.erase(receivedData.begin(), receivedData.end());

									//clock_t startingTimeReceiving = clock();
									ReceiveData(socket, receivedData);
									//clock_t endingTimeTeceiving = clock();

									//cout << "resulting time  for receving process " << ((double)endingTimeTeceiving - startingTimeReceiving) / (double)CLOCKS_PER_SEC << endl;

									if (receivedData.size() > 0)
									{
										VectorTimer* temp = new FixedVectorTimer();

										int startingPoint = 0;
										temp->FromByteArray(receivedData, startingPoint);
										//if global states not equal 
										
										if (receivedData.size() > 20)
										{
											#ifdef USE_OBSERVABLE_EVENTS_CONSUMER
												m_observablEventConsumer->AddEvents(receivedData, (m_vectorTimer->GetAmountTimers() * 4 + 4));
											#else
												std::lock_guard<std::mutex> myMutex(m_mutex);
												m_updatesList->MergeEvents(receivedData, (m_vectorTimer->GetAmountTimers() * 4 + 4));
											#endif
										}

										m_vectorTimer->TakeSup(temp);

										m_vectorTimer->IncrementTimer();
										cout << "[LISTENER] resulting clock" << m_vectorTimer->ToString() << "\n";
										delete temp;
									}
									else
									{
										cout << "[LISTENER] received less than 20 bytes when communicating" << "\n";
									}

									CloseConnection(socket);
									neighbors->FlushQueue();
							}
						}
						catch (exception e)
						{
							cout << "connection details " << currentPeerIp << endl;
							cout << " [LISTENER] problem " << e.what() <<"\n";
							CloseConnection(socket);
						}
					}
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Behaviour of the sender thread, trys to communicate with a random peer every delta time </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			////////////////////////////////////////////////////////////////////////////////////////////////////

			virtual void SenderThread()
			{
				IoSrevice aios;
				Socket socket(aios);
				vector<unsigned char> receiveData;
				vector<unsigned char> replyingData;
				bool firstTimer = true;
			EXIT_SENDING_THREAD:				
				while (!m_canExit)
				{
					bool foundNewNeighbour = false;
					//contacting the gatekeeper for the last peer's details
					if (!notServer && firstTimer)
					{
						try
						{
							cout << "opening a new connection" << bootStrapAddress << "and" << GATEKEEPER_LISTENING_PORT << "\n";
							OpenConnection(bootStrapAddress.c_str(), to_string(GATEKEEPER_LISTENING_PORT).c_str(), socket, aios);
							string ipAddress = socket.local_endpoint().address().to_string();
							neighbors->SetInternalIp(ipAddress);
							m_ipAddress = ipAddress;
							ipSet = true;
							SendData(to_string(m_sockerNumber) + "|", socket);
							string data = ReceiveData(socket);
							
							CloseConnection(socket);
							firstTimer = false;
							neighbors->AddNewNeighbours(data);
						}
						catch (exception ex)
						{
							cout << "issue when communicating with gate keeper" << endl;
							break;
						}
					}

					N* oldDetails = nullptr;
					N* details = nullptr;

					//pick a random peer for communication
					while (!foundNewNeighbour && !m_canExit)
					{
						if (m_canExit)
							break;
					
						details = nullptr;
						//cout << "[SENDER] getting random neighbor" << "\n";
						details = (N*)neighbors->GetRandomNeighbor();
						if (neighbors->NumberOfNeighbours() > 1 && details != nullptr && oldDetails != nullptr && ((string)details->m_Ip) == ((string)oldDetails->m_Ip))
						{
							continue;
						}
						if (details != nullptr && (details->m_Ip != "127.0.0.1" && details->m_Ip != ""))
						{
							//will be opened form the thread sender
							foundNewNeighbour = OpenConnection(details->m_Ip, const_cast<char*>(to_string(m_sockerNumber).c_str()), socket, aios);
							oldDetails = details;
						}

						if (details != nullptr && !foundNewNeighbour )
						{
							details->ReadyFromPeer();
							this_thread::sleep_for(chrono::milliseconds(m_fltSleepAfterFailedAttempt));
						}
					}

					if (!foundNewNeighbour&&m_canExit)
					{
						break;
					}

					//communicate with the other peer 
					string stringIp = (string)(details->m_Ip);
					int receiverIndex = 0;
					try
					{
						details->IncrementLogicalTime();
						neighbors->UpdateNeighborTime(string(details->m_Ip));
						//receive peers
						std::string receivedPeers = ReceiveData(socket);
						
						//sending peers
						SendData(neighbors->GetRandomNeighbors(details->m_Ip) + "|", socket);
						neighbors->AddNewNeighbours(receivedPeers);

						receiveData.erase(receiveData.begin(), receiveData.end());
						replyingData.erase(replyingData.begin(), replyingData.end());

						//exchange superpeers details
						if (m_exchangeSuperPeers)
						{
							m_superPeerManager->SerializeSuperPeers(replyingData);
							ReceiveData(socket, receiveData);
							SendData(replyingData, socket);
							m_superPeerManager->MatchSuperPeers(receiveData);
						}

						//receive peerIndex
						receiveData.erase(receiveData.begin(), receiveData.end());
						replyingData.erase(replyingData.begin(), replyingData.end());

						ReceiveData(socket, receiveData);
						HelperClass::ByteArrayToInt(receiverIndex, receiveData);
						
						receiveData.erase(receiveData.begin(), receiveData.end());
						HelperClass::AddIntBytes(replyingData, 0);
						SendData(replyingData, socket);

						//tick
						m_vectorTimer->IncrementTimer();

						//receiving all uuids
						ReceiveData(socket, receiveData);

						replyingData.erase(replyingData.begin(), replyingData.end());

						//finding missing uuids and sending them back
						if (receiveData.size() > 0)
							m_updatesList->FindMissingOE(receiveData, replyingData);
						else
							HelperClass::AddIntBytes(replyingData, 0);

						SendData(replyingData, socket);
						receiveData.erase(receiveData.begin(), receiveData.end());
						
						//receiving requested OE
						ReceiveData(socket, receiveData);

						if (receiveData.size() > 20)
						{
							VectorTimer* temp = new FixedVectorTimer();

							int tempStart = 0;
							temp->FromByteArray(receiveData, tempStart);

							//if global states not equal 
							if (receiveData.size() > 0)
							{
								#ifdef USE_OBSERVABLE_EVENTS_CONSUMER
									m_observablEventConsumer->AddEvents(receiveData, (m_vectorTimer->GetAmountTimers() * 4 + 4));
								#else
									std::lock_guard<std::mutex> myMutex(m_mutex);
									m_updatesList->MergeEvents(receivedData, (m_vectorTimer->GetAmountTimers() * 4 + 4));
								#endif
							}

							//take supremum of the two clocks 
							m_vectorTimer->TakeSup(temp);
							m_vectorTimer->IncrementTimer();
							cout << "[sender] resulting clock" << m_vectorTimer->ToString() << "\n";
							delete temp;
						}
						else
						{
							cout << "received less than 20 bytes when communicating" << "\n";
						}

						//send available events
						receiveData.erase(receiveData.begin(), receiveData.end());
						replyingData.erase(replyingData.begin(), replyingData.end());

						m_updatesList->SerializeAllUuids(replyingData);
						//cout << "[SENDER] serialized all uuids" << endl;
						SendData(replyingData, socket);

						receiveData.erase(receiveData.begin(), receiveData.end());
						replyingData.erase(replyingData.begin(), replyingData.end());
						//receive requested OE
						// 
						ReceiveData(socket, receiveData);
						m_vectorTimer->Serialize(replyingData);
						m_updatesList->GetNeededOE(receiveData, replyingData, 1, receiverIndex);

						SendData(replyingData, socket);

					}
					catch (std::exception ex)
					{
						//m_errorLogger.LogError(ex.what());
						details->ReadyFromPeer();
						cout << "[SENDER] problem " << ex.what() << endl;
						this_thread::sleep_for(chrono::milliseconds(m_fltSleepAfterComm));
					}
					
					//close connection with the other peer 
					CloseConnection(socket);

					//unlock peer so that it can be removed is need be 
					details->ReadyFromPeer();
					neighbors->FlushQueue();
				
					this_thread::sleep_for(chrono::milliseconds(m_fltSleepAfterComm));
				}
			}


			string GetMissingOE(string p_oeList)
			{
				vector<string> uuidList;
				HelperClass::ParseStringSentence(p_oeList, ";", uuidList);

				stringstream outputStream;

				for (int index = 0; index < uuidList.size(); index++)
				{
					if (!m_updatesList->find(boost::lexical_cast<uuid>(uuidList[index])))
					{
						outputStream << uuidList[index] << ";";
					}
				}

				return outputStream.str();
			}

			virtual void UpdateFunction()
			{}


			using AbstarctPeer<T,S,U, AS, N>::AddUpdate;
			virtual void AddUpdate(T* p_updateToAdd)
			{}
			
			FixedVectorTimer* GetCurrentVectorTimer()
			{
				return (FixedVectorTimer*)m_vectorTimer;
			}

			~P2pPeer()
			{

			}

		private:
			N* m_chosenNeighbor;
			bool m_openedConnection = false;
			int m_attemptCounter = 0;
			bool m_currentlyExchanging = false;
			thread m_senderThread;
			std::mutex m_mutex;
			int m_fltSleepAfterComm = 0;
			int m_fltSleepAfterFailedAttempt = 0;
			int m_dontStore = 0;
			boost::asio::ip::tcp::acceptor* gateKeeperAcceptor;
			int m_peerIndex;

			SuperPeerManager* m_superPeerManager;
			bool m_exchangeSuperPeers = false;

			#ifdef USE_OBSERVABLE_EVENTS_CONSUMER
				Network::ObservableEvents::ObservableEventsConsumer<T, S, AS>* m_observablEventConsumer;
			#endif
		};//class
	}//peer
}//network

 