#pragma once 


#include <string>


using std::string;
namespace Network
{
	namespace Peer
	{
		namespace Testing
		{

			enum InstructionType
			{
				SET,
				COMM,
				UPDATE,
				EXIT
			};

			class TestingInstruction
			{
			public:
				TestingInstruction(InstructionType p_instructionType, int p_peerIndex)
				{
					m_instructionType = p_instructionType;
					m_aimedPeer = p_peerIndex;
				}

				TestingInstruction(InstructionType p_instructionType, int p_peerIndex, string p_data)
				{
					m_instructionType = p_instructionType;
					m_aimedPeer = p_peerIndex;
					m_data = p_data;
				}

				TestingInstruction(InstructionType p_instructionType, int p_peerIndex, int p_secondaryPeer)
				{
					m_instructionType = p_instructionType;
					m_aimedPeer = p_peerIndex;
					m_secondaryPeer = p_secondaryPeer;
				}

				string ToString()
				{
					switch (m_instructionType)
					{
					case SET:
						return "Setting " + to_string(m_aimedPeer) + " " + m_data;

					case COMM:
						return "Communication between " + to_string(m_aimedPeer) + " " + to_string(m_secondaryPeer);

					case UPDATE:
						return "Update requested " + to_string(m_aimedPeer);
					}
				}

				InstructionType m_instructionType;
				
				int m_secondaryPeer;
				string m_data;
				int m_aimedPeer;
			};
		}
	}
}