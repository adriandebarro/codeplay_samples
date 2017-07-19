#pragma once 

#include <atomic>

#include "./SuperPeer.h"


using Network::SuperPeers::SuperPeer;
namespace Network
{
	namespace Context
	{
		struct ContextStatus
		{
			//signifies that the current super peer is running if true
			bool m_contextOnline = true;
			//if false the rival superpeer details must be established
			SuperPeers::SuperPeer* m_tackledHandler;
			
			std::mutex mtx;

			inline void SetContextOffline()
			{
				/*std::lock_guard<std::mutex> lkg(mtx);*/
				m_contextOnline = false;
			}

			inline bool GetContextStatus()
			{
				//std::lock_guard<std::mutex> lkg(mtx);
				return m_contextOnline;
			}
		};
	}
}