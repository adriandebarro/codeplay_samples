#pragma once 

#include <vector>
#include <iostream>

#include "../../CoreClasses/Point/Point3D/Point3D.h"

//DONT USE

namespace Engine
{
	namespace Networking
	{
		namespace ContextRepresentation
		{
			template<typename T>
			class StateRepresentation
			{
			public:
				StateRepresentation(T p_representation)
				{
					m_representation = p_representation;
				}

				StateRepresentation()
				{}

				void Serialize(std::vector<unsigned char>&)
				{
					std::cout << "Type not handled" << std::endl;
					std::throw;
				}
				
				void FromSerialize(std::vector<unsigned char>&, int& p_startingPoint)
				{
					std::cout << "Type not handled" << std::endl;
					std::throw;
				}

				void SetState(T p_representation)
				{
					m_representation = p_representation;
				}

			private:
				T m_representation;
			};

			template<>
			void StateRepresentation<char>::Serialize(std::vector<unsigned char>& p_update)
			{
				p_update.push_back((unsigned char)m_representation);
			}

			template<>
			void StateRepresentation<char>::FromSerialize(std::vector<unsigned char>& p_update, int& p_startingPoint)
			{
				m_representation = *(char*)&p_update[p_startingPoint];
				p_startingPoint += sizeof(char);
			}

			template<>
			void StateRepresentation<Engine::Point3D>::Serialize(std::vector<unsigned char>& p_update)
			{
				m_representation.SerializePoint(p_update);
			}

			template<>
			void StateRepresentation<Engine::Point3D>::FromSerialize(std::vector<unsigned char>& p_update, int& p_startingPoint)
			{
				m_representation = *(Engine::Point3D*)&p_update[p_startingPoint];
				p_startingPoint += sizeof(Engine::Point3D);
			}

			template<>
			void StateRepresentation<int>::Serialize(std::vector<unsigned char>& p_update)
			{
				for (int index = 0; index < 4; index++)
				{
					p_update.push_back((unsigned char)((char *)&m_representation)[index]);
				}
			}

			template<>
			void StateRepresentation<int>::FromSerialize(std::vector<unsigned char>& p_update, int& p_startingPoint)
			{
				m_representation = *(int*)&p_update[p_startingPoint];
				p_startingPoint += sizeof(int);
			}
		}
	}
}