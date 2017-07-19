#pragma once 

#include <string>
#include <hash_map>
#include <iostream>
#include <atomic>

#include "../../UtilityClasses/Common.h"

using std::string;

namespace Network
{
	namespace Peer
	{
		
		class Neighbour
		{
		public:
			char* m_Ip;
			char* m_port;
			int lastConnection;
			float m_weight;
			int m_neighbourIndex;

			// flag preventing a neighbour from 
			// being removed on cache clearing 
			std::atomic<int> used;

			Neighbour()
			{
				m_Ip = "0.0.0.0";
				m_port = "0000";
				lastConnection = 0;
				std::atomic_init(&used, 0);
			}

			Neighbour(char* p_ipAddress, char* p_port)
			{
				m_Ip = p_ipAddress;
				m_port = p_port;
				lastConnection = 0;
				std::atomic_init(&used, 0);
			}

			Neighbour(string p_ipAddress, string p_port)
			{
				m_Ip = new char[p_ipAddress.size()];
				m_port = new char[p_port.size()];

				std::strcpy(m_Ip, p_ipAddress.c_str());
				std::strcpy(m_port , p_port.c_str());
				
				lastConnection = 0;
				std::atomic_init(&used, 0);
			}

			Neighbour(char* p_ipAddress, char* p_port, int _logicalTime)
			{
				m_Ip = p_ipAddress;
				m_port = p_port;
				lastConnection = _logicalTime;
				std::atomic_init(&used, 0);
			}

			Neighbour(char* p_ipAddress)
			{
				int size = strlen(p_ipAddress);
				m_Ip = new char[size];
				strcpy(m_Ip, p_ipAddress);
				//m_Ip = p_ipAddress;
				const char* port = (std::to_string(LISTENING_PORT)).c_str();
				m_port = new char[sizeof(port)];
				std::strcpy(m_port, port);
				lastConnection = 0;
				std::atomic_init(&used, 0);
			}

			Neighbour(const char* p_ipAddress,const char* p_port, int _logicalTime)
			{
				m_Ip = new char[sizeof(p_ipAddress)];
				m_port = new char[sizeof(p_port)];
				std::strcpy(m_Ip,p_ipAddress);
				std::strcpy(m_port, p_port);
				lastConnection = _logicalTime;
				std::atomic_init(&used, 0);
			}

			Neighbour(string p_neighbourDetails)
			{
				lastConnection = 0;
				FromString(p_neighbourDetails);
				std::atomic_init(&used, 0);
			}

			Neighbour(string p_ipAddress, float p_weight)
			{
				lastConnection = 0;
				string port = std::to_string(LISTENING_PORT);
				m_Ip = new char[p_ipAddress.size()];
				m_port = new char[port.size()];
				m_weight = p_weight;
				std::strcpy(m_Ip, p_ipAddress.c_str());
				std::strcpy(m_port, port.c_str());
				std::atomic_init(&used, 0);
			}

			Neighbour(const Neighbour& p_neighbour)
			{
				strcpy(m_Ip, p_neighbour.m_Ip);
				strcpy(m_port, p_neighbour.m_port);
				lastConnection = p_neighbour.lastConnection;
				std::atomic_init(&used, 0);
			}


			void BeingUsed()
			{
				std::atomic_fetch_add(&used, 1);
			}

			void ReadyFromPeer()
			{
				std::atomic_fetch_sub(&used, 1);
			}

			int NeighbourIsUsed()
			{
				return std::atomic_load(&used);
			}

			void UpdateLogicalTime(int time)
			{
				lastConnection = time;
			}

			void IncrementLogicalTime()
			{
				lastConnection++;
			}

			std::string ToString()
			{
				return std::string(m_Ip) + "--" + std::to_string(lastConnection);
			}

			void FromString(std::string p_data)
			{
				try
				{
					std::string data[3];
					HelperClass::ParseStringSentence(p_data, "-", data);

					string tempPort = std::to_string(LISTENING_PORT);

					m_Ip = new char[data[0].size()];
					m_port = new char[sizeof(tempPort.c_str())];


					std::strcpy(m_Ip, data[0].c_str());
					std::strcpy(m_port, tempPort.c_str());
					lastConnection = stoi(data[1]);
				}
				catch (std::exception ex)
				{
					std::cout << "problem here" << std::endl;
				}
			}

			virtual string GetPeerRepresentation()
			{
				return string(m_Ip) + ":" + std::to_string(LISTENING_PORT);
			}

			static string GetPeerRepresentation(string p_ip)
			{
				return p_ip + ":" + std::to_string(LISTENING_PORT);
			}

			int GetNeighborIndex()
			{
				return m_neighbourIndex;
			}

			void SetNeighborIndex(int p_neighborIndex)
			{
				m_neighbourIndex = p_neighborIndex;
			}

			float GetWeight()
			{
				return m_weight;
			}

			float SetWeight(float p_weight)
			{
				m_weight = p_weight;
			}

			bool operator < (const Neighbour& neighbour)
			{
				return lastConnection < neighbour.lastConnection;
			}

			bool operator >(const Neighbour& neighbour)
			{
				return lastConnection > neighbour.lastConnection;
			}

			bool SortFunction(const Neighbour lhs, const Neighbour rhs)
			{
				return lhs.lastConnection > rhs.lastConnection;
			}

			bool operator == (const Neighbour& rhs) const
			{
				return GetNeigbourRepresentation() == rhs.GetNeigbourRepresentation();
			}

			virtual string GetNeigbourRepresentation() const
			{
				return std::string(m_Ip) + ":" + std::to_string(LISTENING_PORT);
			}

			string PeerRepresentation()
			{
				return std::string(m_Ip) + ":" + std::to_string(LISTENING_PORT);
			}

			Neighbour& operator = (const Neighbour& p_neighbour)
			{
				std::strcpy(m_Ip, p_neighbour.m_Ip);
				std::strcpy(m_port, p_neighbour.m_port);
				lastConnection = p_neighbour.lastConnection;
				return *this;
			}

			Neighbour& operator = (Neighbour* p_neighbour)
			{

				std::strcpy(m_Ip, p_neighbour->m_Ip);
				std::strcpy(m_port, p_neighbour->m_port);
				lastConnection = p_neighbour->lastConnection;
				return *this;
			}

			~Neighbour()
			{
				std::cout << "destructor called" << std::endl;
			}
		};
	}
}


