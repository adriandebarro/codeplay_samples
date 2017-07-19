#pragma once 

#include <string>
#include <iostream>

#include "../AbstractPeer.h"
#include "TestingCommands\TestingInstruction.h"

using std::string;
using std::cin;
namespace Network
{
	namespace Peer
	{
		namespace Testing
		{
			///-------------------------------------------------------------------------------------------------
			/// <summary>	A Peer framework that allows to test the peer to peer network </summary>
			///
			/// <remarks>	Adrian, 31/08/2015. </remarks>
			///-------------------------------------------------------------------------------------------------
			template<typename T, typename S, typename U, typename AS>
			class TestingFramework : public AbstarctPeer<T,S, U, AS>
			{
			public:

				using AbstarctPeer<T, S, U, AS>::m_updatesList;
				using AbstarctPeer<T, S, U, AS>::CloseConnection;
				using AbstarctPeer<T, S, U, AS>::OpenConnection;
				using AbstarctPeer<T, S, U, AS>::neighbors;
				using AbstarctPeer<T, S, U, AS>::SendData;
				using AbstarctPeer<T, S, U, AS>::m_updateThread;
				using AbstarctPeer<T, S, U, AS>::BootUpSequence;
				using AbstarctPeer<T, S, U, AS>::ListenOnNetwork;
				using AbstarctPeer<T, S, U, AS>::peerData;
				using AbstarctPeer<T, S, U, AS>::bootStrapAddress;
				using AbstarctPeer<T, S, U, AS>::logicalTimer;
				using AbstarctPeer<T, S, U, AS>::GetStringFromBitStream;
				using AbstarctPeer<T, S, U, AS>::UpdateFunction;

				TestingInstruction* currentInstruction;

				TestingFramework(string p_ipAddress, int p_socketNumber, string p_nameAssigned, string p_path = "") : AbstarctPeer(p_ipAddress, p_socketNumber, p_nameAssigned)
				{
					cout << "Testing Server" << endl;
					m_path = p_path;
					//m_vectorTimer = new FixedVectorTimer<string>(p_socketNumber);
					currentInstruction = NULL;
					BootUpSequence();
				}

				
				virtual void BootUpSequence()
				{
					
					m_updateThread = std::thread(&TestingFramework<T,S>::ListenOnNetwork, this);

					if (m_path != "")
					{
						//need to load up script sequence 
					}
					else
					{
						//default script //tests the vector timer : Replicating paper content
						//set peers data
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::SET, 0, "a"));
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::SET, 1, "b"));
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::SET, 3, "c"));
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::UPDATE, 1));
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::COMM, 0, 1));
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::COMM, 3, 2));
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::COMM, 2, 1));
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::SET, 3, "d"));
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::COMM, 3, 2));
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::UPDATE, 0));
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::UPDATE, 1));
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::UPDATE, 2));
						m_testingInstructions.push_back(new TestingInstruction(InstructionType::UPDATE, 3));
					}
					UpdateFunction();
				}

				
				virtual void ListenOnNetwork()
				{
					//updates recieved should be minimal 
					Packet *packet;
					OpenConnection(bootStrapAddress, to_string(LISTENING_PORT));
					while (1)
					{
						for (packet = peerData->Receive(); packet; peerData->DeallocatePacket(packet), packet = peerData->Receive())
						{
							switch (packet->data[0])
							{

							case ID_CONNECTION_ATTEMPT_FAILED:
								printf("COULD NOT CONNECT TO PEER \n");
								break;

							case ID_REMOTE_DISCONNECTION_NOTIFICATION:
								printf("Another client has disconnected.\n");
								break;

							case ID_REMOTE_CONNECTION_LOST:
								printf("Another client has lost the connection.\n");
								break;

							case ID_REMOTE_NEW_INCOMING_CONNECTION:
								printf("Another client has connected.\n");
								break;

								//connection accepted by a peer
							case ID_CONNECTION_REQUEST_ACCEPTED:
							{
								printf("Our connection request has been accepted \n");
							}
								if (currentInstruction == NULL)
								{
									printf("Requesting peers\n");
									SendData(SystemAddress(bootStrapAddress.c_str(), LISTENING_PORT), TESTING_GET_PEERS, "---------");
								}
								else
								{
									switch (currentInstruction->m_instructionType)
									{
									case InstructionType::COMM:
										SendData(*(neighbors->GetNeighbour(currentInstruction->m_aimedPeer)->address), TESTING_COMM, (neighbors->GetNeighbour(currentInstruction->m_secondaryPeer)->address->ToString()));
										break;

									case InstructionType::SET:
 										SendData(*(neighbors->GetNeighbour(currentInstruction->m_aimedPeer)->address), TESTING_SET, currentInstruction->m_data);
										break;

									case InstructionType::UPDATE:
										SendData(*(neighbors->GetNeighbour(currentInstruction->m_aimedPeer)->address), TESTING_UPDATE, "");
										break;
									}
								}
								break;

							case ID_NEW_INCOMING_CONNECTION:
								printf("A connection is incoming %s .\n", packet->systemAddress.ToString());
								break;
							//update requestr by testign framework
							case SEND_UPDATES:
								{
									string data = GetStringFromBitStream(*packet);
									cout << "recieved update is:" + data << endl;
									CloseConnection(packet);
								}
								break;

							case TESTING_REPLY_STATUS:
								{
									UseUpdate(GetStringFromBitStream(*packet));
								}
								break;

							case TESTING_SEND_PEERS:
								{
									string data = GetStringFromBitStream(*packet);
									cout << "recieved data" << data << endl;
									neighbors->AddNewNeighbours(data);
									CloseConnection(packet);
								}
								break;

							default:
								printf("Message with identifier %i has arrived.  \n", packet->data[0]);
								break;
							}
						}
					}
				}

				
				virtual void UpdateFunction()
				{
					int counter = 0;
					string input = "";
					while (counter  < m_testingInstructions.size())
					{
						TestingInstruction* currentInstruction = m_testingInstructions[counter];
						//pause before each instruction is executed
						while (neighbors->GetAmountPeers() == 0)
						{
							this_thread::sleep_for(chrono::milliseconds(500));
						}
						system("pause");
						cout << "Update function current Instruction-> " << currentInstruction->ToString() << endl;
						cout << "Connecting to peer-> " << neighbors->GetNeighbour(currentInstruction->m_aimedPeer)->address->ToString() << endl;
						switch (currentInstruction->m_instructionType)
						{
						case InstructionType::COMM:
							OpenConnection(*(neighbors->GetNeighbour(currentInstruction->m_aimedPeer)->address));
							this->currentInstruction = currentInstruction;
							break;

						case InstructionType::SET:
							OpenConnection(*(neighbors->GetNeighbour(currentInstruction->m_aimedPeer)->address));
							this->currentInstruction = currentInstruction;
							break;

						case InstructionType::UPDATE:
							OpenConnection(*(neighbors->GetNeighbour(currentInstruction->m_aimedPeer)->address));
							this->currentInstruction = currentInstruction;
							break;
						}
						counter++;
					}
					system("pause");
				}

				
				virtual void AddUpdate(T* p_toAdd)
				{}

				void UseUpdate(string p_updateToProcess)
				{
					cout << p_updateToProcess << endl;
				}

				
				void ExecuteScript(string p_scriptPath)
				{
					while (true)
					{
						//will execute the isntructions 1 by 1 until all is ready
					}
				}

				~TestingFramework()
				{
					//delete instructions after all is ready
				}
			private:
				vector<TestingInstruction*> m_testingInstructions;
				string m_path;
			};
		}
	}
}