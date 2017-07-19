#pragma once

#include "../../CoreClasses/Point/Point3D/Point3D.h"

namespace Engine
{
	namespace Networking
	{
		class Update
		{
		public:

			Update()
			{}

			Update(Point3D p_location, string p_ipAddress, float p_bandwidth)
			{
				m_peerPosition = p_location;
				m_IpAddress = p_ipAddress;
				m_bandwidth = p_bandwidth;
			}

			void SetLocation(Point3D p_location)
			{
				m_peerPosition = p_location;
			}

			Point3D GetLocation()
			{
				return m_peerPosition;
			}

			void SetIpAddress(string p_IpAddress)
			{
				m_IpAddress = p_IpAddress;
			}

			string GetIpAddress()
			{
				return m_IpAddress;
			}

			float GetProbability()
			{
				return m_localityProbability;
			}

			void SetLocalityrobability(float p_probability)
			{
				m_localityProbability = p_probability;
			}

			void SetLocation(float p_X, float p_Y, float p_Z)
			{
				m_peerPosition.x = p_X;
				m_peerPosition.y = p_Y;
				m_peerPosition.z = p_Z;
			}

			void SetBandwidth(float p_bandwidth)
			{
				m_bandwidth = p_bandwidth;
			}

			void SetCpu(float p_cpu)
			{
				m_cpu = p_cpu;
			}

			float GetBandwidth()
			{
				return m_bandwidth;
			}

			float GetCpu()
			{
				return m_cpu;
			}

			int GetPeerIndex()
			{
				return m_peerIndex;
			}
			
			void SetPeerIndex(int p_peerIndex)
			{
				m_peerIndex = p_peerIndex;
			}

		private:
			Point3D m_peerPosition;
			string m_IpAddress;
			float m_localityProbability;
			float m_bandwidth;
			float m_cpu;
			int m_peerIndex;
			//object contating the update to be sent to the oracle
		};
	}//Networking
}//Engine