#pragma once 
#include <boost/asio.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <string>
#include <vector>

#include "../VectorTimer/FixedVectorTimer.h"

using boost::uuids::uuid;
namespace Network
{
	namespace SuperPeers
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	A super peer. </summary>
		///
		/// <remarks>	Adrian, 19/10/2016. </remarks>
		////////////////////////////////////////////////////////////////////////////////////////////////////
		struct SuperPeer
		{
			uuid m_contextUuid;
			unsigned long m_ipAddress;
			int m_contextNumber;
			bool m_storedLocally = false;
			FixedVectorTimer* m_timeStamp;

			SuperPeer()
			{}

			SuperPeer(unsigned long p_peerIp, int p_contextNumber, FixedVectorTimer* p_timeStamp) : m_ipAddress(p_peerIp), m_contextNumber(p_contextNumber), m_storedLocally(true)
			{
				m_contextUuid = boost::uuids::random_generator()();
				m_timeStamp = new FixedVectorTimer(*p_timeStamp);
			}

			SuperPeer(unsigned long p_peerIp, int p_contextNumber, uuid p_uuid, FixedVectorTimer* p_timeStamp) 
				: m_ipAddress(p_peerIp), 
				  m_contextNumber(p_contextNumber), 
				  m_contextUuid(m_contextUuid), 
				  m_storedLocally(false)
			{
				m_timeStamp = new FixedVectorTimer(*p_timeStamp);
			}

			SuperPeer(const SuperPeer& p_superPeer)
			{
				m_contextUuid = p_superPeer.m_contextUuid;
				m_ipAddress = p_superPeer.m_ipAddress;
				m_contextNumber = p_superPeer.m_contextNumber;
				m_storedLocally = p_superPeer.m_storedLocally;
				m_timeStamp = new FixedVectorTimer(*(p_superPeer.m_timeStamp));
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Cosntructor for a superpeer that has been shared  </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_array">		  	[in,out] The array. </param>
			/// <param name="p_startingPoint">	[in,out] The starting point. </param>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			SuperPeer(std::vector<unsigned char>& p_array, int& p_startingPoint)
			{
				FromSerialize(p_array, p_startingPoint);
			}

			SuperPeer(std::string p_peerIp, int p_contextNumber, bool p_storedLocally)
			{
				IpFromString(p_peerIp);
				m_contextNumber - p_contextNumber;
				m_storedLocally = p_storedLocally;
			}

			void IpFromString(std::string p_ip)
			{
				m_ipAddress = inet_addr(p_ip.c_str());
			}

			std::string GetStringIp()
			{
				struct in_addr addr;
				addr.S_un.S_addr = m_ipAddress;
				char* dotIp = inet_ntoa(addr);
				
				return std::string(dotIp);
			}

			unsigned long GetLongIp()
			{
				return m_ipAddress;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Change Superpeer to byte array </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_byeVector">	[in,out] The bye vector. </param>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			void SerializeSuperPeer(std::vector<unsigned char>& p_byeVector)
			{
				HelperClass::GetByteArrayFromLong(p_byeVector, m_ipAddress);
				HelperClass::UuidToByteArray(p_byeVector, m_contextUuid);
				HelperClass::AddIntBytes(p_byeVector, m_contextNumber);
				m_timeStamp->Serialize(p_byeVector);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Deserialize a superpeer byte array </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_array">		  	[in,out] The array. </param>
			/// <param name="p_startingPoint">	[in,out] The starting point. </param>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			void FromSerialize(std::vector<unsigned char>& p_array, int& p_startingPoint)
			{
				m_ipAddress = HelperClass::GetLongFromByte(p_array, p_startingPoint);
				m_contextUuid = HelperClass::GetUuid(p_array, p_startingPoint);
				int contextNumber = 0;
				HelperClass::ByteArrayToInt(m_contextNumber, p_array, p_startingPoint);
				m_timeStamp = new FixedVectorTimer();
				m_timeStamp->FromByteArray(p_array, p_startingPoint);
			}

			string GetStringUuid()
			{
				std::stringstream temp;
				temp << to_string(m_contextUuid);
				return temp.str();
			}
		};
	}
}