#pragma once 

#include <atomic>
#include <string>
#include <vector>
#include <IPExport.h>

#include <boost/bind.hpp>

#include <SuperPeer/SuperPeerManager.h>
#include <SuperPeer/SuperPeer.h>
#include <UtilityClasses/HelperClass.h>
#include <UtilityClasses/Common.h>
#include <Peer/Neighbors/Neighbour.h>

#include "../ConreteNeighborManagers/PdfNeighborManager.h"
#include "../ConreteNeighborManagers/StatePeer.h"
#include "../Contexts/ContextStrategy.h"
#include "../../CoreClasses/Camera/Camera.h"
#include "../ConreteNeighborManagers/WeightingMetric.h"
#include "../../CoreClasses/BoundingShapes/BoundingBoxAnalyzer.h"
#include "../NetworkingUtil/TcpServerAsync.h"

#include "./ContextDataParser.h"

using Engine::BoundingShapes::BoundingBoxAnalyzer;
using Network::SuperPeers::SuperPeer;
using Network::SuperPeers::SuperPeerManager;
using Engine::Camera;
using Engine::Networking::Contexts::tcp_connection_context;
using Engine::Networking::Contexts::NetworkPropertiesParser;
namespace Engine
{
	namespace Networking
	{
		namespace Contexts
		{
			class ContextUpdater
			{
			public:

				ContextUpdater(SuperPeerManager* p_superPeerManager, Neighbors::PdfNeighborManager<Engine::Networking::Neighbours::StatePeer>* p_peers,
					int p_periodTimeUpdate, clock_t* p_startingTime, std::string p_boundsBoxesFilePath, BoundingBox* p_mapLimits, int p_deviceTier, Camera* p_camera,
					std::atomic<int> *p_latency, NetworkPropertiesParser* p_networkProperties)
				{
					m_superPeerManager = p_superPeerManager;
					m_peers = p_peers;
					m_periodTimeUpdate = p_periodTimeUpdate;
					m_viewPointDistance = p_mapLimits->GetLongestLengthAxis() / 10;
					m_boundingBoxAnalyzer = new BoundingBoxAnalyzer(p_boundsBoxesFilePath, p_mapLimits);
					m_camera = p_camera;
					m_metricManager = new Engine::Networking::Neighbors::Metrics::WeightingMetrics(p_peers, m_boundingBoxAnalyzer, m_camera, m_latency);
					m_deviceTier = p_deviceTier;
					m_latency = p_latency;
					m_contextParams = p_networkProperties;
					m_startingTime = p_startingTime;
					InitiateThreads();
				}

				ContextUpdater()
				{
					m_receivingThread = thread(boost::bind(&ContextUpdater::ContextsUpdateReceiver, this));
				}

				void InitiateThreads()
				{
					m_receivingThread = thread(boost::bind(&ContextUpdater::ContextsUpdateReceiver, this));
					m_updateSenderThread = thread(boost::bind(&ContextUpdater::UpdateSuperPeer, this));
				}

				//periodically send updates to superpeers 
				void UpdateSuperPeer()
				{
					this_thread::sleep_for(std::chrono::seconds(10));

					while (m_continueUpdating)
					{

						/*m_superPeerManager->GetMutex().lock();*/
						std::vector<SuperPeer> allSuperPeers;
						m_superPeerManager->GetAllSuperPeersWLock(allSuperPeers);

						for (int index = 0; index < allSuperPeers.size(); index++)
						{
							//cout << "updating context " << index << endl;
							bool result = UpdateContext(&allSuperPeers[index]);

							//timeout or false
							if (!result)
							{
								//timeout or superpeer fialeure occures delete superpeer
								// //if superpeer local it will die automatically
								m_superPeerManager->DeleteSuperPeerWLock(&allSuperPeers[index]);
							}
						}
						/*std::lock_guard<std::mutex> lkGrd(mtx);*/
						this_thread::sleep_for(std::chrono::seconds(m_periodTimeUpdate));
					}
				}

