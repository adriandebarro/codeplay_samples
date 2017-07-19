#pragma once 

#include <hash_map>
#include <string>
#include <thread>
#include <mutex>

#include <boost/lexical_cast.hpp>



#include "../AbstractPeer.h"
#include "../PeerTestingFramework/TestingCommoands/TestingInstrcution.h"
#include "../../ObservableEvents/ObservableEventsList/ObservaleEventsList.h"
#include "../../UtilityClasses/HelperClass.h"

using std::string;
namespace Network
{
	namespace Peer
	{
		template<typename T>
		class P2pPeer : public AbstarctPeer<T>
		{
		public:
			using AbstarctPeer<T>::m_updatesList;
			using AbstarctPeer<T>::CloseConnection;
			using AbstarctPeer<T>::OpenConnection;
			using AbstarctPeer<T>::neighbors;
			using AbstarctPeer<T>::SendData;
			using AbstarctPeer<T>::m_updateThread;
			using AbstarctPeer<T>::BootUpSequence;
			using AbstarctPeer<T>::ListenOnNetwork;
			using AbstarctPeer<T>::bootStrapAddress;
			using AbstarctPeer<T>::logicalTimer;
			using AbstarctPeer<T>::GetStringFromBitStream;
			using AbstarctPeer<T>::UpdateFunction;
			using AbstarctPeer<T>::m_listeningThread;
			using AbstarctPeer<T>::m_canExit;
			using AbstarctPeer<T>::m_sockerNumber;

			VectorTimer* m_vectorTimer;
			bool notServer;

			P2pPeer() :AbstarctPeer()
			{
				notServer = true;
				//BootUpSequence();
				ListenOnNetwork();
			}

			P2pPeer(string p_ipAddress, int p_socketNumber, int p_sequenceNumber, string p_nameAssigned) : AbstarctPeer(p_ipAddress, p_socketNumber, p_nameAssigned)
			{
				notServer = false;
				m_vectorTimer = new FixedVectorTimer(p_sequenceNumber);

				m_updatesList->SetVectorTimer(m_vectorTimer);
				this->m_updatesList = AbstarctPeer<T>::m_updatesList;
				BootUpSequence();
			}

			P2pPeer(string p_ipAddress, int p_socketNumber, int p_sequenceNumber, string p_nameAssigned, ObservableEvents::ObservableEventsList<T>* p_updateList) : AbstarctPeer(p_ipAddress, p_socketNumber, p_nameAssigned, p_updateList)
			{
				notServer = false;
				m_vectorTimer = new FixedVectorTimer(p_sequenceNumber);
				m_updatesList->SetVectorTimer(m_vectorTimer);
				BootUpSequence();
			}

			virtual void BootUpSequence()
			{
				//init sender peer 
				m_senderInterface = RakNet::RakPeerInterface::GetInstance();
				int senderPortNumber = LISTENING_PORT + m_sockerNumber + 2;
				SocketDescriptor senderSocket = SocketDescriptor(senderPortNumber, 0);
				m_senderInterface->Startup(MAXIMUM_AMOUNT_NEIGHBORS_GATE_KEEPER, &senderSocket, 1, THREAD_PRIORITY_NORMAL);
				m_senderInterface->SetMaximumIncomingConnections(MAXIMUM_AMOUNT_CONNECTIONS_PEER);

				logicalTimer = 0;
				m_updateThread = std::thread(&P2pPeer::UpdateFunction, this);
				m_senderThread = std::thread(&P2pPeer::SenderThread, this);
				//ListenOnNetwork();
			}

