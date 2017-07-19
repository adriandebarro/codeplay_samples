#pragma once 

#include <iostream>
#include <vector>
#include <string>
#include <mutex>

#include <boost/filesystem/convenience.hpp>

#include "../../../Integrators/RadianceDetails/RadiancePoint.h"
#include "../../../Integrators/IrradianceCache.h"

using std::vector;
using Engine::RadiancePoint;
using Engine::IrradianceCache;
namespace Engine
{
	namespace Logs
	{
		namespace CommuincationLogs
		{
			class CommunicationToFileLogger : public CommunicationLogger
			{
			public:

				CommunicationToFileLogger(string p_fileName, bool p_context = false)
				{
					//init file stream, name and folder structure if need be 
					m_weightedContext = p_context;
					InitStream(p_fileName);
				}

				string GetCurrentDateTime(int type = 0)
				{
					time_t rawtime;
					struct tm timeinfo;
					char buffer[80];

					time(&rawtime);
					localtime_s(&timeinfo, &rawtime);

					if (type == 0)
						strftime(buffer, 80, "%d%m%Y%I%M%S", &timeinfo);
					else 
						strftime(buffer, 80, "%d/%m/%Y-%I:%M:%S", &timeinfo);
					std::string str(buffer);

					return str;
				}

				void InitStream(string p_fileName)
				{
					time_t _tm = time(NULL);

					struct tm * curtime = localtime(&_tm);

					if (!(boost::filesystem::exists(COMMUNICATION_HISTROY_FOLDER)))
					{
						boost::filesystem::create_directory(COMMUNICATION_HISTROY_FOLDER);
					}

					string path = COMMUNICATION_HISTROY_FOLDER +p_fileName+"--"+GetCurrentDateTime() + ".txt";

					m_fileStream.open(path);
					CreateHeader();
				}

				void CreateHeader()
				{
					if (!m_weightedContext)
						m_fileStream << "Sender,Receiver,Amount of Uuids, Uuids seperate by(||), Time of communication, Sender or Receiver \n";
					else
						m_fileStream << "Sender,Receiver,Weight of sender,Amount of Uuids, Uuids seperate by(||), Time of communication, Sender or Receiver \n";
				}

				using CommunicationLogger::AddCommunicationDetails;
				void AddCommunicationDetails(vector<ObservableEvent<RadiancePoint, IrradianceCache, LockingOctree>*>& p_allUpdates, Neighbour* p_neighborDetails, Neighbour* p_internalRepresentation, int p_typeOfCommunication)
				{

					string seperator = "||";

					m_lock.lock();

					//sender details
					m_fileStream << p_neighborDetails->ToString() << ",";
					//receiver details
					m_fileStream << p_internalRepresentation->ToString() << ",";
					//amount of uuids 
					m_fileStream << std::to_string(p_allUpdates.size()) << ",";
					
					if (m_weightedContext)
						m_fileStream << std::to_string(p_neighborDetails->m_weight) << ",";

					for (int index = 0; index < p_allUpdates.size(); index++)
					{
						m_fileStream << p_allUpdates[index]->GetUuid() << seperator;
					}

					//time of communication 
					m_fileStream <<  ","+GetCurrentDateTime(1)+",";

					//sender or receiver 

					if (p_typeOfCommunication == 0)
						m_fileStream <<  "Receiver\n";
					else
						m_fileStream << "Sender\n";


					m_lock.unlock();
				}

				void FlushCommunicationLoggerBuffer()
				{
					m_fileStream.flush();
				}

			private:

				const string COMMUNICATION_HISTROY_FOLDER = "./CommunicationHistroy/";
				std::ofstream m_fileStream;
				std::mutex m_lock;
				bool m_weightedContext = false;
			};
		}
	}
}