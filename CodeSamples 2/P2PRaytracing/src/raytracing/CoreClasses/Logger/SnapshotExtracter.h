#pragma once 

#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

#include <boost/filesystem/convenience.hpp>

#include "../../Integrators/IrradianceCache.h"
#include "../../Integrators/TwoLayerIrradianceCache.h"
//#include "../../Networking/PeerInfo/ContextOracleInteractor.h"
//#include "../../Networking/Metrics/Locality/ClusteringPoint.h"

#define DELTA_TIME 2

using std::thread;
using std::string;
//using Engine::Networking::ContextOracleInteractor;

namespace Engine
{
	namespace Logs
	{
		const string BENCH_MARK_DIRECTORY = "./DataOutput/";

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Logs the time taken to render on a per frame basis </summary>
		///
		/// <remarks>	Adrian, 19/07/2017. </remarks>
		///-------------------------------------------------------------------------------------------------
		class SnapShotExtracter
		{
		public:

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Constructor. </summary>
			///
			/// <remarks>	Adrian, 15/01/2016. </remarks>
			///
			/// <param name="p_IC">				[in,out] If non-null, the ic. </param>
			/// <param name="p_camera">			[in,out] If non-null, the camera. </param>
			/// <param name="p_loggingType">	Type of the logging. </param>
			/// <param name="p_fileName">   	Filename of the file. </param>
			///-------------------------------------------------------------------------------------------------

			SnapShotExtracter(int p_icType, Integrator* p_IC, int p_pathNumber ,Camera* p_camera, int p_loggingType = 0, string p_fileName = "")
			{
				m_startingTime = clock();
				m_Ic = p_IC;
				m_camera = p_camera;
				m_irradianceCacheType = p_icType;
				m_loggingType = p_loggingType;
				m_sendUpdateMessage = false;

				string fileName = GetFileName(p_fileName, p_pathNumber);

				if (m_loggingType == 0)
				{
					InitPostFrameSnapShotLogger(fileName);
				}
				else
				{
					InitFstreamCsvFormat(fileName);
				}

			}

			/////-------------------------------------------------------------------------------------------------
			///// <summary>	Constructor. </summary>
			/////
			///// <remarks>	Adrian, 15/01/2016. </remarks>
			/////
			///// <param name="p_IC">				 	[in,out] If non-null, the ic. </param>
			///// <param name="p_camera">			 	[in,out] If non-null, the camera. </param>
			///// <param name="p_oracleInteractor">	[in,out] If non-null, the oracle interactor. </param>
			///// <param name="p_loggingType">	 	Type of the logging. </param>
			///// <param name="p_fileName">		 	Filename of the file. </param>
			/////-------------------------------------------------------------------------------------------------

			//SnapShotExtracter(int p_icType, Integrator* p_IC, int p_pathNumber, Camera* p_camera, int p_loggingType = 0, string p_fileName = "")
			//{
			//	m_startingTime = clock();
			//	m_Ic = p_IC;
			//	m_camera = p_camera;
			//	//m_oracleInteractor = p_oracleInteractor;
			//	m_sendUpdateMessage = true;
			//	m_loggingType = p_loggingType;

			//	string fileName = GetFileName(p_fileName, p_pathNumber);

			//	if (m_loggingType == 0)
			//	{
			//		InitPostFrameSnapShotLogger(fileName);
			//		InitFstreamCsvFormat(p_fileName);
			//	}
			//	else
			//	{
			//		InitFstreamCsvFormat(p_fileName);
			//	}
			//}

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

				boost::filesystem::path dir(BENCH_MARK_DIRECTORY);

				if (!(boost::filesystem::exists(BENCH_MARK_DIRECTORY)))
				{
					boost::filesystem::create_directory(BENCH_MARK_DIRECTORY);
				}
				
				string path = BENCH_MARK_DIRECTORY + GetCurrentDateTime() + ".txt";


				m_fileStream.open(path);

				m_snapshotThread = thread(&SnapShotExtracter::CodeSnapshotThreadCsvFormat, this);
			}

			void InitPostFrameSnapShotLogger(string p_fileName = "")
			{
				time_t _tm = time(NULL);

				struct tm * curtime = localtime(&_tm);

				boost::filesystem::path dir(BENCH_MARK_DIRECTORY);

				if (!(boost::filesystem::exists(BENCH_MARK_DIRECTORY)))
				{
					boost::filesystem::create_directory(BENCH_MARK_DIRECTORY);
				}

				string path = BENCH_MARK_DIRECTORY + p_fileName + GetCurrentDateTime() + ".csv";
				m_fileStream.open(path);
				
				//init here inital detaiols 
				ProduceHeader(0);
			}
			
			void InitFstreamCsvFormat(string p_fileName = "")
			{

				if (m_loggingType == 1)
				{
					time_t _tm = time(NULL);

					struct tm * curtime = localtime(&_tm);

					boost::filesystem::path dir(BENCH_MARK_DIRECTORY);

					if (!(boost::filesystem::exists(BENCH_MARK_DIRECTORY)))
					{
						boost::filesystem::create_directory(BENCH_MARK_DIRECTORY);
					}

					string path = BENCH_MARK_DIRECTORY + p_fileName + GetCurrentDateTime() + ".csv";

					m_fileStream.open(path);
				}
				m_snapshotThread = thread(&SnapShotExtracter::CodeSnapshotThreadCsvFormat, this);
			}

