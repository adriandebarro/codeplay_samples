#pragma once 

#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include <ObservableEvents/ObservableEvent.h>
#include <VectorTimer/VectorTimer.h>


#include "../../Integrators/RadianceDetails/RadiancePoint.h"
#include "../../AccelerationStructures/CachingStructure/LockingOctree.h"
#include "../../AccelerationStructures/CachingStructure/Octree.h"
#include "../../CoreClasses/UtilClasses/Common.h"
#include "../../CoreClasses/UtilClasses/ConvertionHelper.h"

using std::string;
using namespace Network::ObservableEvents;
using namespace Engine::Common;
using Network::VectorTimer;
using namespace Engine::Communication::Serialization;
namespace Engine
{
	namespace Networking
	{
		namespace ObservableEvents 
		{
			const int AMOUNT_PARTS_INSERT_EVENT = 4;

			template<typename T = RadiancePoint, typename IC = Integrator, typename AS = LockingOctree>
			class InsertEvent :	public ObservableEvent<T, IC, AS>
			{
			public:
				using ObservableEvent<T, IC,AS>::m_uuid;
				using ObservableEvent<T, IC,AS>::GetStringUpdate;
				using ObservableEvent<T, IC,AS>::m_timeStamp;
				using ObservableEvent<T, IC,AS>::m_type;
				using ObservableEvent<T, IC,AS>::m_sourceOfOe;
				using ObservableEvent<T, IC,AS>::SetEpoch;
				using ObservableEvent<T, IC,AS>::m_updateRepresented;
				using ObservableEvent<T, IC,AS>::preFunction;
				using ObservableEvent<T, IC,AS>::GetNextUuid;
				using ObservableEvent<T, IC,AS>::ToSerialize;
				using ObservableEvent<T, IC,AS>::FromSerialize;
				using ObservableEvent<T, IC,AS>::SetLockingStructure;
				using ObservableEvent<T, IC, AS>::ByteArrayToUuid;
				using ObservableEvent<T, IC, AS>::SerializeUuid;
				
				const int INSERT_TYPE_NUM = 0;

				//#ifdef MULTI_THREADED
					//static 
				//#else
					//Octree *m_cachingStructure;
				//#endif

				InsertEvent() : ObservableEvent()
				{}

				InsertEvent(vector<RadiancePoint*> p_inputLocalEvents, AS* p_octree, OBSERVABLE_PRODUCED_TYPE p_provenenance = INTERNAL_UPDATE) 
					: ObservableEvent(p_inputLocalEvents, 0, p_provenenance)
				{
					m_type = OBSERVABLE_EVENT_TYPE::INSERT_EVENT;
					m_cachingStructure = p_octree;
				}

				InsertEvent(int p_position, string p_inputVector) 
					: ObservableEvent(p_position, p_inputVector)
				{
					m_type = OBSERVABLE_EVENT_TYPE::INSERT_EVENT;
				}


				virtual void Commit(IC* p_sturcture)
				{
					for (int index = 0; index < 100; index++)
					{
						#ifdef WAIT_FREE
							p_sturcture->IncrementNetworkRadiancePointCount();
							m_cachingStructure->PoissonInsertion(m_updateRepresented[index]);
							
						#else
							m_cachingStructure->InsertPoint(*m_updateRepresented[index]);
							p_sturcture->IncrementNetworkRadiancePointCount();
						#endif
					} 
				}

				virtual string ToString()
				{
					std::stringstream ss;
					ss << m_type;
					ss << "_";
					ss << GetStringUpdate();
					ss << "_";
					ss << m_uuid;
					ss << "_";
					ss << m_timeStamp->ToString();
					return ss.str();
				}

				virtual string GetStringUpdate()
				{
					stringstream ss;

					for (int index = 0; index < m_updateRepresented.size(); index++)
					{
						ss << m_updateRepresented[index]->ToString();
						ss << ";";
					}

					return ss.str();
				}

