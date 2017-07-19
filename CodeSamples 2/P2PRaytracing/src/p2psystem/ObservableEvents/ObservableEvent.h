///-------------------------------------------------------------------------------------------------
// file:	ObservableEvents\ObservableEvent.h
//
// summary:	Declares the observable event class
//			TODO: Finish this
///-------------------------------------------------------------------------------------------------

#pragma once 

#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <sstream>

#include "../VectorTimer/VectorTimer.h"
#include "../VectorTimer/FixedVectorTimer.h"
#include "../UtilityClasses/HelperClass.h"

//#include "../../RayTracing/Integrators/RadianceDetails/RadiancePoint.h"



using std::string;
using namespace boost::uuids;

namespace Network
{
	namespace ObservableEvents
	{

		template <typename T, typename S, typename AS>
		class ObservableEventsList;

		

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Values that represent observable types. </summary>
		///
		/// <remarks>	Adrian, 05/09/2015. </remarks>
		enum OBSERVABLE_PRODUCED_TYPE
		{
			INTERNAL_UPDATE = 0,
			EXTERNAL_UPDATE
		};

		enum OBSERVABLE_EVENT_TYPE
		{
			INSERT_EVENT = 0,
			INVALIDATION_EVENT = 1,
			PARTIAL_INVALIDATION = 2,
			SCENE_EVENT = 3
		};

		///-------------------------------------------------------------------------------------------------
		/// <summary>	An observable event. </summary>
		///
		/// <remarks>	Adrian, 05/09/2015. </remarks>
		///-------------------------------------------------------------------------------------------------
		template <typename T, typename IC, typename AS>
		class ObservableEvent
		{

		public:
			friend class ObservableEventsList<T, IC, AS>;

			virtual void ToSerialize(std::vector<unsigned char>&) = 0;
			virtual void FromSerialize(vector<unsigned char>&, int&) = 0;
			virtual void SetLockingStructure(AS*) = 0;

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Constructors. </summary>
			///
			/// <remarks>	Adrian, 23/09/2015. </remarks>
			///
			/// <param name="p_type">	The type. </param>
			///-------------------------------------------------------------------------------------------------
			//by defualt insert 
			ObservableEvent()
			{
				//m_uuid = GetNextUuid();
				m_timeStamp = new FixedVectorTimer();
				m_committed = false;
			}

			ObservableEvent(int p_type, uuid p_eventId, VectorTimer* p_timeStamp)
			{
				//m_uuid = GetNextUuid();
				m_timeStamp = new FixedVectorTimer(p_timeStamp);
				m_uuid = p_eventId;
				m_committed = false;
			}

			ObservableEvent(int p_type, uuid p_eventId)
			{
				//m_uuid = GetNextUuid();
				m_timeStamp = new FixedVectorTimer();
				SetUuid(p_eventId);
				m_committed = false;
			}

			ObservableEvent(int p_type)
			{
				//m_uuid = GetNextUuid();
				m_timeStamp = new FixedVectorTimer();
				m_uuid = GetNextUuid();
				m_type = p_type;
				m_committed = false;
			}

			ObservableEvent(int p_type, OBSERVABLE_PRODUCED_TYPE p_proveneance)
			{
				m_timeStamp = new FixedVectorTimer();
				m_uuid = GetNextUuid();
				m_type = p_type;
				m_committed = false;
				m_sourceOfOe = p_proveneance;
			}

			ObservableEvent(VectorTimer* p_currentTime, T p_updateRepresented, int p_epoch, int p_updateType, OBSERVABLE_PRODUCED_TYPE p_provenance = INTERNAL_UPDATE)
			{
				m_uuid = GetNextUuid();
				m_timeStamp = new FixedVectorTimer(*((FixedVectorTimer*)p_currentTime));
				T* temp = new T(p_updateRepresented);
				m_updateRepresented.push_back(temp);
				m_type = p_updateType;

				m_sourceOfOe = p_provenance;
				
				if (p_provenance == EXTERNAL_UPDATE)
					m_highEpochNumber = p_epoch;
				else
					m_lowEpochNumber = p_epoch;

				m_committed = false;
			}

