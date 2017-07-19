#pragma once 

#include <vector>

namespace Network
{
	namespace Context
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
			{}

			void FromSerialize(std::vector<unsigned char>&, int& p_startingPoint)
			{}

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

		void StateRepresentation<int>::Serialize(std::vector<unsigned char>& p_update)
		{
			for (int index = 0; index < 4; index++)
			{
				p_update.push_back((unsigned char)((char *)&m_representation)[index]);
			}
		}

		void StateRepresentation<int>::FromSerialize(std::vector<unsigned char>& p_update, int& p_startingPoint)
		{
			m_representation = *(int*)&p_update[p_startingPoint];
			p_startingPoint += sizeof(int);
		}

	}
}