//#include "./CudaApplication-n2.h"
//
//NBody::Cuda::CudaApplication_n2::CudaApplication_n2(std::string p_inputFilePath, float p_deltaT) : 
//	Application(p_inputFilePath, p_deltaT, 0)
//{}
//
//
//
//__host__ __device__ Vector2 NBody::Cuda::CudaApplication_n2::CalculateForce(Body* p_currentBody, Body* p_otherBody)
//{
//	Vector2 direction = p_currentBody->Position - p_otherBody->Position;
//	// to avoid a singularity
//	float distance = max(0.5f * (p_otherBody->Mass + p_currentBody->Mass), direction.Length());
//	Vector2 tempForce = direction / (distance * distance * distance)  * (p_otherBody->Mass / 1);
//	return tempForce;
//}
//__host__ __device__ void NBody::Cuda::CudaApplication_n2::CalculateForce(Body* p_currentBody, Body* p_otherBody, Vector2* p_force)
//{
//	//direction of the force between the two bodies 
//	Vector2 direction = p_currentBody->Position - p_otherBody->Position;
//	// to avoid a singularity
//	float distance = max(0.5f * (p_otherBody->Mass + p_currentBody->Mass), direction.Length());
//	//calculate the 
//	Vector2 tempForce = direction / (distance * distance * distance)  * (p_otherBody->Mass / 1);
//	(*p_force) += tempForce;
//}
//
//__host__ __device__ uint_t NBody::Cuda::CudaApplication_n2::GetGlobalIndex()
//{
//	int blockDimension = blockDim.x * blockDim.y * blockDim.z;
//	int blockInnerIndex = threadIdx.x + blockDim.x * threadIdx.y;
//	int globalIndex = blockInnerIndex + gridDim.x * blockIdx.y * blockDimension + blockDimension * blockIdx.x;
//	return globalIndex;
//}
//
//__host__ __device__ dim3 NBody::Cuda::CudaApplication_n2::CreateParallelisationStructure(const int p_bodyCount)
//{
//	cudaDeviceProp prop;
//	//get device 0
//	dim3 configuration;
//
//	cudaGetDeviceProperties(&prop, 0);
//
//	//number of threads per block
//	int maxNumberThreads = prop.maxThreadsPerBlock;
//	//max number of blocks in the x direction
//	int maxNumberBlocks = prop.maxGridSize[1];
//
//	if (p_bodyCount < maxNumberThreads)
//	{
//		configuration.x = p_bodyCount;
//		configuration.y = 1;
//		configuration.z = 1;
//	}
//	else
//	{
//		int amountBlocks = std::ceilf(p_bodyCount / maxNumberThreads);
//
//		configuration.x = maxNumberThreads;
//		configuration.y = amountBlocks;
//		configuration.z = 1;
//	}
//
//	return configuration;
//}
//
// __global__ void NBody::Cuda::CudaApplication_n2::ComputeIteration(Body* p_bodies, Body* p_updatedBodies, float p_deltaT, int p_bodyCount, float p_gTerm)
//{
//	int blockDimension = blockDim.x * blockDim.y * blockDim.z;
//	int blockInnerIndex = threadIdx.x + blockDim.x * threadIdx.y;
//	int globalIndex = blockInnerIndex + gridDim.x * blockIdx.y * blockDimension + blockDimension * blockIdx.x;
//
//	if (globalIndex < p_bodyCount)
//	{
//		Vector2 force(0), acceleration(0);
//		Body chosenBody = p_bodies[globalIndex];
//
//		for (int index = 0; index < p_bodyCount; index++)
//		{
//			if (index == globalIndex) continue;
//			CalculateForce(&chosenBody, &p_bodies[index], &force);
//		}
//
//		acceleration = force * p_gTerm;
//		Body *outcome = &p_updatedBodies[globalIndex];
//
//		p_updatedBodies[globalIndex].Velocity += acceleration * p_deltaT;
//		p_updatedBodies[globalIndex].Position += outcome->Velocity * p_deltaT;
//	}
//}
//
//__host__ __device__ bool NBody::Cuda::CudaApplication_n2::Initialise()
//{
//	bool success = false;
//	m_allBodies = Core::System::IO::ParseBodiesInputFile(".//InputFiles//input_4096.txt", &m_bodyCount, &success);
//
//	if (!success)
//	{
//		std::cout << "Bodies not loaded properly!" << std::endl;
//		Core::System::IO::Pause();
//		return false;
//	}
//
//	CheckForErrors(cudaMalloc(&d_allBodies, sizeof(Body) * m_bodyCount));
//	CheckForErrors(cudaMalloc(&d_allUpdatedBodies, sizeof(Body) * m_bodyCount));
//	CheckForErrors(cudaMemcpy(d_allBodies, m_allBodies, m_bodyCount * sizeof(Body), cudaMemcpyHostToDevice));
//	CheckForErrors(cudaMemcpy(d_allUpdatedBodies, m_allBodies, m_bodyCount * sizeof(Body), cudaMemcpyHostToDevice));
//
//	return true;
//}
//
//__host__ void NBody::Cuda::CudaApplication_n2::Terminate()
//{
//	cudaFree(d_allBodies);
//	cudaFree(d_allUpdatedBodies);
//
//	delete[] m_allBodies;
//	delete[] m_allBodiesDump;
//}
//
