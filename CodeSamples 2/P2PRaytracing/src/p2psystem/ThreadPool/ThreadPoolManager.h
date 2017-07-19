#ifndef __THREADPOOLMANAGER__
#define __THREADPOOLMANAGER__

#include <threadpoolapiset.h>
#include <queue>
#include <vector>

using std::vector;
using std::queue;

//struct Task
//{
//	Task(void* funcPointer, vector<auto> argumentList)
//	{
//
//	}
//
//	void* functionPointer;
//
//};


class ThreadPoolManger
{
private:
	// *threadPool;
	std::queue<int> testQueue;
public:
	void CheckLocalHardware();
};

#endif // !__THREADPOOLMANAGER__
