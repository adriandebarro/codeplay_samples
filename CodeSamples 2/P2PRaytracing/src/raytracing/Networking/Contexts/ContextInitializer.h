#pragma once 

#include <string>


#include <SuperPeer/SuperPeerManager.h>

#include "./ContextStrategy.h"
#include "../../CoreClasses/UtilClasses/Common.h"
#include "./ContextDataParser.h"

#include "../../System/SystemHelper.h"

#include <VectorTimer/FixedVectorTimer.h>

#include "../../CoreClasses/Logger/PostContextInfo.h"

using Network::SuperPeers::SuperPeerManager;
using Engine::Networking::Contexts::NetworkPropertiesParser;
using Engine::Contexts::Logger::ContextStatusLogger;
using std::string;
namespace Engine
{
	namespace Networking
	{
		namespace Contexts
		{
			class ContextInitializer
			{
			public:

				ContextInitializer(ContextStrategy* p_contextStrategy, SuperPeerManager* p_superPeerManager, bool* p_continue, int p_superPeerCheckerPeriod, FixedVectorTimer* p_vectorTimer, int* p_enabledContexts, int p_peerIndex, string p_hostUrl)
				{
					m_continueExecuting = p_continue;
					m_contextStrategy = p_contextStrategy;
					m_superPeerManager = p_superPeerManager;
					m_superPeerCheckerPeriod = p_superPeerCheckerPeriod;
					m_vectorTimer = p_vectorTimer;
					m_enabledContexts = p_enabledContexts;
					m_peerIndex = p_peerIndex;
					m_loggingHostUrl = p_hostUrl;
					ShortCutInitializer();
				}

				void FindIpAddress()
				{
					boost::asio::io_service io_service;
					tcp::resolver resolver(io_service);
					tcp::resolver::query query(boost::asio::ip::host_name(), "");
					tcp::resolver::iterator iter = resolver.resolve(query);
					tcp::resolver::iterator end; // End marker.
					while (iter != end)
					{
						stringstream ss;
						tcp::endpoint ep = *iter++;
						ss << ep;
						string ip = ss.str();
						const char* ipChar = ip.c_str();
						
						if (ipChar[0] == '1' && ipChar[1] == '0')
						{
							vector<string> localAddressParts;
							string splitter = ":";
							Common::Tokenizer::ParseStringSentence(ip, splitter, localAddressParts);
							m_ipAddress = inet_addr(localAddressParts[0].c_str());
							break;
						}
					}
				}

				void ShortCutInitializer()
				{
					//spawn initializer process
					m_contextInitializerThread = thread(boost::bind(&ContextInitializer::InitContexts, this));
				}


				static std::string GetStringIp(unsigned long p_ip)
				{
					struct in_addr addr;
					addr.S_un.S_addr = p_ip;
					char* dotIp = inet_ntoa(addr);

					return std::string(dotIp);
				}


