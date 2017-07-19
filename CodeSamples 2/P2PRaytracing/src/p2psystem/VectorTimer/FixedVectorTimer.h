///-------------------------------------------------------------------------------------------------
// file:	VectorTimer\FixedVectorTimer.h
//
// summary:	Declares the fixed vector timer class
///-------------------------------------------------------------------------------------------------

#pragma once

#include "VectorTimer.h"
#include "../UtilityClasses/Common.h"
#include "../UtilityClasses/HelperClass.h"

///-------------------------------------------------------------------------------------------------
/// <summary>	A fixed vector timer. </summary>
///
/// <remarks>	Adrian, 31/08/2015. </remarks>
///
/// <typeparam name="T">	Generic type parameter. </typeparam>
///-------------------------------------------------------------------------------------------------

class FixedVectorTimer : public Network::VectorTimer
{
public:
	/// <summary>	true to initialise only. </summary>
	bool initOnly;
	/// <summary>	The vector timer. </summary>

	///-------------------------------------------------------------------------------------------------
	/// <summary>	Default constructor. </summary>
	///
	/// <remarks>	Adrian, 31/08/2015. </remarks>
	///-------------------------------------------------------------------------------------------------

	FixedVectorTimer() : VectorTimer()
	{}


	FixedVectorTimer(int m_vectorSize) : VectorTimer(m_vectorSize)
	{
		m_amountTimers = m_vectorSize;
	}

	FixedVectorTimer(vector<unsigned char>& p_receivedVector, int p_startingPoint)
	{
		int startingPoint = p_startingPoint;
		FromByteArray(p_receivedVector, startingPoint);
	}


	FixedVectorTimer(const FixedVectorTimer& p_other)
	{
		m_amountTimers = p_other.m_amountTimers;
		index = p_other.index;
		vectorTimer = p_other.vectorTimer;
	}

	///-------------------------------------------------------------------------------------------------
	/// <summary>	Constructor. </summary>
	///
	/// <remarks>	Adrian, 31/08/2015. </remarks>
	///
	/// <param name="p_index">	The index of the specific peer </param>
	///-------------------------------------------------------------------------------------------------
	FixedVectorTimer(int p_index, int p_amountPeers)
	{
		m_amountTimers = p_amountPeers;
		vectorTimer = vector<int>(p_amountPeers);
		index = p_index;

		for (int index = 0; index < m_amountTimers; index++)
		{
			vectorTimer[index] = 0;
		}
		//updateRepresented = T(" ");
	}

	FixedVectorTimer(int p_index, std::string p_dataRepresented, int p_amountPeers)
	{
		m_amountTimers = p_amountPeers;
		vectorTimer = vector<int>(m_amountTimers);
		index = p_index;

		for (int index = 0; index < m_amountTimers; index++)
		{
			vectorTimer[index] = 0;
		}
		//updateRepresented = T(" ");
		FromString(p_dataRepresented);
	}

	///-------------------------------------------------------------------------------------------------
	/// <summary>	Constructor. </summary>
	///
	/// <remarks>	Adrian, 31/08/2015. </remarks>
	///
	/// <param name="p_stringInt">	The string int. </param>
	///-------------------------------------------------------------------------------------------------

	FixedVectorTimer(std::string p_stringInt)
	{
		m_amountTimers = FIXED_AMOUNT_PEERS;
		FromString(p_stringInt);
		//updateRepresented = T(" ");
	}

	///-------------------------------------------------------------------------------------------------
	/// <summary>	Gets the value. </summary>
	///
	/// <remarks>	Adrian, 31/08/2015. </remarks>
	///
	/// <returns>	The value. </returns>
	///-------------------------------------------------------------------------------------------------

	//T& GetValue()
	//{
	//	//return updateRepresented;
	//}

	using Network::VectorTimer::ToString;
	virtual std::string ToString()
	{
		std::string output = "";
		//output = output + (updateRepresented + "-");
		for (int index = 0; index < vectorTimer.size(); index++)
		{
			output += std::to_string(vectorTimer[index]) + (index == vectorTimer.size() - 1 ? "" : ",");
		}

		return output;
	}

	void ParseStringSentence(std::string toParse, std::string splittingString, std::string output[])
	{
		boost::char_separator<char> sep(splittingString.c_str());
		tokeniz tok(toParse, sep);


		int index = 0;
		for (tokeniz::iterator tokIter = tok.begin(); tokIter != tok.end(); ++tokIter)
		{
			output[index] = tokIter.current_token().c_str();
			++index;
		}
	}

	using Network::VectorTimer::FromString;
	virtual void FromString(std::string input)
	{
		try
		{
			std::string timerInput[FIXED_AMOUNT_PEERS];
			ParseStringSentence(input, ",", timerInput);

			vectorTimer = vector<int>();

			for (int index = 0; index < FIXED_AMOUNT_PEERS; index++)
			{
				if (timerInput[index].empty())
					vectorTimer.push_back(0);
				else
				{
					try
					{
						vectorTimer.push_back(std::stoi(timerInput[index].c_str()));
					}
					catch (std::exception ex)
					{
						std::cout << "exception" << std::endl;
					}
				}
					
			}
		}
		catch (std::exception ex)
		{
			std::cout << "problem arose" << std::endl;
		}
		//updateRepresented = T(stringArray[0]);
	}


	/*void SetUpdatedVersion(T _updateRepresented)
	{
		updateRepresented = _updateRepresented;
	}*/

	void SetIndex(int _index)
	{
		index = _index;
	}

	private:

};