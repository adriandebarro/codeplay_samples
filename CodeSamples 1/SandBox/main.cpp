#include <iostream>
#include <immintrin.h>
#include <emmintrin.h>

//
//_declspec(align(16))int test2[8] = { 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF };
//
//_declspec(align(16)) float floatTest[8];
//for (int index = 0; index < 8; index++)
//{
//	floatTest[index] = index + 1;
//}
//
//_declspec(align(16))int* mask_1_ptr = &test2[4];
//
//__m128i mask_0 = _mm_loadu_si128((__m128i*)test2);
//__m128i mask_1 = _mm_loadu_si128((__m128i*) &test2[4]);
//
//__m256i mask = _mm256_set_m128i(mask_1, mask_0);
//
//
//
//__m256 loadedValues = _mm256_maskload_ps(floatTest, mask);

union test
{
	float myFloat;
	int myInt;
};

int main(void)
{

	
//	_declspec(align(16))int test2[8] = { 0xFF000000, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0 };
//
//	_declspec(align(16)) float floatTest[8];
//	for (int index = 0; index < 8; index++)
//	{
//		floatTest[index] = index + 5.2;
//	}
//	
//	//_declspec(align(16))int* mask_1_ptr = &test2[4];
//
//	//float result = (5.0f) & 0xFFFFFFFF;
//
///*
//	__m128i mask_0 = _mm_loadu_si128((__m128i*)test2);
//	__m128i mask_1 = _mm_loadu_si128((__m128i*) &test2[4]);
//
//	__m256i mask = _mm256_set_m128i(mask_1, mask_0 );
//	*/
//	
//
//	__m128i floattest_0 = _mm_loadu_si128((__m128i*)test2);
//	__m128i floattest_1 = _mm_loadu_si128((__m128i*) &test2[4]);
//
//	__m256i maskdd = _mm256_set_m128i(floattest_1, floattest_0);
//
//	//__m256 mask = _mm256_load_ps(test2);
//	__m256 elements = _mm256_load_ps(floatTest);
//
//	__m256 loadedValues = _mm256_maskload_ps((float*)&elements, maskdd);


	int data[4] = { 0 };
	__cpuid(data, 1);
	bool avxSupported = data[2] & (1 << 28) || false;
	


//__declspec(align(16))int test2[8] = { 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
//
//__declspec(align(16)) float floatTest[8];
//for (int index = 0; index < 8; index++)
//{
//	floatTest[index] = index + 1;
//}
//
////__declspec(align(16))int* mask_1_ptr = &test2[4];
//__m128i mask_1 = _mm_load_si128((__m128i*)(test2+4));
//__m128i mask_0 = _mm_load_si128((__m128i*)test2);
//
////__m256i mask = _mm256_set1_epi32(0);
////mask = _mm256_insertf128_si256(mask, mask_0, 0x00);
////mask = _mm256_insertf128_si256(mask, mask_1, 0x01);
////
////__m256i mask = _mm256_set_m128i(mask_1, mask_0);
//__m256i mask = _mm256_insertf128_si256(_mm256_castsi128_si256(mask_0), (mask_1), 0x0);
//
//
//
//__m256 loadedValues = _mm256_maskload_ps(floatTest, mask);


//_declspec(align(16))int test2[8] = { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
//
//_declspec(align(16)) float floatTest[8];
//for (int index = 0; index < 8; index++)
//{
//	floatTest[index] = index + 1;
//}
//
//_declspec(align(16))int* mask_1_ptr = &test2[4];
//
//__m128i mask_0 = _mm_loadu_si128((__m128i*)test2);
//__m128i mask_1 = _mm_loadu_si128((__m128i*) &test2[4]);
//
//__m256i mask = _mm256_set_m128i(mask_1, mask_0);
//
//
//
//__m256 loadedValues = _mm256_maskload_ps(floatTest, mask);




	//__m256 filteredValues = _mm256_and_ps(loadedValues, test2);
	/*float *filteredValuess = ((float*)&loadedValues);
	
	for (int index = 0; index < 8; index++)
	{
		std::cout << (float)filteredValuess[index] << std::endl;
	}*/
	//__m256 floatsss = _mm256_set1_ps(floatTest[0]);
	//test2 = _mm256_load_ps(&floatTest[0]);



	//int startingIndex = 1;
	//int jumpLength = 5 - 2;
	//int arrayIndex = startingIndex - 1;
	//int remaningBodies = 5 - 1;
	////execute jump forces aggregation
	//for (int jumpIndex = startingIndex; jumpIndex > 0; jumpIndex--)
	//{
	//	//force += p_decomposedForces[arrayIndex];
	//	std::cout << arrayIndex << std::endl;
	//	arrayIndex += jumpLength;
	//	
	//	jumpLength--, remaningBodies--;
	//}

	//arrayIndex = 4;
	////execute linear summation of remaning force components 
	//for (int index = 0; index < remaningBodies; index++, arrayIndex++)
	//{
	//	std::cout << arrayIndex << std::endl;
	//	//force += p_decomposedForces[arrayIndex];
	//}

	////std::cout << matrixIndex << std::endl;

	std::cin.get();
	return 0;
}
