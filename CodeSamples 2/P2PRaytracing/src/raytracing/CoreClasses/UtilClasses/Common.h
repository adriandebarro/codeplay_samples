///-------------------------------------------------------------------------------------------------
// file:	CoreClasses\UtilClasses\Common.h
//
// summary:	Declares the common class
///-------------------------------------------------------------------------------------------------

#pragma once

#include <memory>
#include <vector>
#include <random>
#include <iostream>
#include <string>
#include <direct.h>
#include <limits>

#include <boost/tokenizer.hpp>

using namespace std;

namespace Engine
{
	namespace Common
	{
		//----------------------------------------------------------------------------------------------------------------
		//										Defined Global Constants
		//----------------------------------------------------------------------------------------------------------------

		//#define MULTIPLE_FRAMES

		#define SINGLE_FRAME

		///-------------------------------------------------------------------------------------------------
		/// <summary>	A macro that defines anti aliasing. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		#define ANTI_ALIASING 1

		///-------------------------------------------------------------------------------------------------
		/// <summary>	A macro that defines anti aliasing constant. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		#define ANTI_ALIASING_CONSTANT 5

		///-------------------------------------------------------------------------------------------------
		/// <summary>	A macro that defines number of samples. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		#define NUMBER_OF_SAMPLES = 25
		/// <summary>	The inverse random maximum. </summary>
		const float invRAND_MAX = 1.0 / 32767;

		bool static USE_VERTEX_NORMALS_SHADING = false;


		//if not unix define 
		#define MULTI_THREADED

		#ifndef __unix__  

		///-------------------------------------------------------------------------------------------------
		/// <summary>	A macro that defines pi number. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		#define PI_NUM 3.141592653589793
		#endif

		///-------------------------------------------------------------------------------------------------
		/// <summary>	A macro that defines pi number. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------
		const std::string SAVE_FILE_PATH = "./Progress/";


		/// <summary>	The huge value. </summary>
		const float kHugeValue = 1 * 10 ^ 6;
		/// <summary>	The epsilon. </summary>
		const float kEpsilon = 1e-3f;
		//const float kEpsilon = 0.1;

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Defines an alias representing the pixel. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		typedef int Pixel;

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Genrate random number. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///
		/// <param name="p_minBound">	The minimum bound. </param>
		/// <param name="p_maxBound">	The maximum bound. </param>
		///
		/// <returns>	A float. </returns>
		///-------------------------------------------------------------------------------------------------

		static float GenrateRandomNumber(int p_minBound, int p_maxBound)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> dis(p_minBound, p_maxBound);

			return (float)dis(gen);
		}

		/*static float GetMax()
		{
			return (float)std::numeric_limits<float>::max();
		}

		static float GetMin()
		{
			return (float)std::numeric_limits<float>::min();
		}
		*/

		//DIRTY
		//[AD] related to the kdtree implementation
		enum Axis { None = -1, X = 0, Y = 1, Z = 2 };
		typedef float Real;


		// Alignment macro for specifying byte boundaries of types
		#define ALIGN_8     __declspec(align(8))
		#define ALIGN_16    __declspec(align(16))
		#define ALIGN_32    __declspec(align(32))

		namespace GeomerticPrimitives
		{
			struct Intersection
			{
				float t,
					gamma,
					beta;

				int triangleId;

				Intersection(void)
					: //t(Mathf)
					/*,*/ gamma((float)0)
					, beta((float)0)
					, triangleId(0)
				{
					t =500000;
				}

				Intersection(const float p_t)
					: t(p_t)
					, gamma((float)0)
					, beta((float)0)
					, triangleId(0)
				{ }
			};
		}




		namespace Tokenizer
		{
			///-------------------------------------------------------------------------------------------------
			/// <summary>	Defines an alias representing the tokenize. </summary>
			///
			/// <remarks>	Adrian, 21/09/2015. </remarks>
			///-------------------------------------------------------------------------------------------------

			typedef boost::tokenizer<boost::char_separator<char>> tokenize;

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Parse string sentence. </summary>
			/// 
			/// <remarks>	Adrian, 21/09/2015. </remarks>
			///
			/// <param name="toParse">		  	to parse. </param>
			/// <param name="splittingString">	The splitting string. </param>
			/// <param name="output">		  	[in,out] The output. </param>
			///-------------------------------------------------------------------------------------------------

