#include "./OpenMPApplication.h"

///-------------------------------------------------------------------------------------------------
/// <summary>	Converts the objects from a  </summary>
///
/// <remarks>	Adrian, 18/07/2017. </remarks>
///-------------------------------------------------------------------------------------------------

void NBody::Application::OpenMPApplication::ConvertToStructureOfArrays()
{
	Body currentBody;
	for (int bodyIndex = 0; bodyIndex < m_bodyCount; ++bodyIndex)
	{
		currentBody = m_allBodies[bodyIndex];
		m_xCoor[bodyIndex] = currentBody.Position.X;
		m_yCoor[bodyIndex] = currentBody.Position.Y;

		m_bodyMass[bodyIndex] = currentBody.Mass;
		m_xVel[bodyIndex] = 0.f;
		m_yVel[bodyIndex] = 0.f;
	}

	__m256 test = _mm256_set1_ps(m_xCoor[0]);
}

bool NBody::Application::OpenMPApplication::Initialise()
{
	bool success = false;
	omp_set_num_threads(m_numThreads);
	
	m_allBodies = Core::System::IO::ParseBodiesInputFile(m_inputFilePath, &m_bodyCount, &success);

	if (!success)
	{
		std::cout << "Error while parsing file " << std::endl;
		Core::System::IO::Pause();
		return false;
	}
	
	m_xCoor = new a16_flt[m_bodyCount];
	m_yCoor = new a16_flt[m_bodyCount];
	m_xVel = new a16_flt[m_bodyCount];
	m_yVel = new a16_flt[m_bodyCount];
	m_bodyMass = new a16_flt[m_bodyCount];
	m_forceX = new a16_flt[m_bodyCount];
	m_forceY = new a16_flt[m_bodyCount];

	ConvertToStructureOfArrays();


	//create bitmask arrays for threads
	m_mask = new a16_int*[m_numThreads];

	for (int threadIndex = 0; threadIndex < m_numThreads; threadIndex++)
	{
		m_mask[threadIndex] = new a16_int[m_bodyCount];
	}

	return true;
}

///-------------------------------------------------------------------------------------------------
/// <summary>	 
/// 	Releases dynamically allocated constructs
/// </summary>
///
/// <remarks>	Adrian, 18/07/2017. </remarks>
///-------------------------------------------------------------------------------------------------
void NBody::Application::OpenMPApplication::Terminate()
{
	delete[] m_allBodies;
	delete[] m_xCoor;
	delete[] m_yCoor;
	delete[] m_xVel;
	delete[] m_yVel;
	delete[] m_forceX;
	delete[] m_forceY;

	for (int threadIndex = 0; threadIndex < m_numThreads; threadIndex++)
	{
		delete[] m_mask[threadIndex];
	}

	delete[] m_mask;
}

///-------------------------------------------------------------------------------------------------
/// <summary>	
/// 	Calculates the new position and velocity of each and every body 
/// </summary>
///
/// <remarks>	Adrian, 18/07/2017. </remarks>
///-------------------------------------------------------------------------------------------------
void NBody::Application::OpenMPApplication::MergeResults()
{
	#pragma omp parallel
	for (int index = 0; index < m_bodyCount; index++)
	{
		Vector2 currentForce = Vector2(m_forceX[index], m_forceY[index]);
		currentForce = currentForce * GTerm;
		//velocity
		m_xVel[index] += currentForce.X * m_deltaT;
		m_yVel[index] += currentForce.Y * m_deltaT;
		
		//new position
		m_xCoor[index] += m_xVel[index] * m_deltaT;
		m_yCoor[index] += m_yVel[index] * m_deltaT;

		//update position
		m_allBodies[index].Position = Vector2(m_xCoor[index], m_yCoor[index]);
		//m_allBodies[index].Velocity = Vector2(m_xVel[index], m_yVel[index]);
	}
}

///-------------------------------------------------------------------------------------------------
/// <summary>	Hsum in  SSE 3.
/// 		Taken from : https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-float-vector-sum-on-x86
///  </summary>
///
/// <remarks>	Adrian, 18/07/2017. </remarks>
///
/// <param name="v">	The __m128 to process. </param>
///
/// <returns>	horizontal summation </returns>
///-------------------------------------------------------------------------------------------------
float hsum_ps_sse3(__m128 v) 
{
	__m128 shuf = _mm_movehdup_ps(v);        // broadcast elements 3,1 to 2,0
	__m128 sums = _mm_add_ps(v, shuf);
	shuf = _mm_movehl_ps(shuf, sums); // high half -> low half
	sums = _mm_add_ss(sums, shuf);
	return _mm_cvtss_f32(sums);
}

///-------------------------------------------------------------------------------------------------
/// <summary>	
/// 	Computes the force between the bodies using SSE3			
/// </summary>
/// 
/// <remarks>	Adrian, 18/07/2017. </remarks>
///-------------------------------------------------------------------------------------------------

