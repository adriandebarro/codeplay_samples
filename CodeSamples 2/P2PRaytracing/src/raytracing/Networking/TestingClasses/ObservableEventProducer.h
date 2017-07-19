#pragma once 

#include <Peer/AbstractPeer.h>
#include <ObservableEvents/ObservableEvent.h>
#include <ObservableEvents/ObservableEventsList/ObservaleEventsList.h>
#include <UtilityClasses/HelperClass.h>


#include "../../Integrators/RadianceDetails/RadiancePoint.h"
#include "../ObservableEvents/InsertEvent.h"
#include "../ObservableEvents/InvalidationEvent.h"
#include "../../CoreClasses/AssetsManager/LightManager.h"

#include "../../Integrators/IrradianceCache.h"
#include "../../Networking/ObservableEventsList/ICObservableEventsList.h"

#include "../../Light/Light.h"

using Network::Peer::AbstarctPeer;
using Network::ObservableEvents::ObservableEventsList;
using Engine::Networking::ObservableEvents::ICObservableEventsList;

namespace Engine
{
	namespace Networking
	{ 
		namespace Testing
		{
			namespace EventProducer
			{
				using Engine::ResourceManager::LightManager;
				using Engine::Light;

				//------------------------------------------------------------------------------
				// ObservableEventProducer: Testing class Event Producer.
				//							The sole purpose of this class is to test 
				//							invalidations and insert events updates 
				//------------------------------------------------------------------------------
				template<typename T, typename S, typename U, typename AS>
				class ObservableEventProducer : public AbstarctPeer<T, S, U, AS>
				{
				public:
					using AbstarctPeer<T, S, U, AS>::m_updatesList;
					using AbstarctPeer<T, S, U, AS>::CloseConnection;
					using AbstarctPeer<T, S, U, AS>::OpenConnection;
					using AbstarctPeer<T, S, U, AS>::neighbors;
					using AbstarctPeer<T, S, U, AS>::SendData;
					using AbstarctPeer<T, S, U, AS>::m_updateThread;
					using AbstarctPeer< T, S, U, AS>::BootUpSequence;
					using AbstarctPeer<T, S, U, AS>::ListenOnNetwork;
					using AbstarctPeer<T, S, U, AS>::bootStrapAddress;
					using AbstarctPeer<T, S, U, AS>::logicalTimer;
					using AbstarctPeer<T, S, U, AS>::UpdateFunction;
					using AbstarctPeer<T, S, U, AS>::m_listeningThread;
					using AbstarctPeer<T, S, U, AS>::m_canExit;
					using AbstarctPeer<T, S, U, AS>::m_sockerNumber;
					using AbstarctPeer<T, S, U, AS>::ReceiveData;
					using AbstarctPeer<T, S, U, AS>::ipSet;

					VectorTimer* m_vectorTimer;
					bool notServer;

					ObservableEventProducer(int p_noReply) :AbstarctPeer()
					{
						m_dontStore = p_noReply;
						notServer = true;
						m_sockerNumber = GATEKEEPER_LISTENING_PORT;
						//BootUpSequence();
						m_updateThread = std::thread(&P2pPeer::ListenNetwork, this);
					}

					ObservableEventProducer(string p_ipAddress, int p_socketNumber, int p_sequenceNumber, string p_nameAssigned, int p_amountPeers) 
						: AbstarctPeer(p_ipAddress, p_socketNumber, p_nameAssigned)
					{
						notServer = false;
						m_vectorTimer = new FixedVectorTimer(p_sequenceNumber);
						m_updatesList->SetVectorTimer(m_vectorTimer);
						this->m_updatesList = AbstarctPeer<T, S, U, AS>::m_updatesList;
						BootUpSequence();
					}

					ObservableEventProducer(string p_ipAddress, int p_socketNumber, int index, string p_nameAssigned, ObservableEventsList<T, S, AS>* p_updateList, LightManager<Light>* p_lightManager ,int p_amountPeers) 
											: AbstarctPeer(p_ipAddress, 
														   p_socketNumber, 
														   p_nameAssigned, 
														   p_updateList)
					{
						notServer = false;
						m_lightManager = p_lightManager;
						m_vectorTimer = new FixedVectorTimer(index, p_amountPeers);
						m_updatesList->SetVectorTimer(m_vectorTimer);
						BootUpSequence();
					}

					virtual void AddUpdate(RadiancePoint* p_currentUpdate)
					{}

					virtual void UpdateFunction()
					{}


					virtual void BootUpSequence()
					{
						CreateRadiancePoint();
						logicalTimer = 0;
						cout << bootStrapAddress << endl;
						m_updateThread = std::thread(&ObservableEventProducer::SenderThread, this);
						m_eventGenerator = std::thread(&ObservableEventProducer::ProduceEvents, this);
					}

					void ProduceEvents()
					{
						Engine::Networking::ObservableEvents::ICObservableEventsList<T, S, AS>* updatesList = (Engine::Networking::ObservableEvents::ICObservableEventsList<T, S, AS>*)m_updatesList;
						while(true)
						{
							for (int index = 0; index < 10; index++)
							{
								//if multiple of 9
								if(index % 9 == 0)
								{
									LightChangeDescription* lightDescription = new LightChangeDescription();

									lightDescription->m_Lightindex = 5;
									lightDescription->m_SwitchStatus = rand() % 1;

									updatesList->AddInvalidationEvent(lightDescription);
								}
								else
								{
									updatesList->AddInsertEvent(m_radiancePoints);
								}
							}

							this_thread::sleep_for(chrono::milliseconds(10000));
						}
					}

