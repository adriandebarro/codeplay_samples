#include "./ThreadApplication.h"

NBody::Application::NBodyThread::NBodyThread(std::string p_inputFilePath, float p_deltaT, int p_numThreads):
	Application(p_inputFilePath, p_deltaT, p_numThreads),
	m_allBodies(nullptr),
	m_bodyCount(0),
	m_cacheTileSize(0)
{}

bool NBody::Application::NBodyThread::Initialise()
{
	_ASSERT(m_inputFilePath != "" && m_bodyCount == 0);
	bool success = false;
	int align = 0, cacheLineSize = Core::System::Util::GetCacheLineSize();
	m_allBodies = Core::System::IO::ParseBodiesInputFile(GetInputFilePath(), &m_bodyCount, &success);

	if (!success)
	{
		std::cout << "An error occurred while parsing the input file" << std::endl;
		return false;
	}

	//accounting for main thread
	m_barrier1 = new boost::barrier(m_numThreads + 1);
	m_barrier2 = new boost::barrier(m_numThreads + 1);

	m_taskScheduler = new Core::TaskSchedulers::TaskScheduler<Body>(m_allBodies, m_bodyCount, 10);
	
	m_threads = new std::thread[m_numThreads];
	m_accelerationValues = new Core::Maths::Vector::Vector2[m_bodyCount];
	m_taskConsumers = new NBody::TaskConsumer::SpanTaskConsumer*[m_numThreads];

	align = cacheLineSize / sizeof(Body);
	m_cacheTileSize = m_bodyCount / std::sqrt(m_numThreads) / 4;
	m_cacheTileSize = (m_cacheTileSize + align - 1) / align * align;
	
	return true;
}

void NBody::Application::NBodyThread::BindExecutionThread(NBody::Application::NBodyThread *p_application, int p_threadIndex)
{
	_ASSERT(p_application != nullptr && p_threadIndex >= 0);
	p_application->m_taskConsumers[p_threadIndex] = new NBody::TaskConsumer::SpanTaskConsumer
		(p_application->m_barrier1, p_application->m_barrier2, p_application->m_accelerationValues, p_threadIndex, p_application->m_bodyCount, p_application->m_deltaT, p_application->m_taskScheduler);
	p_application->m_taskConsumers[p_threadIndex]->Execute();
}


void NBody::Application::NBodyThread::SpawnThreads()
{
	_ASSERT(m_numThreads > 0);
	for (int threadIndex = 0; threadIndex < m_numThreads; threadIndex++)
	{
		m_threads[threadIndex] = std::thread(&BindExecutionThread, this, threadIndex);
	}
}

void NBody::Application::NBodyThread::ExecuteApplication()
{
	SpawnThreads();
	//outer loop for iterations
	for (int index = 0; index < 1000; index++)
	{
		Core::System::IO::WriteProgressMessage(index);
		//start computations
		m_barrier1->wait();
		
		//add the new position and velocity
		for (int bodyIndex = 0; bodyIndex < m_bodyCount; bodyIndex++)
		{
			m_allBodies[bodyIndex].Velocity += m_accelerationValues[bodyIndex] * m_deltaT;
			m_allBodies[bodyIndex].Position += m_allBodies[bodyIndex].Velocity * m_deltaT;
		}
		
		Core::System::IO::WriteBodiesToFile(index, m_allBodies, m_bodyCount);

		//restart tasks for the next iteration
		m_taskScheduler->PrepNextIteration();

		//reset the accelerations to 0
		for (int accelerationIndex = 0; accelerationIndex < m_bodyCount; accelerationIndex++)
		{
			m_accelerationValues[accelerationIndex] = Core::Maths::Vector::Vector2(0);
		}

		//finished from the serial part
		m_barrier2->wait();
	}
	std::cout << std::endl;
}

void NBody::Application::NBodyThread::Terminate()
{
	delete[]m_accelerationValues;
	m_accelerationValues = nullptr;

	NBody::TaskConsumer::SpanTaskConsumer *currentConsumer;
	for (int threadIndex = 0; threadIndex < m_numThreads; threadIndex++)
	{
		currentConsumer = m_taskConsumers[threadIndex];
		delete currentConsumer;
		currentConsumer = nullptr;
	}

	delete[] m_taskConsumers;
	delete m_barrier1;
	delete m_barrier2;
}