void NBody::Application::OpenMPApplication::ComputeForces()
{
	#pragma omp parallel 
	for (int i = 0; i < m_bodyCount; i++)
	{
		int threadID = omp_get_thread_num();
		a16_int *mask = m_mask[threadID];

		for (int index = 0; index < m_bodyCount; index+=4)
		{
			mask[index] = 0xFFFFFFFF;
			mask[index+1] = 0xFFFFFFFF;
			mask[index+2] = 0xFFFFFFFF;
			mask[index+3] = 0xFFFFFFFF;
		}
		
		//set mask for body 
		mask[i] = 0;

		//resulting new positions
		__m128 xi = _mm_setzero_ps();
		__m128 yi = _mm_setzero_ps();

		__m128 cxi = _mm_set1_ps(m_xCoor[i]);
		__m128 cyi = _mm_set1_ps(m_yCoor[i]);

		__m128 sample = _mm_load_ps(&m_xCoor[0]);

		for (int j = 0; j < m_bodyCount; j+=4)
		{
			a16_int* maskPtr = &mask[j];

			//issue here need to solve it, mask not working
			__m128i mask_0 = _mm_load_si128((__m128i*)(maskPtr));
			//instruction not working correctly (leaving hi 128 bits = 0)
			//__m128i mask_10 = _mm_set_m128i(mask_1, mask_0);
			
			//load positions at this point
			__m128 pX = _mm_load_ps(&m_xCoor[j]);
			__m128 pY = _mm_load_ps(&m_yCoor[j]);
			__m128 mj = _mm_load_ps(&m_bodyMass[j]);

			//find direction
			pY = _mm_sub_ps(pY, cyi);
			pX = _mm_sub_ps(pX, cxi);

			//  distance squared 
			__m128 r2X = _mm_set1_ps(0.0f);
			__m128 r2Y = _mm_set1_ps(0.0f);
			r2X = _mm_mul_ps(pX, pX);
			r2Y = _mm_mul_ps(pY, pY);
			__m128 r2 = _mm_set1_ps(0.0f);
			r2 = _mm_add_ps(r2X, r2Y);

			//compute 1 / sqrt(r2) * m/1
			__m128 invR = _mm_set1_ps(0.0f);
			invR = _mm_rsqrt_ps(r2);
			__m128 invR_ = _mm_mul_ps(invR, invR);
			invR_ = _mm_mul_ps(invR_, invR);
			invR_ = _mm_mul_ps(invR_, mj);
			pX = _mm_mul_ps(pX, invR_);
			pY = _mm_mul_ps(pY, invR_);

			//remove any of the arithmetic on the same body i . i
			pX = _mm_maskload_ps((float*)&pX, mask_0);
			pY = _mm_maskload_ps((float*)&pY, mask_0);

			// sum force
			xi = _mm_add_ps(pX, xi);
			yi = _mm_add_ps(pY, yi);
		}
		
		mask[i] = 0xFFFFFFFF;

		m_forceX[i] = hsum_ps_sse3(xi);
		m_forceY[i] = hsum_ps_sse3(yi);
	}
}

///-------------------------------------------------------------------------------------------------
/// <summary>	Computation execute: Split into two functions
/// 			Compute forces - computes the forces between the bodies 
/// 			Merge Results - takes care of updating the bodies to the respective new positions
/// </summary>
///
/// <remarks>	Adrian, 18/07/2017. </remarks>
///-------------------------------------------------------------------------------------------------
void NBody::Application::OpenMPApplication::ExecuteApplication()
{
	for (int iterationIndex = 0; iterationIndex < 1000; ++iterationIndex)
	{
		Core::System::IO::WriteProgressMessage(iterationIndex);
		ComputeForces();
		MergeResults();
		//dump results to file
		Core::System::IO::WriteBodiesToFile(iterationIndex, m_allBodies, m_bodyCount);
	}
}

///-------------------------------------------------------------------------------------------------
/// <summary>	
/// 	Overloaded to use the OpenMP timing constructs. 
/// </summary>
///
/// <remarks>	Adrian, 18/07/2017. </remarks>
///-------------------------------------------------------------------------------------------------
void NBody::Application::OpenMPApplication::Execute()
{
	double startingTime = 0, endingTime = 0;
	startingTime = omp_get_wtime();
	ExecuteApplication();
	endingTime = omp_get_wtime();

	double timeTaken = endingTime - startingTime;
	std::cout << "Total time taken is "<< timeTaken << std::endl;
}