			virtual void ListenOnNetwork()
			{
				bool firstTimer = true;
				Packet *packet;
				if (!notServer)
				{
					OpenConnection(bootStrapAddress, to_string(LISTENING_PORT));
				}

				while (!m_canExit)
				{
					for (packet = peerData->Receive(); packet; peerData->DeallocatePacket(packet), packet = peerData->Receive())
					{
						switch (packet->data[0])
						{
						case ID_CONNECTION_ATTEMPT_FAILED:
						{
															 //retry to connect 
						}
							break;

						case ID_REMOTE_DISCONNECTION_NOTIFICATION:
							printf("Another client has disconnected.\n");
							break;

						case ID_REMOTE_CONNECTION_LOST:
							printf("Another client has lost the connection.\n");
							break;

						case ID_REMOTE_NEW_INCOMING_CONNECTION:
							printf("Another client has connected.\n");
							break;

							//connection accepted by a peer
						case ID_CONNECTION_REQUEST_ACCEPTED:
						{
															   cout << "connectio naccepted by peer " << packet->systemAddress.ToString() << endl;
															   ConnectionAttemptResult result = OpenConnection(packet->systemAddress);
															   if (result == ConnectionAttemptResult::ALREADY_CONNECTED_TO_ENDPOINT)
															   {
																   if (firstTimer)
																   {
																	   firstTimer = false;
																	   printf("Our connection request has been accepted");
																	   SendData(packet->systemAddress, "---", PEER_INTRO, peerData);
																	   printf("Our connection request has been accepted.\n");
																   }
																   else
																   {
																	   //initiate exchange between peers
																	   std::string localPeers = neighbors->GetRandomNeighbors();
																	   SendData(packet->systemAddress, localPeers, SEND_PEERS, peerData);
																	   m_openedConnection = false;
																   }
															   }
						}
							break;

						case PEER_INTRO:
							cout << "peer added" << endl;
							neighbors->AddNeighbor(packet->guid, packet->systemAddress);
							SendData(packet->systemAddress, neighbors->GetRandomNeighbors(), REPLY_TO_PEER_INTRO, peerData);
							//CloseConnection(packet);
							break;

						case REPLY_TO_PEER_INTRO:
						{
													string neighboursInfo = GetStringFromBitStream(*packet);
													cout << neighboursInfo << endl;
													neighbors->AddNewNeighbours(neighboursInfo);
													cout << neighbors->GetAmountPeers() << endl;
													CloseConnection(packet, peerData);
						}

							break;

						case ID_NEW_INCOMING_CONNECTION:
						{
														   printf("A connection is incoming %s .\n", packet->systemAddress.ToString());
						}
							break;

						case ID_NO_FREE_INCOMING_CONNECTIONS:
							printf("The server is full .\n");
							break;
						case ID_DISCONNECTION_NOTIFICATION:
							printf("We have been disconnected.%s .\n", packet->systemAddress.ToString());
							break;
						case ID_CONNECTION_LOST:
							printf("Connection lost.\n");
							//RemoveNeighbour(packet->systemAddress.ToString());
							break;

						case SEND_PEERS:
						{
										   string acquiredData = GetStringFromBitStream(*packet);
										   string peersToSend = neighbors->GetRandomNeighbors();
										   SendData(packet->systemAddress, peersToSend, REPLY_TO_PEERS, peerData);
										   neighbors->AddNewNeighbours(acquiredData);
						}
							break;

						case REPLY_TO_PEERS:
						{
											   string acquiredData = GetStringFromBitStream(*packet);
											   neighbors->AddNewNeighbours(acquiredData);
											   //first tick
											   m_vectorTimer->IncrementTimer();

											   string observableEvents = m_updatesList->GetOEUuids();

											   SendData(packet->systemAddress, observableEvents, SEND_OE_LIST, peerData);
						}
							break;
						case SEND_OE_LIST:
						{
											 string allEventsList = GetStringFromBitStream(*packet);
											 cout << "received uuids are " << allEventsList << endl;
											 string missingEvents = m_updatesList->GetMissingOes(allEventsList);
											 cout << "missing required events : " << missingEvents << endl;
											 SendData(packet->systemAddress, missingEvents, REQUEST_OE, peerData);
						}
							break;

						case REQUEST_OE:
						{
										   string requestedEvents = GetStringFromBitStream(*packet);
										   string stringifiedRequestedEvents = m_updatesList->ToString(requestedEvents);
										   cout << stringifiedRequestedEvents.size() << endl;
										   //cout << "the requested events are : " << requestedEvents << " the reply is :" <<stringifiedRequestedEvents << endl;
										   SendData(packet->systemAddress, stringifiedRequestedEvents, SEND_UPDATES, peerData);
						}
							break;

						case SEND_UPDATES:
						{
											 string totalInput = GetStringFromBitStream(*packet);
											 cout << "recieved data!!" << endl;
											 vector<string> data;
											 //cout << totalInput << endl;
											 HelperClass::ParseStringSentence(totalInput, "&", data);
											 VectorTimer* temp = new FixedVectorTimer(data[0]);
											 //if global states not equal 
											 std::lock_guard<std::mutex> myMutex(m_mutex);
											 if (!(m_vectorTimer == temp) && data.size() == 2)
											 {
												 m_updatesList->MergeEvents(data[1]);
											 }//if

											 m_vectorTimer->TakeSup(temp);
											 m_vectorTimer->IncrementTimer();

											 delete temp;
											 m_vectorTimer->IncrementTimer();
											 string returnString = m_updatesList->GetOEUuids();
											 SendData(packet->systemAddress, returnString, SECOND_SEND_OE_LIST, peerData);
						}
							break;
							//
						case SECOND_SEND_OE_LIST:
						{
													string allEventsList = GetStringFromBitStream(*packet);
													string missingEvents = m_updatesList->GetMissingOes(allEventsList);
													cout << "all recived events are " << allEventsList << " and the missing events are " << missingEvents << "" << endl;
													SendData(packet->systemAddress, missingEvents, SECOND_REQUEST_OE, peerData);
						}
							break;

						case SECOND_REQUEST_OE:
						{
												  string requestedEvents = GetStringFromBitStream(*packet);
												  string stringifiedRequestedEvents = m_updatesList->ToString(requestedEvents);
												  cout << "requested events  " << requestedEvents << " and the response is " << stringifiedRequestedEvents << endl;
												  SendData(packet->systemAddress, stringifiedRequestedEvents, REPLY_UPDATES, peerData);
						}
							break;

						case REPLY_UPDATES:
						{
											  string totalInput = GetStringFromBitStream(*packet);
											  vector<string> data;
											  //cout << totalInput << endl;
											  HelperClass::ParseStringSentence(totalInput, "&", data);
											  VectorTimer* temp = new FixedVectorTimer(data[0]);
											  //if global states not equal 
											  std::lock_guard<std::mutex> myMutex(m_mutex);
											  if (!(m_vectorTimer == temp) && data.size() == 2)
											  {
												  m_updatesList->MergeEvents(data[1]);
											  }

											  m_vectorTimer->TakeSup(temp);
											  m_vectorTimer->IncrementTimer();

											  delete temp;
											  CloseConnection(packet, peerData);
						}
							break;

						case TESTING_SET:
						{
											T stringVar = GetStringFromBitStream(*packet);
											//cout << "commanded to set " << stringVar << endl;
											m_vectorTimer->IncrementTimer();
											//m_updatesList->AddTestingUpdate(stringVar);
											//((FixedVectorTimer<string>*)m_vectorTimer)->SetUpdatedVersion(stringVar);
											CloseConnection(packet, peerData);
						}
							break;

						case TESTING_COMM:
						{
											 cout << "commanded to comm" << endl;
											 string stringVar = GetStringFromBitStream(*packet);
											 //fix here 
											 SystemAddress current;
											 current.FromString(stringVar.c_str());
											 CloseConnection(packet, peerData);
											 TestingRunUpdateProcedure(current);
						}
							break;

						case TESTING_UPDATE:
						{
											   cout << "UPDATE---------------------------" << endl;
											   cout << "vector timer : " << m_vectorTimer->ToString() << endl;
											   cout << "Observable events " << m_updatesList->ToString() << endl;
											   CloseConnection(packet, peerData);
						}
							break;
							//only used by testing framework
						case TESTING_GET_PEERS:
						{
												  cout << "Request by testing peer for all peers" << endl;
												  SendData(packet->systemAddress, neighbors->ParseNeighbours(), TESTING_SEND_PEERS, peerData);
						}
							break;

						default:
							//printf("Message with identifier %i has arrived. %s \n", packet->data[0]);
							break;
						}
					}
				}
			}