				virtual void ToSerialize(std::vector<unsigned char>& p_list)
				{
					//set type
					ConvertionClass::ConvertInt(m_type,p_list);
					//set update
					ConvertionClass::ConvertInt(m_updateRepresented.size(), p_list);
					for (int index = 0, temp = m_updateRepresented.size(); index < temp; index++)
					{
						m_updateRepresented[index]->SerializePoint(p_list);
					}
					//set uuid
					SerializeUuid(p_list);
					// set timestamp
					m_timeStamp->Serialize(p_list);
				}

				virtual void ToSerialize(std::vector<unsigned char>& p_list, int p_startingPoint)
				{
					int startingPoint = p_startingPoint;
					//set type
					ConvertionClass::ConvertInt(m_type, startingPoint,p_list);
					//set update
					ConvertionClass::ConvertInt(m_updateRepresented.size(), startingPoint,p_list);
					for (int index = 0, temp = m_updateRepresented.size(); index < temp; index++)
					{
						m_updateRepresented[index]->SerializePoint(p_list);
					}
					//set uuid
					SerializeUuid(p_list);
					// set timestamp
					m_timeStamp->Serialize(p_list);
				}

				virtual void FromSerialize(vector<unsigned char>& p_charArray)
				{

					int startingPoint = 0;
					ConvertionClass::ByteArrayToInt(m_type, p_charArray, startingPoint, 4);

					int amountSamples = 0;

					ConvertionClass::ByteArrayToInt(amountSamples, p_charArray, startingPoint, 4);

					//starting at seventh element

					for (int radianceIndex = 0; radianceIndex < amountSamples; radianceIndex++)
					{
						T* tempT = new T(1);
						vector<unsigned char>::iterator start = p_charArray.begin() + startingPoint;
						vector<unsigned char> tempData(start, start + 40);
						tempT->FromSerialize(tempData);
						startingPoint += 40;
					}
					//uuid
					ByteArrayToEventUuid(p_charArray, startingPoint);


					m_timeStamp->FromByteArray(p_charArray, startingPoint);
				}

				virtual void FromSerialize(vector<unsigned char>& p_charArray, int &p_startingPoint)
				{
					ConvertionClass::ByteArrayToInt(m_type, p_charArray, p_startingPoint, 4);

					int amountSamples = 0;
					
					ConvertionClass::ByteArrayToInt(amountSamples, p_charArray, p_startingPoint, 4);

					for (int radianceIndex = 0; radianceIndex < 100; radianceIndex++)
					{
						T* tempT = new T(1);
						tempT->FromSerialize(p_charArray, p_startingPoint);

						
						m_updateRepresented.push_back(tempT);
						
					}
					//uuid
					ByteArrayToEventUuid(p_charArray, p_startingPoint);

					m_timeStamp->FromByteArray(p_charArray, p_startingPoint);
					
				}

				void ByteArrayToEventUuid(vector<unsigned char>& p_outputArray, int &p_startingPoint)
				{
					ByteArrayToUuid(p_outputArray, p_startingPoint, m_uuid);
				}
	

				virtual void FromString(string p_input)
				{
					vector<string> stringData;
					Common::Tokenizer::ParseStringSentence(p_input, "_", stringData);

					if (stringData.size() == AMOUNT_PARTS_INSERT_EVENT)
					{
						//if a test case
						if (stringData[1] == "-")
						{
							m_type = stoi(stringData[0]);
							m_timeStamp = new FixedVectorTimer(stringData[3]);
						}
						else
						{
							m_type = stoi(stringData[0]);
							InsertSerializedUpdates(stringData[1]);
							//m_uuid = boost::lexical_cast<uuid>(stringData[2]);
							m_timeStamp = new FixedVectorTimer(stringData[3]);
						}

						
					}	
				}

				void InsertSerializedUpdates(string p_input)
				{
					vector<string> radiancePoints;
					Tokenizer::ParseStringSentence(p_input, ";", radiancePoints);

					if (radiancePoints.size() > 0)
					{
						for (int index = 0; index < radiancePoints.size(); index++)
						{
							RadiancePoint* current = new RadiancePoint(radiancePoints[index]);
							m_updateRepresented.push_back(current);
						}
					}
				}

				inline bool operator < (InsertEvent& p_other)
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

			private:
				AS* m_cachingStructure;
			};
			
		}
	}
}//Engine