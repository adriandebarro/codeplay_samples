#pragma once 

#include <string>
#include <iostream>
#include <stdio.h>

#include "../../CoreClasses/JsonParser/document.h"
#include "../../CoreClasses/JsonParser/filereadstream.h"
#include "../../CoreClasses/JsonParser/prettywriter.h"
#include <cstdio>

using namespace rapidjson;
using std::string;
using std::cout;
using std::cin;
using std::endl;

using std::string;
namespace Engine
{
	namespace Networking
	{
		namespace Contexts
		{

			class NetworkPropertiesParser
			{
			public:
				
				Document m_parsedJson;
				string m_jsonPath;
				bool m_parsedOk = true;


				NetworkPropertiesParser(string p_path)
				{
					m_jsonPath = p_path;
					m_enabledContexts = new int[5];
					bool readFile = LoadJson();
					m_parsedOk = ParseData();
					
				}

				float m_localityWeighting;
				float m_boundsWieghting;
				float m_latencyWeighting;
				float m_viewpointWeighting;
				float m_framerateWeigthing;

				int m_localityBroadcastPeriod;
				int	m_boundsBroadcastPeriod;
				int	m_latencyBroadcastPeriod;
				int	m_viewpointBroadcastPeriod;
				int	m_framerateBroadcastPeriod;

				int m_contextInitializerPeriod;
				int m_peerUpdatePeriod;
				int m_contextSendUpdatePeriod;

				int m_contextSubscriberLimit = 5;
				int* m_enabledContexts;

			private:

				const string WEIGHT_PARAMS = "weight_params";
				const string BROADCAST_PARAMS = "broadcast_params";

				const string LOCALITY_WEIGHT = "locality_weight";
				const string BOUNDS_WEIGHT = "bounds_weight";
				const string LATENCY_WEIGHT = "latency_weight";
				const string VIEW_POINT_WEIGHT = "viewpoint_weight";
				const string FRAME_RATE_WEIGHT = "frame_rate_weight";

				const string LOCALITY_BROADCAST_PERIOD = "locality_broadcast_period";
				const string BOUNDS_BROADCAST_PERIOD = "bounds_broadcast_period";
				const string LATENCY_BROADCAST_PERIOD = "latency_broadcast_period";
				const string VIEW_POINT_PERIOD = "view_point_period";
				const string FRAME_RATE_PERIOD = "frame_rate_period";

				const string CONTEXT_SEND_UPDATE = "context_update";
				const string PEER_SEND_UPDATE = "peer_update";
				const string CONTEXT_INITIALIZER_PERIOD = "initializer_period";
				const string ENABLED_CONTEXTS = "enabled_contexts";

				const string CONTEXT_SUBSCRIBERS_LIMIT = "context_subscribers_limit";

				bool LoadJson()
				{
					#ifdef _WIN32
						std::FILE *file = fopen(m_jsonPath.c_str(), "r");
					#else
						std::FILE *file = fopen(m_jsonPath.c_str(), "r");
					#endif

					if (file != nullptr)
					{
						char buffer[65536];
						FileReadStream is(file, buffer, sizeof(buffer));
						m_parsedJson.ParseStream(is);
						return !m_parsedJson.HasParseError();
					}
					else {
						return false;
					}
					return false;
				}

				static void ParseStringSentence(std::string toParse, std::string splittingString, vector<std::string>& output)
				{
					boost::char_separator<char> sep(splittingString.c_str());
					tokeniz tok(toParse, sep);

					for (tokeniz::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
					{
						output.push_back((std::string)tokIter.current_token().c_str());
					}
				}

				void ParseEnabledContexts(std::string p_stringifiedContexts)
				{
					vector<string> splitParts;

					ParseStringSentence(p_stringifiedContexts, ",", splitParts);

					for (int index = 0; index < splitParts.size(); index++)
					{
						m_enabledContexts[index] = std::stoi(splitParts[index]);
					}
				}

				bool ParseData()
				{
					try
					{
						if (m_parsedJson.HasMember(WEIGHT_PARAMS.c_str()))
						{
							const Value& weights = m_parsedJson[WEIGHT_PARAMS.c_str()];

							m_localityWeighting = weights[LOCALITY_WEIGHT.c_str()].GetFloat();
							m_boundsWieghting = weights[BOUNDS_WEIGHT.c_str()].GetFloat();
							m_latencyWeighting = weights[LATENCY_WEIGHT.c_str()].GetFloat();
							m_viewpointWeighting = weights[VIEW_POINT_WEIGHT.c_str()].GetFloat();
							m_framerateWeigthing = weights[FRAME_RATE_WEIGHT.c_str()].GetFloat();
						}

						if (m_parsedJson.HasMember(BROADCAST_PARAMS.c_str()))
						{
							const Value& broadcast = m_parsedJson[BROADCAST_PARAMS.c_str()];

							m_localityBroadcastPeriod = broadcast[LOCALITY_BROADCAST_PERIOD.c_str()].GetInt();
							m_boundsBroadcastPeriod = broadcast[BOUNDS_BROADCAST_PERIOD.c_str()].GetInt();
							m_latencyBroadcastPeriod = broadcast[LATENCY_BROADCAST_PERIOD.c_str()].GetInt();
							m_viewpointBroadcastPeriod = broadcast[VIEW_POINT_PERIOD.c_str()].GetInt();
							m_framerateBroadcastPeriod = broadcast[FRAME_RATE_PERIOD.c_str()].GetInt();
						}

						m_contextInitializerPeriod = m_parsedJson[CONTEXT_INITIALIZER_PERIOD.c_str()].GetInt();
						m_peerUpdatePeriod = m_parsedJson[PEER_SEND_UPDATE.c_str()].GetInt();
						m_contextSendUpdatePeriod = m_parsedJson[CONTEXT_SEND_UPDATE.c_str()].GetInt();
						m_contextSubscriberLimit = m_parsedJson[CONTEXT_SUBSCRIBERS_LIMIT.c_str()].GetInt();
						std::string stringifiedEnabledContexts = m_parsedJson[ENABLED_CONTEXTS.c_str()].GetString();
						//parsing which contexts will be utilised 
						ParseEnabledContexts(stringifiedEnabledContexts);


						return true;
					}
					catch (std::exception ex)
					{
						std::cout << "Failed to write " << __FILE__ << "" << __LINE__ << std::endl;
						return false;
					}
				}

			};
		}
	}
}