			virtual void SenderThread()
			{
				bool firstTimer = true;
				Packet *packet;

				while (!m_canExit)
				{
					for (packet = m_senderInterface->Receive(); packet; m_senderInterface->DeallocatePacket(packet), packet = m_senderInterface->Receive())
					{
						switch (packet->data[0])
						{
						case ID_CONNECTION_ATTEMPT_FAILED:
						{
															 //retry to connect 
						}
							break;

						case ID_REMOTE_DISCONNECTION_NOTIFICATION:
							printf("Another client has disconnected.\n");
							break;

						case ID_REMOTE_CONNECTION_LOST:
							printf("Another client has lost the connection.\n");
							break;

						case ID_REMOTE_NEW_INCOMING_CONNECTION:
							printf("Another client has connected.\n");
							break;

							//connection accepted by a peer
						case ID_CONNECTION_REQUEST_ACCEPTED:
						{
															   cout << "connectio naccepted by peer " << packet->systemAddress.ToString() << endl;
															   ConnectionAttemptResult result = OpenConnection(packet->systemAddress);
															   if (result == ConnectionAttemptResult::ALREADY_CONNECTED_TO_ENDPOINT)
															   {
																   //initiate exchange between peers
																   std::string localPeers = neighbors->GetRandomNeighbors();
																   SendData(packet->systemAddress, localPeers, SEND_PEERS, m_senderInterface);
																   m_openedConnection = false;
															   }
						}
							break;

						case PEER_INTRO:
							cout << "peer added" << endl;
							neighbors->AddNeighbor(packet->guid, packet->systemAddress);
							SendData(packet->systemAddress, neighbors->GetRandomNeighbors(), REPLY_TO_PEER_INTRO, m_senderInterface);
							//CloseConnection(packet);
							break;

						case REPLY_TO_PEER_INTRO:
						{
													string neighboursInfo = GetStringFromBitStream(*packet);
													cout << neighboursInfo << endl;
													neighbors->AddNewNeighbours(neighboursInfo);
													cout << neighbors->GetAmountPeers() << endl;
													CloseConnection(packet, m_senderInterface);
						}

							break;

						case ID_NEW_INCOMING_CONNECTION:
						{
														   printf("A connection is incoming %s .\n", packet->systemAddress.ToString());
						}
							break;

						case ID_NO_FREE_INCOMING_CONNECTIONS:
							printf("The server is full .\n");
							break;
						case ID_DISCONNECTION_NOTIFICATION:
							printf("We have been disconnected.%s .\n", packet->systemAddress.ToString());
							break;
						case ID_CONNECTION_LOST:
							printf("Connection lost.\n");
							//RemoveNeighbour(packet->systemAddress.ToString());
							break;

						case SEND_PEERS:
						{
										   string peerData = GetStringFromBitStream(*packet);
										   string peersToSend = neighbors->GetRandomNeighbors();
										   SendData(packet->systemAddress, peersToSend, REPLY_TO_PEERS, m_senderInterface);
										   neighbors->AddNewNeighbours(peerData);
						}
							break;

						case REPLY_TO_PEERS:
						{
											   string peerData = GetStringFromBitStream(*packet);
											   neighbors->AddNewNeighbours(peerData);
											   //first tick
											   m_vectorTimer->IncrementTimer();

											   string observableEvents = m_updatesList->GetOEUuids();

											   SendData(packet->systemAddress, observableEvents, SEND_OE_LIST, m_senderInterface);
						}
							break;
						case SEND_OE_LIST:
						{
											 string allEventsList = GetStringFromBitStream(*packet);
											 cout << "received uuids are " << allEventsList << endl;
											 string missingEvents = m_updatesList->GetMissingOes(allEventsList);
											 cout << "missing required events : " << missingEvents << endl;
											 SendData(packet->systemAddress, missingEvents, REQUEST_OE, m_senderInterface);
						}
							break;

						case REQUEST_OE:
						{
										   string requestedEvents = GetStringFromBitStream(*packet);
										   string stringifiedRequestedEvents = m_updatesList->ToString(requestedEvents);
										   cout << stringifiedRequestedEvents.size() << endl;
										   //cout << "the requested events are : " << requestedEvents << " the reply is :" <<stringifiedRequestedEvents << endl;
										   SendData(packet->systemAddress, stringifiedRequestedEvents, SEND_UPDATES, m_senderInterface);
						}
							break;

						case SEND_UPDATES:
						{
											 string totalInput = GetStringFromBitStream(*packet);
											 cout << "recieved data!!" << endl;
											 vector<string> data;
											 //cout << totalInput << endl;
											 HelperClass::ParseStringSentence(totalInput, "&", data);
											 VectorTimer* temp = new FixedVectorTimer(data[0]);
											 //if global states not equal 
											 std::lock_guard<std::mutex> myMutex(m_mutex);
											 if (!(m_vectorTimer == temp) && data.size() == 2)
											 {
												 m_updatesList->MergeEvents(data[1]);
											 }//if

											 m_vectorTimer->TakeSup(temp);
											 m_vectorTimer->IncrementTimer();

											 delete temp;
											 m_vectorTimer->IncrementTimer();
											 string returnString = m_updatesList->GetOEUuids();
											 SendData(packet->systemAddress, returnString, SECOND_SEND_OE_LIST, m_senderInterface);
						}
							break;
							//
						case SECOND_SEND_OE_LIST:
						{
													string allEventsList = GetStringFromBitStream(*packet);
													string missingEvents = m_updatesList->GetMissingOes(allEventsList);
													cout << "all recived events are " << allEventsList << " and the missing events are " << missingEvents << "" << endl;
													SendData(packet->systemAddress, missingEvents, SECOND_REQUEST_OE, m_senderInterface);
						}
							break;

						case SECOND_REQUEST_OE:
						{
												  string requestedEvents = GetStringFromBitStream(*packet);
												  string stringifiedRequestedEvents = m_updatesList->ToString(requestedEvents);
												  cout << "requested events  " << requestedEvents << " and the response is " << stringifiedRequestedEvents << endl;
												  SendData(packet->systemAddress, stringifiedRequestedEvents, REPLY_UPDATES, m_senderInterface);
						}
							break;

						case REPLY_UPDATES:
						{
											  string totalInput = GetStringFromBitStream(*packet);
											  vector<string> data;
											  //cout << totalInput << endl;
											  HelperClass::ParseStringSentence(totalInput, "&", data);
											  VectorTimer* temp = new FixedVectorTimer(data[0]);
											  //if global states not equal 
											  std::lock_guard<std::mutex> myMutex(m_mutex);
											  if (!(m_vectorTimer == temp) && data.size() == 2)
											  {
												  m_updatesList->MergeEvents(data[1]);
											  }

											  m_vectorTimer->TakeSup(temp);
											  m_vectorTimer->IncrementTimer();

											  delete temp;
											  CloseConnection(packet, m_senderInterface);
						}
							break;

						case TESTING_SET:
						{
											T stringVar = GetStringFromBitStream(*packet);
											//cout << "commanded to set " << stringVar << endl;
											m_vectorTimer->IncrementTimer();
											//m_updatesList->AddTestingUpdate(stringVar);
											//((FixedVectorTimer<string>*)m_vectorTimer)->SetUpdatedVersion(stringVar);
											CloseConnection(packet, m_senderInterface);
						}
							break;

						case TESTING_COMM:
						{
											 cout << "commanded to comm" << endl;
											 string stringVar = GetStringFromBitStream(*packet);
											 //fix here 
											 SystemAddress current;
											 current.FromString(stringVar.c_str());
											 CloseConnection(packet, m_senderInterface);
											 TestingRunUpdateProcedure(current);
						}
							break;

						case TESTING_UPDATE:
						{
											   cout << "UPDATE---------------------------" << endl;
											   cout << "vector timer : " << m_vectorTimer->ToString() << endl;
											   cout << "Observable events " << m_updatesList->ToString() << endl;
											   CloseConnection(packet, m_senderInterface);
						}
							break;
							//only used by testing framework
						case TESTING_GET_PEERS:
						{
												  cout << "Request by testing peer for all peers" << endl;
												  SendData(packet->systemAddress, neighbors->ParseNeighbours(), TESTING_SEND_PEERS, m_senderInterface);
						}
							break;

						default:
							//printf("Message with identifier %i has arrived. %s \n", packet->data[0]);
							break;
						}
					}
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


			///-------------------------------------------------------------------------------------------------
			/// <summary>	Anti Entropy Starter. </summary>
			///
			/// <remarks>	Adrian, 21/09/2015. </remarks>
			///-------------------------------------------------------------------------------------------------

			virtual void UpdateFunction()
			{
				cout << "Initiating update sequence " << endl;
				while (1)
				{
					Neighbour* details = nullptr;
					details = neighbors->GetRandomNeighbour();
					if ((details != nullptr) && m_updatesList->GetAmountObservablEvents() > 0)
					{
						m_connectionList[details->address->ToString()] = *details->address;
						cout << "opening connection" << endl;
						//will be opened form the thread sender
						OpenConnection(details, m_senderInterface);
						m_openedConnection = true;
					}
					else if (m_attemptCounter >= 40 && m_openedConnection == true)
					{
						m_openedConnection = false;
						m_attemptCounter = 0;
					}
					else
					{
						m_attemptCounter++;
					}

					this_thread::sleep_for(chrono::milliseconds(11000));
				}
			}

			using AbstarctPeer<T>::AddUpdate;
			virtual void AddUpdate(T* p_updateToAdd)
			{
				/*if (m_updatesList->AddLocalUpdate(p_updateToAdd))
				{
				UpdateFunction();
				}*/
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Executes the update procedure operation. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///-------------------------------------------------------------------------------------------------

			//void RunUpdateProcedure()
			//{
			//	//tick
			//	//m_vectorTimer->IncrementTimer();
			//	std::string localPeers = neighbors->ParseNeighbours();
			//	updatesExist = false;
			//	RakNet::ConnectionAttemptResult result;
			//	Neighbour* details = nullptr;
			//	do
			//	{
			//		details = neighbors->GetRandomNeighbour();
			//		OpenConnection(details);
			//		
			//	} while (result != 0);
			//	//send vector timer
			//	SendData(details, localPeers, SEND_PEERS);
			//	//std::this_thread::sleep_for(std::chrono::microseconds(HelperClass::SetAmounTimeSleep(FIXED_AMOUNT_PEERS) * 100));
			//}

			void TestingRunUpdateProcedure(SystemAddress p_systemAddress)
			{
				//tick
				//m_vectorTimer->IncrementTimer();
				string returnString = m_vectorTimer->ToString() + "&" + m_updatesList->ToString();
				OpenConnection(p_systemAddress);
			}


			~P2pPeer()
			{

			}

		private:
			Neighbour* m_chosenNeighbor;

			bool m_openedConnection = false;

			int m_attemptCounter = 0;

			hash_map<string, SystemAddress> m_connectionList;

			bool m_currentlyExchanging = false;

			RakNet::RakPeerInterface* m_senderInterface;
			thread m_senderThread;
			std::mutex m_mutex;

		};//class
	}//peer
}//network

