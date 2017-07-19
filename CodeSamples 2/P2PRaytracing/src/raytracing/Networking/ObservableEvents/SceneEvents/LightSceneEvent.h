#pragma once 

#include  <vector>

#include "./SceneInvalidationDetails.h"

#include "../../../Scene/Scene.h"
#include "../../../AccelerationStructures/CachingStructure/LockingOctree.h"
#include "../../../Integrators/RadianceDetails/RadiancePoint.h"
#include "../../../Integrators/IrradianceCache.h"
#include "../../../CoreClasses/UtilClasses/ConvertionHelper.h"

#include "../../../CoreClasses/AssetsManager/LightManager.h"


using std::vector;

using Network::ObservableEvents::ObservableEvent;
using Engine::IrradianceCache;
using Engine::RadiancePoint;
using Engine::LockingOctree;

using Engine::Networking::ObservableEvents::SceneEvents::SceneEvent;
using Engine::Communication::Serialization::ConvertionClass;
using Engine::ResourceManager::LightChangeDescription;
using Engine::ResourceManager::LightManager;


namespace Engine
{
	namespace Networking
	{
		namespace ObservableEvents
		{
			namespace SceneEvents
			{

				typedef LightManager<Light> TM;
				typedef LightChangeDescription ED;
				
				template<typename AS>
				class LightSceneEvent : public SceneEvent<TM, ED, AS>
				{
				private:
					using SceneEvent<TM, ED, AS>::m_uuid;
					using SceneEvent<TM, ED, AS>::GetStringUpdate;
					using SceneEvent<TM, ED, AS>::m_timeStamp;
					using SceneEvent<TM, ED, AS>::m_type;
					using SceneEvent<TM, ED, AS>::m_sourceOfOe;
					using SceneEvent<TM, ED, AS>::m_committed;
					using SceneEvent<TM, ED, AS>::SetEpoch;
					using SceneEvent<TM, ED, AS>::m_updateRepresented;
					using SceneEvent<TM, ED, AS>::preFunction;
					using SceneEvent<TM, ED, AS>::ToSerialize;
					using SceneEvent<TM, ED, AS>::FromSerialize;
					using SceneEvent<TM, ED, AS>::SetLockingStructure;
					using SceneEvent<TM, ED, AS>::ByteArrayToEventUuid;
					using SceneEvent<TM, ED, AS>::SerializeUuid;
					using SceneEvent<TM, ED, AS>::Commit;
					using SceneEvent<TM, ED, AS>::GetSceneChangeDescription;
					using SceneEvent<TM, ED, AS>::SetSceneChangeDescription;

				public:

					LightSceneEvent(uuid p_uuid, LightChangeDescription p_eventDescription)
						: SceneEvent(p_uuid)
					{
						m_lightStatus = -2;
						m_lightIndex = -2;
						m_subType = SCENE_EVENT_TYPE::LightEvent;
					}

					LightSceneEvent(LightChangeDescription* p_eventDescription, OBSERVABLE_PRODUCED_TYPE p_sourceOfOe = OBSERVABLE_PRODUCED_TYPE::INTERNAL_UPDATE )
						: SceneEvent(p_sourceOfOe)
					{
						m_type = OBSERVABLE_EVENT_TYPE::SCENE_EVENT;
						m_subType = SCENE_EVENT_TYPE::LightEvent;
						SetSceneChangeDescription(p_eventDescription);
					}

					LightSceneEvent()
						: SceneEvent()
					{
						m_lightStatus = -2;
						m_lightIndex = -2;
					}

					virtual void SetLockingStructure(AS* p_cachingStructure)
					{
						//not usefule at this level
					}

					virtual void Commit(TM* p_scene)
					{
						//execute on scene data 
						m_committed = true;
					}
					virtual ED* GetSceneChangeDescription()
					{
						LightChangeDescription* temp = new LightChangeDescription();
						temp->m_Lightindex = m_lightIndex;
						temp->m_SwitchStatus = m_lightStatus;

						return temp;
					}

					virtual void SetSceneChangeDescription(LightChangeDescription* p_changeDescription)
					{
						m_lightIndex = p_changeDescription->m_Lightindex;
						m_lightStatus = p_changeDescription->m_SwitchStatus;
					}

					//format
					//SceneDetails-SceneEventType-TimeStamp-LightName/Light index-LightStatus-uuid
					virtual void ToSerialize(vector<unsigned char> &p_outputVector)
					{
						ConvertionClass::ConvertInt(m_type, p_outputVector);
						//subtype
						ConvertionClass::ConvertInt(SCENE_EVENT_TYPE::LightEvent, p_outputVector);
						m_timeStamp->Serialize(p_outputVector);
						ConvertionClass::ConvertInt(m_lightIndex, p_outputVector);
						ConvertionClass::ConvertInt(m_lightStatus, p_outputVector);
						SerializeUuid(m_uuid, p_outputVector);
					}

					virtual void FromSerialize(vector<unsigned char> &p_byteArray, int &p_startingPoint)
					{
						ConvertionClass::ByteArrayToInt(m_type, p_byteArray, p_startingPoint, 4);
						int eventType = 0;
						ConvertionClass::ByteArrayToInt(m_subType, p_byteArray, p_startingPoint, 4);
						m_subType = eventType == 0 ? LightEvent : GeometryEvent;

						m_timeStamp->FromByteArray(p_byteArray, p_startingPoint);
						ConvertionClass::ByteArrayToInt(m_lightIndex, p_byteArray, p_startingPoint, 4);
						ConvertionClass::ByteArrayToInt(m_lightStatus, p_byteArray, p_startingPoint, 4);
						ByteArrayToEventUuid(p_byteArray, p_startingPoint);
					}

					uuid GetEventUuid()
					{
						return m_uuid;
					}

					int GetLightStatus()
					{
						return m_lightStatus;
					}

					int GetLightIndex()
					{
						return m_lightIndex;
					}

				private:
					int m_lightIndex = 0;
					int m_lightStatus;
				};
			}
		}
	}
}
