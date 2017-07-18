#pragma once 

#include <iostream>
#include <vector>
#include <atomic>
#include <assert.h>

#include "../Common/Common.h"
#include "../System/Util.h"
#include "../System/PropertiesList.h"

using namespace Core::Common;

namespace Core
{
	namespace TaskSchedulers
	{
		template<typename T>
		struct Task
		{
			int m_span;
			int m_startingIndex;
			T *m_arrayStartingPoint;

			
			Task(int p_span, int p_startingIndex, T* p_arrayStartingPoint):
				m_span(p_span),
				m_startingIndex(p_startingIndex),
				m_arrayStartingPoint(p_arrayStartingPoint)
			{}
		};

		template<typename T>
		class TaskScheduler 
		{
		private:
			std::vector<Task<T>*> m_allTasks;
			std::atomic_int  m_currentTaskIndex;
			T* m_allBodies;
			uint_t m_bodyCount;
			uint_t m_totalTasks;
			int m_taskSize; 

			virtual void ProcessObjects(T* p_allBodies, uint_t p_bodyCount)
			{
				_ASSERT(p_allBodies != nullptr && p_bodyCount > 0);
				int remaning = 0;

				for (int taskIndex = 0; taskIndex < m_bodyCount; taskIndex += m_taskSize)
				{
					remaning = m_bodyCount - taskIndex;
					m_allTasks.push_back(new Task<T>(remaning < m_taskSize? remaning : m_taskSize, taskIndex, m_allBodies));
					m_totalTasks++;
				}
			}

		public:

			TaskScheduler(T* p_allBodies, uint_t p_bodyCount) :
				m_allBodies(p_allBodies),
				m_bodyCount(p_bodyCount),
				m_totalTasks(0),
				m_taskSize(0)
			{
				std::atomic_store(&m_currentTaskIndex, 0);
				m_taskSize = CalculateTaskSize();
			}

			TaskScheduler(T* p_allBodies, uint_t p_bodyCount, int p_taskSize) :
				m_allBodies(p_allBodies),
				m_bodyCount(p_bodyCount),
				m_totalTasks(0),
				m_taskSize(p_taskSize)
			{
				std::atomic_store(&m_currentTaskIndex, 0);
				ProcessObjects(m_allBodies, m_bodyCount);
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	
			///    Atomic increment of the index of the task assigned and 
			///  </summary>
			///
			/// <remarks>	Adrian, 12/07/2017. </remarks>
			///
			/// <param name="p_success">	[in,out] If non-null, the success. </param>
			///
			/// <returns>	null if it fails, else the next task. </returns>
			///-------------------------------------------------------------------------------------------------
			Task<T>* GetNextTask(bool *p_success)
			{
				uint_t fetchedElement = std::atomic_fetch_add(&m_currentTaskIndex, 1);

				if (fetchedElement < m_totalTasks)
				{
					*p_success = true;
					return m_allTasks[fetchedElement];
				}
				else
				{
					p_success = false;
					return nullptr;
				}
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	
			/// 	Resets the atomic index of task to 0 
			/// </summary>
			///
			/// <remarks>	Adrian, 12/07/2017. </remarks>
			///-------------------------------------------------------------------------------------------------

			void PrepNextIteration()
			{
				std::atomic_store(&m_currentTaskIndex, 0);
			}

			int GetTaskSize()
			{
				return m_taskSize;
			}

		};
	}
}
