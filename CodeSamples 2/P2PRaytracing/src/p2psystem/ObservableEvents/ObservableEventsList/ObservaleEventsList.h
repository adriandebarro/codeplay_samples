#pragma once 

#include <string>
#include <sstream>

#include "../ObservableEvent.h"
#include "../../VectorTimer/VectorTimer.h"
#include "../../VectorTimer/FixedVectorTimer.h"
#include "../../UtilityClasses/Common.h"


using std::stringstream;
namespace Network
{
	namespace ObservableEvents
	{
		///-------------------------------------------------------------------------------------------------
		/// <summary>
		/// 	Struct that dectates the type of invalidations to be carried out
		/// 	--Invalidation events still not operational	
		///  </summary>
		///
		/// <remarks>	Adrian, 19/07/2017. </remarks>
		///-------------------------------------------------------------------------------------------------

		struct InvalidationDetails
		{
			int id;
			int type;
			
			InvalidationDetails(int p_type)
			{
				type = p_type;
				id = 0;
			}

			InvalidationDetails(int p_type, int p_index)
			{
				type = p_type;
				id = p_index;
			}
		};

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	List of observable events. </summary>
		///
		/// <remarks>	Adrian, 07/07/2017. </remarks>
		///
		/// <typeparam name="T"> 	Generic type parameter. </typeparam>
		/// <typeparam name="S"> 	Type of the s. </typeparam>
		/// <typeparam name="AS">	Type of as. </typeparam>
		////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename T, typename S, typename AS>
		class ObservableEventsList
		{
			public:
				virtual void MergeEvents(vector<unsigned char>& p_allData, int p_skip = 0) = 0;
				virtual vector<ObservableEvent<T, S,AS>*> DataToMerge(vector<unsigned char>& p_allData, int p_amount) = 0;
				virtual void FindMissingOE(vector<unsigned char>& p_array, vector<unsigned char>& p_outputArray) = 0;
				virtual void PushQueuedEvents() = 0;
				virtual void AddInvalidationEvents(vector<InvalidationDetails*> &p_allNewInvlaidations, int p_invalidationType, int p_boudningBox) = 0;

				virtual void ExecuteInvalidation()
				{}

				virtual inline void SetInvalidationToFalse()
				{}
				///-------------------------------------------------------------------------------------------------
				/// <summary>	Constructor. </summary>
				///
				/// <remarks>	Adrian, 21/09/2015. </remarks>
				///
				/// <param name="p_peerIndex">	Zero-based index of the peer. </param>
				///-------------------------------------------------------------------------------------------------

				ObservableEventsList(int p_peerIndex, int p_amountPeers)
				{
					m_peerLogicalTime = new FixedVectorTimer(p_peerIndex,p_amountPeers);
					m_highEpochCounter = DELTAGAP_EPOCH;
					m_lowEpochCounter = 0;
					
				}

				void AddObservableEvent(string p_toAdd)
				{
					ObservableEvent<T,S>* currentEvent = new ObservableEvent<T,S>(p_toAdd ,  m_highEpochCounter );
					m_highEpochCounter++;
					m_observableEvents[currentEvent->GetUuid()] = currentEvent;
				}