				void InitContexts()
				{
					FindIpAddress();
					while (continueUpdating)
					{
						//lock
						int enabledContextIndex = 0;

						for (int contextIndex = 0; contextIndex <= MAX_ENUM_NUMBER_CONTEXTS; contextIndex++)
						{
							
							//this procedure won't create all of the updates all at once 
							if (contextIndex != 1 && !m_superPeerManager->FindContextWLock(contextIndex) && m_enabledContexts[enabledContextIndex] == 1)
							{

								if (*m_continueExecuting)
								{
									m_superPeerManager->GetMutex().lock();
									bool contextCreated = m_contextStrategy->SetContext((CONTEXT_ENUM)contextIndex, const_cast<char*>(GetStringIp((long)m_ipAddress).c_str()));
									if (contextCreated)
									{
										Engine::System::SystemUtil::PrintAlertMessage("Hosting a context " + std::to_string(contextIndex));
										SuperPeer* currentSuperPeer = new SuperPeer(m_ipAddress, contextIndex, m_vectorTimer);
										m_superPeerManager->AddSuperPeer(currentSuperPeer);
										m_superPeerManager->GetMutex().unlock();
										//continue here 
										ContextStatusLogger::PostContextsDetails(m_loggingHostUrl, m_peerIndex, contextIndex, 1, currentSuperPeer->GetStringUuid());
										break;
									}
									
								}
							}
							else if (contextIndex != 1 && m_enabledContexts[enabledContextIndex] == 1)
							{
								bool post = false;
								SuperPeer current;
								m_superPeerManager->GetMutex().lock();
								SuperPeer* currentContext = m_superPeerManager->GetSuperPeer(contextIndex);

								switch ((CONTEXT_ENUM)currentContext->m_contextNumber)
								{
								case CONTEXT_ENUM::locality:
								{	
									LOCALITY_CONTEXT* context = m_contextStrategy->m_localityContext;
									if (context != nullptr && !context->m_contextStatus.GetContextStatus())
									{
										m_contextStrategy->DecomissionContext(CONTEXT_ENUM::locality);
										current = SuperPeer(*m_superPeerManager->GetSuperPeer(currentContext->m_contextNumber));
										post = m_superPeerManager->RemoveSuperPeer(currentContext->m_contextNumber);
									}
									m_superPeerManager->GetMutex().unlock();
									if(post)
										ContextStatusLogger::PostContextsDetails(m_loggingHostUrl, m_peerIndex, contextIndex, 0, current.GetStringUuid());
									break;
								}

								case CONTEXT_ENUM::viewpoint:
								{
									VIEWPOINT_CONTEXT* context = m_contextStrategy->m_viewPointContext;
									if (context != nullptr && !context->m_contextStatus.m_contextOnline)
									{
										m_contextStrategy->DecomissionContext(CONTEXT_ENUM::viewpoint);
										current = SuperPeer(*m_superPeerManager->GetSuperPeer(currentContext->m_contextNumber));
										post = m_superPeerManager->RemoveSuperPeer(currentContext->m_contextNumber);
									}
									m_superPeerManager->GetMutex().unlock();
									if (post)
										ContextStatusLogger::PostContextsDetails(m_loggingHostUrl, m_peerIndex, contextIndex, 0, current.GetStringUuid());
									break;
								}

								case CONTEXT_ENUM::bounds:
								{
									BOUNDS_CONTEXT* context = m_contextStrategy->m_boundsContext;
									if (context != nullptr && !context->m_contextStatus.m_contextOnline)
									{
										m_contextStrategy->DecomissionContext(CONTEXT_ENUM::bounds);
										current = SuperPeer(*m_superPeerManager->GetSuperPeer(currentContext->m_contextNumber));
										post = m_superPeerManager->RemoveSuperPeer(currentContext->m_contextNumber);
									}
									m_superPeerManager->GetMutex().unlock();
									if (post)
										ContextStatusLogger::PostContextsDetails(m_loggingHostUrl, m_peerIndex, contextIndex, 0, current.GetStringUuid());
									break;
								}

								case CONTEXT_ENUM::frameRate:
								{
									FRAME_RATE_CONTEXT* context = m_contextStrategy->m_frameRateContext;
									if (context != nullptr && !context->m_contextStatus.m_contextOnline)
									{
										m_contextStrategy->DecomissionContext(CONTEXT_ENUM::frameRate);
										current = SuperPeer(*m_superPeerManager->GetSuperPeer(currentContext->m_contextNumber));
										post = m_superPeerManager->RemoveSuperPeer(currentContext->m_contextNumber);
									}
									m_superPeerManager->GetMutex().unlock();
									if (post)
										ContextStatusLogger::PostContextsDetails(m_loggingHostUrl, m_peerIndex, contextIndex,0, current.GetStringUuid());
									break;
								}

								case CONTEXT_ENUM::latency:
								{
									LATENCY_CONTEXT* context = m_contextStrategy->m_latencyContext;
									if (context != nullptr && !context->m_contextStatus.m_contextOnline)
									{
										m_contextStrategy->DecomissionContext(CONTEXT_ENUM::latency);
										current = SuperPeer(*m_superPeerManager->GetSuperPeer(currentContext->m_contextNumber));
										post = m_superPeerManager->RemoveSuperPeer(currentContext->m_contextNumber);
									}
									m_superPeerManager->GetMutex().unlock();
									if (post)
										ContextStatusLogger::PostContextsDetails(m_loggingHostUrl, m_peerIndex, contextIndex, 0, current.GetStringUuid());
									break;
								}

								default:
									m_superPeerManager->GetMutex().unlock();
									break;
								}

							}

							if(contextIndex != 1)
							{
								enabledContextIndex++;
							}
						}
						
						this_thread::sleep_for(std::chrono::seconds(m_superPeerCheckerPeriod));
					}
				}


				bool CheckForRunningContexts()
				{
					for (int contextIndex = 0; contextIndex <= MAX_ENUM_NUMBER_CONTEXTS; contextIndex++)
					{
						bool contextStatus = m_contextStrategy->CheckForRunningContexts(contextIndex);
						if (contextStatus)
							return true;
					}


					continueUpdating = false;
					return false;
				}

			private:
				ContextStrategy* m_contextStrategy;
				SuperPeerManager* m_superPeerManager;
				FixedVectorTimer* m_vectorTimer;
				bool* m_continueExecuting;
				int m_superPeerCheckerPeriod;
				thread m_contextInitializerThread;
				unsigned long m_ipAddress;
				bool continueUpdating = true;
				int* m_enabledContexts = nullptr;
				int m_peerIndex;
				string m_loggingHostUrl;
			};
		}
	}
}