#pragma once 

#include <ObservableEvents/ObservableEvent.h>


#include "../../../Scene/Scene.h"
#include "../../../AccelerationStructures/CachingStructure/LockingOctree.h"
#include "../../../Integrators/RadianceDetails/RadiancePoint.h"
#include "../../../Integrators/IrradianceCache.h"


#include <ObservableEvents/ObservableEvent.h>

using Network::ObservableEvents::ObservableEvent;
using Engine::IrradianceCache;
using Engine::RadiancePoint;
using Engine::LockingOctree;
using Network::ObservableEvents::ObservableEvent;

namespace Engine
{
	namespace Networking
	{
		namespace ObservableEvents
		{
			namespace SceneEvents
			{
				enum SCENE_EVENT_TYPE
				{
					LightEvent = 1,
					GeometryEvent = 2
				};

				typedef RadiancePoint RT;
				typedef IrradianceCache IC;

				template<typename T, typename ED, typename AS>
				class SceneEvent : public ObservableEvent<RadiancePoint, IrradianceCache, AS>
				{
				public:

					using ObservableEvent<RT, IC, AS>::m_uuid;
					using ObservableEvent<RT, IC, AS>::GetStringUpdate;
					using ObservableEvent<RT, IC, AS>::m_timeStamp;
					using ObservableEvent<RT, IC, AS>::m_type;
					using ObservableEvent<RT, IC, AS>::m_sourceOfOe;
					using ObservableEvent<RT, IC, AS>::m_committed;
					using ObservableEvent<RT, IC, AS>::SetEpoch;
					using ObservableEvent<RT, IC, AS>::m_updateRepresented;
					using ObservableEvent<RT, IC, AS>::preFunction;
					using ObservableEvent<RT, IC, AS>::ToSerialize;
					using ObservableEvent<RT, IC, AS>::FromSerialize;
					using ObservableEvent<RT, IC, AS>::SetLockingStructure;
					using ObservableEvent<RT, IC, AS>::ByteArrayToEventUuid;
					using ObservableEvent<RT, IC, AS>::SerializeUuid;
					using ObservableEvent<RT, IC, AS>::m_subType;

					virtual void Commit(T* p_manager) = 0;
					virtual ED* GetSceneChangeDescription() = 0;
					virtual void SetSceneChangeDescription(ED*) = 0;
	
					SceneEvent(uuid p_uuid) 
						: ObservableEvent(OBSERVABLE_EVENT_TYPE::SCENE_EVENT,
										  p_uuid)
					{}

					SceneEvent(OBSERVABLE_PRODUCED_TYPE p_sourceOfOe) 
						: ObservableEvent(OBSERVABLE_EVENT_TYPE::SCENE_EVENT, 
										 p_sourceOfOe)
					{}

					SceneEvent()
						: ObservableEvent(OBSERVABLE_EVENT_TYPE::SCENE_EVENT)
					{}

				};
			}
		}
	}
}