			ObservableEvent(vector<T*> p_updateRepresented, int p_updateType, OBSERVABLE_PRODUCED_TYPE p_provenance = INTERNAL_UPDATE)
			{
				m_uuid = GetNextUuid();
				m_updateRepresented = p_updateRepresented;
				m_type = p_updateType;
				m_sourceOfOe = p_provenance;
			}

			ObservableEvent(string inputstring, int p_epoch, OBSERVABLE_PRODUCED_TYPE p_provenance = INTERNAL_UPDATE)
			{
				//FromString(inputstring);
				m_uuid = GetNextUuid();
				m_sourceOfOe = p_provenance;
				SetEpoch(p_epoch);
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Convert this object into a string representation. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <returns>	A string that represents this object. </returns>
			///-------------------------------------------------------------------------------------------------
			virtual void Commit(IC*)
			{}

			void ParseUpdate(string p_inputupdate)
			{
				//test puproses only for now to see that everything is working ok
				T* temp = new T(p_inputupdate);
				m_updateRepresented.push_back(temp);
			}

			void SerializeUuid(vector<unsigned char>& p_array)
			{
				unsigned char* uuidArray = (unsigned char*)&m_uuid;

				for (int index = 0; index < sizeof(boost::uuids::uuid); index++)
				{
					p_array.push_back(uuidArray[index]);
				}
			}

			void SerializeUuid(uuid &p_currentUuid,vector<unsigned char>& p_array)
			{
				unsigned char* uuidArray = (unsigned char*)&p_currentUuid;

				for (int index = 0; index < sizeof(boost::uuids::uuid); index++)
				{
					p_array.push_back(uuidArray[index]);
				}
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets string update. </summary>
			///
			/// <remarks>	Adrian, 23/09/2015. </remarks>
			///
			/// <returns>	The string update. </returns>
			///-------------------------------------------------------------------------------------------------
			virtual string GetStringUpdate()
			{
				return "";
			}
			
			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets time stamps. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <returns>	null if it fails, else the time stamps. </returns>
			///-------------------------------------------------------------------------------------------------
			VectorTimer* GetTimeStamps()
			{
				return m_timeStamp;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets observable update. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <returns>	The observable update. </returns>
			///-------------------------------------------------------------------------------------------------
			T GetObservableUpdate()
			{
				return m_updateRepresented;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Equal observable events </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <param name="p_comparer">	The comparer. </param>
			///
			/// <returns>	true if the parameters are considered equivalent. </returns>
			///-------------------------------------------------------------------------------------------------
			T operator = (const ObservableEvent<T, IC,AS>& p_comparer)
			{
				m_uuid = p_comparer.GetUuid();
				m_timeStamp = p_comparer.GetTimeStamps();
				m_sourceOfOe = p_comparer.m_sourceOfOe();
				return *this;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets event type. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <returns>	The event type. </returns>
			///-------------------------------------------------------------------------------------------------
			int GetType()
			{
				return m_type;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets the uuid. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <returns>	The uuid. </returns>
			///-------------------------------------------------------------------------------------------------
			uuid GetUuid()
			{
				return m_uuid;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets the Subtype. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <returns>	The SubType. </returns>
			///-------------------------------------------------------------------------------------------------
			int GetSubType()
			{
				return m_subType;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets the Subtype. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <returns>	The SubType. </returns>
			///-------------------------------------------------------------------------------------------------
			void SetSubType(int p_subType)
			{
				m_subType = p_subType;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Sets time stamp. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <param name="p_timeStamp">	[in,out] If non-null, the time stamp. </param>
			///-------------------------------------------------------------------------------------------------
			void SetTimeStamp(VectorTimer* p_timeStamp)
			{
				m_timeStamp = new FixedVectorTimer(*((FixedVectorTimer*)p_timeStamp));
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Sets represented update. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <param name="p_update">	The update. </param>
			///-------------------------------------------------------------------------------------------------
			void SetRepresentedUpdate(T p_update)
			{
				m_updateRepresented = p_update;
			}

			bool operator == (const ObservableEvent<T, IC,AS>& p_currentEvent)
			{
				return m_uuid == p_currentEvent.m_uuid;
			}

			void SetEpoch(int p_epoch)
			{
				switch (m_sourceOfOe)
				{
				case INTERNAL_UPDATE:
					m_lowEpochNumber = p_epoch;
					break;
				case EXTERNAL_UPDATE:
					m_highEpochNumber = p_epoch;
					break;
				default:
					m_lowEpochNumber = p_epoch;
					break;
				}
			}

			void SetCommitted(bool p_value)
			{
				m_committed = p_value;
			}

			bool GetCommitted()
			{
				return m_committed;
			}

			bool operator < (const ObservableEvent<T, IC,AS>& p_other)
			{
				return preFunction(p_other);
			}

			bool GetObservableEventStatus()
			{
				return m_parskedOk;
			}

			void InsertPoint(T* p_currentPoint)
			{
				m_updateRepresented.push_back(p_currentPoint);
			}

			static bool preFunction(ObservableEvent<T, IC, AS>* p_lhs, ObservableEvent <T, IC, AS>* p_rhs)
			{
				//check if equal 
				if (p_lhs->GetUuid() == p_rhs->GetUuid())
				{
					return false;
				}
				else if (EventsConcurrent(p_lhs, p_rhs))
				{
					return TieFunction(p_lhs, p_rhs);
				}
				else if (VectorTimer::IsGreater(p_lhs->m_timeStamp, p_rhs->m_timeStamp))
				{
					return true;
				}
				else if (VectorTimer::IsGreater(p_rhs->m_timeStamp, p_lhs->m_timeStamp))
				{
					return false;
				}
				else
				{
					return	TieFunction(p_lhs, p_rhs);
				}
			}
				
			void ByteArrayToEventUuid(vector<unsigned char>& p_array, int &p_startingPoint)
			{
				ByteArrayToUuid(p_array, p_startingPoint, m_uuid);
			}
			
			void static ByteArrayToUuid(vector<unsigned char>& p_array, int &p_startingPoint, uuid &p_uuid)
			{
				unsigned char uuidData[16];

				for (int index = 0; index < sizeof(boost::uuids::uuid); index++, p_startingPoint++)
				{
					uuidData[index] = p_array[p_startingPoint];
				}
				memcpy(&p_uuid, uuidData, 16);
			}

			void SetUuid(uuid p_uuid)
			{
				memcpy(&p_uuid, &m_uuid, 16);
			}

			void SetSourceOfOe(OBSERVABLE_PRODUCED_TYPE p_type)
			{
				m_sourceOfOe = p_type;
			}

			int GetSourceOfOE()
			{
				return m_sourceOfOe;
			}

			virtual ~ObservableEvent()
			{}

		protected:
			/// <summary>	The uuid. </summary>
			boost::uuids::uuid m_uuid;
			/// <summary>	The type of event. </summary>
			int m_type;
			/// <summary>	The sub-type of event. </summary
			int m_subType;
			/// <summary>How was observable event produced. </summary>
			OBSERVABLE_PRODUCED_TYPE m_sourceOfOe;
			/// <summary>	The time stamp. </summary>
			VectorTimer* m_timeStamp;
			/// <summary>	The update represented. </summary>
			vector<T*> m_updateRepresented;
			/// <summary>	The high epoch number. </summary>
			int m_highEpochNumber;
			/// <summary>	The low epoch number. </summary>
			int m_lowEpochNumber;
			
			bool m_committed;

			bool m_parskedOk = true;

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets the next uuid. </summary>
			///
			/// <remarks>	Adrian, 11/09/2015. </remarks>
			///
			/// <returns>	The next uuid. </returns>
			///-------------------------------------------------------------------------------------------------
			static uuid GetNextUuid()
			{
				random_generator gen;
				uuid id = gen();

				return id;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Tie function. </summary>
			///
			/// <remarks>	Adrian, 23/09/2015. </remarks>
			///
			/// <param name="p_lhs">	Left observable event. </param>
			/// 
			/// <param name="p_rhs">	Right observable event. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			///-------------------------------------------------------------------------------------------------
			static bool TieFunction(ObservableEvent<T, IC,AS>* p_lhs ,ObservableEvent < T, IC, AS>* p_rhs)
			{
				if (p_lhs->GetType() == OBSERVABLE_EVENT_TYPE::INSERT_EVENT  && p_rhs->GetType() == OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT)
					return true;
				else if (p_lhs->GetType() == OBSERVABLE_EVENT_TYPE::PARTIAL_INVALIDATION  && p_rhs->GetType() == OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT)
					return true;
				else if (p_lhs->GetType() == OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT  && p_rhs->GetType() == OBSERVABLE_EVENT_TYPE::PARTIAL_INVALIDATION)
					return false;
				else if (p_lhs->GetType() == OBSERVABLE_EVENT_TYPE::INSERT_EVENT  && p_rhs->GetType() == OBSERVABLE_EVENT_TYPE::PARTIAL_INVALIDATION)
					return true;
				else if (p_lhs->GetType() == OBSERVABLE_EVENT_TYPE::PARTIAL_INVALIDATION  && p_rhs->GetType() == OBSERVABLE_EVENT_TYPE::INSERT_EVENT)
					return false;
				else if (p_lhs->GetType() == OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT && p_rhs->GetType() == OBSERVABLE_EVENT_TYPE::INSERT_EVENT)
					return false;
				else 
					return (p_lhs->GetUuid() < p_rhs->GetUuid());
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Check uuids. </summary>
			///
			/// <remarks>	Adrian, 23/09/2015. </remarks>
			///
			/// <param name="p_rhs">	[in,out] If non-null, the right hand side. </param>
			///
			/// <returns>	true if uuids are equal and false if not. </returns>
			///-------------------------------------------------------------------------------------------------
			inline static bool CheckUuids(ObservableEvent<T, IC, AS>* p_lhs, ObservableEvent<T, IC, AS>* p_rhs)
			{
				return (p_lhs->GetUuid() == p_rhs->GetUuid());
			}

			inline bool operator == (ObservableEvent<T, IC,AS>& p_rhs)
			{
				return m_uuid == p_rhs.GetUuid();
			}

			static bool EventsConcurrent(ObservableEvent<T, IC, AS>* p_lhs, ObservableEvent<T, IC,AS>* p_rhs)
			{
				bool lhsIsSmaller = false;
				bool rhsIsSmaller = false;

				for (int index = 0; index < p_lhs->m_timeStamp->vectorTimer.size(); index++)
				{
					//lhs is smaller 
					if (p_lhs->m_timeStamp->vectorTimer[index] < p_rhs->m_timeStamp->vectorTimer[index])
					{
						lhsIsSmaller = true;
					}
					//rhs is smaller
					else if (p_rhs->m_timeStamp->vectorTimer[index] < p_lhs->m_timeStamp->vectorTimer[index])
					{
						rhsIsSmaller = true;
					}
				}

				bool output = lhsIsSmaller && rhsIsSmaller;
				return output;
			}
		};

		template<typename T, typename IC, typename AS>
		inline bool operator == (const ObservableEvent<T, IC, AS>& lhs, const ObservableEvent<T, IC,AS>& rhs)
		{
			return lhs.GetUuid() == rhs.GetUuid();
		};
	}
}