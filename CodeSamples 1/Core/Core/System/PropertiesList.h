#pragma once 

#include <map>
#include <iostream>
#include <string>

namespace Core 
{
	namespace System
	{
		template<typename T1, typename T2>
		class PropertiesList
		{
		private:
			/// map of property names and the corresponding details
			std::map<T1, T2> m_map;
			/// properties list 
			std::string m_propertiesTitle;  
			
			virtual bool Populate(int argc, char* argv)
			{
				std::cin.get();
				return false;
			}

		public:
			PropertiesList(std::string p_propertiesTitle):
				m_propertiesTitle(p_propertiesTitle)
			{}

			PropertiesList(int argc, char *argv, std::string p_propertiesList):
				m_propertiesTitle(p_propertiesList)
			{

				Populate(argc, argv);
			}

			std::string GetProperty(T1 p_propertyName)
			{
				if (m_map.find(p_propeertyName) != m_map.end())
					return m_map[p_propertyName];
				else
					return "";
			}

			bool AddProperty(T1 p_propertyName, T2 p_propertyValue)
			{
				if (m_map.find(p_propertyName) == m_map.end())
				{
					m_map[p_propertyName] = p_propertyValue;
					return false;
				}
				else
					return false;
			}
		};
	}

}
