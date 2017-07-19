#ifndef __THREADPOOL__
#define __THREADPOOL__

#include <vector>
#include <mutex>
#include <iostream>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

using boost::thread;
using boost::thread_group;

class ThreadPool
{
public:
	//boost::asio::io_service io_service;
	//
	//boost::thread_group threads;

	//void TestFunction(int sleepSecons)
	//{
	//	while (1)
	//	{
	//		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	//		std::cout << "test thread will re sleep for 1000" << std::endl;
	//	}
	//}

	//ThreadPool(const int& index = 1)
	//{
	//	boost::asio::io_service::work work(io_service);

	//	for (int threadIndex = 0; threadIndex < index; ++threadIndex)
	//	{
	//		threads.create_thread(
	//			boost::bind(&boost::asio::io_service::run, &io_service)
	//		);
	//	}

	//	io_service.post(boost::bind(TestFunction, 11));

	//}

	//~ThreadPool()
	//{
	//	io_service.stop();
	//	//threads.join_all();
	//}
};



#endif // !__THREADPOOL__
