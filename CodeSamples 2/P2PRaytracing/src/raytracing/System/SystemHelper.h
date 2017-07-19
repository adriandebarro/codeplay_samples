#pragma once 

#include <iostream>
#include <windows.h>
#include <string>

namespace Engine
{
	namespace System
	{
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Platform independent Utilitu functions </summary>
		///
		/// <remarks>	Adrian, 26/12/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		static const WORD colors[] =
		{
			0x1A, 0x2B, 0x3C, 0x4D, 0x5E, 0x6F,
			0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6
		};

		const int ALERT_COLOR = 1;
		const int ERROR_COLOR = 8;
		const int MESSAGE_COLOR = 0;

		class SystemUtil
		{
		public:

			///-------------------------------------------------------------------------------------------------
			/// <summary>	A platform independent pause() implementation</summary>
			///
			/// <remarks>	Adrian, 19/07/2017. </remarks>
			///-------------------------------------------------------------------------------------------------
			static void SystemPause()
			{
				std::cout << "Press any key to contine....." << std::endl;
				std::cin.get();
			}
			
			///-------------------------------------------------------------------------------------------------
			/// <summary>	
			///     Color coded print outs for debugging purposes
			///     PLATFORM DEPENDENT
			/// </summary>
			///
			/// <remarks>	Adrian, 19/07/2017. </remarks>
			///-------------------------------------------------------------------------------------------------
			#ifdef _WIN32
				static void PrintErrorMessage(std::string p_message)
				{
					WriteHighligthed(ERROR_COLOR, p_message);
				}

				static void PrintAlertMessage(std::string p_message)
				{
					WriteHighligthed(ALERT_COLOR,  p_message);
				}

				static void WriteHighligthed(int p_color, string p_stringToWrite)
				{
					HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
					HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
					CONSOLE_SCREEN_BUFFER_INFO csbi;
					GetConsoleScreenBufferInfo(hstdout, &csbi);
					SetConsoleTextAttribute(hstdout, colors[p_color]);
					std::cout << p_stringToWrite << std::endl;
					FlushConsoleInputBuffer(hstdin);
					SetConsoleTextAttribute(hstdout, csbi.wAttributes);
				}
			#endif
		};
	}
}