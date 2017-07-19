#pragma once 

#include <vector>
#include <algorithm>
#include <hash_map>
#include <mutex>
#include <queue>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast/lexical_cast_old.hpp>

#include "../Neighbors/Neighbour.h"
#include "../../UtilityClasses/HelperClass.h"
#include "../../UtilityClasses/Common.h"


using std::vector;
using std::hash_map;
namespace Network
{
	namespace Peer
	{

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Defines an alias representing the tokenizer. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		typedef boost::tokenizer<boost::char_separator<char>> tokenizer;

		
		template<typename T>
		class NeighbourManager
		{
		public:

			int m_cacheSizeLimit = 5;

			NeighbourManager(int p_amountPeersInCache = 5)
			{
				m_cacheSizeLimit = p_amountPeersInCache;
				m_internalDescription = new T("127.0.0.1");
			}


			///-------------------------------------------------------------------------------------------------
			/// <summary>	Constructor. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <param name="p_peerGuid"> 	Unique identifier for the peer. </param>
			/// <param name="p_myAddress">	my address. </param>
			///-------------------------------------------------------------------------------------------------

			NeighbourManager(T* p_internalRepresentation)
			{
				m_internalDescription = p_internalRepresentation;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets random neighbour. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <returns>	The random neighbour. </returns>
			///-------------------------------------------------------------------------------------------------

			virtual T* GetRandomNeighbor()
			{
				std::lock_guard<std::mutex> lg(mtx);
				if (m_neighbours.size() == 0)
				{
					return nullptr;
				}
				else
				{
					int randInteger = rand() % m_neighbours.size();

					int counter = 0;

					for (auto const &iter : m_neighbours)
					{
						if (counter == randInteger)
						{
							T* current = iter.second;
							//((T*)current)->IncrementLogicalTime();
							current->BeingUsed();
							return current;
						}
						else
							++counter;
					}
				}

				return nullptr;
			}

			virtual string GetNeighborRep(std::string p_currentNeighbor)
			{
				std::lock_guard<std::mutex> lg(mtx);
				if (m_neighbours.size() == 0)
				{
					return "";
				}
				else
				{
					string representation = p_currentNeighbor + ":" + std::to_string(LISTENING_PORT);

					bool found = FindNeighbour(representation);
					if (found)
					{
						T* previousNeighbor = m_neighbours[representation];
						return previousNeighbor->ToString();
					}
					else
					{
						return "";
					}
										
				}

				return "";
			}


			virtual void UpdateNeighbours(vector<unsigned char>& p_updates)
			{

			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Removes the neighbour described by neighboursAddress. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <param name="neighboursAddress">	The neighbours address. </param>
			///-------------------------------------------------------------------------------------------------
			//removes the neighbour with the lowest commication count 
			void RemoveNeighbour(std::string neighboursAddress)
			{
				std::lock_guard<std::mutex> lg(mtx);
				int peerIndex = 0;
				bool result = FindNeighbour(neighboursAddress.c_str());

				if (result)
				{
					m_neighbours.erase(neighboursAddress.c_str());
				}
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets a neighbour. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <param name="index">	Zero-based index of the. </param>
			///
			/// <returns>	The neighbour. </returns>
			///-------------------------------------------------------------------------------------------------
			T* GetNeighbour(int index)
			{
				int counter = 0;
				if (index < m_neighbours.size() || index >= 0)
				{
					for (auto const &iter : m_neighbours)
					{
						if (counter == index)
						{
							T* current = iter.second;
							return current;
						}
						else
							++counter;
					}
				}
				return nullptr;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets a neighbour. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <param name="address">	[in,out] The address. </param>
			///
			/// <returns>	The neighbour. </returns>
			///-------------------------------------------------------------------------------------------------
			virtual T* GetNeighbour(string p_nieghborSignitaure)
			{
				int index = 0;
				if (FindNeighbour(p_nieghborSignitaure + ":" + to_string(LISTENING_PORT)))
					return m_neighbours[p_nieghborSignitaure+":"+to_string(LISTENING_PORT)];
				else
					return nullptr;
				
			}
			
			int NumberOfNeighbours()
			{
				return m_neighbours.size();
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Parse neighbours. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <returns>	A std::string. </returns>
			///-------------------------------------------------------------------------------------------------
			std::string ParseNeighbours()
			{
				std::stringstream output;
				for (auto const &iter : m_neighbours)
				{
					Neighbour* current = iter.second;
					output << current->ToString();
					output << ";";
				}
				return output.str();
			}

			void AddNewNeighbours(std::string stringNeighbours)
			{
				std::lock_guard<std::mutex> lg(mtx);
				boost::char_separator<char> sep(";");
				vector<string> parsedTokens;

				HelperClass::ParseStringSentence(stringNeighbours, ";", parsedTokens);

				vector<T*> neighborsList;
				
				for (int index = 0; index < parsedTokens.size(); index++)
				{
					T* temp_neighbour = new T(parsedTokens[index]);
					if ((temp_neighbour->GetNeigbourRepresentation()) != this->m_internalDescription->GetNeigbourRepresentation() && !FindNeighbour(temp_neighbour->GetNeigbourRepresentation()))
						neighborsList.push_back(temp_neighbour);
					else if((temp_neighbour->GetNeigbourRepresentation()) != this->m_internalDescription->GetNeigbourRepresentation())
					{
						T* currentNeighour = GetNeighbour(string(temp_neighbour->m_Ip));
						currentNeighour->lastConnection = std::max(currentNeighour->lastConnection, temp_neighbour->lastConnection);
						delete temp_neighbour;
					}
				}

				for (auto const &iter : m_neighbours)
				{
					neighborsList.push_back(iter.second);
				}

				std::sort(neighborsList.begin(), neighborsList.end(), SortFunction);

				if (neighborsList.size() > m_cacheSizeLimit)
				{
					m_neighbours.erase(m_neighbours.begin(), m_neighbours.end());
					
					for (int neighborIndex = 0; neighborIndex < neighborsList.size(); neighborIndex++)
					{
						if (neighborIndex < m_cacheSizeLimit)
						{
							m_neighbours[neighborsList[neighborIndex]->GetNeigbourRepresentation()] = neighborsList[neighborIndex];
						}
						else
						{
							// Do not delete immediatley 
							T* current = neighborsList[neighborIndex];
							AddToFlushBuffer(current);
						}
					}
				}
				else
				{
					m_neighbours.erase(m_neighbours.begin(), m_neighbours.end());

					for (int neighborIndex = 0; neighborIndex < neighborsList.size(); neighborIndex++)
					{						
						m_neighbours[neighborsList[neighborIndex]->GetNeigbourRepresentation()] = neighborsList[neighborIndex];
					}
				}
			}


			virtual void UpdateNeighbors(vector<unsigned char>& p_updatesReceived)
			{}


			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets the neighbours. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <param name="p_stringNeighbors">	The string neighbors. </param>
			/// <param name="p_neighborsList">  	[in,out] [in,out] If non-null, list of neighbors. </param>
			///-------------------------------------------------------------------------------------------------

			void GetNeighbours(string p_stringNeighbors, vector<T*>& p_neighborsList)
			{
				std::lock_guard<std::mutex> lg(mtx);
				boost::char_separator<char> sep(";");
				std::string output = std::string(p_stringNeighbors);
				tokenizer tokens(output, sep);

				for (tokenizer::iterator iter = tokens.begin(); iter != tokens.end(); iter++)
				{
					p_neighborsList.push_back(ProduceNeighbor(*iter));
				}
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Sets the list to the top most peers  </summary>
			///
			/// <remarks>	Adrian, 05/09/2015. </remarks>
			///
			/// <param name="p_recievedPeerList">	List of recieved peers. </param>
			///-------------------------------------------------------------------------------------------------

			void GetTopNeighbors(string p_recievedPeerList)
			{
				vector<T*> allNeighbors;
				GetNeighbours(p_recievedPeerList, allNeighbors);

				for (auto const &iter : m_neighbours)
				{
					T* current = iter.second;
					allNeighbors.push_back(current);
				}

				//std::sort(allNeighbors.begin(), allNeighbors.end());

				bool output = m_neighbours.empty();
				int counter = allNeighbors.size() - 1;
				for (int index = 0; index < m_cacheSizeLimit; index++)
				{
					T* currentNeighbor = allNeighbors[counter];
					m_neighbours[currentNeighbor->GetNeigbourRepresentation()] = currentNeighbor;
				}
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Produce neighbor. </summary>
			///
			/// <remarks>	Adrian, 05/09/2015. </remarks>
			///
			/// <param name="currentNeighbor">	The current neighbor. </param>
			///
			/// <returns>	null if it fails, else a Neighbour*. </returns>
			///-------------------------------------------------------------------------------------------------
			T* ProduceNeighbor(std::string currentNeighbor)
			{
				std::string myArray[3];
				HelperClass::ParseStringSentence(currentNeighbor, "-", myArray);
				return new T(myArray[1].c_str(), myArray[0].c_str(), (int)stoi(myArray[2].c_str()));
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Adds a neighbor to 'address'. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <param name="peerName">	[in,out] Name of the peer. </param>
			/// <param name="address"> 	[in,out] The address. </param>
			///-------------------------------------------------------------------------------------------------

			/*void AddNeighbor(char* p_ipAddress)
			{
				cout << "adding new neighbor" << p_ipAddress<< ":::" << to_string(LISTENING_PORT) << endl;
				int testInt = 0;
				string  toSearch = string(p_ipAddress) + ":" + to_string(LISTENING_PORT);
				if (!FindNeighbour(toSearch))
				{
					if (m_neighbours.size() >= MAXIMUM_AMOUNT_NEIGHBORS)
					{
						SortCache();
						RemoveNeighbour(toSearch);
					}
					T* newNeighbor = new T(p_ipAddress);
					m_neighbours[toSearch] =  newNeighbor;
				}
			}
*/
			void AddNeighbor(char* p_ipChar)
			{
				string p_ip = (string)p_ipChar;

				std::lock_guard<std::mutex> lg(mtx);
				boost::char_separator<char> sep(";");
				vector<string> parsedTokens;

				//HelperClass::ParseStringSentence(stringNeighbours, ";", parsedTokens);
				string newPeerRep = p_ip + ":" + std::to_string(LISTENING_PORT);
				bool neighborFound = FindNeighbour(newPeerRep);

				vector<T*> neighborsList;
				if (!neighborFound)
				{
					T* newNeighbour = new T(p_ip);
					newNeighbour->IncrementLogicalTime();
					neighborsList.push_back(newNeighbour);
				}
				else
				{
					T* currentNeighbor = GetNeighbour(p_ip);
					currentNeighbor->IncrementLogicalTime();
				}
				
				for (auto const &iter : m_neighbours)
				{
					neighborsList.push_back(iter.second);
				}

				std::sort(neighborsList.begin(), neighborsList.end(), SortFunction);

				if (neighborsList.size() > m_cacheSizeLimit)
				{
					m_neighbours.erase(m_neighbours.begin(), m_neighbours.end());

					for (int neighborIndex = 0; neighborIndex < neighborsList.size(); neighborIndex++)
					{

						if (neighborIndex < m_cacheSizeLimit)
						{
							m_neighbours[neighborsList[neighborIndex]->GetNeigbourRepresentation()] = neighborsList[neighborIndex];
						}
						else
						{
							T* current = neighborsList[neighborIndex];
							neighborsList.erase(neighborsList.begin() + neighborIndex);
							AddToFlushBuffer(current);
						}
					}
				}
				else
				{
					m_neighbours.erase(m_neighbours.begin(), m_neighbours.end());

					for (int neighborIndex = 0; neighborIndex < neighborsList.size(); neighborIndex++)
					{
						m_neighbours[neighborsList[neighborIndex]->GetNeigbourRepresentation()] = neighborsList[neighborIndex];
					}
				}
			}


			///-------------------------------------------------------------------------------------------------
			/// <summary>	Searches for the first neighbour. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <param name="address">	[in,out] The address. </param>
			/// <param name="index">  	[in,out] Zero-based index of the. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			///-------------------------------------------------------------------------------------------------

			virtual bool FindNeighbour(string p_neighbourRepresentation)
			{

				if (m_neighbours.find(p_neighbourRepresentation) == m_neighbours.end())
					return false;
				else
					return true;
			}

			virtual bool FindNeighbourWLock(string p_neighbourRepresentation)
			{
				std::lock_guard<std::mutex> lg(mtx);
				if (m_neighbours.find(p_neighbourRepresentation + ":" + std::to_string(LISTENING_PORT)) == m_neighbours.end())
					return false;
				else
					return true;
			}


			///-------------------------------------------------------------------------------------------------
			/// <summary>	Updates the neighbours cache described by p_recievedPeers. </summary>
			///
			/// <remarks>	Adrian, 10/09/2015. </remarks>
			///
			/// <param name="p_recievedPeers">	The recieved peers. </param>
			///-------------------------------------------------------------------------------------------------

			void UpdateNeighboursCache(string p_recievedPeers)
			{
				GetTopNeighbors(p_recievedPeers);
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets random neighbors. </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///
			/// <returns>	The random neighbors. </returns>
			///-------------------------------------------------------------------------------------------------
			std::string GetRandomNeighbors(string p_currentNeighbor)
			{
				std::lock_guard<std::mutex> lg(mtx);
				int amountOfNeighbours = m_neighbours.size();
				std::string output = "";

				for (auto const &iter: m_neighbours)
				{
					T* current = iter.second;

					if(std::strcmp(p_currentNeighbor.c_str(), current->m_Ip) != 0)
						output = output + current->ToString() + ";";
				}
				
				return output;
			}

			void FlushQueue()
			{
				std::lock_guard<std::mutex> lg(m_queueMtx);
				bool checkAnother = true;
				while (checkAnother && m_flushQueue.size() > 0)
				{
					T* currentNeighbour = m_flushQueue.front();

					if (currentNeighbour->NeighbourIsUsed() <= 0)
					{
						m_flushQueue.pop();
						delete currentNeighbour;
						currentNeighbour = nullptr;
					}
					else
					{
						checkAnother = false;
					}
				}
			}

			virtual void GetAllNeighbours(vector<T*>& p_allNeighborsVector)
			{
				std::lock_guard<std::mutex> lg(mtx);
				int amountOfNeighbours = m_neighbours.size();
				std::string output = "";

				for (auto const &iter : m_neighbours)
				{
					T* current = iter.second;
					p_allNeighborsVector.push_back(current);
				}
			}

			virtual void GetAllNeighboursNoLock(vector<T*>& p_allNeighborsVector)
			{
				int amountOfNeighbours = m_neighbours.size();
				std::string output = "";

				for (auto const &iter : m_neighbours)
				{
					T* current = iter.second;
					p_allNeighborsVector.push_back(current);
				}
			}

			void AddToFlushBuffer(T* p_currentNeighbour)
			{
				std::lock_guard<std::mutex> lg(m_queueMtx);
				m_flushQueue.push(p_currentNeighbour);
			}


			void AddNeighbor(T* p_currentNeighbor)
			{
				std::lock_guard<std::mutex> lg(mtx);
				m_neighbours[p_currentNeighbor->GetNeigbourRepresentation()] = p_currentNeighbor;
	
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Sort function descending order on last connection </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <param name="p_lhs">	[in,out] If non-null, the left hand side. </param>
			/// <param name="p_rhs">	[in,out] If non-null, the right hand side. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			///-------------------------------------------------------------------------------------------------

			static bool SortFunction(const T* p_lhs, const T* p_rhs) 
			{
				return p_lhs->lastConnection > p_rhs->lastConnection;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Clean hash map. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///-------------------------------------------------------------------------------------------------

			void CleanHashMap()
			{
				auto const &iter = m_neighbours.begin();
				std::next(iter, m_cacheSizeLimit);
				m_neighbours.erase(iter, m_neighbours.end());
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets amount peers. </summary>
			///
			/// <remarks>	Adrian, 09/09/2015. </remarks>
			///
			/// <returns>	The amount peers. </returns>
			///-------------------------------------------------------------------------------------------------

			int GetAmountPeers()
			{
				std::lock_guard<std::mutex> lg(mtx);
				return this->m_neighbours.size();
			}

			void SetInternalIp(string ipAddress)
			{
				/*m_internalDescription->m_Ip = new char[ipAddress.size()];
				std::strcpy(m_internalDescription->m_Ip, ipAddress.c_str());*/
			}

			void UpdateNeighborTime(string p_IpAddress)
			{
				string idString = p_IpAddress + ":" + to_string(LISTENING_PORT);

				T* neighborToUpdate = GetNeighbour(idString);

				if (neighborToUpdate != nullptr)
					neighborToUpdate->IncrementLogicalTime();
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Destructor. </summary>
			///
			/// <remarks>	Adrian, 09/09/2015. </remarks>
			///-------------------------------------------------------------------------------------------------

			~NeighbourManager()
			{
				/*Engine::Common::EmptyPointerVector(m_neighbours);*/
			}

			string GetLocalIp()
			{
				return ((T*)m_internalDescription)->m_Ip;
			}

			void SetCacheSize(int p_cacheSize)
			{
				m_cacheSizeLimit = p_cacheSize;
			}

			int GetCacheSize()
			{
				return m_cacheSizeLimit;
			}

			hash_map<string, T*> m_neighbours;
			T* m_internalDescription;
		private:
			std::mutex mtx;
			std::mutex m_queueMtx;
			std::queue<T*> m_flushQueue;
		};
	}
}