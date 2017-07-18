
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <vector>
#include <thread>

#include <System/Util.h>
#include <Bodies/Particle.h>
#include <Common/Common.h>
#include <Maths/Vector2.h>


using Core::Maths::Vector::Vector2;
using namespace Core::Common;

// Enables the use of core classes to the device
#define __CUDACC__

//to be used to switch between the different methods
//#define NORMAL_PARALLELISATION

///-------------------------------------------------------------------------------------------------
/// <summary>	Calculates the force. </summary>
///
/// <remarks>	Adrian, 10/07/2017. </remarks>
///
/// <param name="p_currentBody">	[in,out] If non-null, the current body. </param>
/// <param name="p_otherBody">  	[in,out] If non-null, the other body. </param>
/// <param name="p_force">			[in,out] If non-null, the force. </param>
///-------------------------------------------------------------------------------------------------
__device__ void CalculateForce(Body* p_currentBody, Body* p_otherBody, Vector2* p_force)
{
	//direction of the force between the two bodies 
	Vector2 direction = p_currentBody->Position - p_otherBody->Position;
	// to avoid a singularity
	float distance = max(0.5f * (p_otherBody->Mass + p_currentBody->Mass), direction.Length());
	//calculate the 
	Vector2 tempForce = direction / (distance * distance * distance)  * (p_otherBody->Mass / 1);
	(*p_force) += tempForce;
}

///-------------------------------------------------------------------------------------------------
/// <summary>	Calculates the force. </summary>
///
/// <remarks>	Adrian, 10/07/2017. </remarks>
///
/// <param name="p_currentBody">	[in,out] If non-null, the current body. </param>
/// <param name="p_otherBody">  	[in,out] If non-null, the other body. </param>
///
/// <returns>	The calculated force. </returns>
///-------------------------------------------------------------------------------------------------
__device__ Vector2 CalculateForce(Body* p_currentBody, Body* p_otherBody)
{
	Vector2 direction = p_currentBody->Position - p_otherBody->Position;
	// avoids singularity
	float distance = max(0.5f * (p_otherBody->Mass + p_currentBody->Mass), direction.Length());
	Vector2 tempForce = direction / (distance * distance * distance)  * (p_otherBody->Mass / 1);
	return tempForce;
}

///-------------------------------------------------------------------------------------------------
/// <summary>	
/// 	Gets global index of a CUDA thread
/// </summary>
///
/// <remarks>	Adrian, 10/07/2017. </remarks>
///
/// <returns>	The global index. </returns>
///-------------------------------------------------------------------------------------------------
__device__ uint_t GetGlobalIndex()
{
	int blockDimension = blockDim.x * blockDim.y * blockDim.z;
	int blockInnerIndex = threadIdx.x + blockDim.x * threadIdx.y;
	int globalIndex = blockInnerIndex + gridDim.x * blockIdx.y * blockDimension + blockDimension * blockIdx.x;
	return globalIndex;
}


///-------------------------------------------------------------------------------------------------
/// <summary>
/// 	This method improves further on, by computing reducing the required computations (n^2)/2
/// </summary>
///
/// <remarks>	Adrian, 10/07/2017. </remarks>
///
/// <param name="p_bodies">			 	[in,out] If non-null, the bodies. </param>
/// <param name="p_decomposedForces">	[in,out] If non-null, the decomposed forces. </param>
/// <param name="p_deltaT">			 	The delta t. </param>
/// <param name="p_bodyCount">		 	Number of bodies. </param>
/// <param name="p_gTerm">			 	The term. </param>
///-------------------------------------------------------------------------------------------------
__global__ void ComputeIterationOptimised
		(Body* p_bodies, Vector2* p_decomposedForces , uint_t* p_startingPositions, int p_bodyCount ,float p_deltaT,  float p_gTerm)
{
	uint_t globalIndex = GetGlobalIndex();

	if (globalIndex < p_bodyCount - 1)
	{
		Body assignedBody = p_bodies[globalIndex];
		int matrixIndex = 0;
		int threadBodyCount = p_bodyCount;

		//find index in the matrix
		threadBodyCount -= 1;
		for (int count = 0; count < globalIndex; count++)
		{
			matrixIndex += threadBodyCount;
			threadBodyCount -= 1;
		}
		
		//save decomposedArray index starting position
		p_startingPositions[globalIndex] = matrixIndex;

		//fill the portion of forces at the specific indices and skip force computation with thy self
		for (int index = globalIndex+1; index < p_bodyCount; index++, matrixIndex++)
		{
			p_decomposedForces[matrixIndex] = CalculateForce(&assignedBody, &p_bodies[index]);
		}
	}
}

