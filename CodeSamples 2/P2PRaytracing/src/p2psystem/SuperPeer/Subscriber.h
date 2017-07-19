#pragma once 

#include <string>
#include <atomic>

#include "../Peer/Neighbors/Neighbour.h"
#include "../SuperPeer/Context/StateRepresentation.h"
#include "./DeviceTier.h"

using Network::SuperPeers::Subcribers::ConnectionTier;
using std::string;
namespace Network
{
	namespace SuperPeers
	{
		namespace Subscribers
		{
			template<typename REP>
			class Subscriber : public Network::Peer::Neighbour
			{
			public:
				using Neighbour::GetNeigbourRepresentation;
					
				Subscriber() : Network::Peer::Neighbour(string("127.0.0.1").c_str())
				{
					std::atomic_store(&m_failCounter, 0);
				}

				Subscriber(char* p_ipAddress) : Network::Peer::Neighbour(p_ipAddress)
				{
					std::atomic_store(&m_failCounter, 0);
				}

				Subscriber(char* p_ipAddress, int p_deviceTeir) : Network::Peer::Neighbour(p_ipAddress), m_deviceTier(p_deviceTeir)
				{
					std::atomic_store(&m_failCounter, 0);
				}

				Subscriber(char* p_ipAddress, char* p_portNumber) : Network::Peer::Neighbour(p_ipAddress, p_portNumber)
				{
					std::atomic_store(&m_failCounter, 0);
				}

				Subscriber(string p_ipAddress) : Network::Peer::Neighbour(p_ipAddress)
				{
					std::atomic_store(&m_failCounter, 0);
				}

				void SetState(vector<unsigned char>& p_byteVector, int& p_startingPoint)
				{
					m_subscriberState.FromSerialize(p_byteVector, p_startingPoint);
				}

				void UpdateState(unsigned char* p_update)
				{
					m_subscriberState = (REP)p_update;
				}

				virtual string GetNeighbourRepresentation()
				{
					return string(m_Ip);
				}

				REP GetState()
				{
					return m_subscriberState;
				}

				void AddState(vector<unsigned char>& p_allUpdates)
				{
					m_subscriberState.Serialize(p_allUpdates);
				}

				void GetSubscriberState(vector<unsigned char>& p_byteVector)
				{
					long address = inet_addr(m_Ip);

					int sizeLong = sizeof(unsigned long);
					unsigned char* tempPtr = (unsigned char*)((char *)&address);
					for (int index = 0; index < sizeLong; index++)
					{
						p_byteVector.push_back(tempPtr[index]);
					}

					m_subscriberState.Serialize(p_byteVector);
				}

				std::atomic<int> m_failCounter;

				ConnectionTier GetDeviceTier()
				{
					return m_deviceTier;
				}

				Network::Context::StateRepresentation<REP> m_subscriberState;

				ConnectionTier m_deviceTier;

			private:
				
			};
		}
	}
}