///-------------------------------------------------------------------------------------------------
/// <summary>	ComputeForces using AVX </summary>
///
/// <remarks>	Adrian, 18/07/2017. </remarks>
///
///-------------------------------------------------------------------------------------------------
//void NBody::Application::OpenMPApplication::ComputeForces()
//{
//	bool firstTime = true;
//#pragma omp parallel 
//	for (int i = 0; i < m_bodyCount; i++)
//	{
//		a16_int *mask = new a16_int[m_bodyCount];
//
//		for (int index = 0; index < m_bodyCount; index += 5)
//		{
//			mask[index] = 0xFFFFFFFF;
//			mask[index + 1] = 0xFFFFFFFF;
//			mask[index + 2] = 0xFFFFFFFF;
//			mask[index + 3] = 0xFFFFFFFF;
//			mask[index + 4] = 0xFFFFFFFF;
//		}
//
//		//set mask for body 
//		mask[i] = 0;
//		//resulting new positions
//		__m256 xi = _mm256_setzero_ps();
//		__m256 yi = _mm256_setzero_ps();
//
//		__m256 cxi = _mm256_set1_ps(m_xCoor[i]);
//		__m256 cyi = _mm256_set1_ps(m_yCoor[i]);
//
//		__m256 sample = _mm256_load_ps(&m_xCoor[0]);
//
//		for (int j = 0; j < m_bodyCount; j += 8)
//		{
//			a16_int* maskPtr = &mask[j];
//
//			//issue here need to solve it, mask not working
//			__m128i mask_0 = _mm_load_si128((__m128i*)(maskPtr));
//			__m128i mask_1 = _mm_load_si128((__m128i*)(maskPtr + 4));
//			//instruction not working correctly (leaving hi 128 bits = 0)
//			//__m256i mask_10 = _mm256_set_m128i(mask_1, mask_0);
//			__m256i mask_01 = _mm256_set1_epi32(0);
//			//set the low bits equal to the actual mask value 
//			mask_01 = _mm256_insertf128_si256(mask_01, mask_0, 0x00);
//
//			//set the hi bits maually (TEMPORARY solution)
//			// TODO: FIX
//			int* mask_01Ptr = ((int*)&mask_01);
//			mask_01Ptr[4] = maskPtr[j + 4];
//			mask_01Ptr[5] = maskPtr[j + 5];
//			mask_01Ptr[6] = maskPtr[j + 6];
//			mask_01Ptr[7] = maskPtr[j + 7];
//
//			//load positions at this point
//			__m256 pX = _mm256_load_ps(&m_xCoor[j]);
//			__m256 pY = _mm256_load_ps(&m_yCoor[j]);
//			__m256 mj = _mm256_load_ps(&m_bodyMass[j]);
//
//			//find direction
//			pY = _mm256_sub_ps(pY, cyi);
//			pX = _mm256_sub_ps(pX, cxi);
//
//			//  distance squared 
//			__m256 r2X = _mm256_set1_ps(0.0f);
//			__m256 r2Y = _mm256_set1_ps(0.0f);
//			r2X = _mm256_mul_ps(pX, pX);
//			r2Y = _mm256_mul_ps(pY, pY);
//			__m256 r2 = _mm256_set1_ps(0.0f);
//			r2 = _mm256_add_ps(r2X, r2Y);
//
//			//compute 1 / sqrt(r2) * m/1
//			__m256 invR = _mm256_set1_ps(0.0f);
//			invR = _mm256_rsqrt_ps(r2);
//			invR = _mm256_mul_ps(invR, invR);
//			invR = _mm256_mul_ps(invR, invR);
//			invR = _mm256_mul_ps(invR, mj);
//
//			pX = _mm256_mul_ps(pX, invR);
//			pY = _mm256_mul_ps(pY, invR);
//
//			//remove any of the arithmetic that include the 
//			pX = _mm256_maskload_ps((float*)&pX, mask_01);
//			pY = _mm256_maskload_ps((float*)&pY, mask_01);
//
//			// sum force
//			xi = _mm256_add_ps(pX, xi);
//			yi = _mm256_add_ps(pY, yi);
//		}
//
//		mask[i] = 0xFFFFFFFF;
//
//		xi = _mm256_hadd_ps(xi, _mm256_permute2f128_ps(xi, xi, 1));
//		yi = _mm256_hadd_ps(yi, _mm256_permute2f128_ps(yi, yi, 1));
//
//		m_forceX[i] = ((float*)&xi)[0];
//		m_forceY[i] = ((float*)&yi)[0];
//	}
//}
//
//void NBody::Application::OpenMPApplication::ExecuteApplication()
//{
//	for (int iterationIndex = 0; iterationIndex < 1000; ++iterationIndex)
//	{
//		Core::System::IO::WriteProgressMessage(iterationIndex);
//		ComputeForces();
//		MergeResults();
//		//dump results to file
//		Core::System::IO::WriteBodiesToFile(iterationIndex, m_allBodies, m_bodyCount);
//	}
//}
//
//void NBody::Application::OpenMPApplication::Execute()
//{
//	double startingTime = 0, endingTime = 0;
//	startingTime = omp_get_wtime();
//	ExecuteApplication();
//	endingTime = omp_get_wtime();
//
//	double timeTaken = endingTime - startingTime;
//	std::cout << "Total time taken is " << timeTaken << std::endl;
//}

///-------------------------------------------------------------------------------------------------
/// <summary>	Constructor. </summary>
///
/// <remarks>	Adrian, 18/07/2017. </remarks>
///
/// <param name="p_inputFilePath">	Full pathname of the input file. </param>
/// <param name="p_deltaT">		  	The delta t. </param>
/// <param name="p_numThreads">   	Number of threads. </param>
///-------------------------------------------------------------------------------------------------
NBody::Application::OpenMPApplication::OpenMPApplication(std::string p_inputFilePath, float p_deltaT, int p_numThreads)
	:Application(p_inputFilePath, p_deltaT, p_numThreads)
{}
