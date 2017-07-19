#pragma once 

#include <vector>

#include <Peer/Neighbors/Neighbour.h>
#include "./State.h"

using Network::Peer::Neighbour;
namespace Engine
{
	namespace Networking
	{
		namespace Neighbours
		{
#define FIXED_WEIGHTING

			class StatePeer : public Neighbour
			{	
			public:

				State m_peerState;

				StatePeer(char* p_ipAddress) : Neighbour(p_ipAddress)
				{}


				StatePeer(std::string p_ipAddress) : Neighbour(p_ipAddress)
				{}

				StatePeer(const StatePeer& p_other) : Neighbour(p_other.m_Ip)
				{
					m_peerState = p_other.m_peerState;
				}

				void UpdatePeerState(int p_contextNumber, std::vector<unsigned char>& p_byteVector, int& p_startingPosition)
				{
					m_peerState.UpdateState(p_contextNumber, p_byteVector, p_startingPosition);
				}

				void UpdatePeer()
				{}

				float GetPeerWeight()
				{
					return m_peerState.m_peerWeight;
				}
			};
		}
	}
}