#pragma once 

#include <string>
#include <vector>

#include <ObservableEvents\ObservableEvent.h>

#include "../../Integrators/IrradianceCache.h"
#include "../../Integrators/RadianceDetails/RadiancePoint.h"
#include "../../AccelerationStructures/CachingStructure/LockingOctree.h"

using Network::ObservableEvents::ObservableEvent;
using Engine::RadiancePoint;
using Engine::IrradianceCache;
using Engine::LockingOctree;
using std::string;
using std::vector;
namespace Engine
{
	namespace Networking
	{
		namespace ObservableEvents
		{
			//fix
			enum INVALIDATION_TYPE
			{
				INVALIDATION_LIGHT = 0,
				GEOMETRY_TYPE = 1
			};

			//subtype issue here chnage t

			template<typename T = RadiancePoint, typename IC = Integrator, typename AS = LockingOctree>
			class InvalidationEvent : public ObservableEvent<T, IC, AS>
			{
			public:
				using ObservableEvent<T, IC,AS>::m_uuid;
				using ObservableEvent<T, IC,AS>::GetStringUpdate;
				using ObservableEvent<T, IC,AS>::m_timeStamp;
				using ObservableEvent<T, IC,AS>::m_type;
				using ObservableEvent<T, IC,AS>::m_sourceOfOe;
				using ObservableEvent<T, IC,AS>::SetEpoch;
				using ObservableEvent<T, IC,AS>::m_updateRepresented;
				using ObservableEvent<T, IC,AS>::preFunction;
				using ObservableEvent<T, IC,AS>::GetNextUuid;
				using ObservableEvent<T, IC,AS>::ToSerialize;
				using ObservableEvent<T, IC,AS>::FromSerialize;
				using ObservableEvent<T, IC,AS>::SetLockingStructure;
				using ObservableEvent<T, IC, AS>::ByteArrayToUuid;
				using ObservableEvent<T, IC, AS>::SerializeUuid;

				InvalidationEvent() : ObservableEvent()
				{
					m_type = OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT;
				}

				InvalidationEvent(AS* p_octree, uuid p_eventUuid, OBSERVABLE_PRODUCED_TYPE p_provenenance = INTERNAL_UPDATE)
					: ObservableEvent(OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT, p_provenenance)
				{
					m_type = OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT;
					m_cachingStructure = p_octree;
					memcpy(&m_sceneEventUuid, &p_eventUuid, sizeof(p_eventUuid));
				}

				InvalidationEvent(uuid p_eventUuid) 
					: ObservableEvent(OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT)
				{
					m_type = OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT;
					m_sceneEventUuid = p_eventUuid;

					memcpy(&m_sceneEventUuid, &p_eventUuid,16);
				}

				virtual void Commit(IC* p_structure)
				{
					//invalidate cache
					p_structure->InvalidateCache();
				}

				virtual string ToString()
				{
					std::stringstream ss;
					ss << m_type;
					ss << "_";
					ss << m_uuid;
					ss << "_";
					ss << m_timeStamp->ToString();
					return ss.str();
					return "";
				}

				virtual string GetStringUpdate()
				{
					return "";
				}

				virtual void ToSerialize(vector<unsigned char>& p_list)
				{
					//set type
					ConvertionClass::ConvertInt(m_type, p_list);
					//set uuid
					SerializeUuid(p_list);
					// set timestamp
					m_timeStamp->Serialize(p_list);
					SceneEventUuidToByteArray(p_list);
				}

				virtual void ToSerialize(std::vector<unsigned char>& p_list, int p_startingPoint)
				{
					int startingPoint = p_startingPoint;
					//set type
					ConvertionClass::ConvertInt(m_type, startingPoint, p_list);
					//set uuid
					SerializeUuid(m_uuid, p_list);
					// set timestamp
					m_timeStamp->Serialize(p_list);
					SceneEventUuidToByteArray(p_list);
				}

				void ByteArrayToSceneEventUuid(vector<unsigned char> &p_array, int &p_startingPoint)
				{
					ByteArrayToUuid(p_array, p_startingPoint, m_sceneEventUuid);
				}


				virtual void FromSerialize(vector<unsigned char>& p_charArray)
				{
					int startingPoint = 0;
					ConvertionClass::ByteArrayToInt(m_type, p_charArray, startingPoint, 4);
					//uuid
					ByteArrayToUuid(p_charArray, startingPoint, m_uuid);
					m_timeStamp->FromByteArray(p_charArray, startingPoint);
					ByteArrayToSceneEventUuid(p_charArray, startingPoint);

				}

				virtual void FromSerialize(vector<unsigned char>& p_charArray, int &p_startingPoint)
				{
					//type of 
					ConvertionClass::ByteArrayToInt(m_type, p_charArray, p_startingPoint, 4);
					ByteArrayToUuid(p_charArray, p_startingPoint ,m_uuid);
					m_timeStamp->FromByteArray(p_charArray, p_startingPoint);
					ByteArrayToSceneEventUuid(p_charArray, p_startingPoint);
				}

				void AddSceneEvent(uuid p_eventUuid)
				{
					m_sceneEventUuid = p_eventUuid;
				}

				void SceneEventUuidToByteArray(vector<unsigned char>& p_outputArray)
				{
					SerializeUuid( m_sceneEventUuid, p_outputArray);
				}


				inline bool operator < (InvalidationEvent& p_other)
				{
					return preFunction(*this, p_other);
				}

				void GenerateUuids()
				{
					char input = 'y';
					do
					{
						std::cin >> input;
						std::stringstream test;
						test << "";
						boost::uuids::uuid currentUuid = GetNextUuid();
						test << currentUuid;
						std::cout << test.str() << std::endl;
					} while (input == 'y');
				}

				virtual void SetLockingStructure(AS* p_cachingStructure)
				{
					m_cachingStructure = p_cachingStructure;
				}

				uuid GetSceneDetailsUuid()
				{
					return m_sceneEventUuid;
				}

			private:
				AS* m_cachingStructure;
				boost::uuids::uuid m_sceneEventUuid;
			};
		}
	}
}