				void AddUpdate(string p_toAdd)
				{
					ObservableEvents::ObservableEvent<T,S>* currentUpdate = new ObservableEvent<T,S>(p_toAdd);
					m_observableEvents.push_back(currentUpdate);
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Merge events. inv + inv = inv | ins + ins = ins | T(ins, inv) inv + ins = ins | ins + inv = inv </summary>
				///
				/// <remarks>	Adrian, 10/09/2015. </remarks>
				///
				/// <param name="p_observableEvents">	[in,out] If non-null, the observable events. </param>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void GetListEvents(string p_stringEvents, vector<string>& p_outputVector)
				{
					vector<string> stringEvents;
					HelperClass::ParseStringSentence(p_stringEvents, ";", stringEvents);
					for (const auto &iter : stringEvents)
					{
						p_outputVector.push_back(new ObservableEvent<T,S>(*iter));
					}
				}

				virtual bool GetInvalidationStatus()
				{
					return false;
				}
				
				vector<unsigned char> GetAllEvents()
				{
					vector<unsigned char> allUpdates;

					for (int index = 0; index < m_observableEvents.size(); index++)
					{
						m_observableEvents[index]->ToSerialize(allUpdates);
					}

					return allUpdates;
				}


				virtual void AddEventToList(ObservableEvent<T,S,AS>* p_currentEvent)
				{
					p_currentEvent->SetEpoch(m_lowEpochCounter);
					m_lowEpochCounter++;
					m_peerLogicalTime->IncrementTimer();
					p_currentEvent->SetTimeStamp(m_peerLogicalTime); 

					m_observableEvents.push_back(p_currentEvent);
					IncrementTimer();

					if (m_observableEvents.size() >= MAX_AMOUNT_OBSERVABLE_ELEMENTS)
					{
						//compact list
						GroupEvents();
					}
				}

				void GroupEvents()
				{
					
				}

				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Convert this object into a string representation. </summary>
				///
				/// <remarks>	Adrian, 10/09/2015. </remarks>
				///
				/// <returns>	A string that represents this object. </returns>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				string ToString()
				{
					stringstream output;

					output << m_peerLogicalTime->ToString();
					output << "&";

					for (int index = 0; index < m_observableEvents.size(); index ++)
					{
						output << m_observableEvents[index]->ToString()<< "*";
					}

					return output.str();
				}

				string ToString(string p_requestedOe)
				{
					stringstream output;

					//append vector timer 

					output << m_peerLogicalTime->ToString();
					output << "&";
					output << GetRequestedOe(p_requestedOe);
					return output.str();
				}


				////////////////////////////////////////////////////////////////////////////////////////////////////
				/// <summary>	Increment timer. </summary>
				///
				/// <remarks>	Adrian, 10/09/2015. </remarks>
				////////////////////////////////////////////////////////////////////////////////////////////////////
				void IncrementTimer()
				{
					m_peerLogicalTime->IncrementTimer();
				}


				bool operator == (ObservableEventsList* p_otherList)
				{
					int index = 0;
					for (int index = 0; index < m_observableEvents.size(); index++)
					{
						if (m_observableEvents[index] != p_otherList->m_observableEvents[index])
							return false;
					}
					return true;
				}

				void SetVectorTimer(VectorTimer* p_vectorTimer)
				{
					m_peerLogicalTime = (FixedVectorTimer*)p_vectorTimer;
				}

				bool CheckNewUpdates()
				{
					return newUpdates.size() > 0;
				}

				void AddObservableEvent(ObservableEvent<T, S, AS>* p_observableEvent)
				{
					m_observableEvents.push_back(p_observableEvent);
				}

				int GetAmountObservablEvents()
				{
					return m_observableEvents.size();
				}

				// DEBUG method
				string GetRequestedOe(string p_uuidList)
				{
					vector<string> requestedUuid;
					HelperClass::ParseStringSentence(p_uuidList, ";", requestedUuid);
					
					stringstream ss;

					for (int index = 0; index < requestedUuid.size(); index++)
					{
						ss << GetObservableEvent(boost::lexical_cast<uuid>(requestedUuid[index]))->ToString() << "*";
					}

					return ss.str();
				}

				//DEBUG method
				string GetMissingOes(string p_uuidList)
				{
					vector<string> allUuids;
					HelperClass::ParseStringSentence(p_uuidList, ";", allUuids);
					stringstream ss;
					for (int index = 0; index < allUuids.size(); index++)
					{
						if (!find(boost::lexical_cast<uuid>(allUuids[index])))
							ss << allUuids[index]<< ";";
					}

					return ss.str();
				}

				// Returns a list of OE that were not found in the list
				vector<uuid> GetMissingOes(vector<uuid> p_allUuids)
				{
					vector<uuid> neededUuids;
					for (int index = 0; index < p_allUuids.size(); index++)
					{
						if (!find(p_allUuids[index]))
							neededUuids.push_back(p_allUuids[index]);
					}

					return ss.str();
				}

				// Serialize a UUID
				void SerializeUuid(vector<unsigned char>& p_array, uuid p_currentUuid)
				{
					unsigned char* uuidArray = (unsigned char*)&p_currentUuid;

					for (int index = 0; index < sizeof(boost::uuids::uuid); index++)
					{
						p_array.push_back(uuidArray[index]);
					}
				}

				virtual vector<uuid> ParseByteArrayToUuids(vector<unsigned char> p_ByteArray) = 0;
				
				// Convertion from byte array to UUID - each UUID is 16 bytes
				void ByteArrayToUuid(vector<unsigned char>& p_array, int &p_startingPoint, vector<uuid>& p_allUuids)
				{
					if (p_startingPoint + 15 < p_array.size())
					{
						uuid tempUuid;
						unsigned char uuidData[16];
						for (int index = 0; index < sizeof(boost::uuids::uuid); index++, p_startingPoint++)
						{
							uuidData[index] = p_array[p_startingPoint];
						}

						memcpy(&tempUuid, uuidData, 16);

						p_allUuids.push_back(tempUuid);
					}
				}

				// Check if a UUID is contained in the list
				bool find(uuid p_inputtedId)
				{
					if (m_containedObservableEvents.find(p_inputtedId) !=  m_containedObservableEvents.end())
					{
						return true;
					}
					
					return false;
				}

				ObservableEvent<T,S, AS>* GetObservableEvent(uuid p_oeUuid)
				{
					for (int index = 0; index < m_observableEvents.size(); index++)
					{
						if (m_observableEvents[index]->GetUuid() == p_oeUuid)
						{
							return m_observableEvents[index];
						}
					}

					return nullptr;
				}

				virtual void SerializeAllUuids(vector<unsigned char>&) = 0;
				virtual void GetNeededOE(vector<unsigned char>&, vector<unsigned char>&, int, int) = 0;

			protected:

				vector<ObservableEvent<T, S, AS>*> m_observableEvents;
				hash_map<uuid, int> m_containedObservableEvents;

				FixedVectorTimer* m_peerLogicalTime;
				

				int m_amountPeers;
				// The epoch of locally produced events
				int m_lowEpochCounter;
				// The epoch of events received form over the network
				int m_highEpochCounter;

				vector<string> newUpdates;

				// Ordering functions, makes use of tie breaker function based on UUID if vector clocks on events are deemed concurrent
				bool preFunction(ObservableEvent<T,S, AS>* p_lhs, ObservableEvent < T,S, AS> * p_rhs)
				{
					if (p_lhs == p_rhs)
					{
						return false;
					}
					else if (p_lhs->GetTimeStamps() == p_rhs->GetTimeStamps())
					{
						return TieFunction(p_lhs, p_rhs);
					}
					else if (p_lhs->GetTimeStamps() <= p_rhs->GetTimeStamps())
					{
						return true;
					}
					else if (p_rhs->GetTimeStamps() <= p_lhs->GetTimeStamps())
					{
						return false;
					}
					else 
						return TieFunction(p_lhs, p_rhs);
				}

				bool TieFunction(ObservableEvent<T, S, AS>* p_lhs, ObservableEvent < T,S, AS> * p_rhs)
				{
					return (p_lhs->GetUuid() < p_rhs->GetUuid());
				}
		};
	}
}
