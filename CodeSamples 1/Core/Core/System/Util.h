#pragma once 

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <chrono>
#include <intrin.h>

#include <boost/tokenizer.hpp>

#include "../Maths/Vector2.h"
#include "../Bodies/Particle.h"
#include "../Common/Common.h"
#include "../System/PropertiesList.h"

using Core::Common::Body;

namespace Core 
{
	namespace System
	{
		namespace Util
		{
			static bool GetSystemProperties(Core::System::PropertiesList<std::string, std::string> *p_propertiesList)
			{
				//get the actual data
				p_propertiesList->AddProperty(Core::Common::SYSTEM_THREADS_COUNT, "0");
				p_propertiesList->AddProperty(Core::Common::SYSTEM_HYPER_THREADED, "1");
				return true;
			}

			inline static int GetCacheLineSize()
			{
				int data[4] = { 0 };
				__cpuid(data, 0x80000006);
				return (data[2] & (0x000000FF));
			}

			inline static int GetL2CacheSize()
			{
				int data[4] = { 0 };
				__cpuid(data, 0x80000006);
				return (data[2] & (0xFFFF0000)) >> 16;
			}
		}

		namespace Maths
		{
			#ifndef max
				#define max( a, b ) ( ((a) > (b)) ? (a) : (b) )
			#endif
		}

		namespace CUDA
		{

			#define CheckForErrors(value)		\
			{									\
				cudaError cudaValue = value;	\
				if(cudaValue != cudaSuccess)	\
				{								\
					std::cout << "Error found " << __LINE__ << "---" << value << std::endl;  \
					std::cin.get();					\
					exit(1);					\
				}								\
			}									
		}

		namespace IO
		{
			typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
			typedef std::vector<Body>::iterator BodyIterator;

			static Body* ParseBodiesInputFile(const std::string p_filePath, int *p_bodyCount, bool *p_success)
			{
				std::vector<Body*> retreivedBodies;
				std::ifstream inputFile(p_filePath);
				std::string currentLine = "";
				float currentMass = 0.f;
				float currentXPos = 0.f;
				float currentYPos = 0.f;

				boost::char_separator<char> sep(", ");

				while ((std::getline(inputFile, currentLine)))
				{
					
					tokenizer tokens(currentLine,sep);

					tokenizer::iterator iter = tokens.begin();
					currentMass = std::stof(*iter);
					iter++;
					currentXPos = std::stof(*iter);
					iter++;
					currentYPos = std::stof(*iter);

					retreivedBodies.push_back(new Body(currentMass, Core::Maths::Vector::Vector2(currentXPos, currentYPos)));
				}

				int bodyCount = retreivedBodies.size();
				
				if (bodyCount == 0)
				{
					*p_success = false;
					return nullptr;
				}
				else
					*p_bodyCount = bodyCount;

				Body* allBodies = new Body[retreivedBodies.size()];

				for (int bodyIndex = 0; bodyIndex < bodyCount; bodyIndex++)
				{
					allBodies[bodyIndex] = *retreivedBodies[bodyIndex];
				}
				
				*p_success = true;

				return allBodies;
			}

			CUDA_CALLABLE_HOST_FUNC static bool WriteBodiesToFile(const int p_iterNumber, Body* p_allBodies, const int p_amount)
			{
				
				std::string fileName = "nbody_" + std::to_string(p_iterNumber) + ".txt";
				std::ofstream currentIteration(fileName);

				if (currentIteration.is_open())
				{
					for (int index = 0; index < p_amount; index++)
					{
						//currentIteration << "Test" << std::endl;
						currentIteration << (p_allBodies[index]).Mass << ", " << (p_allBodies[index]).Position.X << ", " << (p_allBodies[index]).Position.Y << std::endl;
					}
				}
				else
					return false;

				return true;
			}

			static void Pause()
			{
				std::cout << "Press any key to continue..." << std::endl;
				std::cin.get();
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	A macro that notifies for an unimplemented method. </summary>
			///
			/// <remarks>	Adrian, 15/07/2017. </remarks>
			///
			/// <param name="value">	The value. </param>
			///-------------------------------------------------------------------------------------------------

			#define ImplementMethodNotif(value) std::cout << "The method " << value << " is not yet implemented in file: \n" << __FILE__ << std::endl; Core::System::IO::Pause();

			
			///-------------------------------------------------------------------------------------------------
			/// <summary>	Process the commandline input. </summary>
			///
			/// <remarks>	Adrian, 10/07/2017. </remarks>
			///
			/// <param name="argc"> 	The argc. </param>
			/// <param name="argv"> 	[in,out] If non-null, the argv. </param>
			/// <param name="p_map">	[in,out] If non-null, the map. </param>
			///-------------------------------------------------------------------------------------------------
			static void ProcessCmdLnInput(int argc, char* argv, std::map<std::string, std::string> *p_map)
			{
				ImplementMethodNotif("ProcessCmdLnInput");
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Writes the progress message in the same line. </summary>
			///
			/// <remarks>	Adrian, 09/07/2017. </remarks>
			///
			/// <param name="p_currentCount">	Number of currents. </param>
			///-------------------------------------------------------------------------------------------------
			static void WriteProgressMessage(const int p_currentCount)
			{
				std::cout << '\r' << std::setw(2) << "Iteration " << std::setfill('0') << p_currentCount << " of 1000 " << std::flush;
			}

			template<typename T, typename T_1>
			static void PrintContentOfReg(T *reg)
			{
				T_1 *regArray = (T_1*)reg;
				for (int index = 0; index < 8; index++)
				{
					std::cout << regArray[index] << std::endl;
				}
			}
		}
	}
}
