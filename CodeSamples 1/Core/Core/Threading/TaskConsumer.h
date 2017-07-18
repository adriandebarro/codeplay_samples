#pragma once 

#include <iostream>


namespace Core
{
	namespace Threading
	{
		class TaskConsumer
		{
			public:
				virtual void Execute() = 0;

		};	
	}
}
