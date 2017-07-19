#pragma once 

#include <hash_map>
#include <vector>
#include <mutex>
#include <atomic>

#include ".\SuperPeer.h"
#include "../UtilityClasses/HelperClass.h"
#include "../UtilityClasses/Common.h"


typedef boost::asio::ip::tcp::socket Socket;
typedef boost::asio::io_service IoSrevice;

namespace Network
{
	namespace SuperPeers
	{
		typedef std::hash_map<int, SuperPeer*>::iterator hash_iter;

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Manager for super peers. </summary>
		///
		/// <remarks>	Adrian, 01/09/2016. </remarks>
		////////////////////////////////////////////////////////////////////////////////////////////////////

		class SuperPeerManager
		{
		private:
			std::mutex mtx;
			int m_connectionTier;
			std::atomic<int>* m_latency;
		public:
			std::hash_map<int, SuperPeer*> m_superPeers;

			SuperPeerManager(int p_connectionTier, std::atomic<int>* p_latency) : m_latency(p_latency)
			{
				m_connectionTier = p_connectionTier;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Adds a super peer. </summary>
			///
			/// <remarks>	Adrian </remarks>
			///
			/// <param name="p_superPeer">	[in,out] If non-null, the super peer. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			bool AddSuperPeer(SuperPeer* p_superPeer)
			{
				if (m_superPeers.find(p_superPeer->m_contextNumber) == m_superPeers.end())
				{
					//add superpeer
					m_superPeers[p_superPeer->m_contextNumber] = p_superPeer;
					return true;
				}
				else
					return false;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Gets super peer for a given context. </summary>
			///
			/// <remarks>	Adrian </remarks>
			///
			/// <param name="p_contextNumber">	The context number. </param>
			///
			/// <returns>	null if it fails, else the super peer. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			SuperPeer* GetSuperPeer(int p_contextNumber)
			{
				return m_superPeers[p_contextNumber];
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Queries if a given check exists. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_contextNumber">	The context number. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			bool CheckExists(int p_contextNumber)
			{
				return m_superPeers.find(p_contextNumber) != m_superPeers.end();
			}

			void GetAllSuperPeers(std::vector<SuperPeer*>& p_superPeerVector)
			{
				for (hash_iter it = m_superPeers.begin(); it != m_superPeers.end(); it++)
				{
					p_superPeerVector.push_back(it->second);
				}
			}

			void GetAllSuperPeersWLock(std::vector<SuperPeer>& p_superPeerVector)
			{
				std::lock_guard<std::mutex> lkGrd(mtx);
				for (hash_iter it = m_superPeers.begin(); it != m_superPeers.end(); it++)
				{
					p_superPeerVector.push_back(SuperPeer(*it->second));
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Match super peers. Compares two lists of super peer details </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_byteVector">	[in,out] The byte vector. </param>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			void MatchSuperPeers(vector<unsigned char>& p_byteVector)
			{
				//compare the received peers and check for any replacements
				std:hash_map<int,SuperPeer*> receivedPeers;
				ParseSuperPeers(p_byteVector, receivedPeers);

				std::lock_guard<std::mutex> lkGrd(mtx);
				for (hash_iter iter = receivedPeers.begin(); iter != receivedPeers.end(); iter++)
				{
					SuperPeer* currentReceivedPeer = iter->second;

					if (m_superPeers.find(currentReceivedPeer->m_contextNumber) != m_superPeers.end())
					{
						bool result = Tackle(m_superPeers[currentReceivedPeer->m_contextNumber], currentReceivedPeer);
						if (!result)
						{
							try
							{
								int superPeerResult = 0;
								SuperPeer* oldPeer = m_superPeers[currentReceivedPeer->m_contextNumber];
								superPeerResult = SendSuperPeerMessage(currentReceivedPeer, SUPER_PEER_HANDSHAKE_MESSAGE, m_connectionTier);
								//if accepted by the first peer 
								if (superPeerResult == 1)
								{
									m_superPeers[currentReceivedPeer->m_contextNumber] = currentReceivedPeer;
									SendSuperPeerMessage(oldPeer, SUPER_PEER_TERMINATE_MESSAGE);
									delete oldPeer;
									std::cout  << "Changing host for context "<< currentReceivedPeer->m_contextNumber << std::endl;
									if (currentReceivedPeer->m_contextNumber == 5)
									{
										std::atomic_store(m_latency, -1);
									}

								}
								else
								{
									receivedPeers.erase(currentReceivedPeer->m_contextNumber);
									delete currentReceivedPeer;
								}
							}
							catch (std::exception ex)
							{
								std::cout << "error occured in the match " << __FILE__<< "-" <<__LINE__ <<endl;
								std::cout << ex.what() << endl;
							}
						}
						else
						{
							//remove the new peer from the received superpeers hash_map
							//receivedPeers.erase(currentReceivedPeer->m_contextNumber);
							delete currentReceivedPeer;
						}
					}
					else
					{
						//add the new superpeer tothe list #
						int superPeerResult = SendSuperPeerMessage(currentReceivedPeer, SUPER_PEER_HANDSHAKE_MESSAGE, m_connectionTier);
						if(superPeerResult == 1)
							m_superPeers[currentReceivedPeer->m_contextNumber] = currentReceivedPeer;
					}
				}
			}

			void SendSuperPeerReceiveMessage(int p_contextNumber)
			{
				m_superPeers.erase(p_contextNumber);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Sends a super peer message. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_currentPeer">	[in,out] If non-null, the current peer. </param>
			/// <param name="message_type"> 	Type of the message. </param>
			/// <param name="p_tier">			The tier. </param>
			///
			/// <returns>	An int. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			int SendSuperPeerMessage(SuperPeer* p_currentPeer, int message_type, int p_tier = 0)
			{
				try
				{
					IoSrevice aios;
					Socket socket(aios);
					int output = 0;
					vector<unsigned char> messageToSend;
					vector<unsigned char> replyMessage;
					Network::HelperClass::AddIntBytes(messageToSend, message_type);

					if (message_type == SUPER_PEER_HANDSHAKE_MESSAGE)
						Network::HelperClass::AddIntBytes(messageToSend, p_tier);

					HelperClass::OpenConnection(p_currentPeer->GetStringIp().c_str(), std::to_string(SUPER_PEER_BASE_PORT + p_currentPeer->m_contextNumber).c_str(), socket, aios);
					HelperClass::SendData(messageToSend, socket);
					HelperClass::ReceiveData(socket, replyMessage);
					HelperClass::CloseConnection(socket);
					
					if (replyMessage.size() > 0)
					{
						HelperClass::ByteArrayToInt(output, replyMessage);
					}
					else
					{
						return -5;
					}

					return output;
				}
				catch (std::exception ex)
				{
					return -5;
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Sends a super peer an ACK message. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_currentPeer">	[in,out] If non-null, the current peer. </param>
			/// <param name="message_type"> 	Type of the message. </param>
			///
			/// <returns>	An int. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			int SendSuperPeerReceiveMessage(SuperPeer* p_currentPeer, int message_type)
			{
				IoSrevice aios;
				Socket socket(aios);

				vector<unsigned char> messageToSend;
				vector<unsigned char> receivedReply;
				Network::HelperClass::AddIntBytes(messageToSend, message_type);

				HelperClass::OpenConnection(p_currentPeer->GetStringIp().c_str(), std::to_string(SUPER_PEER_BASE_PORT + p_currentPeer->m_contextNumber).c_str(), socket, aios);
				HelperClass::SendData(messageToSend, socket);
				HelperClass::ReceiveData(socket, receivedReply);
				HelperClass::CloseConnection(socket);

				int output = 0;
				HelperClass::ByteArrayToInt(output, receivedReply);

				return output;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Tie breaker function for two super peers  </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_localPeer">	[in,out] If non-null, the local peer. </param>
			/// <param name="p_otherPeer">	[in,out] If non-null, the other peer. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			bool Tackle(SuperPeer* p_localPeer, SuperPeer* p_otherPeer)
			{
				if (p_localPeer->m_contextUuid > p_otherPeer->m_contextUuid)
					return false;
				else
					return true;

			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Serialize contexts. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_byteVector">	[in,out] The byte vector. </param>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			void SerializeContexts(std::vector<unsigned char>& p_byteVector)
			{
				std::lock_guard<std::mutex> lkGrd(mtx);
				std::vector<SuperPeer*> allSuperPeers;
				GetAllSuperPeers(allSuperPeers);

				for (int superPeerIndex = 0; superPeerIndex < allSuperPeers.size(); superPeerIndex++)
				{
					allSuperPeers[superPeerIndex]->SerializeSuperPeer(p_byteVector);
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Searches for a particular context. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_contextIndex">	Zero-based index of the context. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			bool FindContext(int p_contextIndex)
			{
				//std::lock_guard<std::mutex> lkGrd(mtx);
				return (m_superPeers.find(p_contextIndex) != m_superPeers.end());
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Searches for the first context w readers lock. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_contextIndex">	Zero-based index of the context. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			bool FindContextWLock(int p_contextIndex)
			{
				std::lock_guard<std::mutex> lkGrd(mtx);
				return (m_superPeers.find(p_contextIndex) != m_superPeers.end());
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Deserialisation of received superpeers </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <param name="p_byteVector">   	[in,out] The byte vector. </param>
			/// <param name="p_tempSuperPeer">	[in,out] [in,out] If non-null, the temporary super peer. </param>
			////////////////////////////////////////////////////////////////////////////////////////////////////

			void ParseSuperPeers(std::vector<unsigned char>& p_byteVector, std::hash_map<int, SuperPeer*>& p_tempSuperPeer)
			{
				//current position in byte array 
				int startingPostion = 0;
				std::lock_guard<std::mutex> lkGrd(mtx);

				int amount = 0;
				HelperClass::ByteArrayToInt(amount, p_byteVector, startingPostion);

				while (startingPostion < p_byteVector.size())
				{
					SuperPeer* currentSP = new SuperPeer(p_byteVector, startingPostion);
					p_tempSuperPeer[currentSP->m_contextNumber] = currentSP;
				}
			}

			void SerializeSuperPeers(std::vector<unsigned char>& p_byteVector)
			{
				std::lock_guard<std::mutex> lkGrd(mtx);
				if (m_superPeers.size() > 0)
				{
					HelperClass::AddIntBytes(p_byteVector, m_superPeers.size());
					for (hash_iter iter = m_superPeers.begin(); iter != m_superPeers.end(); iter++)
					{
						SuperPeer* currentSP = (SuperPeer*)iter->second;
						currentSP->SerializeSuperPeer(p_byteVector);
					}
				}
				else
				{
					HelperClass::AddIntBytes(p_byteVector,0);
				}
			}

			std::mutex& GetMutex()
			{
				return mtx;
			}

			bool RemoveSuperPeer(int p_contextNumber)
			{
				m_superPeers.erase(p_contextNumber);
				return true;
			}

			void DeleteSuperPeer(SuperPeer* p_cuirrentSuperPeer)
			{
				m_superPeers.erase(p_cuirrentSuperPeer->m_contextNumber);
				delete p_cuirrentSuperPeer;
			}

			void DeleteSuperPeerWLock(SuperPeer* p_cuirrentSuperPeer)
			{
				std::lock_guard<std::mutex> lkGrd(mtx);
				m_superPeers.erase(p_cuirrentSuperPeer->m_contextNumber);
				delete p_cuirrentSuperPeer;
			}

		};
	}
}