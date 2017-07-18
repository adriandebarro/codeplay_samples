#pragma once 

#include <string>
#include <thread>

#include <boost/thread/barrier.hpp>

#include <System/Application.h>
#include <Common/Common.h>
#include <System/Util.h>
#include <Scheduler/TaskScheduler.h>
#include <Maths/Vector2.h>
#include <Threading/TaskConsumer.h>

#include "../TaskConsumer/TrivialTaskConsumer.h"

using namespace Core::Common;
using namespace Core::Threading;



namespace NBody
{
	namespace Application
	{
		
		class NBodyThread : public Core::Application::Application
		{
			friend NBody::TaskConsumer::SpanTaskConsumer;
			private:
				virtual void ExecuteApplication();

				Body* m_allBodies;
				Core::Maths::Vector::Vector2 *m_accelerationValues;

				int m_bodyCount;

				boost::barrier *m_barrier1;
				boost::barrier *m_barrier2;

				std::thread* m_threads;
				NBody::TaskConsumer::SpanTaskConsumer **m_taskConsumers;

				Core::TaskSchedulers::TaskScheduler<Body> *m_taskScheduler;
				
				int m_cacheTileSize;


			public:
				NBodyThread(std::string p_inputFilePath, float p_deltaT, int m_numThreads);

				virtual bool Initialise();
				virtual void Terminate();

				void SpawnThreads();
				static void BindExecutionThread(NBodyThread *p_globalApplication, int p_threadIndex);
		};
	}

}

