#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Logger.h
//			  Author:		Adrian De Barro
//			    Date:		13/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <string>
#include <fstream>
#include <iostream>

#include "../../System/DumpFileCreator.h"

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP
#define ELPP_THREADING_ENABLED
#define ELPP_DISABLE_DEFAULT_CRASH_HANDLING
#define _ELPP_FORCE_USE_STD_THREAD
#define ELPP_THREAD_SAFE
#define RUN_MODE

namespace Engine
{
	using std::string;
	using std::ofstream;
	using std::cout;
	using std::endl;
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: Logger
	//		   Description: Serves as a wrapper around easylogging++.
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: 
	//					    TODO expose more of easy logging functionality
	//----------------------------------------------------------------------------------------------------------------
	class Logger
	{
	public:

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Logger()
		{
			//el::Helpers::setCrashHandler(Engine::System::DumpFile::unhandled_handler);
			//needs to be a singleton
			/*m_configuration.setToDefault();
			m_configuration.set(el::Level::Info, el::ConfigurationType::Format, "%datetime %level %msg");

			LogInfo("Logger Initialized");

			el::Loggers::reconfigureLogger("default", m_configuration);

			m_configuration.setGlobally(el::ConfigurationType::Format, "%date %msg"); 
			el::Loggers::reconfigureLogger("default", m_configuration);*/
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Logger(bool p_writeToFile)
		{
			
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Logger(string p_pathToConfigFile)
		{}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: LogInfo
		//		Return Value: VOID
		//		  Parameters: String to Log
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		static void LogInfo(string p_stringToLog)
		{
			//#ifdef DEBUG_MODE
			//LOG(INFO) << p_stringToLog << endl;
			cout << p_stringToLog << endl;
			//#endif
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: LogError
		//		Return Value: VOID
		//		  Parameters: String to Log
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		static void LogError(string p_stringToLog)
		{
			//el::LOG(ERROR) << p_stringToLog << endl;
			cout << p_stringToLog << endl;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: Log Trace
		//		Return Value: VOID
		//		  Parameters: String to Log
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		static void LogTrace(string p_stringToLog)
		{
			//LOG(TRACE) << p_stringToLog << endl;
			cout << p_stringToLog << endl;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: WrtieToFile
		//		Return Value: VOID
		//		  Parameters: String to log to file
		//			  Status: 
		//					  TODO use of sinks
		//----------------------------------------------------------------------------------------------------------------
		void WriteToFile(string p_stringToWrite)
		{
			fp << p_stringToWrite << "\n";
		}

	private:
		//static el::Configurations m_configuration;
		ofstream fp;
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------