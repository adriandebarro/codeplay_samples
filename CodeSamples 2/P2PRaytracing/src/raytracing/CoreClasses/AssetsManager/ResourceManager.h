#pragma once 

#include <vector>

#include "../Point/Point3D/Point3D.h"

#include "ObservableEvents/ObservableEventsList/ObservaleEventsList.h"

using std::vector;

namespace Engine
{
	namespace ResourceManager
     	{   
		template<typename T>
		class ResourceManager
		{
			private:
				vector<T*> m_allReosurces;
			public:
				virtual bool CheckForNewEvents(vector<Network::ObservableEvents::InvalidationDetails*>&, const Point3D&) = 0;
				virtual T* GetItemById(int) = 0;
		};
	}

}
