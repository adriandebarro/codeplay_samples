////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	F:\SchoolDocuments\workingdirectory\Masters-Svn\mschfrdistributed\P2P_Raytracing\src\p2psystem\SuperPeer\SubscribersManager.h
//
// summary:	Declares the subscribers manager class
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once 

#include <string>
#include <mutex>
#include <vector>
#include <math.h>

#include "../UtilityClasses/HelperClass.h"
#include "./Subscriber.h"
#include "../Peer/NeighbourManager/NeighboursManager.h"

using std::string;
using std::vector;
using namespace Network::Peer;
namespace Network
{
	namespace SuperPeers
	{
		namespace Subscribers
		{
			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Manages subscribers. </summary>
			///
			/// <remarks>	Adrian, 01/09/2016. </remarks>
			///
			/// <typeparam name="T">	Generic type parameter. </typeparam>
			////////////////////////////////////////////////////////////////////////////////////////////////////

			template<typename T>
			class SubscriberManager : public Network::Peer::NeighbourManager<Subscriber<T>>
			{
			public:
				using NeighbourManager<Subscriber<T>>::FindNeighbour;
				//using NeighbourManager<Subscriber<T>>::GetAllNeighbours;

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Constructor. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_amountSubscribers">	The amount subscribers. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////

				SubscriberManager(int p_amountSubscribers = 10) : NeighbourManager()
				{
					m_subscriberPopulationLimit = p_amountSubscribers;
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Updates the subscriber state. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_ipAddress">	  	[in,out] If non-null, the IP address. </param>
				/// <param name="p_state">		  	[in,out] The state. </param>
				/// <param name="p_startingPoint">	[in,out] The starting point. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void UpdateSubscriberState(char* p_ipAddress, vector<unsigned char>& p_state, int& p_startingPoint)
				{
					//find subscriber 
					std::lock_guard<std::mutex> lg(subs_manager_mtx);
					Subscriber<T>* subscriberRepresentation = GetNeighbour(string(p_ipAddress));
					
					if (subscriberRepresentation == nullptr)
					{
						//else new state 
						Subscriber<T>* newSubscriber = new Subscriber<T>(p_ipAddress);
						newSubscriber->SetState(p_state, p_startingPoint);
						AddNewElement(newSubscriber);
					}
					else
					{
						//update state
						subscriberRepresentation->SetState(p_state, p_startingPoint);
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Gets string IP. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_ip">	The IP. </param>
				///
				/// <returns>	The string IP. </returns>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				static std::string GetStringIp(unsigned long p_ip)
				{
					struct in_addr addr;
					addr.S_un.S_addr = p_ip;
					char* dotIp = inet_ntoa(addr);

					return std::string(dotIp);
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Gets a neighbour. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_subscruiberIp">	The subscruiber IP. </param>
				///
				/// <returns>	null if it fails, else the neighbour. </returns>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				virtual Subscriber<T>* GetNeighbour(string p_subscruiberIp)
				{
					int index = 0;
					if (FindNeighbour(p_subscruiberIp))
						return m_neighbours[p_subscruiberIp];
					else
						return nullptr;
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Updates the subscriber state. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_ipAddress">	  	The IP address. </param>
				/// <param name="p_state">		  	[in,out] The state. </param>
				/// <param name="p_startingPoint">	[in,out] The starting point. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void UpdateSubscriberState(unsigned long p_ipAddress, vector<unsigned char>& p_state, int& p_startingPoint)
				{
					string ipAddress = GetStringIp(p_ipAddress);
					//find subscriber 
					std::lock_guard<std::mutex> lg(subs_manager_mtx);
					//int typeOfMessage  = HelperClass::
					
					Subscriber<T>* subscriberRepresentation = GetNeighbour(GetStringIp(p_ipAddress));

					if (subscriberRepresentation != nullptr)
					{
						//update state
						subscriberRepresentation->SetState(p_state, p_startingPoint);
					}
					else
					{
						//else new state 
						Subscriber<T>* newSubscriber = new Subscriber<T>(GetStringIp(p_ipAddress));
						newSubscriber->SetState(p_state, p_startingPoint);
						AddNewElement(newSubscriber);
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Updates the subscriber state. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_ipAddress">	  	The IP address. </param>
				/// <param name="p_state">		  	[in,out] The state. </param>
				/// <param name="p_startingPoint">	[in,out] The starting point. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void UpdateSubscriberState(string p_ipAddress, vector<unsigned char>& p_state, int& p_startingPoint)
				{
					string ipAddress = GetStringIp(p_ipAddress);
					//find subscriber 
					std::lock_guard<std::mutex> lg(subs_manager_mtx);
					//int typeOfMessage  = HelperClass::

					Subscriber<T>* subscriberRepresentation = GetNeighbour(p_ipAddress);

					if (subscriberRepresentation != nullptr)
					{
						//update state
						subscriberRepresentation->SetState(p_state, p_startingPoint);
					}
					else
					{
						//else new state 
						Subscriber<T>* newSubscriber = new Subscriber<T>(GetStringIp(p_ipAddress));
						newSubscriber->SetState(p_state, p_startingPoint);
						AddNewElement(newSubscriber);
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Removes the subscriber w lock described by p_subscriber. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_subscriber">	[in,out] If non-null, the subscriber. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void RemoveSubscriberWLock(Subscriber<T>* p_subscriber)
				{
					std::lock_guard<std::mutex> lg(subs_manager_mtx);
					m_neighbours.erase(p_subscriber->GetNeighbourRepresentation());
					delete p_subscriber;
					p_subscriber = nullptr;
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Removes the subscriber described by p_subscriber. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_subscriber">	[in,out] If non-null, the subscriber. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void RemoveSubscriber(Subscriber<T>* p_subscriber)
				{
					m_neighbours.erase(p_subscriber->GetNeighbourRepresentation());
					delete p_subscriber;
					p_subscriber = nullptr;
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Searches for the first neighbour w lock. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_neighbourRepresentation">	The neighbour representation. </param>
				///
				/// <returns>	true if it succeeds, false if it fails. </returns>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				bool FindNeighbourWLock(string p_neighbourRepresentation)
				{
					try
					{
						subs_manager_mtx.lock();
						if (m_neighbours.find(p_neighbourRepresentation) == m_neighbours.end())
						{
							subs_manager_mtx.unlock();
							return false;
						}
						else
						{
							subs_manager_mtx.unlock();
							return true;
						}
					}
					catch (std::exception ex)
					{
						std::cout << ex.what() << std::endl;
					}

				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Searches for the first neighbour. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_neighbourRepresentation">	The neighbour representation. </param>
				///
				/// <returns>	true if it succeeds, false if it fails. </returns>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				virtual bool FindNeighbour(string p_neighbourRepresentation)
				{
					if (m_neighbours.find(p_neighbourRepresentation) == m_neighbours.end())
						return false;
					else
						return true;
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Removes the subscriber described by p_representation. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_representation">	The representation. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void RemoveSubscriber(std::string p_representation)
				{
					bool found = FindNeighbour(p_representation);
					if (found)
					{
						Subscriber<T>* current = m_neighbours[p_representation];
						m_neighbours.erase(p_representation);
						delete current;
						current = nullptr;
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Adds a subscriber. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_ipAddress">	[in,out] If non-null, the IP address. </param>
				///
				/// <returns>	true if it succeeds, false if it fails. </returns>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				bool AddSubscriber(char* p_ipAddress, int p_connectionTier = 1)
				{
					std::lock_guard<std::mutex> lg(subs_manager_mtx);
					if (m_neighbours.size() >= m_subscriberPopulationLimit)
						return false;
					else
					{
						Subscriber<T>* newSubscriber = new Subscriber<T>(p_ipAddress, p_connectionTier);
						AddNewElement(newSubscriber);
						return true;
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Adds a subscriber. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_newSubscriber">	[in,out] If non-null, the new subscriber. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void AddSubscriber(Subscriber<T>* p_newSubscriber)
				{
					bool foundSubscriber = FindNeighbour(p_newSubscriber->GetNeigbourRepresentation());
					if (!foundSubscriber)
						AddNewElement(p_newSubscriber);
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Adds a new element. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_newNeighbor">	[in,out] If non-null, the new neighbor. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void AddNewElement(Subscriber<T>* p_newNeighbor)
				{
					m_neighbours[p_newNeighbor->GetNeighbourRepresentation()] = p_newNeighbor;
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Adds a new elementw lock. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_newNeighbor">	[in,out] If non-null, the new neighbor. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////

				void AddNewElementwLock(Subscriber<T>* p_newNeighbor)
				{
					std::lock_guard<std::mutex> lg(subs_manager_mtx);
					m_neighbours[p_newNeighbor->GetNeighbourRepresentation()] = p_newNeighbor;
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Gets all subscribers. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_allNeighborsVector">	[in,out] [in,out] If non-null, all neighbors vector. </param>
				/// <param name="p_iterNumber">		   	The iterator number. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void GetAllSubscribers(vector<Subscriber<T>>& p_allNeighborsVector, int p_iterNumber)
				{
					int amountOfNeighbours = m_neighbours.size();
					std::string output = "";
					//std::hash_map<string, Subscriber<T>
					for (auto const &iter : m_neighbours)
					{
						Subscriber<T>* current = (Subscriber<T>*)iter.second;
						if(p_iterNumber % current->GetDeviceTier().m_deviceTier == 0)
							p_allNeighborsVector.push_back(T(*current));
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Gets all subscribers w lock. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_allNeighborsVector">	[in,out] [in,out] If non-null, all neighbors vector. </param>
				/// <param name="p_iterNumber">		   	The iterator number. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void GetAllSubscribersWLock(vector<Subscriber<T>*>& p_allNeighborsVector, int p_iterNumber)
				{
					std::lock_guard<std::mutex> lg(subs_manager_mtx);
					int amountOfNeighbours = m_neighbours.size();
					std::string output = "";
					//std::hash_map<string, Subscriber<T>
					for (auto const &iter : m_neighbours)
					{
						Subscriber<T>* current = (Subscriber<T>*)iter.second;
						if (p_iterNumber % current->m_deviceTier.GetTierInt() == 0)
							p_allNeighborsVector.push_back(current);
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Gets all updates. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_allInfo">	[in,out] Information describing all. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void GetAllUpdates(vector<unsigned char>& p_allInfo)
				{
					vector<Subscriber<T>*> allSubscribers;
					GetAllNeighboursNoLock(allSubscribers);

					for (int subscriberIndex = 0; subscriberIndex < allSubscribers.size(); subscriberIndex++)
					{
						Subscriber<T>* currentSubs = allSubscribers[subscriberIndex];
						currentSubs->GetSubscriberState(p_allInfo);
					}
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Gets all updates w lock. </summary>
				///
				/// <remarks>	Adrian, 01/09/2016. </remarks>
				///
				/// <param name="p_allInfo">	[in,out] Information describing all. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void GetAllUpdatesWLock(vector<unsigned char>& p_allInfo)
				{
					std::lock_guard<std::mutex> lg(subs_manager_mtx);
					vector<Subscriber<T>*> allSubscribers;

					for (int subscriberIndex = 0; subscriberIndex < allSubscribers.size(); subscriberIndex++)
					{
						Subscriber<T>* currentSubs = allSubscribers[subscriberIndex];
						currentSubs->GetSubscriberState(p_allInfo);
					}

					for (int subscriberIndex = 0; subscriberIndex < allSubscribers.size(); subscriberIndex++)
					{
						Subscriber<T>* currentSubs = allSubscribers[subscriberIndex];
						currentSubs->GetSubscriberState(p_allInfo);
					}
				}
				/// <summary>	The subs manager mtx. </summary>
				std::mutex subs_manager_mtx;

			private:
				
				/// <summary>	The subscriber population limit. </summary>
				int m_subscriberPopulationLimit;
			};
		}
	}
}