#pragma once 

#include <iostream>
#include <vector>

#include <boost/filesystem/convenience.hpp>
#include <ObservableEvents\ObservableEvent.h>

#include "../../Integrators/RadianceDetails/RadiancePoint.h"
#include "../../Integrators/IrradianceCache.h"

using std::vector;
using Network::ObservableEvents::ObservableEvent;
using Engine::RadiancePoint;
using Engine::IrradianceCache;
namespace Engine
{
	namespace Logs
	{
		namespace CommunicationLogs
		{
			///-------------------------------------------------------------------------------------------------
			/// <summary>	Locally log communication logs in an asychronous fashion </summary>
			///
			/// <remarks>	Adrian, 19/07/2017. </remarks>
			///-------------------------------------------------------------------------------------------------
			class CommunicationLogger
			{
			public:
				CommunicationLogger(){
					InitFstream();
				}

				string GetFileName(string p_fileName, int p_pathNumber)
				{
					return p_fileName + "Path--" + to_string(p_pathNumber) + "--";;
				}

				string GetCurrentDateTime()
				{
					time_t rawtime;
					struct tm timeinfo;
					char buffer[80];

					time(&rawtime);
					localtime_s(&timeinfo, &rawtime);

					strftime(buffer, 80, "%d%m%Y%I%M%S", &timeinfo);
					std::string str(buffer);

					return str;
				}

				void InitFstream()
				{
					time_t _tm = time(NULL);

					struct tm * curtime = localtime(&_tm);

					boost::filesystem::path dir("./logging/");

					if (!(boost::filesystem::exists("./logging/")))
					{
						boost::filesystem::create_directory("./logging/");
					}

					string path = "./logging/" + GetCurrentDateTime() + ".txt";


					m_fileStream.open(path);
				}

				void AddCommunicationDetails(int p_peerIndex, int p_numberEvents)
				{
					m_fileStream << "[SENDER] communicating with " << p_peerIndex << ", " << p_numberEvents << " events " << "\n";
				}
				
			private:
				std::ofstream m_fileStream;

			};
		}
	}
}
