#pragma once 

#include <atomic>
#include <algorithm>
#include <vector>

#include <ObservableEvents/ObservableEvent.h>
#include <ObservableEvents/ObservableEventsList/ObservaleEventsList.h>
#include <VectorTimer/VectorTimer.h>
#include <VectorTimer/FixedVectorTimer.h>
#include <Peer\Neighbors\Neighbour.h>

#include "Integrators/IrradianceCache.h"
#include "../../Integrators/RadianceDetails/RadiancePoint.h"
#include "../../Networking/ObservableEvents/InsertEvent.h"
#include "../../Networking/ObservableEvents/InvalidationEvent.h"
#include "../../Networking/ObservableEvents/PartialInvalidationEvent.h"
#include "../../Networking/ObservableEvents/SceneEvents/LightSceneEvent.h"
#include "../../CoreClasses/UtilClasses/ConvertionHelper.h"
#include "../../AccelerationStructures/CachingStructure/LockingOctree.h"

#include "../../CoreClasses/AssetsManager/SceneManager.h"
#include "../../Networking/ObservableEvents/SceneEvents/SceneInvalidationDetails.h"

#include "../../CoreClasses/Logger/PostInfoLogger.h"
#include "../../CoreClasses/Logger/CommunicationLogger.h"

#include "../../CoreClasses/BoundingShapes/BoundingBoxAnalyzer.h"

#include "../ObservableEvents/PartialInvalidationEvent.h"

using namespace Network::ObservableEvents;
using Network::VectorTimer;
using namespace Engine::Communication::Serialization;
using Engine::LockingOctree;
using std::vector;
using Engine::Networking::ObservableEvents::SceneEvents::LightSceneEvent;
using Engine::ResourceManager::SceneManager;
using Engine::Networking::ObservableEvents::SceneEvents::SCENE_EVENT_TYPE;
using Engine::Logs::CommunicationLogs::CommunicationLogger;
using Engine::BoundingShapes::BoundingBoxAnalyzer;
using namespace Engine::Networking::ObservableEvents;
namespace Engine
{
	namespace Networking
	{
		namespace ObservableEvents
		{
			template<typename T, typename IC, typename OCT>
			class ICObservableEventsList : public ObservableEventsList<T, IC, OCT>
			{
			public:
				
				using ObservableEventsList<T, IC, OCT>::m_observableEvents;
				using ObservableEventsList<T, IC, OCT>::find;
				using ObservableEventsList<T, IC, OCT>::SerializeAllUuids;
				using ObservableEventsList<T, IC, OCT>::GetNeededOE;
				using ObservableEventsList<T, IC, OCT>::FindMissingOE;
				using ObservableEventsList<T, IC, OCT>::SerializeUuid;
				using ObservableEventsList<T, IC, OCT>::ByteArrayToUuid;
				using ObservableEventsList<T, IC, OCT>::ParseByteArrayToUuids;
				using ObservableEventsList<T, IC, OCT>::DataToMerge;
				using ObservableEventsList<T, IC, OCT>::MergeEvents;
				using ObservableEventsList<T, IC, OCT>::PushQueuedEvents;
				using ObservableEventsList<T, IC, OCT>::AddInvalidationEvents;
				using ObservableEventsList<T, IC, OCT>::AddEventToList;
				using ObservableEventsList<T, IC, OCT>::m_lowEpochCounter;
				using ObservableEventsList<T, IC, OCT>::m_highEpochCounter;
				

				ICObservableEventsList() :ObservableEventsList(0,0)
				{}

				ICObservableEventsList(int p_peerIndex,int p_amountPeers,  IrradianceCache* p_IC, SceneManager* p_sceneManager, string p_hostUrl,  bool p_queuedEvents = false) : ObservableEventsList(p_peerIndex, p_amountPeers)
				{
					m_queuedEvents = p_queuedEvents;
					m_amountPeers = p_amountPeers;
					m_IC = p_IC;
					m_localIC = nullptr;
					m_twoLevelCache = false;
					m_sceneManager = p_sceneManager;
					m_peerIndex = p_peerIndex;
					m_hostUrl = p_hostUrl;
					m_communicationLogger = new Engine::Logs::CommunicationLogs::CommunicationLogger();
					m_boundingBoxAnalyzer = nullptr;
					std::atomic_store(&m_committedInvalidation, false);
				}