					void CreateRadiancePoint()
					{
						RadiancePoint* temp = nullptr;
						for (int index = 0; index < 100; index++)
						{

							Point3D position(index);
							Point3D normal(index);
							float minRayLength = index;
							RGBColor color(index / 100.f);

							temp = new RadiancePoint(position, normal, minRayLength, color);
							m_radiancePoints.push_back(temp);
						}

					}

					void CloseGateKeeper()
					{
						m_canExit = true;
						gateKeeperAcceptor->close();
					}


					virtual void ListenOnNetwork()
					{}

					void SenderThread()
					{
						//produce events and send them 
						while (true)
						{
							IoSrevice aios;
							Socket socket(aios);
							vector<unsigned char> receiveData;
							vector<unsigned char> replyingData;
							bool firstTimer = true;

							bool foundNewNeighbour = false;
							if (!notServer && firstTimer)
							{
								try
								{
									cout << "opening a new connection" << bootStrapAddress << "and" << GATEKEEPER_LISTENING_PORT << endl;
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
									//m_errorLogger.LogError("issue to communicate with gate keeper");
									cout << "issue when communicating with gate keeper" << endl;
									break;
								}
							}

							Neighbour* details = nullptr;
							while (!foundNewNeighbour && !m_canExit)
							{
								if (m_canExit)
									break;

								details = nullptr;
								cout << "[SENDER] getting random neighbor" << "\n";
								details = (Neighbour*)neighbors->GetRandomNeighbor();

								if (details != nullptr)
								{
									//will be opened form the thread sender
									foundNewNeighbour = OpenConnection(details->m_Ip, const_cast<char*>(to_string(m_sockerNumber).c_str()), socket, aios);
								}

								if (!foundNewNeighbour)
								{
									cout << "could not connect to other ip" << endl;
									this_thread::sleep_for(chrono::milliseconds(1000));
								}

							}

							if (!foundNewNeighbour&&m_canExit)
							{
								break;
							}

							int receivedIndex = 0;

							string stringIp = (string)(details->m_Ip);

							try
							{

								details->IncrementLogicalTime();

								//receive peers
								std::string receivedPeers = ReceiveData(socket);

								//sending peers
								SendData(neighbors->GetRandomNeighbors(details->m_Ip) + "|", socket);
								neighbors->AddNewNeighbours(receivedPeers);

								receiveData.erase(receiveData.begin(), receiveData.end());
								
								//Recevie Peerindex, to be used to add exchange details to the database
								receiveData.erase(receiveData.begin(), receiveData.end());
								ReceiveData(socket, receiveData);
								Network::HelperClass::ByteArrayToInt(receivedIndex, receiveData);

								receiveData.erase(receiveData.begin(), receiveData.end());

								//[AD] Send used only for synchronizatgion purposes 
								Network::HelperClass::AddIntBytes(replyingData, 10);
								SendData(replyingData, socket);

								cout << "the received index  is " << receivedIndex << "\n";

								neighbors->AddNewNeighbours(receivedPeers);
								//tick
								m_vectorTimer->IncrementTimer();

								//receive available events
								cout << "waiting for updates from other peer " << "\n";
								ReceiveData(socket, receiveData);
								cout << "received available uuids are :" << receiveData.size() << endl;

								Network::HelperClass::AddIntBytes(replyingData, 0);

								cout << "[SENDER] sending required uuids" << replyingData.size() << endl;
								SendData(replyingData, socket);

								receiveData.erase(receiveData.begin(), receiveData.end());
								//cout << "[SENDER] Receiving all data" << endl;
								ReceiveData(socket, receiveData);

								
								receiveData.erase(receiveData.begin(), receiveData.end());
								replyingData.erase(replyingData.begin(), replyingData.end());
								
								//send available events
								m_updatesList->SerializeAllUuids(replyingData);
								cout << "[SENDER] serialized all uuids" << endl;
								SendData(replyingData, socket);

								receiveData.erase(receiveData.begin(), receiveData.end());
								replyingData.erase(replyingData.begin(), replyingData.end());
								
								//receive requested OE
								ReceiveData(socket, receiveData);
								m_vectorTimer->Serialize(replyingData);
								m_updatesList->GetNeededOE(receiveData, replyingData, 1, 5);
								cout << "sending requested events" << "\n";
								SendData(replyingData, socket);
								//sent all the data 
							}
							catch (std::exception ex)
							{
								cout << "[SENDER] problem " << ex.what() << endl;
							}


							CloseConnection(socket);
							cout << "[SENDER] Closed connection with " + stringIp + "::" + to_string(LISTENING_PORT) << "\n";
							this_thread::sleep_for(chrono::milliseconds(5000));
						}
					}


					thread& GetEventGeneratorThread()
					{
						return m_eventGenerator;
					}

					thread& GetSenderThread()
					{
						return m_updateThread;
					}

				private:
					boost::asio::ip::tcp::acceptor* gateKeeperAcceptor;
					LightManager<Light>* m_lightManager;
					vector <RadiancePoint*> m_radiancePoints;
					thread m_eventGenerator;
				};
			}
		}
	}
}