#pragma once 

#include <time.h>
#include <stdio.h>

namespace Engine
{
	namespace Testing
	{
		namespace Timers
		{
			
//#define PRINT_TIME

			static clock_t GetTime()
			{
				return clock();
			};

			static double GetTimeInSeconds(clock_t &p_lastTimer, clock_t &p_firstTimer)
			{
				return  ((double)p_lastTimer - p_firstTimer) / (double)CLOCKS_PER_SEC;
			};
			
			class Timer
			{
			private:
				clock_t m_timer1;
				clock_t m_timer2;
				bool m_firstTimeTaken = false;

				double GetTime()
				{
					return  ((double)m_timer2 - m_timer1) / (double)CLOCKS_PER_SEC;
				}

			public:
				void TakeTime()
				{
					if(!m_firstTimeTaken)
					{
						m_timer1 = clock();
						m_firstTimeTaken = true;
					}
					else
					{
						m_timer2 = clock();
						#ifdef PRINT_TIME
							std::cout << GetTime() << std::endl;
						#endif //PRINT_TIME		
						m_firstTimeTaken = false;
					}
				}
			};

		}
	}
}