			inline static void ParseStringSentence(std::string toParse,const std::string &splittingString, std::vector<std::string>& output)
			{
				boost::char_separator<char> sep(splittingString.c_str());
				tokenize tok(toParse, sep);
				int index = 0;
				for (tokenize::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
				{
					output.push_back(tokIter.current_token().c_str());
				}
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Parse string. </summary>
			///
			/// <remarks>	Adrian, 21/09/2015. </remarks>
			///
			/// <param name="toParse">		  	to parse. </param>
			/// <param name="splittingString">	The splitting string. </param>
			///
			/// <returns>	A vector&lt;std::string&gt; </returns>
			///-------------------------------------------------------------------------------------------------

			inline static vector<std::string> ParseString(std::string toParse,const std::string &splittingString)
			{
				boost::char_separator<char> sep(splittingString.c_str());
				tokenize tok(toParse, sep);
				vector<string> output;

				int index = 0;
				for (tokenize::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
				{
					output.push_back(tokIter.current_token().c_str());
				}

				return output;
			}

			///-------------------------------------------------------------------------------------------------
			/// <summary>	Parse string. </summary>
			///
			/// <remarks>	Adrian, 21/09/2015. </remarks>
			///
			/// <param name="toParse">		  	to parse. </param>
			/// <param name="splittingString">	The splitting string. </param>
			/// <param name="p_inputArray">   	Array of inputs. </param>
			///-------------------------------------------------------------------------------------------------

			inline static void ParseString(std::string toParse, std::string splittingString, string p_inputArray[])
			{
				boost::char_separator<char> sep(splittingString.c_str());
				tokenize tok(toParse, sep);

				int index = 0;
				for (tokenize::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
				{
					p_inputArray[index] = tokIter.current_token().c_str();
					index++;
				}
			}
		}//Tokenizer

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Phis. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///
		/// <param name="j">	The int to process. </param>
		///
		/// <returns>	A double. </returns>
		///-------------------------------------------------------------------------------------------------

		/*double phi(int j)
		{
			double x = 0.0;
			double f = 0.5;

			while (j)
			{
				x += f * (double)(!j & 1);
				j /= 2;
				f *= 0.5;
			}

			return x;
		}*/

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Prints. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///
		/// <param name="p_toPrint">	to print. </param>
		///-------------------------------------------------------------------------------------------------

		static void Print(const string &p_toPrint)
		{
			std::cout << p_toPrint << std::endl;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Deletes all described by theElement. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///
		/// <typeparam name="T">	Generic type parameter. </typeparam>
		/// <param name="theElement">	[in,out] If non-null, the element. </param>
		///
		/// <returns>	true if it succeeds, false if it fails. </returns>
		///-------------------------------------------------------------------------------------------------

		template<typename T>
		static bool deleteAll(T* theElement)
		{
			delete theElement; return true;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Deletes the vector described by toDelete. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///
		/// <typeparam name="T">	Generic type parameter. </typeparam>
		/// <param name="toDelete">	[in,out] to delete. </param>
		///-------------------------------------------------------------------------------------------------

		template<typename T>
		static void DeleteVector(std::vector<T>& toDelete)
		{
			std::for_each(toDelete.begin(), toDelete.end(), std::default_delete<T>());
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Empty pointer vector. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///
		/// <typeparam name="T">	Generic type parameter. </typeparam>
		/// <param name="deleteme">	[in,out] [in,out] If non-null, the deleteme. </param>
		///-------------------------------------------------------------------------------------------------

		template <class T>
		static void EmptyPointerVector(vector<T*>& deleteme) {
			while (!deleteme.empty()) {
				if (deleteme.back() != nullptr)
				{
					delete deleteme.back();
					deleteme.back() = nullptr;
				}
				deleteme.pop_back();
			}
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets squared number. </summary>
		///
		/// <remarks>	Adrian, 26/08/2015. </remarks>
		///
		/// <param name="p_squaredNumber">	The squared number. </param>
		///
		/// <returns>	The squared number. </returns>
		///-------------------------------------------------------------------------------------------------

		static int GetSquaredNumber(int p_squaredNumber) {

			int currentTester = 0;
			int index = 0;

			do
			{
				index++;
				currentTester = index * index;
			} while ((currentTester < p_squaredNumber));

			return index;
		}
		//----------------------------------------------------------------------------

		//----------------------------------------------------------------------------
		// WaitFreeLinkedList common params
		//----------------------------------------------------------------------------
		const int ELEMENTS_PER_LINKED_LIST_NODE = 200;
		const int  DEFAULT_VALUE_LIST = -1;
		#define WAIT_FREE
		//----------------------------------------------------------------------------


	/// <summary>	Basic handy Colors. </summary>
		/*const RGBColor BLACK_COLOR(0, 0, 0);
		const RGBColor RED_COLOR(255, 0, 0);
		const RGBColor GREEN_COLOR(0, 255, 0);
		const RGBColor BLUE_COLOR(0, 0, 255);
		const RGBColor GREY_COLOR(192, 192, 192);*/
	}
}