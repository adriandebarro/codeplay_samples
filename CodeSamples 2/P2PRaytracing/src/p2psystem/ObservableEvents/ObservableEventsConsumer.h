#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <tuple>

#include "./ObservableEventsList/ObservaleEventsList.h"

namespace Network
{
	namespace ObservableEvents
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	An observable events consumer. </summary>
		///	<remark>	Events received from the network are pushed over to queue, such that they can be consumed by different threads </remark>
		/// <remarks>	Adrian, 03/10/2016. </remarks>
		////////////////////////////////////////////////////////////////////////////////////////////////////
		template<typename T, typename S, typename AS>
		class ObservableEventsConsumer
		{
		public:

			ObservableEventsConsumer()
			{}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Constructor. </summary>
			///
			/// <remarks>	Adrian, 07 </remarks>
			///
			/// <param name="p_observableEventList">	[in,out] If non-null, list of observable events. </param>
			////////////////////////////////////////////////////////////////////////////////////////////////////

			ObservableEventsConsumer(ObservableEventsList<T,S,AS>* p_observableEventList)
			{
				m_observableEventsList = p_observableEventList;
				m_consumerThread = std::thread(&ShortcutMethod, this);
			}

			static void ShortcutMethod(ObservableEventsConsumer<T,S,AS>* p_instance)
			{
				p_instance->ProcessUpdates();
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Adds the events. </summary>
			///
			/// <remarks>	Adrian, 03/10/2016. </remarks>
			///
			/// <param name="p_currentUpdate">	The current update. </param>
			///
			////////////////////////////////////////////////////////////////////////////////////////////////////

			bool AddEvents(std::vector<unsigned char> p_currentUpdate, int p_startingFrom)
			{
				try
				{
					std::unique_lock<std::mutex> m(m_mutex);
					m_allEvents.push(std::tuple<std::vector<unsigned char>, int>(p_currentUpdate,p_startingFrom));
					return true;
				}
				catch (std::exception ex)
				{
					return false;
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Gets an event. </summary>
			///
			/// <remarks>	Adrian, 03/10/2016. </remarks>
			///
			/// <param name="p_toPut">	[in,out] to put. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			bool GetEvent(std::tuple<std::vector<unsigned char>, int>& p_result)
			{
				std::unique_lock<std::mutex> m(m_mutex);
				
				if (m_allEvents.size() == 0)
				{
					return false;
				}
				else
				{
					p_result = m_allEvents.front();
					m_allEvents.pop();
					return true;
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Process the updates. </summary>
			///
			/// <remarks>	Adrian, 03/10/2016. </remarks>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			void ProcessUpdates()
			{
				bool updateFound = false;
				while (true)
				{
					//std::cout << "New data" << std::endl;
					updateFound = false;
					std::tuple<vector<unsigned char>, int> currentUpdate;
					bool updateFound = GetEvent(currentUpdate);

					if (updateFound && std::get<0>(currentUpdate).size() > std::get<1>(currentUpdate))
					{
						m_observableEventsList->MergeEvents(std::get<0>(currentUpdate), std::get<1>(currentUpdate));
						m_observableEventsList->SetInvalidationToFalse();
					}
					else if (m_observableEventsList->GetInvalidationStatus())
					{
						m_observableEventsList->ExecuteInvalidation();
						m_observableEventsList->SetInvalidationToFalse();
					}
					else
						std::this_thread::sleep_for(std::chrono::seconds(4));
				}
			}

		private:
			std::queue<std::tuple<std::vector<unsigned char>,int>> m_allEvents;
			std::condition_variable m_cond;
			std::mutex m_mutex;
			ObservableEventsList<T,S,AS>* m_observableEventsList;
			std::thread m_consumerThread;
		};
	}
}


