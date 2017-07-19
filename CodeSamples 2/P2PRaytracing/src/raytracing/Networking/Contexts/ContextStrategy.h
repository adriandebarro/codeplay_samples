#pragma once 

#include <SuperPeer/Context/Context.h>
//#include <TCP/AsyncTcpServer.h>
#include <SuperPeer/Context/StateRepresentation.h>

#include "../../CoreClasses/Point/Point3D/Point3D.h"
#include "../../CoreClasses/Point/Point3D/Point3D.h"

#include "./ContextDataParser.h"

using Network::Context::Context;
using Network::Context::StateRepresentation; 
using Engine::Networking::Contexts::NetworkPropertiesParser;

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
namespace Engine
{
	namespace Networking
	{
		namespace Contexts
		{
			typedef Context<int> BANDWIDTH_CONTEXT;
			typedef Context<Point3D> LOCALITY_CONTEXT;
			typedef Context<Point3D> VIEWPOINT_CONTEXT;
			typedef Context<int> OCCLUSION_CONTEXT;
			typedef Context<int> BOUNDS_CONTEXT;
			typedef Context<float> FRAME_RATE_CONTEXT;
			typedef Context<int> LATENCY_CONTEXT;

			const int MAX_ENUM_NUMBER_CONTEXTS = 5;

			enum CONTEXT_ENUM
			{
				locality = 0,
				bandwidth = 1,
				viewpoint = 2,
				bounds = 3,
				frameRate = 4,
				latency = 5,
				//cpuSpeer = 6
			};

			int GetSizeOfUpdate(int p_contextType)
			{
				switch (p_contextType)
				{
				case 0:
					return sizeof(Point3D);
					break;

				case 2:
					return sizeof(Point3D);
					break;

				case 3:
					return sizeof(int);
					break;

				case 4:
					return sizeof(float);
					break;

				case 5:
					return sizeof(int);
					break;
				}
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	
			/// 	Contexts hosted by superpeers are handled by a thread executing this class
			/// 	Superpeers are hosted periodically on demand and decomissioned after a period of inactivety. 
			/// </summary>
			///
			/// <remarks>	Adrian, 19/07/2017. </remarks>
			///-------------------------------------------------------------------------------------------------

			class ContextStrategy
			{
			public:

				ContextStrategy(int p_broadcastPeriod)
				{
					m_broadcastPeriod = p_broadcastPeriod;
					m_localityContext = nullptr;
					m_bandwidthContext = nullptr;
					m_boundsContext = nullptr;
					m_viewPointContext = nullptr;
					m_latencyContext = nullptr;
					m_frameRateContext = nullptr;
				}


				ContextStrategy(NetworkPropertiesParser* p_contextProperties)
				{
					m_contextParams = p_contextProperties;
					m_localityContext = nullptr;
					m_bandwidthContext = nullptr;
					m_boundsContext = nullptr;
					m_viewPointContext = nullptr;
					m_latencyContext = nullptr;
					m_frameRateContext = nullptr;
				}

				bool SetContext(CONTEXT_ENUM p_contextNumber, char* p_ipAddress)
				{
					switch (p_contextNumber)
					{
					case CONTEXT_ENUM::locality:
						if (m_localityContext == nullptr)
						{
							m_localityContext = new LOCALITY_CONTEXT(CONTEXT_ENUM::locality, m_contextParams->m_localityBroadcastPeriod, p_ipAddress, m_contextParams->m_contextSubscriberLimit);
							return true;
						}
						return false;

					case CONTEXT_ENUM::viewpoint:
						if (m_viewPointContext == nullptr)
						{
							m_viewPointContext = new VIEWPOINT_CONTEXT(CONTEXT_ENUM::viewpoint, m_contextParams->m_viewpointBroadcastPeriod, p_ipAddress, m_contextParams->m_contextSubscriberLimit);
							return true;
						}
						return false;

					case CONTEXT_ENUM::bounds:
						if (m_boundsContext == nullptr)
						{
							m_boundsContext = new BOUNDS_CONTEXT(CONTEXT_ENUM::bounds, m_contextParams->m_boundsBroadcastPeriod, p_ipAddress, m_contextParams->m_contextSubscriberLimit);
							return true;
						}
						return false;

					case CONTEXT_ENUM::frameRate:
						if (m_frameRateContext == nullptr)
						{
							m_frameRateContext = new FRAME_RATE_CONTEXT(CONTEXT_ENUM::frameRate, m_contextParams->m_framerateBroadcastPeriod, p_ipAddress, m_contextParams->m_contextSubscriberLimit);
							return true;
						}
						return false;


					case CONTEXT_ENUM::latency:
						if (m_latencyContext == nullptr)
						{
							m_latencyContext = new LATENCY_CONTEXT(CONTEXT_ENUM::latency, m_contextParams->m_latencyBroadcastPeriod, p_ipAddress, m_contextParams->m_contextSubscriberLimit);
							return true;
						}
						return false;

					default:
						return false;
					}
				}

				void DecomissionContext(CONTEXT_ENUM p_contextNumber)
				{
					switch (p_contextNumber)
					{
					case CONTEXT_ENUM::locality:
						if (m_localityContext != nullptr)
						{
							m_localityContext->DecommiosionThreads();
							m_localityContext = nullptr;
						}
						break;

					case CONTEXT_ENUM::viewpoint:
						if (m_viewPointContext != nullptr)
						{
							m_viewPointContext->DecommiosionThreads();
							m_viewPointContext = nullptr;
						}
						break;

					case CONTEXT_ENUM::frameRate:
						if (m_frameRateContext != nullptr)
						{
							m_frameRateContext->DecommiosionThreads();
							m_frameRateContext = nullptr;
						}
						break;

					case CONTEXT_ENUM::bounds:
						if (m_boundsContext != nullptr)
						{
							m_boundsContext->DecommiosionThreads();
							m_boundsContext = nullptr;
						}
						break;

					case CONTEXT_ENUM::latency:
						if (m_latencyContext != nullptr)
						{
							m_latencyContext->DecommiosionThreads();
							//delete m_latencyContext;
							m_latencyContext = nullptr;
						}
						break;

					default:
						break;
					}
				}

				bool CheckForRunningContexts(int p_context)
				{
					switch ((CONTEXT_ENUM)p_context)
					{
						case CONTEXT_ENUM::locality:
							if (m_localityContext != nullptr)
							{
								return true;
							}
							return false;

						case CONTEXT_ENUM::viewpoint:
							if (m_viewPointContext != nullptr)
							{
								return true;
							}
							return false;

						case CONTEXT_ENUM::frameRate:
							if (m_frameRateContext != nullptr)
							{
								return true;
							}
							return false;

						case CONTEXT_ENUM::bounds:
							if (m_boundsContext != nullptr)
							{
								return true;
							}
							return false;

						case CONTEXT_ENUM::latency:
							if (m_latencyContext != nullptr)
							{
								return true;
							}
							return false;

						default:
							return false;
					}
				}



				Context<Point3D>* m_localityContext;
				VIEWPOINT_CONTEXT* m_viewPointContext;
				BANDWIDTH_CONTEXT* m_bandwidthContext;
				BOUNDS_CONTEXT* m_boundsContext;
				FRAME_RATE_CONTEXT* m_frameRateContext;
				LATENCY_CONTEXT* m_latencyContext;

			private:
			
				int m_broadcastPeriod;
				NetworkPropertiesParser* m_contextParams;
			};
		}
	}
}