///-------------------------------------------------------------------------------------------------
/// <summary>	
/// 			Merge results (used for the secondary method)
/// </summary>
///
/// <remarks>	Adrian, 10/07/2017. </remarks>
///
/// <param name="p_bodies">			 	[in,out] If non-null, the bodies. </param>
/// <param name="p_decomposedForces">	[in,out] If non-null, the decomposed forces. </param>
/// <param name="p_bodyCount">		 	Number of bodies. </param>
///-------------------------------------------------------------------------------------------------
__global__ void MergeResults
		(Body* p_bodies, Vector2* p_decomposedForces, uint_t* p_startingPositions ,int p_bodyCount, float p_gTerm, float p_deltaT)
{
	uint_t globalIndex = GetGlobalIndex();
	Vector2 force(0), acceleration(0);

	if (globalIndex > 0)
	{
		//find the starting index from the forces array 
		uint_t startingIndex = threadIdx.x;
		uint_t jumpLength = p_bodyCount - 2;
		uint_t arrayIndex = threadIdx.x;
		uint_t remaningBodies = p_bodyCount - 1;

		//execute jump indices to get force aggregation
		for (int jumpIndex = startingIndex; jumpIndex > 0; jumpIndex--)
		{
			force += p_decomposedForces[arrayIndex];
			arrayIndex += jumpLength;
			jumpLength--, remaningBodies--;
		}

		arrayIndex = p_startingPositions[globalIndex];
		//execute linear summation of remaning force components 
		for (int index = 0; index < remaningBodies; index++, arrayIndex++)
		{
			force += p_decomposedForces[arrayIndex];
		}
	}
	//else if index  equal to 0 just traverse the first elements of the forces array
	else
	{
		for (int index = 0; index < p_bodyCount - 1; index++)
		{
			force += p_decomposedForces[index];
		}
	}

	//compute the new position
	acceleration = force * p_gTerm;
	Body *outcome = &p_bodies[globalIndex];

	p_bodies[globalIndex].Velocity += acceleration * p_deltaT;
	p_bodies[globalIndex].Position += outcome->Velocity * p_deltaT;
}

///-------------------------------------------------------------------------------------------------
/// <summary>
/// 	Trivial compute iteration.
/// 	Parallelise by spawning 1 thread per body 
/// </summary>
///
/// <remarks>	Adrian, 10/07/2017. </remarks>
///
/// <param name="p_bodies">		  	[in,out] If non-null, the bodies. </param>
/// <param name="p_updatedBodies">	[in,out] If non-null, the updated bodies. </param>
/// <param name="p_deltaT">		  	The delta t. </param>
/// <param name="p_bodyCount">	  	Number of bodies. </param>
/// <param name="p_gTerm">		  	The term. </param>
///-------------------------------------------------------------------------------------------------

__global__ void TrivialComputeIteration(Body* p_bodies, Body* p_updatedBodies, float p_deltaT, int p_bodyCount, float p_gTerm )
{
	int blockDimension = blockDim.x * blockDim.y * blockDim.z;
	int blockInnerIndex = threadIdx.x + blockDim.x * threadIdx.y;
	int globalIndex = blockInnerIndex + gridDim.x * blockIdx.y * blockDimension + blockDimension * blockIdx.x;

	if (globalIndex < p_bodyCount )
	{
		Vector2 force(0), acceleration(0);
		Body chosenBody = p_bodies[globalIndex];

		for (int index = 0; index < p_bodyCount; index++)
		{
			if (index == globalIndex) continue;
			CalculateForce(&chosenBody, &p_bodies[index], &force);
		}

		acceleration = force * p_gTerm;
		Body *outcome = &p_updatedBodies[globalIndex];

		p_updatedBodies[globalIndex].Velocity += acceleration * p_deltaT;
		p_updatedBodies[globalIndex].Position += outcome->Velocity * p_deltaT;
	}
}

///-------------------------------------------------------------------------------------------------
/// <summary>	
/// 			Creates parallelisation structure.
/// 			Creates the parallelisation structure through a greedy approach
/// </summary>
///
/// <remarks>	Adrian, 09/07/2017. </remarks>
///
/// <param name="p_bodyCount">	Number of bodies. </param>
///
/// <returns>	Returns dimensions of grid and block size. </returns>
///-------------------------------------------------------------------------------------------------
__host__ dim3 CreateParallelisationStructure(const int p_bodyCount)
{
	cudaDeviceProp prop;
	//get device 0
	dim3 configuration;
	
	cudaGetDeviceProperties(&prop, 0);
	
	//number of threads per block
	int maxNumberThreads = prop.maxThreadsPerBlock;
	//max number of blocks in the x direction
	int maxNumberBlocks = prop.maxGridSize[1];

	if (p_bodyCount < maxNumberThreads)
	{
		configuration.x = p_bodyCount;
		configuration.y = 1;
		configuration.z = 1;
	}
	else
	{
		int amountBlocks = std::ceilf(p_bodyCount / maxNumberThreads);
		
		configuration.x = maxNumberThreads;
		configuration.y = amountBlocks;
		configuration.z = 1;
	}

	return configuration;
}

