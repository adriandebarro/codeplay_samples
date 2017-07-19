#pragma once 

#include <iostream>
#include <map>
#include <thread>
#include <string>
#include <future>

#include "../UtilityClasses/http_client.hpp"

using std::stringstream;
using std::string;
using std::map;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;

namespace Engine
{
		namespace Logger
		{
			class ContextStatusLogger
			{
			public:

				//-----------------------------------------------------------------------------------------
				// PostExchangeDetails: Initializes an async operation for the PostPackateInfo function
				//-----------------------------------------------------------------------------------------
				static void PostExchangeDetails(string p_hostUrl, int p_sender, int p_contextNumber, int p_status, string p_contextUuid)
				{
					std::async(PostPackateInfo, p_hostUrl, p_sender, p_contextNumber, p_status, p_contextUuid);
				}



			private:

				static void PostInformation(string p_hostUrl, string p_data, string& p_reply)
				{

					std::map<string, string> headerMap;
					headerMap["Content-Type"] = "application/json";

					try
					{
						HttpClient client(p_hostUrl);
						auto r3 = client.request("POST", "/context/status", p_data, headerMap);

						std::string s(std::istreambuf_iterator<char>(r3->content), {});
						p_reply = s;
					}
					catch (std::exception ex)
					{
						std::cout << ex.what() << "\n";
						std::cout << "Error occured during post to server " << "\n";
					}
				}

				//-----------------------------------------------------------------------------------------
				// PostPackateInfo: devides the data to be sent into json files containing 15 details per 
				//					json
				//-----------------------------------------------------------------------------------------
				static void PostPackateInfo(string p_hostUrl, int p_hostPeerIndex, int p_contextNumber, int p_status, string p_contextUuid)
				{
					//default assertion
					_ASSERT(p_hostUrl != "" && p_sender != 0 && p_receiver != 0);

					//datatosend senderInfo
					stringstream jsonString;
					int counter = 0;
					int index = 0;
					string replyString = "";
					
					std::stringstream jsonOutput;
					jsonOutput << "{ \"peer_id\" : " << p_hostPeerIndex << ", \"context_number\" : " << p_contextNumber << " , \"uuid\" : \"" << p_contextUuid << "\"}";

					PostInformation(p_hostUrl, jsonOutput.str(), replyString);

					#ifdef _DEBUG
						this_thread::sleep_for(chrono::milliseconds(2000));
						cout << replyString << "\n";

					#endif


					if (replyString == "")
					{
						std::cout << "did not receive replyt from server" << std::endl;
					}
				}
			};
		}
	}
}