#pragma once 

#include <iostream>
#include <chrono>
#include <string>


namespace Core
{
	namespace Application
	{
		typedef std::chrono::high_resolution_clock::time_point high_res_clock;

		class Application
		{
		protected:
			std::string m_inputFilePath;
			float m_deltaT;
			int m_numThreads;


			///-------------------------------------------------------------------------------------------------
			/// <summary>	Executes the application operation. </summary>
			///
			/// <remarks>	Adrian, 15/07/2017. </remarks>
			///-------------------------------------------------------------------------------------------------
			virtual void ExecuteApplication() = 0;

		public:
			virtual bool Initialise() = 0;
			virtual void Terminate() = 0;

			Application(std::string p_inputFilePath, float p_deltaT, int p_numThreads):
				m_inputFilePath(p_inputFilePath),
				m_deltaT(p_deltaT),
				m_numThreads(p_numThreads)
			{ }

			///-------------------------------------------------------------------------------------------------
			/// <summary>
			/// 	Starts timing the duration and calls the Application executer
			/// </summary>
			///
			/// <remarks>	Adrian, 15/07/2017. </remarks>
			///-------------------------------------------------------------------------------------------------

			virtual void Execute()
			{
				high_res_clock startingTime, endingTime;
				startingTime = std::chrono::high_resolution_clock::now();
				ExecuteApplication();
				endingTime = std::chrono::high_resolution_clock::now();
				const auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endingTime - startingTime).count();
				std::cout << "Total time taken " << elapsedTime << '\n';
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Runs the application </summary>
			///
			/// <remarks>	Adrian, 15/07/2017. </remarks>
			///-------------------------------------------------------------------------------------------------

			virtual void Run()
			{
				Initialise();
				std::cout << "Application initialised --- Starting Execution" << '\n';
				Execute();
				Terminate();
				std::cout << "Run terminated" << std::endl;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Gets input file path. </summary>
			///
			/// <remarks>	Adrian, 15/07/2017. </remarks>
			///
			/// <returns>	The input file path. </returns>
			///-------------------------------------------------------------------------------------------------
			std::string GetInputFilePath()
			{
				return m_inputFilePath;
			}
		};
	}

}