///-------------------------------------------------------------------------------------------------
/// <summary>	
/// 	Main entry-point for this application. 
/// 	Switch between the two methods via the definition NORMAL_PARALLELISATION
/// </summary>
///
/// <remarks>	Adrian, 10/07/2017. </remarks>
///
/// <param name="argc">	Number of command-line arguments. </param>
/// <param name="argv">	Array of command-line argument strings. </param>
///
/// <returns>	Exit-code for the process - 0 for success, else an error code. </returns>
///-------------------------------------------------------------------------------------------------
int main(int argc, char* argv)
{
	
	cudaSetDevice(0);
	cudaDeviceSynchronize();
	cudaThreadSynchronize();

	int bodyCount = 0;
	bool success = false;

	float gTerm = 20.f;
	float deltaT = 0.1;

	Body *d_allBodies = nullptr;
	Body *d_allUpdatedBodies = nullptr;
	Vector2 *d_forcesMatrix = nullptr;
	uint_t* d_matrixIndex = nullptr;

	Body* allBodies = Core::System::IO::ParseBodiesInputFile(".//InputFiles//input_4096.txt", &bodyCount, &success);
	Body* allBodiesDump = new Body[bodyCount];

	if (!success)
	{
		std::cout << "Bodies not loaded properly!" << std::endl;
		Core::System::IO::Pause();
		return -1;
	}

	CheckForErrors(cudaMalloc(&d_allBodies, sizeof(Body) * bodyCount));
	CheckForErrors(cudaMalloc(&d_allUpdatedBodies, sizeof(Body) * bodyCount));
	CheckForErrors(cudaMemcpy(d_allBodies, allBodies, bodyCount * sizeof(Body), cudaMemcpyHostToDevice));
	CheckForErrors(cudaMemcpy(d_allUpdatedBodies, allBodies, bodyCount * sizeof(Body), cudaMemcpyHostToDevice));

	dim3 parallel = CreateParallelisationStructure(bodyCount);

	#ifdef NORMAL_PARALLELISATION
		for (int index = 0; index < 1000; index++)
		{
			Core::System::Util::WriteProgressMessage(index + 1);
			TrivialComputeIteration <<<parallel.y, parallel.x >>> (d_allBodies, d_allUpdatedBodies, deltaT, bodyCount, gTerm);
			cudaDeviceSynchronize();
			CheckForErrors(cudaMemcpy(allBodiesDump, d_allUpdatedBodies, bodyCount * sizeof(Body), cudaMemcpyDeviceToHost));
			CheckForErrors(cudaMemcpy(d_allBodies, d_allUpdatedBodies, bodyCount * sizeof(Body), cudaMemcpyDeviceToDevice));
			//dump data to file
			Core::System::IO::WriteBodiesToFile(index, allBodiesDump, bodyCount);
		}
	#else
		
		int matrixSize = (bodyCount * bodyCount) - bodyCount / 2;

		CheckForErrors(cudaMalloc(&d_forcesMatrix, matrixSize * sizeof(float)));
		CheckForErrors(cudaMalloc(&d_matrixIndex, matrixSize * sizeof(int)));

		for (int index = 0; index < 1000; index++)
		{
			Core::System::IO::WriteProgressMessage(index + 1);
			ComputeIterationOptimised <<<parallel.y, parallel.x>>>(d_allBodies, d_forcesMatrix, d_matrixIndex, bodyCount, gTerm, deltaT);
			cudaDeviceSynchronize();
			MergeResults <<<parallel.y, parallel.x>>>(d_allBodies, d_forcesMatrix, d_matrixIndex, bodyCount, 20.f, 0.5f);
			CheckForErrors(cudaMemcpy(allBodiesDump, d_allBodies, bodyCount * sizeof(Body), cudaMemcpyDeviceToHost));
			Core::System::IO::WriteBodiesToFile(index, allBodiesDump, bodyCount);
		}

	#endif 
	cudaFree(d_allBodies);
	cudaFree(d_allUpdatedBodies);

	delete[] allBodies;
	delete[] allBodiesDump;

	Core::System::IO::Pause();

   return 0;
}


