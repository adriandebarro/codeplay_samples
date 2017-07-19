#pragma once 
#include <iostream>
#include<future>
#include <vector>
#include <sstream>
#include <map>
#include <string>
#include <thread>

#include "../../Networking/RESTapi/client_http.hpp"
#include <ObservableEvents/ObservableEvent.h>

#include "../JsonParser/document.h"
#include "../JsonParser/filereadstream.h"

using std::stringstream;
using std::vector;
using std::string;
typedef SimpleWeb::Client<SimpleWeb::HTTP> HttpClient;
using Network::ObservableEvents::ObservableEvent;
namespace Engine
{
	namespace Logs
	{
		namespace RESTapi
		{
			//-----------------------------------------------------------------------------------------
			//	  Class:	PostInfoLogger 
			//	Summary:	Provides functionality for the sending of information to a RESTful server
			//   Author:	Adrian De Barro
			//-----------------------------------------------------------------------------------------
			template<typename T, typename IC, typename AS>
			class PostInfoLogger
			{
			private:
				//-----------------------------------------------------------------------------------------
				// PostInformation: Sends the logs to the specified host  
				//-----------------------------------------------------------------------------------------
				static void PostInformation(string p_hostUrl,string p_data, string& p_reply)
				{
					std::map<string, string> headerMap;
					headerMap["Content-Type"] =  "application/json";

					try
					{
						HttpClient client(p_hostUrl);
						auto r3 = client.request("POST", "/peer/communicate", p_data, headerMap);
						
						std::string s(std::istreambuf_iterator<char>(r3->content), {});
						p_reply = s;
					}
					catch (std::exception ex)
					{
						std::cout << ex.what() << "\n";
						cout << "Error occured during post to server " << "\n";
					}
				}

				static void GetInformation(string p_hostUrl)
				{
					std::map<string, string> headerMap;
					headerMap["Content-Type"] = "application/json";

					try
					{
						HttpClient client(p_hostUrl);
						auto r3 = client.request("GET", "/peer/view", "", headerMap);
						std::cout << "get was replied with: " << r3->content.rdbuf() << "\n";
					}
					catch (std::exception ex)
					{
						std::cout << ex.what() << std::endl;
					}
				}


				//-----------------------------------------------------------------------------------------
				// PostPackateInfo: devides the data to be sent into json files containing 15 details per 
				//					json
				//-----------------------------------------------------------------------------------------
				static void PostPackateInfo(string p_hostUrl, int p_sender, int p_receiver, vector<ObservableEvent<T, IC, AS>*> p_updates)
				{
					//default assertion
					//_ASSERT(p_hostUrl != "" && p_sender != 0 && p_receiver != 0);

					//datatosend senderInfo
					stringstream jsonString;
					int counter = 0;
					int index = 0;
					string replyString = "";
					Document parsedJson;

					while (counter != p_updates.size())
					{
						//if the first time the post needs to be pushed in 
						if(counter == 0)
							jsonString << "{ \"first_time\" : true, " << "\"senderId\" : " << p_sender << ", \"receiverId\" : " << p_receiver << ", \"data_sent\" : [";
						else
						{
							try
							{
								parsedJson.Parse(replyString.c_str());
								int postId = parsedJson["post_id"].GetInt();
								int runId = parsedJson["run_id"].GetInt();
								jsonString.clear();
								jsonString.str(string());
								//parse reply string
								jsonString << "{ \"first_time\" : false, " << "\"senderId\" : " << p_sender << ", \"receiverId\" : " << p_receiver << ", \"post_id\" : " << postId << ", \"run_id\" : " << runId << ", \"data_sent\" : [";
							}
							catch(exception ex)
							{
								cout << "Error happened while parsing the following json text:" << replyString << "\n";
								break;
							}
						}
						//append observable events uuid
						for (index = 0; index < 15; index++)
						{
							if (counter == p_updates.size()) break;

							ObservableEvent<T, IC, AS>* current = p_updates[counter];

							if (index != 14 && counter != p_updates.size() - 1)
								jsonString << "[\"" << current->GetUuid() << "\", \"" << current->GetTimeStamps()->ToString() << "\"," << current->GetType() << "],";
							else
								jsonString << "[\"" << current->GetUuid() << "\", \"" << current->GetTimeStamps()->ToString() << "\"," << current->GetType() << "]]}";

							counter++;
						}

						if (p_updates.size() == 0)
							jsonString << "{}]}";

						PostInformation(p_hostUrl, jsonString.str(), replyString);

						if(replyString == "")
						{
							break;
						}
					}
				}

			public:

				//-----------------------------------------------------------------------------------------
				// PostExchangeDetails: Initializes an async operation for the PostPackateInfo function
				//-----------------------------------------------------------------------------------------
				static void PostExchangeDetails(string p_hostUrl, int p_sender, int p_receiver, vector<ObservableEvent<T, IC, AS>*> p_updates)
				{
					std::async(PostPackateInfo, p_hostUrl, p_sender, p_receiver, p_updates);
				}

				//-----------------------------------------------------------------------------------------
				// GetPeerView: Initializes an async operation for the GetInformation testing function
				//-----------------------------------------------------------------------------------------
				static void GetPeerView(string p_hostUrl)
				{
					GetInformation(p_hostUrl);
				}

			};
		}
	}
}