#pragma once 



namespace Core
{
	namespace Bodies
	{
		template<typename T>
		class Particle;
	}

	namespace Maths
	{
		namespace Vector
		{
			class Vector2;
		}
	}

	namespace Common
	{
		typedef uint32_t uint_t;
		typedef uint16_t ushort_t;

		typedef _declspec(align(16)) float a16_flt;
		typedef _declspec(align(16)) int a16_int;
		typedef _declspec(align(16)) unsigned int a16_uint;

		typedef Core::Bodies::Particle<Core::Maths::Vector::Vector2> Body;

		#define KEpsillon 1e-5f
		#define GTerm 20.f

		const std::string SYSTEM_THREADS_COUNT = "threads_count";
		const std::string SYSTEM_HYPER_THREADED = "hyper_threaded_check";


		#ifdef __CUDACC__
			#define CUDA_CALLABLE_MEMBER __host__ __device__
			#define CUDA_CALLABLE_HOST_FUNC __host__ 	
		#else
			#define CUDA_CALLABLE_MEMBER
			#define CUDA_CALLABLE_HOST_FUNC
		#endif 
	}
}