				//add connection timeout 
				bool UpdateContext(SuperPeer* p_contextSuperpeer)
				{
					CONTEXT_ENUM currentConext = (CONTEXT_ENUM)p_contextSuperpeer->m_contextNumber;

					switch (currentConext)
					{
						case CONTEXT_ENUM::locality:
						{
							return LocalitySendUpdate(p_contextSuperpeer);
						}

						case CONTEXT_ENUM::bounds:
						{
							return BoundsSendUpdate(p_contextSuperpeer);
						}

						case CONTEXT_ENUM::frameRate:
						{
							return FrameRateSendUpdate(p_contextSuperpeer);
						}
							

						case CONTEXT_ENUM::viewpoint:
						{
							return ViewPointSendUpdate(p_contextSuperpeer);
						}

						case CONTEXT_ENUM::latency:
						{
							return LatencySendUpdate(p_contextSuperpeer);
						}

						default:
							break;
					}
				}

				float GetLatency(SuperPeer* p_latencySuperPeer)
				{
					int currentLatency = std::atomic_load(m_latency);

					if (currentLatency < 0)
					{
						PICMP_ECHO_REPLY status;
						int* latency = new int(-1);
						
						int ttl = Network::HelperClass::PingUtil(p_latencySuperPeer->GetStringIp(), status);

						std::atomic_compare_exchange_strong(m_latency, latency, ttl);
						return ttl;
					}
					else
					{
						return std::atomic_load(m_latency);
					}
					return 0.0f;
				}

				bool LatencySendUpdate(SuperPeer* p_latencySuperPeer)
				{
					vector<unsigned char> updateToSend;
					Network::HelperClass::AddIntBytes(updateToSend, SUPER_PEER_UPDATE_MESSAGE);
					Network::HelperClass::AddIntBytes(updateToSend, GetLatency(p_latencySuperPeer));
					int status = SendSuperPeerUpdate(p_latencySuperPeer, updateToSend);
					return status == 1;
				}

				float CalculateFrameRate()
				{
					clock_t currentTime = clock();
					int amountTimePassed = currentTime - *m_startingTime;
					float frameRate = m_camera->m_totalCreatedFrames / amountTimePassed;
					return frameRate;
				}

				bool FrameRateSendUpdate(SuperPeer* p_framerateSuperPeer)
				{
					vector<unsigned char> updateToSend;
					Network::HelperClass::AddIntBytes(updateToSend, SUPER_PEER_UPDATE_MESSAGE);
					Network::HelperClass::AddFloatBytes(updateToSend, CalculateFrameRate());
					int status = SendSuperPeerUpdate(p_framerateSuperPeer, updateToSend);
					return status == 1;
				}

				bool BoundsSendUpdate(SuperPeer* p_boundsSuperPeer)
				{
					vector<unsigned char> updateToSend;
					Network::HelperClass::AddIntBytes(updateToSend, SUPER_PEER_UPDATE_MESSAGE);
					Network::HelperClass::AddIntBytes(updateToSend, m_boundingBoxAnalyzer->CheckContainingBoundingBox(m_camera->GetCameraPosition()));
					int status = SendSuperPeerUpdate(p_boundsSuperPeer, updateToSend);
					return status == 1;
				}


				bool ViewPointSendUpdate(SuperPeer* p_viewPointSuperPeer)
				{
					vector<unsigned char> updateToSend;
					Network::HelperClass::AddIntBytes(updateToSend, SUPER_PEER_UPDATE_MESSAGE);
					Point3D currentDirection = m_camera->GetCameraDirection();
					currentDirection.SerializePoint(updateToSend);
					int status = SendSuperPeerUpdate(p_viewPointSuperPeer, updateToSend);
					return status == 1;
				}

				bool LocalitySendUpdate(SuperPeer* p_localitySuperPeer)
				{
					vector<unsigned char> updateToSend;
					Network::HelperClass::AddIntBytes(updateToSend, SUPER_PEER_UPDATE_MESSAGE);
					m_camera->GetCameraPosition().SerializePoint(updateToSend);
					int status = SendSuperPeerUpdate(p_localitySuperPeer, updateToSend);
					return status == 1;
				}