				ICObservableEventsList(int p_peerIndex, int p_amountPeers, IrradianceCache* p_globalIC, IrradianceCache* p_localIC, SceneManager* p_sceneManager, bool p_queuedEvents = false, bool p_logToServer = false, string p_hostPath = "") 
					: ObservableEventsList(
						p_peerIndex, 
						p_amountPeers)
				{
					m_queuedEvents = p_queuedEvents;
					m_amountPeers = p_amountPeers;
					m_IC = p_globalIC;
					m_localIC = p_localIC;
					m_twoLevelCache = true;
					m_sceneManager = p_sceneManager;
					m_peerIndex = p_peerIndex;
					m_logToServer = p_logToServer;
					m_hostUrl = p_hostPath;
					m_communicationLogger = new Engine::Logs::CommunicationLogs::CommunicationLogger();
					m_boundingBoxAnalyzer = nullptr;
					std::atomic_store(&m_committedInvalidation, false);
				}

				void SetBoundingBoxAnalyser(BoundingBoxAnalyzer* p_boudningBoxAnalayser)
				{
					m_boundingBoxAnalyzer = p_boudningBoxAnalayser;
				}

			

				bool CheckInvalidationExist(int& p_startingPosition_out, 
					vector<ObservableEvent<T, IC, OCT>*> &p_observableList, bool &p_multipleInvalidations_out, int &p_finalPositon_out)
				{
					bool foundInvalidation = false;
					p_multipleInvalidations_out = false;
					int index = m_observableEvents.size() - 1;

					for (int index = m_observableEvents.size() - 1; index >= 0; index--)
					{
						//once sorted the observable eventsw ill try and find an observable event of type invalidation which is yet stil lto get committed
						if ((p_observableList[index]->GetType() == OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT ) && p_observableList[index]->GetCommitted() == false)
						{

							if (foundInvalidation == false)
							{
								p_finalPositon_out= index;
								foundInvalidation = true;
							}
							else
							{
								p_multipleInvalidations_out = true;
								p_startingPosition_out = index;
							}
						}
						else if(p_observableList[index]->GetCommitted() == true)
						{
							p_finalPositon_out = index + 1;
							return foundInvalidation;
						}
					}

					return foundInvalidation;
				}


				bool CheckInvalidationExistsInList(int& p_position)
				{
					int temp = 0;
					bool tempBool = false;
					return CheckInvalidationExist(p_position, m_observableEvents, tempBool,temp);
				}


				int GetLatestInvalidationInQueue(bool &p_multipleInvalidations_out, int &p_finalInvalidation_out)
				{
					int position = 0;
					CheckInvalidationExist(position, m_observableEvents, p_multipleInvalidations_out, p_finalInvalidation_out);
					return position;
				}

				bool CheckForPendingUpdates()
				{
					return m_eventsQueue.size() > 0;
				}

				virtual void AddInvalidationEvents(vector<InvalidationDetails*>& p_invalidationEventsDetails, int p_invalidationType, int p_currentPosition)
				{
					//use mutex here 
					//foreach p_invalidation
					ObservableEvent<T, IC, OCT>* currentInvalidation = nullptr;
					cout << "Adding  invalidation events" << endl;
					std::lock_guard<std::mutex> m(m_commitingMutex);
					for (int index = 0; index < p_invalidationEventsDetails.size(); index++)
					{
						InvalidationDetails* currentDetails = p_invalidationEventsDetails[index];

						if(currentDetails->type == INVALIDATION_TYPE::INVALIDATION_LIGHT)
						{
							ResourceManager::LightChangeDescription* currentLightDetails = (ResourceManager::LightChangeDescription*)currentDetails;
							LightSceneEvent<OCT>* currentEvent = new LightSceneEvent<OCT>(currentLightDetails);
							

							if (p_invalidationType)
								currentInvalidation = new InvalidationEvent<T, IC, OCT>(m_IC->GetOctree(), currentEvent->GetEventUuid());
							else
								currentInvalidation = new PartialInvalidationEvent<T, IC, OCT>(m_IC->GetOctree(), currentEvent->GetEventUuid(), m_boundingBoxAnalyzer, p_currentPosition);

							AddEventToList(currentInvalidation);
							AddEventToList(currentEvent);
							std::atomic_store(&m_committedInvalidation, true);
						}
					}
				}


