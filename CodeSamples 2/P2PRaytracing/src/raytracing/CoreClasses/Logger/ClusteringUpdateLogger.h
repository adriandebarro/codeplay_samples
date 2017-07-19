#pragma once 

#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

#include <boost/filesystem/convenience.hpp>

#include "../../Networking/Metrics/Locality/ClusteringPoint.h"


using std::thread;
using std::string;


namespace Engine
{
	namespace Logs
	{
		const string NETWORK_CLUSTERING_DIRECTORY = "./ClusteringHistroy/";
		const string LATEST_UPDATE_File_NAME = "ClusteringHistroy.txt";


		class ClusteringUpdateLogger
		{
		public:

			ClusteringUpdateLogger()
			{
				InitFstreamCsvFormat();
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

			void InitFstreamCsvFormat()
			{
				time_t _tm = time(NULL);

				struct tm * curtime = localtime(&_tm);

				boost::filesystem::path updateLatest(NETWORK_CLUSTERING_DIRECTORY);

				if (!(boost::filesystem::exists(NETWORK_CLUSTERING_DIRECTORY)))
				{
					boost::filesystem::create_directory(NETWORK_CLUSTERING_DIRECTORY);
				}

				string clusteringUpdatesPath = NETWORK_CLUSTERING_DIRECTORY + GetCurrentDateTime() +LATEST_UPDATE_File_NAME;
				m_networkClusteringStream.open(clusteringUpdatesPath, ios::out);
			}

			void LogClusters(vector<ClusteringPoint*> p_allClusters,string p_clusteirngMethod = "undefined")
			{
				m_networkClusteringStream << "---------- New Clusters amount : " << p_allClusters.size() << "Type: " <<p_clusteirngMethod << " ------------------" << "\n";

				for (ClusteringPoint* currentCluster : p_allClusters)
				{
					m_networkClusteringStream << "Current Cluster" << currentCluster->ClusterCenter.toString() << "\n";

					for (int index = 0; index < currentCluster->clusteredPoints.size(); index++)
					{
						m_networkClusteringStream << currentCluster->clusteredPoints[index]->GetIpAddress() << "\n";
					}
				}

				m_networkClusteringStream << "---------End new Cluster ------------------------------------------------------------------------------------------" << "\n";

				m_networkClusteringStream.flush();
			}

			

			~ClusteringUpdateLogger()
			{}
		private:
			const int DELTA_TIME = 5;
			std::ofstream m_networkClusteringStream;
		};
	}
}//Engine