				int SendSuperPeerUpdate(SuperPeer* p_currentPeer, vector<unsigned char> p_update)
				{
					IoSrevice aios;
					Socket socket(aios);

					vector<unsigned char> receivedReply;
					int output = 0;
					try
					{

						bool connOpened = Network::HelperClass::OpenConnection(p_currentPeer->GetStringIp().c_str(), to_string(SUPER_PEER_BASE_PORT + p_currentPeer->m_contextNumber).c_str(), socket, aios);
						if (connOpened)
						{
							try
							{
								Network::HelperClass::SendData(p_update, socket);
								//std::cout << "waiting for approval from context " << std::endl;
								Network::HelperClass::ReceiveData(socket, receivedReply);
								Network::HelperClass::CloseConnection(socket);

								if (receivedReply.size() > 0)
									Network::HelperClass::ByteArrayToInt(output, receivedReply);
								else
									output = 1;
							}
							catch (std::exception ex)
							{
								std::cout << "error occured " << std::endl;
								output = 1;
							}
						}
						else
						{
							output = -5;
						}
					}
					catch (std::exception ex)
					{
						output = -5;
					}
					return output;
				}

				//|contextnumber|     ip-data     |....
				//|  4bytes     |4byt-sizeof(data)|....
				//receive updates form superpeer
				void ContextsUpdateReceiver()
				{
					try
					{
						boost::asio::io_service asio;
						tcp_server_peer tcp_server(asio, m_peers, m_metricManager, &m_continueUpdating);
						asio.run();
					}
					catch (std::exception ex)
					{
						std::cout << "" << std::endl;
					}
				}

				void UpdateContextWeighting(int p_contextType)
				{
					switch ((CONTEXT_ENUM)p_contextType)
					{
					case CONTEXT_ENUM::locality:
						m_metricManager->UpdateWeighting(m_camera->GetCameraPosition(), p_contextType);
						break;

					case CONTEXT_ENUM::viewpoint:
						m_metricManager->UpdateWeighting(m_camera->GetCameraPosition(), m_camera->GetCameraDirection(), m_camera->m_fov, m_viewPointDistance ,p_contextType);
						break;

					case CONTEXT_ENUM::bandwidth:
						m_metricManager->UpdateWeighting(0, p_contextType);
						break;

					case CONTEXT_ENUM::bounds:
						m_metricManager->UpdateWeighting(m_boundingBoxAnalyzer->CheckContainingBoundingBox(m_camera->GetCameraPosition()), p_contextType);
						break;

					case CONTEXT_ENUM::frameRate:
						m_metricManager->UpdateWeighting(0, p_contextType);
						break;

					case CONTEXT_ENUM::latency:
						m_metricManager->UpdateWeighting(p_contextType);
						break;

					default:
						break;
					}
				}

				void TurnOffContextUpdates()
				{
					m_continueUpdating = false;
					m_receivingThread.detach();
				}

				
				BoundingBoxAnalyzer* GetBoundingBoxAnalyser()
				{
					return m_boundingBoxAnalyzer;
				}

				
				bool m_continueUpdating = true;
			private:
				SuperPeerManager* m_superPeerManager;
				Neighbors::PdfNeighborManager<Engine::Networking::Neighbours::StatePeer>* m_peers;
				
				
				int m_periodTimeUpdate;

				std::thread m_receivingThread;
				std::thread m_updateSenderThread;
				
				Engine::Networking::Neighbors::Metrics::WeightingMetrics* m_metricManager;
				
				clock_t* m_startingTime;
				Camera* m_camera;
				float m_viewPointDistance;
				BoundingBoxAnalyzer* m_boundingBoxAnalyzer;

				std::atomic<int>* m_latency;
				float m_bandwidth = -1;
				tcp_server_peer* m_tcp_server;

				std::mutex m_updatePeerInfo;
				int m_deviceTier;
				NetworkPropertiesParser* m_contextParams;
			};
		}
	}
}