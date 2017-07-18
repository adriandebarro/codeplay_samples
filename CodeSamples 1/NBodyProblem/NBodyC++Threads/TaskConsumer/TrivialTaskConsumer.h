#pragma once 
#include <thread>

#include <boost/thread/barrier.hpp>

#include <Threading/TaskConsumer.h>
#include <Scheduler/TaskScheduler.h>
#include <Maths/Vector2.h>

using namespace Core::Threading;
using namespace Core::TaskSchedulers;
using namespace Core::Maths::Vector;

namespace NBody
{
	namespace TaskConsumer
	{
		class SpanTaskConsumer : public TaskConsumer
		{
			private:
				boost::barrier *m_barrier1;
				boost::barrier *m_barrier2;
				TaskScheduler<Body> *m_taskScheduler;
				uint_t m_bodyCount;
				float m_deltaT;
				Vector2* m_computedAcceleration;
				int m_threadIndex;
				int m_cacheTileSize;
				
			public:
				SpanTaskConsumer():
					m_barrier1(nullptr),
					m_barrier2(nullptr),
					m_threadIndex(0),
					m_bodyCount(0),
					m_deltaT(0),
					m_computedAcceleration(nullptr)
				{}

				SpanTaskConsumer(boost::barrier* p_barrier1, boost::barrier* p_barrier2, Vector2* p_computedAcceleration, int p_threadIndex,int p_bodyCount, float p_deltaT, TaskScheduler<Body> *p_taskScheduler) :
					m_barrier1(p_barrier1),
					m_barrier2(p_barrier2),
					m_threadIndex(p_threadIndex),
					m_bodyCount(p_bodyCount),
					m_deltaT(p_deltaT),
					m_computedAcceleration(p_computedAcceleration),
					m_taskScheduler(p_taskScheduler),
					m_cacheTileSize(0)
				{}

				virtual void Execute()
				{
					bool success = true;
					Task<Body>* currentTask = nullptr;

					for (int iterationIndex = 0; iterationIndex < 1000; iterationIndex++)
					{
						bool value = m_barrier1->wait();

						do
						{
							success = false;
							//get task 
							currentTask = m_taskScheduler->GetNextTask(&success);

							//if task found 
							if (success)
							{
								//improve through loop tiling----
								for (int currentIndex = currentTask->m_startingIndex, index = 0; index < currentTask->m_span; currentIndex++, index++)
								{
									Body currentBody = currentTask->m_arrayStartingPoint[currentIndex];
									Vector2 resultingForce(0), acceleration(0);
									for (int bodyIndex = 0; bodyIndex < m_bodyCount; bodyIndex++)
									{
										if (currentIndex == bodyIndex) continue;
										//direction of the force between the two bodies 
										Vector2 direction = currentBody.Position - currentTask->m_arrayStartingPoint[bodyIndex].Position;
										// avoids a singularity
										float distance = max(0.5f * (currentBody.Mass + currentTask->m_arrayStartingPoint[bodyIndex].Mass), direction.Length());
										//calculate the resulting force
										resultingForce += direction / (distance * distance * distance)  * (currentTask->m_arrayStartingPoint[bodyIndex].Mass / 1);
									}
									acceleration = resultingForce * GTerm;
									m_computedAcceleration[index] = acceleration;
								}
							}
						} 
						while (success && currentTask != nullptr);
						m_barrier2->wait();
					}
				}
		};
	}

}