				virtual void AddEventToList(ObservableEvent<T, IC, OCT>* p_currentEvent)
				{
					m_commitingMutex.lock();
					p_currentEvent->SetEpoch(m_lowEpochCounter);
					m_lowEpochCounter++;
					p_currentEvent->SetTimeStamp(m_peerLogicalTime);
					
					if (!m_queuedEvents)
						m_observableEvents.push_back(p_currentEvent);
					else
						m_eventsQueue.push_back(p_currentEvent);
					
					m_containedObservableEvents[p_currentEvent->GetUuid()] = 0;

					IncrementTimer();
					m_commitingMutex.unlock();
				}
				
				virtual void AsyncMergeEvents(vector<unsigned char> p_data, int p_skip = 0)
				{

				}
				
				//run new megre on for testing 
				virtual void MergeEventsParallel(vector<unsigned char> p_data, int p_skip = 0)
				{
					vector<string> events;
					vector<ObservableEvent<T, IC, OCT>*> updates;

					if (p_skip == 0)
						updates = DataToMerge(p_data, 0);
					else
						updates = DataToMerge(p_data, p_skip);

					System::SystemUtil::WriteHighligthed(System::ALERT_COLOR, "Recevied this amount of new events------------" + std::to_string(updates.size()));

					vector<ObservableEvent<T, IC, OCT>*> toAdd;
					vector<ObservableEvent<T, IC, OCT>*> toDelete;

					for (int index = 0; index < updates.size(); index++)
					{
						ObservableEvent<T, IC, OCT>* currentEvent = updates[index];

						if (!find(currentEvent->GetUuid()) && currentEvent->GetObservableEventStatus())
						{
							m_highEpochCounter++;
							m_observableEvents.push_back(currentEvent);
							m_containedObservableEvents[currentEvent->GetUuid()] = 0;
							toAdd.push_back(currentEvent);
						}
						else
							toDelete.push_back(currentEvent);
					}

					std::sort(m_observableEvents.begin(), m_observableEvents.end(), ObservableEvent<T, IC, OCT>::preFunction);

					int indexInvalidation = 0;
					//check this method
					bool multipleInvalidations = false;
					int finalInvalidations = 0;

					if (CheckInvalidationExist(indexInvalidation, m_observableEvents, multipleInvalidations, finalInvalidations))
					{
						cout << "received invalidation event" << endl;
						//need to check if updates will be inputted after the frame has finsihed executing 
						if (m_queuedEvents)
						{
							for (int index = indexInvalidation; index < m_observableEvents.size(); index++)
							{
								ObservableEvent<T, IC, OCT>* current = m_observableEvents[index];
								m_eventsQueue.push_back(current);
							}
						}
						else
						{
							ExecuteInvalidation(indexInvalidation, multipleInvalidations, finalInvalidations);
						}
					}

					else
					{
						//if the mode is to queue events and the queue is not empty push to queue
						if (m_queuedEvents && m_eventsQueue.size() > 0)
						{
							for (int index = 0; index < toAdd.size(); index++)
							{
								m_eventsQueue.push_back(toAdd[index]);
							}
						}
						else
						{
							//commit all the events which have not been committed to the tree 
							for (int index = 0; index < toAdd.size(); index++)
							{
								ObservableEvent<T, IC, OCT>* current = toAdd[index];
								current->Commit(m_IC);
								current->SetCommitted(true);
							}
						}
					}
				}


