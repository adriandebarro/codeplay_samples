//#include <iostream>
//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
//
//#include <System/Application.h>
//#include <Maths/Vector2.h>
//#include <System/Util.h>
//
//using namespace Core::Maths::Vector;
//using namespace Core::Common;
//
//namespace NBody
//{
//	namespace Cuda 
//	{
//		#define __CUDACC__
//
//		class CudaApplication_n2 : public Core::Application::Application
//		{
//			private:	
//
//				Body *d_allBodies = nullptr;
//				Body *d_allUpdatedBodies = nullptr;
//
//				Body* m_allBodies;
//				Body* m_allBodiesDump;
//				int m_bodyCount;
//
//				__host__ virtual void ExecuteApplication()
//				{
//					dim3 parallel = CreateParallelisationStructure(m_bodyCount);
//					for (int index = 0; index < 1000; index++)
//					{
//						Core::System::IO::WriteProgressMessage(index + 1);
//						ComputeIteration <<<parallel.y, parallel.x >> > (d_allBodies, d_allUpdatedBodies, m_deltaT, m_bodyCount, GTerm);
//						cudaDeviceSynchronize();
//						CheckForErrors(cudaMemcpy(m_allBodiesDump, d_allUpdatedBodies, m_bodyCount * sizeof(Body), cudaMemcpyDeviceToHost));
//						CheckForErrors(cudaMemcpy(d_allBodies, d_allUpdatedBodies, m_bodyCount * sizeof(Body), cudaMemcpyDeviceToDevice));
//						//dump data to file
//						Core::System::IO::WriteBodiesToFile(index, m_allBodiesDump, m_bodyCount);
//					}
//				}
//				__host__ __device__ Vector2 CalculateForce(Body*, Body*);
//				__host__ __device__ void CalculateForce(Body*, Body*, Vector2*);
//				__host__ __device__ uint_t GetGlobalIndex();
//				__global__ void ComputeIteration(Body* , Body* , float , int , float );
//				__host__ __device__ dim3 CreateParallelisationStructure(const int);
//
//			public:
//				CudaApplication_n2(std::string, float);
//				__host__ virtual bool Initialise();
//				__host__ virtual void Terminate();
//		};
//
//	}
//
//}
