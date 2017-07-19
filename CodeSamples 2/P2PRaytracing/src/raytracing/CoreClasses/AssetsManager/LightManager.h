#pragma once 

#include <vector>
#include <map>
#include <stack>


#include "ResourceManager.h"
#include "../../Light/Light.h"
#include "../Point/Point3D/Point3D.h"

#include "../../Networking/ObservableEvents/InvalidationEvent.h"

using std::vector;
using std::map;
using std::stack;

namespace Engine
{
	namespace ResourceManager
	{

		struct LightChangeDescription : InvalidationDetails
		{
			int m_Lightindex;
			int m_SwitchStatus;

			LightChangeDescription() : 
				InvalidationDetails(Engine::Networking::ObservableEvents::INVALIDATION_TYPE::INVALIDATION_LIGHT)
			{
				m_Lightindex = 0;
				m_SwitchStatus = 0;
			}

			LightChangeDescription(int p_lightIndex, int p_switchStatus) : 
				InvalidationDetails(Engine::Networking::ObservableEvents::INVALIDATION_TYPE::INVALIDATION_LIGHT, p_lightIndex)
			{
   				m_Lightindex = p_lightIndex;
				m_SwitchStatus = p_switchStatus;
			}

			LightChangeDescription(const LightChangeDescription& p_lightChangeDescription) : 
				InvalidationDetails(0)
			{
				m_Lightindex = p_lightChangeDescription.m_Lightindex;
				m_SwitchStatus = p_lightChangeDescription.m_SwitchStatus;
			}
		};

		typedef map<string, Light*>::iterator iter;
		typedef map<string, string>::iterator hIter;

		template<typename T = Light>
		class LightManager: public ResourceManager<T>
		{
		private:
			map<string, Light*> m_allLight;
			float m_acceptedRange = 3;
			map<string, string> m_allUpdatedLights;


		public:
			LightManager(float p_range, vector<Light*> &p_allLight)
			{
				m_acceptedRange = p_range;
				AddLights(p_allLight);
			}

			void AddLights(vector<Light*> &p_allLights)
			{
				for (int index = 0; index < p_allLights.size(); index++)
				{
					Light* currentLight = p_allLights[index];
					currentLight->SetIndex(index);
					m_allLight[currentLight->GetLightName()] = currentLight;
				}
			}

			using ResourceManager<T>::GetItemById;
			virtual T* GetItemById(int p_index)
			{
				int index = 0;
				if (p_index < m_allLight.size())
				{

					for (iter it = m_allLight.begin(); it != m_allLight.end(); ++it)
					{
						if (index == p_index)
						{
							return (T*)it->second;
						}
						else
						{
							index++;
						}
					}

					return nullptr;
				}
				else
					return nullptr;
			}

			
			using ResourceManager<T>::CheckForNewEvents;
			virtual bool CheckForNewEvents(vector<InvalidationDetails*> &p_eventDetails_out, const Point3D &p_cameraPosition)
			{
				vector<InvalidationDetails> allUpdates; 

				//iterate over all the point lights
				for(iter it = m_allLight.begin(); it != m_allLight.end(); ++it)
				{
					Light* currentLight = (Light*)it->second;
					bool found = m_allUpdatedLights.find(currentLight->GetLightName()) == m_allUpdatedLights.end();

					float distance = Point3D::DistanceBetweenPoints(currentLight->GetLocation(), p_cameraPosition);

					//check if the current light is a point light and is near the range distance
					if(currentLight->GetLightType() == LIGHT_TYPE::POINT_LIGHT && 
						 found && (distance <= currentLight->GetRange()))
					{
						LightChangeDescription current;
						//switch light
						currentLight->SwitchStatus();
						p_eventDetails_out.push_back(new LightChangeDescription(currentLight->GetIndex(), currentLight->GetStatus()));

						LightChangeDescription* tester = (LightChangeDescription*)p_eventDetails_out[0];

						//add to updated lights 
						m_allUpdatedLights[currentLight->GetLightName()] = currentLight->GetLightName();
						cout << "A new point light has been switched " << currentLight->GetLightName() << endl;
					}
				}

				stack<hIter> toBeRemoved;

				for (hIter it = m_allUpdatedLights.begin(); it != m_allUpdatedLights.end(); ++it)
				{
					string lightName = it->first;
					Light* currentLight = m_allLight[lightName];
					
					if(Point3D::DistanceBetweenPoints(currentLight->GetLocation(), p_cameraPosition) > currentLight->GetRange())
					{
						hIter current = m_allUpdatedLights.find(lightName);
						toBeRemoved.push(current);
						//switch again 
						currentLight->SwitchLight();
						p_eventDetails_out.push_back(new LightChangeDescription(currentLight->GetIndex(), currentLight->GetStatus()));
						cout << "Light has been switched back on " << currentLight->GetIndex() << "\n";
					}
				}

				while(toBeRemoved.size() > 0)
				{
					hIter current = toBeRemoved.top();
					m_allUpdatedLights.erase(current);
					toBeRemoved.pop();
				}

					
				return p_eventDetails_out.size() > 0;
			}

			map<string, Light*>& GetLightMap()
			{
				return m_allLight;
			}

		};
	}
}