			/*void GetCurrentSnapshot()
			{
				m_fileStream << "---------- \n";
				m_fileStream << GetLocalNetworkRadianceCount() << "\n";
				m_fileStream << "Frame Rate " << GetFrameRate() << "\n";
				m_fileStream << m_camera->eye.toString() << " \n";
			}*/

			void GetCurrentSnapshotCsvFormat()
			{

				m_fileStream << GetLocalNetworkRadianceCountToCsv() << "," << GetFrameRate() << "," << m_camera->eye.toString() << " \n";
				//m_networkClusteringStream << GetLocalNetworkRadianceCountToCsv() << "," << GetFrameRate() << "," << m_camera->eye.toString();
				//m_networkClusteringStream.flush();
			}

			string GetStringUpdate()
			{
				return m_camera->eye.toString();
			}

			void ProduceHeader(int p_TypeOfHeader)
			{
				if (p_TypeOfHeader == 0)
				{
					m_fileStream << "LocalRadianceCount,GlobalRadianceCount,Position,TimeTaken \n";
				}
				else
				{
					m_fileStream << "LocalRadianceCount,GlobalRadianceCount,FrameRate,Position \n";
				}
			}

			void CodeSnapshotThread()
			{
				int index = 0;
				while (!m_sessionEnded)
				{
					//GetCurrentSnapshot();

					if (index == 100)
					{
						m_fileStream.flush();
						index = 0;
					}
					else
						index++;
					
					this_thread::sleep_for(std::chrono::seconds(DELTA_TIME));
				}
			}

			void EnterEndOfFrameDetails(double p_timeTaken)
			{
				m_fileStream << GetLocalNetworkRadianceCountToCsv() << "," << m_camera->eye.toString() <<"," << to_string(p_timeTaken) <<" \n";
			}


			void CodeSnapshotThreadCsvFormat()
			{
				if (m_loggingType == 1)
					ProduceHeader(1);
				int index = 0;
				while (!m_sessionEnded)
				{
					if (m_loggingType == 1)
					{
						GetCurrentSnapshotCsvFormat();
					}
					
					/*if (m_sendUpdateMessage && m_oracleInteractor != nullptr)
					{
						string currentUpdate = GetStringUpdate();
						m_oracleInteractor->SendUpdates(currentUpdate);
					}*/

					this_thread::sleep_for(std::chrono::seconds(DELTA_TIME));
				}

			}
			// -----
			// -
			// -----
			string GetLocalNetworkRadianceCountToCsv()
			{
				if (m_irradianceCacheType == 0)
				{
					return to_string(((IrradianceCache*)m_Ic)->GetLocalRadianceCount()) + "," + to_string(((IrradianceCache*)m_Ic)->GetNetworkRadianceCount());
				}
				else
					return to_string(((TwoLayerIrradianceCache*)m_Ic)->GetLocalIC()->GetLocalRadianceCount()) + "," + to_string(((TwoLayerIrradianceCache*)m_Ic)->GetGlobalIC()->GetNetworkRadianceCount());
			}

			//-----
			//- Flush IO buffer
			//-----
			void ForceFlush()
			{
				m_fileStream.flush();
			}

			float CalculateFrameRate()
			{
				clock_t currentTime = clock();
				double timeInSeconds = ((double)currentTime - m_startingTime) / (double)CLOCKS_PER_SEC;

				if (m_camera->m_totalCreatedFrames == 0)
					return 0;
				else
				{
					float total = m_camera->m_totalCreatedFrames / timeInSeconds;
					return total > 100000 ? 0 : total;
				}
			}

			void TotalTimeTaken(double p_timeTaken)
			{
				m_fileStream << "Total Time taken is :" << to_string(p_timeTaken) + "\n";
			}

			void TotalUsedNetworkPoints(int p_amount)
			{
				m_fileStream << "Total amount of used points:" << to_string(p_amount) + "\n";
			}

			void TotalUsedNetworkPoints(int p_localAmount, int p_globalAmount)
			{
				m_fileStream << "Total Tamount of used points:" << to_string(p_localAmount) + "\n";
			}

			string GetFrameRate()
			{
				return to_string(CalculateFrameRate());
			}

			void StopBenchMarks()
			{
				ForceFlush();
				m_sessionEnded = true;
				m_fileStream.close();
			}

			~SnapShotExtracter()
			{}
		private:

			Engine::Integrator* m_Ic;
			int m_irradianceCacheType = 0;
			Engine::Camera* m_camera;
			std::ofstream  m_fileStream;
			std::ofstream m_networkClusteringStream;

			clock_t m_startingTime;
			thread m_snapshotThread;
			bool m_sessionEnded = false;
			bool m_sendUpdateMessage;
			//ContextOracleInteractor<>* m_oracleInteractor;
			int m_loggingType;
		};
	}
}//Engine

