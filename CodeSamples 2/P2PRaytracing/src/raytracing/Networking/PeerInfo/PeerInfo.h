#pragma once 

#include <string>

#include "../../CoreClasses/Point/Point3D/Point3D.h"
#include "../../CoreClasses/UtilClasses/Common.h"
#include "../PeerInfo/Update.h"

using std::string;
using Engine::Networking::Update;

namespace Engine
{
	namespace Networking
	{
		namespace Peer
		{
			class PeerInfo
			{
			public:
				PeerInfo()
				{}

				PeerInfo(Update* p_receivedUpdate)
				{
					m_location = p_receivedUpdate->GetLocation();
					m_bandwidth = p_receivedUpdate->GetBandwidth();
					m_cpu = p_receivedUpdate->GetCpu();
					m_ipAddress = p_receivedUpdate->GetIpAddress();
					m_weight = p_receivedUpdate->GetProbability();
				}

				Point3D GetLocation()
				{
					return m_location;
				}

				float GetCpu()
				{
					return m_cpu;
				}

				float GetBandwidth()
				{
					return m_bandwidth;
				}

				void SetLocation(Point3D p_location)
				{
					m_location = p_location;
				}

				void SetCpu(float p_cpu)
				{
					m_cpu = p_cpu;
				}

				void SetBandwidth(float p_bandwidth)
				{
					m_bandwidth = p_bandwidth;
				}
				
				void SetIpAddress(string p_ipAddress)
				{
					m_ipAddress = p_ipAddress;
				}

				string GetIpAddress()
				{
					return m_ipAddress;
				}

				void SetWeight(float p_weight)
				{
					m_weight = p_weight;
				}

				float GetWeight()
				{
					return m_weight;
				}


			private:

				Point3D m_location;
				float m_cpu;
				float m_bandwidth;
				std::string m_ipAddress;
				float m_weight;

				~PeerInfo()
				{}
			};
		}//Peer
	}//Network
}//Engine