				virtual void MergeEvents(vector<unsigned char>& p_data, int p_skip = 0)
				{
					m_commitingMutex.lock();
					vector<string> events;
					vector<ObservableEvent<T, IC, OCT>*> updates;

					if (p_skip == 0)
						updates = DataToMerge(p_data, 0);
					else
						updates = DataToMerge(p_data, p_skip);

					System::SystemUtil::WriteHighligthed(System::ALERT_COLOR, "Recevied this amount of new events------------" + std::to_string(updates.size()));

					vector<ObservableEvent<T, IC, OCT>*> toAdd;
					vector<ObservableEvent<T, IC, OCT>*> toDelete;

					for (int index = 0; index < updates.size(); index++)
					{
						ObservableEvent<T, IC, OCT>* currentEvent = updates[index];

						if (!find(currentEvent->GetUuid()) && currentEvent->GetObservableEventStatus())
						{
							m_highEpochCounter++;
							m_observableEvents.push_back(currentEvent);
							m_containedObservableEvents[currentEvent->GetUuid()] = 0;
							toAdd.push_back(currentEvent);
						}
						else
							toDelete.push_back(currentEvent);
					}

					std::sort(m_observableEvents.begin(), m_observableEvents.end(), ObservableEvent<T, IC, OCT>::preFunction);

					bool multipleInvalidations = false;
					int startingIndex = 0;
					int firstInvalidationPosition = 0;
					int finalPosition = 0;
					bool invalidationExists = CheckInvalidationExist(startingIndex, m_observableEvents, multipleInvalidations, finalPosition);

					//commit any invalidations
					bool onlyInvalidationCommited = false;
					for (int index = finalPosition; index < m_observableEvents.size(); index++)
					{
						ObservableEvent<T, IC, OCT>* current = m_observableEvents[index];
						
						//commit any further insertions
						if (current->GetType() == OBSERVABLE_EVENT_TYPE::INSERT_EVENT)
						{
							current->Commit(m_IC);
							current->SetCommitted(true);
							if (!onlyInvalidationCommited)
								onlyInvalidationCommited = false;
						}
						//execute for the partial invalidation only if not only an invalidation occured beforehand
						else if (current->GetType() == OBSERVABLE_EVENT_TYPE::PARTIAL_INVALIDATION)
						{
							current->SetCommitted(true);
							if (!onlyInvalidationCommited)
							{
								ObservableEvent<T, IC, OCT>* sceneEvent = GetObservableEvent(((PartialInvalidationEvent<T, IC, OCT>*)current)->GetSceneDetailsUuid());
								SceneEvent<LightManager<Light>, LightChangeDescription, OCT>* sceneDetails = (SceneEvent<LightManager<Light>, LightChangeDescription, OCT>*) sceneEvent;
								//need type of scene event
								if (sceneDetails->GetSubType() == SceneEvents::LightEvent)
								{
									LightSceneEvent<OCT>* currentEvent = (LightSceneEvent<OCT>*)sceneEvent;

									//if from network fix light
									if (currentEvent->GetSourceOfOE() == 1)
									{
										Light* currentLight = m_sceneManager->GetLightManager()->GetItemById(currentEvent->GetLightIndex());

										if (currentLight != nullptr)
										{
											currentLight->SetLightStatus(currentEvent->GetLightStatus());
										}
									}
								}

								current->Commit(m_IC);
								if (m_twoLevelCache)
									current->Commit(m_localIC);
							}
						}
						else if (current->GetType() == OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT)
						{
							InvalidationEvent<T, IC, OCT>* tempInvalidator = (InvalidationEvent<T, IC, OCT>*) current;
							//get the scene event and add updates

							ObservableEvent<T, IC, OCT>* sceneEvent = GetObservableEvent(tempInvalidator->GetSceneDetailsUuid());
							SceneEvent<LightManager<Light>, LightChangeDescription, OCT>* sceneDetails = (SceneEvent<LightManager<Light>, LightChangeDescription, OCT>*) sceneEvent;
							//need type of scene event
							if (sceneDetails->GetSubType() == SceneEvents::LightEvent)
							{
								LightSceneEvent<OCT>* currentEvent = (LightSceneEvent<OCT>*)sceneEvent;

								//if from network fix light
								if (currentEvent->GetSourceOfOE() == 1)
								{
									Light* currentLight = m_sceneManager->GetLightManager()->GetItemById(currentEvent->GetLightIndex());

									if (currentLight != nullptr)
									{
										currentLight->SetLightStatus(currentEvent->GetLightStatus());
									}
								}
							}

							tempInvalidator->Commit(m_IC);
							if (m_twoLevelCache)
								tempInvalidator->Commit(m_localIC);

							onlyInvalidationCommited = true;
						}
					}
					m_commitingMutex.unlock();
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Data to merge. Usually it will alays contain  a vectortimer beforehand. 
				/// 			would be prudent to take care of it as a starting point form over here
				/// </summary>
				///
				/// <remarks>	Adrian, 29/10/2015. </remarks>
				///
				/// <param name="p_array">	The array. </param>
				///-------------------------------------------------------------------------------------------------
				virtual std::vector<ObservableEvent<T, IC, OCT>*> DataToMerge(vector<unsigned char>& p_array, int p_startingPoint = 0)
				{
					vector<ObservableEvent<T, IC, OCT>*> toOutput;
					int loopLength = 0;
					ConvertionClass::ByteArrayToInt(loopLength, p_array, p_startingPoint, 4);
					//|type of  obs event|Num Samples|Samples   |UUID		|Time Stamp
					//| 4 bytes		   | 4 bytes   |40 * 100  | 16 bytes| 20 bytes|
					int amountBytes = 4 + 4 + 40 * 100 + 16 + (m_amountPeers * 4 + 4);

					for (int index = 0; index < loopLength; index++)
					{
						int temp = p_startingPoint;
						int eventType = GetTypeofEvent(p_array, temp);
						ObservableEvent<T, IC, OCT>* current = nullptr;

						//create event type according to type of event
						if (eventType == OBSERVABLE_EVENT_TYPE::INSERT_EVENT)
							current = new InsertEvent<T, IC, OCT>();
						else if (eventType == OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT)
							current = new InvalidationEvent<T, IC, OCT>();
						else if (eventType == OBSERVABLE_EVENT_TYPE::PARTIAL_INVALIDATION)
							current = new PartialInvalidationEvent<T, IC, OCT>(m_boundingBoxAnalyzer);
						else if (eventType == OBSERVABLE_EVENT_TYPE::SCENE_EVENT)
						{
							//parse second int to get if liht or scene 
							int sceneEventType = GetTypeofEvent(p_array, 4 + temp);
							if (sceneEventType == SCENE_EVENT_TYPE::LightEvent)
								current = new LightSceneEvent<OCT>();
						}

						current->FromSerialize(p_array, p_startingPoint);
						current->SetLockingStructure(m_IC->GetOctree());
						current->SetSourceOfOe(EXTERNAL_UPDATE);
						toOutput.push_back(current);
					}

					return toOutput;
				}

				//-------------------------------------------------------------
				//Used for testing purposes 
				void AddInsertEvent(vector<RadiancePoint*> p_radiancePoints)
				{
					InsertEvent<T, IC, OCT>* currentEvent = new InsertEvent<T, IC, OCT >(p_radiancePoints, nullptr);
					AddEventToList(currentEvent);
				}

				void AddInvalidationEvent(LightChangeDescription* p_description)
				{
					LightSceneEvent<OCT>* currentEvent = new LightSceneEvent<OCT>(p_description);
					currentEvent->SetSourceOfOe(INTERNAL_UPDATE);
					InvalidationEvent<T, IC, OCT>* currentInvalidation = new InvalidationEvent<T, IC, OCT>(nullptr, currentEvent->GetEventUuid());
					currentInvalidation->SetSourceOfOe(INTERNAL_UPDATE);
					AddEventToList(currentInvalidation);
					AddEventToList(currentEvent);
				}

				//--------------------------------------------------------------


				int GetTypeofEvent(vector<unsigned char>& p_array, int p_startingPoint)
				{
					int startingPoint = p_startingPoint;
					int eventType = 0;
					ConvertionClass::ByteArrayToInt(eventType, p_array, startingPoint, 4);
					return eventType;
				}



				///-------------------------------------------------------------------------------------------------
				/// <summary>	Serialize all uuids. </summary>
				///
				/// <remarks>	Adrian, 29/10/2015. </remarks>
				///
				/// <param name="p_array">	[in,out] The array. </param>
				///-------------------------------------------------------------------------------------------------

				virtual void SerializeAllUuids(vector<unsigned char>& p_array)
				{
					//add amount of uuids
					int amountUuids = m_observableEvents.size();
					ConvertionClass::ConvertInt(amountUuids, p_array);
					//add uuids
					for (int index = 0; index < m_observableEvents.size(); index++)
					{
						SerializeUuid(p_array, m_observableEvents[index]->GetUuid());
					}
				}

				void GetAllUuids(vector<unsigned char>& p_array)
				{
					//add amount of uuids
					ConvertionClass::ConvertInt(m_observableEvents.size(), p_array);
					//add uuids
					for (int index = 0; index < m_observableEvents.size(); index++)
					{
						SerializeUuid(p_array, m_observableEvents[index]->GetUuid());
					}
				}

				virtual vector<uuid> ParseByteArrayToUuids(vector<unsigned char> p_ByteArray)
				{
					vector<uuid> allUuids;
					int startingPoint = 0;
					try
					{

						int amountUuids = 0;
						ConvertionClass::ByteArrayToInt(amountUuids, p_ByteArray, startingPoint, 4);
						
						for (int uuidIndex = 0; uuidIndex < amountUuids; uuidIndex++)
						{
							ByteArrayToUuid(p_ByteArray, startingPoint, allUuids);
						}
					}
					catch (exception ex)
					{
						cout << "Error occured at silenced bug " << p_ByteArray.size() << "---- current location " << startingPoint << endl;
					}

					return allUuids;
				}

				///-------------------------------------------------------------------------------------------------
				/// <summary>	Gets needed observable events. </summary>
				///
				/// <remarks>	Adrian, 29/10/2015. </remarks>
				///
				/// <param name="p_array">			[in,out] The array. </param>
				/// <param name="p_outputArray">	[in,out] Array of outputs. </param>
				///-------------------------------------------------------------------------------------------------
				virtual void GetNeededOE(vector<unsigned char>& p_array, vector<unsigned char>& p_outputArray, int p_senderType, int p_receiver)
				{
					vector<ObservableEvent<T, IC, OCT>*> sentUpdates;
					cout << "requested updates " << p_array.size() << endl;
					vector<uuid> allNeededOe = ParseByteArrayToUuids(p_array);
					cout << "resulting updates " << allNeededOe.size() << endl;
					vector<unsigned char> tempOutput;
					int checkedAmount = 0;
					int amount = allNeededOe.size();
					for (int index = 0, tempLimit = amount; index < tempLimit;  index++)
					{
						ObservableEvent<T, IC, OCT>* currentSample = GetObservableEvent(allNeededOe[index]);
						sentUpdates.push_back(currentSample);
						if (currentSample != nullptr)
						{
							//continue here 
							currentSample->ToSerialize(tempOutput);
							checkedAmount++;
						}
					}

					//need details of sender and receiver 
					if (p_senderType == 1 && sentUpdates.size() > 0)
					{
						cout << "logging data to be sent" << "\n";
						Engine::Logs::RESTapi::PostInfoLogger<T, IC, OCT>::PostExchangeDetails(m_hostUrl, m_peerIndex, p_receiver, sentUpdates);
					}
					else
					{
						//save to file
						m_communicationLogger->AddCommunicationDetails(p_receiver, sentUpdates.size());
					}
						

					ConvertionClass::ConvertInt(checkedAmount, p_outputArray);

					int index = 0;
					for (int index = 0; index < tempOutput.size(); index++)
					{
						p_outputArray.push_back(tempOutput[index]);
					}
				}

				virtual void FindMissingOE(vector<unsigned char>& p_array, vector<unsigned char>& p_outputArray)
				{
					try
					{
						vector<uuid> allNeededOe = ParseByteArrayToUuids(p_array);

						vector<unsigned char> tempOutput;
						int checkedAmount = 0;
						int amount = allNeededOe.size();
						for (int index = 0, tempLimit = amount; index < tempLimit; index++)
						{
							if (!find(allNeededOe[index]))
							{
								SerializeUuid(tempOutput, allNeededOe[index]);
								checkedAmount++;
							}
						}

						//cout << "amount of bytes in request : " << 4 + tempOutput.size() << endl;
						ConvertionClass::ConvertInt(checkedAmount, p_outputArray);

						int index = 0;
						for (int index = 0; index < tempOutput.size(); index++)
						{
							p_outputArray.push_back(tempOutput[index]);
						}
					}
					catch (exception ex)
					{
						cout << "Error occured while executing the FindingMissingOE" << endl;
					}
				}

				//non partial invalidations till now 
				virtual void ExecuteInvalidation()
				{
					m_commitingMutex.lock();
					
					std::sort(m_observableEvents.begin(), m_observableEvents.end(), ObservableEvent<T, IC, OCT>::preFunction);
					bool multipleInvalidations = false;
					int startingIndex = 0;
					int firstInvalidationPosition = 0;
					int finalPosition = 0;
					//int latestinvalidationEvents = GetLatestInvalidationInQueue(multipleInvalidations, finalPositon);
					bool invalidationExists = CheckInvalidationExist(startingIndex, m_observableEvents, multipleInvalidations, finalPosition);

					//commit any invalidations
					bool onlyInvalidationCommited = false;
					for (int index = finalPosition; index < m_observableEvents.size(); index++)
					{
						ObservableEvent<T, IC, OCT>* current = m_observableEvents[index];
						//commit any further insertions
						if(current->GetType() == OBSERVABLE_EVENT_TYPE::INSERT_EVENT)
						{
							current->Commit(m_IC);
							current->SetCommitted(true);
							if(!onlyInvalidationCommited)
								onlyInvalidationCommited = false;
						}
						//execute for the partial invalidation only if not only an invalidation occured beforehand
						else if (current->GetType() == OBSERVABLE_EVENT_TYPE::PARTIAL_INVALIDATION)
						{
							current->SetCommitted(true);
							if (!onlyInvalidationCommited)
							{
								ObservableEvent<T, IC, OCT>* sceneEvent = GetObservableEvent(((PartialInvalidationEvent<T, IC, OCT>*)current)->GetSceneDetailsUuid());
								SceneEvent<LightManager<Light>, LightChangeDescription, OCT>* sceneDetails = (SceneEvent<LightManager<Light>, LightChangeDescription, OCT>*) sceneEvent;
								//need type of scene event
								if (sceneDetails->GetSubType() == SceneEvents::LightEvent)
								{
									LightSceneEvent<OCT>* currentEvent = (LightSceneEvent<OCT>*)sceneEvent;

									//if from network fix light
									if (currentEvent->GetSourceOfOE() == 1)
									{
										Light* currentLight = m_sceneManager->GetLightManager()->GetItemById(currentEvent->GetLightIndex());

										if (currentLight != nullptr)
										{
											currentLight->SetLightStatus(currentEvent->GetLightStatus());
										}
									}
								}

								current->Commit(m_IC);
								if (m_twoLevelCache)
									current->Commit(m_localIC);
							}
						}
						else if(current->GetType() == OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT)
						{
							InvalidationEvent<T, IC, OCT>* tempInvalidator = (InvalidationEvent<T, IC, OCT>*) current;
							//get the scene event and add updates

							ObservableEvent<T, IC, OCT>* sceneEvent = GetObservableEvent(tempInvalidator->GetSceneDetailsUuid());
							SceneEvent<LightManager<Light>, LightChangeDescription, OCT>* sceneDetails = (SceneEvent<LightManager<Light>, LightChangeDescription, OCT>*) sceneEvent;
							//need type of scene event
							if (sceneDetails != nullptr && sceneDetails->GetSubType() == SceneEvents::LightEvent)
							{
								LightSceneEvent<OCT>* currentEvent = (LightSceneEvent<OCT>*)sceneEvent;

								//if from network fix light
								if (currentEvent->GetSourceOfOE() == 1)
								{
									Light* currentLight = m_sceneManager->GetLightManager()->GetItemById(currentEvent->GetLightIndex());

									if (currentLight != nullptr)
									{
										currentLight->SetLightStatus(currentEvent->GetLightStatus());
									}
								}
							}

							tempInvalidator->Commit(m_IC);
							if (m_twoLevelCache)
								tempInvalidator->Commit(m_localIC);

							onlyInvalidationCommited = true;

							//set the lights 
						}
					}
					
					m_commitingMutex.unlock();
				}


				void ExecuteInvalidation(int p_startingIndex, bool p_multipleInvalidations, int p_finalInvalidationPosition)
				{
					for (int index = p_startingIndex; index < m_observableEvents.size(); index++)
					{
						ObservableEvent<T, IC, OCT>* current = m_observableEvents[index];
						if (current->GetType() == OBSERVABLE_EVENT_TYPE::INVALIDATION_EVENT)
						{
							InvalidationEvent<T, IC, OCT>* tempInvalidator = (InvalidationEvent<T, IC, OCT>*) current;
							//get the scene event and add updates

							ObservableEvent<T, IC, OCT>* sceneEvent = GetObservableEvent(tempInvalidator->GetSceneDetailsUuid());
							SceneEvent<LightManager<Light>, LightChangeDescription, OCT>* sceneDetails = (SceneEvent<LightManager<Light>, LightChangeDescription, OCT>*) sceneEvent;
							//need type of scene event
							if (sceneDetails->GetSubType() == SCENE_EVENT_TYPE::LightEvent)
							{
								LightSceneEvent<OCT>* currentEvent = (LightSceneEvent<OCT>*)sceneEvent;
								
								//if from network fix light
								if (currentEvent->GetSourceOfOE() == EXTERNAL_UPDATE)
								{
									Light* currentLight = m_sceneManager->GetLightManager()->GetItemById(currentEvent->GetLightIndex());

									if (currentLight != nullptr)
									{
										currentLight->SetLightStatus(currentEvent->GetLightStatus());
									}
								}

							}

 							if (index == p_finalInvalidationPosition)
							{
								tempInvalidator->Commit(m_IC);
								if (m_twoLevelCache)
									tempInvalidator->Commit(m_localIC);
							}
							//set the lights 
						}
						else if (current->GetType() == OBSERVABLE_EVENT_TYPE::INSERT_EVENT && index > p_finalInvalidationPosition && !current->GetCommitted())
						{
							current->Commit(m_IC);
							current->SetCommitted(true);
						}
					}

					for (int index = p_startingIndex; index < m_observableEvents.size(); index++)
					{
						ObservableEvent<T, IC, OCT>* current = m_observableEvents[index];
						current->SetCommitted(true);
					}
				}

				void SetBoundingBoxAnalyzer(BoundingBoxAnalyzer* p_boundingBoxAnalyzer)
				{
					m_boundingBoxAnalyzer = p_boundingBoxAnalyzer;
				}

				virtual inline bool GetInvalidationStatus()
				{
					return std::atomic_load(&m_committedInvalidation);
				}

				virtual inline void SetInvalidationToFalse()
				{
					std::atomic_store(&m_committedInvalidation, false);
				}

				
				void PushQueuedEvents()
				{
					if (m_queuedEvents)
					{
						std::lock_guard<std::mutex> m(m_commitingMutex);
						ExecuteInvalidation();
						m_eventsQueue.clear();
					}
				}

			private:
				IrradianceCache* m_IC;
				IrradianceCache* m_localIC;
				bool m_twoLevelCache;
				int m_amountPeers;
				vector<ObservableEvent<T, IC, OCT>*> m_eventsQueue;
				bool m_queuedEvents = false;
				SceneManager* m_sceneManager;
				mutex m_commitingMutex;
				int m_peerIndex = 0;
				string m_hostUrl = "";
				bool m_logToServer = false;
				Engine::Logs::CommunicationLogs::CommunicationLogger* m_communicationLogger;
				BoundingBoxAnalyzer* m_boundingBoxAnalyzer;
				std::atomic<bool> m_committedInvalidation;
			};
		}//observable events
	}//Network
}//Engine
