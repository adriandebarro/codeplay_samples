#pragma once 

#include <ObservableEvents\ObservableEvent.h>

#include "../../Integrators/RadianceDetails/RadiancePoint.h"
#include "../../Integrators/IrradianceCache.h"
#include "../../AccelerationStructures/CachingStructure/LockingOctree.h"
#include "InvalidationEvent.h"

#include "../../CoreClasses/BoundingShapes/BoundingBoxAnalyzer.h"



using namespace Network::ObservableEvents;
using Engine::RadiancePoint;
using Engine::IrradianceCache;
using Engine::LockingOctree;

namespace Engine
{
	namespace Networking
	{
		namespace ObservableEvents
		{
			//------------------------------------------------------------------------------------------------------
			//	Class: PartialInvalidationEvent
			//Summary: Representation of a partial invalidation event
			//------------------------------------------------------------------------------------------------------
			template<typename T, typename IC, typename AS>
			class PartialInvalidationEvent : public ObservableEvent<T, IC, AS>
			{
	

			public:

				//if initialised i nthis manner it would mean that it is being used in the 
				PartialInvalidationEvent(Engine::BoundingShapes::BoundingBoxAnalyzer* p_boundingBoxAnalyzer) 
					: ObservableEvent()
				{
					m_type = OBSERVABLE_EVENT_TYPE::PARTIAL_INVALIDATION;
					m_boundingBoxAnalyser = p_boundingBoxAnalyzer;
				}

				/// object received ror
				PartialInvalidationEvent(uuid p_eventUuid, Engine::BoundingShapes::BoundingBoxAnalyzer* p_boundingBoxAnalyzer, Point3D p_position) : ObservableEvent()
				{
					m_type = OBSERVABLE_EVENT_TYPE::PARTIAL_INVALIDATION;
					m_sceneEventUuid = p_eventUuid;
					memcpy(&m_sceneEventUuid, &p_eventUuid, 16);
					m_boundingBoxAnalyser = p_boundingBoxAnalyzer;
					m_boundingBoxNumber = m_boundingBoxAnalyser->CheckContainingBoundingBox(p_position);
				}

				PartialInvalidationEvent(AS* p_octree, uuid p_eventUuid,  Engine::BoundingShapes::BoundingBoxAnalyzer* p_boundingBoxAnalyzer, Point3D p_position,OBSERVABLE_PRODUCED_TYPE p_provenance =  OBSERVABLE_PRODUCED_TYPE::INTERNAL_UPDATE) : ObservableEvent(OBSERVABLE_EVENT_TYPE::PARTIAL_INVALIDATION, p_provenance)
				{
					m_type = OBSERVABLE_EVENT_TYPE::PARTIAL_INVALIDATION;
					m_cachingStructure = p_octree;
					memcpy(&m_sceneEventUuid, &p_eventUuid, sizeof(p_eventUuid));
					m_boundingBoxAnalyser = p_boundingBoxAnalyzer;
					m_boundingBoxNumber = m_boundingBoxAnalyser->CheckContainingBoundingBox(p_position);
				}

				virtual void Commit(IC* p_structure)
				{
					p_structure->PartialInvalidate(m_boundingBoxAnalyser->GetBoundingBox(m_boundingBoxNumber));
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
					ConvertionClass::ConvertInt(m_boundingBoxNumber, p_list);
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
					ConvertionClass::ByteArrayToInt(m_boundingBoxNumber, p_charArray,startingPoint,4);
				}

				virtual void FromSerialize(vector<unsigned char>& p_charArray, int &p_startingPoint)
				{
					//type of 
					ConvertionClass::ByteArrayToInt(m_type, p_charArray, p_startingPoint, 4);
					ByteArrayToUuid(p_charArray, p_startingPoint, m_uuid);
					m_timeStamp->FromByteArray(p_charArray, p_startingPoint);
					ByteArrayToSceneEventUuid(p_charArray, p_startingPoint);
					ConvertionClass::ByteArrayToInt(m_boundingBoxNumber, p_charArray ,p_startingPoint, 4);
				}

				void AddSceneEvent(uuid p_eventUuid)
				{
					m_sceneEventUuid = p_eventUuid;
				}

				void SceneEventUuidToByteArray(vector<unsigned char>& p_outputArray)
				{
					SerializeUuid(m_sceneEventUuid, p_outputArray);
				}


				inline bool operator < (InvalidationEvent<T, IC,AS>& p_other)
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
				uuid m_sceneEventUuid;
				AS* m_cachingStructure;
				int m_boundingBoxNumber;
				Engine::BoundingShapes::BoundingBoxAnalyzer* m_boundingBoxAnalyser;
			};
		}
	}
}
