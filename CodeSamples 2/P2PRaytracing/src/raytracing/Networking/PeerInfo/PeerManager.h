#pragma once

#include <string>
#include <hash_map>
#include <fstream>

#include "PeerInfo.h"
#include "../PeerInfo/Update.h"

using std::string;
using std::hash_map;
using std::fstream;
using Engine::Networking::Update;

namespace Engine
{
	namespace Networking
	{
		namespace Peer
		{
			class PeerManager
			{
			private:
				

			public:
				void AddPeer(PeerInfo* p_currentPeer)
				{
					if (m_peerDetails.find(p_currentPeer->GetIpAddress()) == m_peerDetails.end())
					{
						m_peerDetails[p_currentPeer->GetIpAddress()] = p_currentPeer;
					}
				}

				void AddNewPeer(Update* p_currentUpdate)
				{
					if (m_peerDetails.find(p_currentUpdate->GetIpAddress()) == m_peerDetails.end())
					{
						m_peerDetails[p_currentUpdate->GetIpAddress()] = new PeerInfo(p_currentUpdate);
					}
				}

				bool FindPeer(string p_IpAddress)
				{
					return (m_peerDetails.find(p_IpAddress) != m_peerDetails.end());
				}

				void UpdatePeer(Update* p_peerInfo)
				{
					PeerInfo* currentPeer = m_peerDetails[p_peerInfo->GetIpAddress()];
					currentPeer->SetBandwidth(p_peerInfo->GetBandwidth());
					currentPeer->SetCpu(p_peerInfo->GetCpu());
					currentPeer->SetLocation(p_peerInfo->GetLocation());
				}

				void UpdatePeer(PeerInfo* p_currentPeer, Update* p_peerInfo)
				{
					p_currentPeer->SetBandwidth(p_peerInfo->GetBandwidth());
					p_currentPeer->SetCpu(p_peerInfo->GetCpu());
					p_currentPeer->SetLocation(p_peerInfo->GetLocation());
					p_currentPeer->SetWeight(p_peerInfo->GetProbability());
				}


				bool FindUpdatePeer(Update* p_peerInfo)
				{
					hash_map<string, PeerInfo*>::iterator position = m_peerDetails.find(p_peerInfo->GetIpAddress());

					if (position != m_peerDetails.end())
					{
						UpdatePeer((*position).second, p_peerInfo);
						return true;
					}
					else
					{
						return false;
					}
				}

				hash_map<string, PeerInfo*> m_peerDetails;

				
			};
		}
	}
}