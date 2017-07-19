#pragma once

#include <vector>
#include <iostream>
#include <random>
#include <iterator>
#include <ctime>
#include <type_traits>
#include <cassert>
#include <string.h>
#include <mutex>

#include <Peer/NeighbourManager/NeighboursManager.h>
#include <Peer/Neighbors/Neighbour.h>

#include "../PeerInfo/PeerManager.h"
#include "../NetworkingUtil/CommunicationUtil.h"
#include "../PeerInfo/Update.h"
#include "./StatePeer.h"

#include "../Contexts/ContextDataParser.h"
#include "../Contexts/ContextStrategy.h"

using Network::Peer::NeighbourManager;
using Network::Peer::Neighbour;
using Engine::Networking::Peer::PeerManager;
using Engine::Networking::Contexts::NetworkPropertiesParser;

using Engine::Networking::Peer::PeerInfo;
using std::vector;

using namespace Engine::Networking::Contexts;

using Engine::Networking::Update;
namespace Engine
{
	namespace Networking
	{
		namespace Neighbors
		{
			template<typename T>
			class PdfNeighborManager : public NeighbourManager<T>
			{
			public:
				std::mutex m_mtx;
				using NeighbourManager<T>::m_neighbours;

				//link to parent class constructor
				PdfNeighborManager()
				{
					SetCacheSize(CACHE_SIZE_LIMIT);
				}

				PdfNeighborManager(NetworkPropertiesParser* p_networkParams, int p_cacheSizeLimit) : NeighbourManager(p_cacheSizeLimit)
				{
					m_networkParams = p_networkParams;
					SetCacheSize(p_cacheSizeLimit);
				}
				
				using NeighbourManager<T>::UpdateNeighbors;
				virtual void UpdateNeighbors(vector<unsigned char>& p_updatesReceived, int& p_contextIndex)
				{
					int currentIndex = 0;
					int contextNumber = Communication::Util::UtilClass::GetNum<int>(p_updatesReceived, currentIndex);
					
					while (currentIndex < p_updatesReceived.size())
					{
						unsigned long currentIp = Communication::Util::UtilClass::GetLong(p_updatesReceived, currentIndex);
						string stringifiedIp = Communication::Util::UtilClass::LongToIP(currentIp);
						string result = Neighbour::GetPeerRepresentation(stringifiedIp);

						std::lock_guard<std::mutex> lkg(m_mtx);
						if (m_neighbours.find(result) != m_neighbours.end())
						{
							//update current Peer
							T* currentPeer = m_neighbours[result];
							((StatePeer*)currentPeer)->m_peerState.UpdateState(contextNumber, p_updatesReceived, currentIndex);
						}
						else
						{
							//skip
							currentIndex += GetSizeOfUpdate(contextNumber);
						}
					}
				}

				
				void UpdatePdfDistribution()
				{
					std::lock_guard<std::mutex> lkg(m_mtx);
					int index = 1;

					m_pdfDistribtion.erase(m_pdfDistribtion.begin(), m_pdfDistribtion.end());
					m_intervals.erase(m_intervals.begin(), m_intervals.end());

					for (std::hash_map<string, T*>::iterator iterator = m_neighbours.begin(); iterator != m_neighbours.end(); iterator++)
					{	 
						iterator->second->m_peerState.ComputeWeighting(m_networkParams->m_localityWeighting, m_networkParams->m_viewpointWeighting, m_networkParams->m_boundsWieghting, m_networkParams->m_latencyWeighting, m_networkParams->m_framerateWeigthing);
						m_pdfDistribtion.push_back(iterator->second->GetPeerWeight());
						m_intervals.push_back(index);
						index++;
					}

					m_intervals.push_back(index);

					//_ASSERT(m_pdfDistribtion.size() > 0);

					if (m_pdfDistribtion.size() > 0)
					{
						dist = std::piecewise_constant_distribution<>(m_intervals.begin(),
							m_intervals.end(),
							m_pdfDistribtion.begin());

						gen = std::mt19937_64(std::time(0));
					}
				}



				using NeighbourManager<T>::GetRandomNeighbor;
				virtual T* GetRandomNeighbor()
				{
					if (m_pdfDistribtion.size() > 0)
					{
						unsigned index = static_cast<unsigned>(dist(gen));
						std::hash_map<string, T*>::iterator iter = m_neighbours.begin();
						for (int temp = 0; temp < index - 1; temp++)
						{
							iter++;
						}

						return (T*)iter->second;
					} 
					else 
					{
						if (m_neighbours.size() > 0)
						{
							int randomIndex = rand() % m_neighbours.size();
							return (T*)GetNeighbour(randomIndex);
						}
						else
							return nullptr;
					}
					return nullptr;
				}

				string GetPeerRepresentation(string p_ipAddress)
				{
					return p_ipAddress + ":" + "20012";
				}

				void PrintNeighbourPDFDistribution()
				{
					for (int index = 0; index < m_neighbours.size(); index++)
					{
						std::hash_map<string, T*>::iterator it = m_neighbours.begin();
						
						T* currentNeighbour = nullptr;
						std::advance(it, index);
						currentNeighbour = it->second;
						std::cout << currentNeighbour->m_Ip << " " << m_pdfDistribtion[index] << std::endl;
					}
				}

			private:
				vector<float> m_pdfDistribtion;
				vector<int> m_intervals;
				std::piecewise_constant_distribution<> dist;
				std::mt19937_64 gen;
				const int CACHE_SIZE_LIMIT = 100;
				NetworkPropertiesParser* m_networkParams;
			};
		}
	}
}