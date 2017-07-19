#pragma once


#include <hash_map>
#include <vector>
#include <string>
#include <mutex>

#include "../UtilityClasses/HelperClass.h"
#include "../UtilityClasses/Common.h"

using std::vector;
using std::hash_map;

namespace Network
{
	///-------------------------------------------------------------------------------------------------
	/// <summary>	Values that represent update relathionships. </summary>
	///
	/// <remarks>	Adrian, 09/09/2015. </remarks>
	///-------------------------------------------------------------------------------------------------

	enum UpdateRelathionship
	{
		UPDATES_CONCURRENT,
		GOOD_UPDATE,
		ALREADY_HAVE_UPDATE,
		NO_COMPARISION
	};
	/*enum UpdateRelathionship
	{
		UPDATES_CONCURRENT,
		LHS_LATEST,
		RHS_LATEST,
		STARING
	};*/
	
	typedef vector<unsigned char>::iterator Iterator;


	///-------------------------------------------------------------------------------------------------
	/// <summary>	A vector timer. </summary>
	///
	/// <remarks>	Adrian, 31/08/2015. </remarks>
	///-------------------------------------------------------------------------------------------------

	class VectorTimer
	{
	public:
		virtual void FromString(std::string) = 0;
		virtual std::string ToString() = 0;

		VectorTimer()
		{}

		VectorTimer(int p_amountPeers) : vectorTimer(p_amountPeers, 0)
		{}

		std::mutex m_incrementingLock;

		void IncrementTimer()
		{
			m_incrementingLock.lock();
			vectorTimer[index - 1]++;
			m_incrementingLock.unlock();
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Adds to the timer. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///
		/// <param name="index">	 	Zero-based index of the. </param>
		/// <param name="timerValue">	The timer value. </param>
		///-------------------------------------------------------------------------------------------------

		void AddToTimer(int index, int timerValue)
		{
			if (index > 0 && index < vectorTimer.size())
			{
				vectorTimer[index] = timerValue;
			}
		}

	/*	void TakeSup(std::string p_vectorTimer)
		{
			std::string stringArray[2];
			HelperClass::ParseStringSentence(p_vectorTimer, "-", stringArray);

			std::string timerInput[m_amountTimers];
			HelperClass::ParseStringSentence(stringArray[1], ",", timerInput);

			for (int currentIndex = 0; currentIndex < FIXED_AMOUNT_PEERS; currentIndex++)
			{
				int currentLogicalTimer = vectorTimer[currentIndex];
				int currentRecLogicalTimer = std::stoi(timerInput[currentIndex]);
				vectorTimer[currentIndex] = currentRecLogicalTimer > currentLogicalTimer ? currentRecLogicalTimer : currentLogicalTimer;
			}
		}*/

		void TakeSup(VectorTimer* p_vectorTimer)
		{
			m_incrementingLock.lock();

			for (int currentIndex = 0; currentIndex < p_vectorTimer->GetAmountTimers(); currentIndex++)
			{
				int currentLogicalTimer = vectorTimer[currentIndex];
				int currentRecLogicalTimer = p_vectorTimer->vectorTimer[currentIndex];
				vectorTimer[currentIndex] = currentRecLogicalTimer > currentLogicalTimer ? currentRecLogicalTimer : currentLogicalTimer;
			}

			m_incrementingLock.unlock();

		}


		int GetPeerTime()
		{
			return vectorTimer[index];
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Updates the peer timer described by p_timerValue. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///
		/// <param name="p_timerValue">	The timer value. </param>
		///-------------------------------------------------------------------------------------------------

		void UpdatePeerTimer(int p_timerValue)
		{
			vectorTimer[index] = p_timerValue;
		}
		
		UpdateRelathionship DetermineUpdatesSequence( VectorTimer* p_otherUpdate)
		{
			UpdateRelathionship toReturn = NO_COMPARISION;

			for (int index = 0; index < p_otherUpdate->vectorTimer.size(); index++)
			{
				int otherTimer = p_otherUpdate->vectorTimer[index];
				int thisTimer = vectorTimer[index];


				if (otherTimer > thisTimer)
				{
					if (toReturn == UpdateRelathionship::ALREADY_HAVE_UPDATE)
					{
						return UpdateRelathionship::UPDATES_CONCURRENT;
					}
					else
					{
						toReturn = UpdateRelathionship::GOOD_UPDATE;
					}
				}
				else if (otherTimer < thisTimer)
				{
					if (toReturn == UpdateRelathionship::GOOD_UPDATE)
					{
						return UpdateRelathionship::UPDATES_CONCURRENT;
					}
					else
					{
						toReturn = UpdateRelathionship::ALREADY_HAVE_UPDATE;
					}
				}
			}
			return toReturn;
		}

		union byteInt
		{
			int i;
			unsigned char b[sizeof(int)];
		};

		void Serialize(vector<unsigned char>& p_array)
		{
			int counter = 0;

			unsigned char* amount = (unsigned char*)&m_amountTimers;

			p_array.push_back(amount[0]);
			p_array.push_back(amount[1]);
			p_array.push_back(amount[2]);
			p_array.push_back(amount[3]);

			for (int index = 0; index < m_amountTimers; index++)
			{
				unsigned char* tempPointer = (unsigned char*)&vectorTimer[index];

				for (int innerIndex = 0; innerIndex < 4; innerIndex++ ,counter++)
				{
					p_array.push_back(tempPointer[innerIndex]);
				}
			}
		}

		void FromByteArray(vector<unsigned char> p_inputData, int &p_startingPoint)
		{
			int incrementVariable = p_startingPoint;
			
			int amount = 0;

			unsigned char* tempAMount = (unsigned char*)&amount;

			for (int index = 0; index < 4; index++, p_startingPoint++)
			{
				tempAMount[index] = p_inputData[p_startingPoint];
			}

			if (m_amountTimers != amount)
				m_amountTimers = amount;
			for (int index = 0; index < m_amountTimers; index++)
			{
				byteInt temp;
				for (int innerIndex = 0; innerIndex < 4; innerIndex++, p_startingPoint++)
				{
					temp.b[innerIndex] = p_inputData[p_startingPoint];
				}
				vectorTimer.push_back(temp.i);
			}
 		}

		int GetAmountTimers()
		{
			return m_amountTimers;
		}

		int GetNumberBytes()
		{
			return 4 * m_amountTimers + 4;
		}


		///-------------------------------------------------------------------------------------------------
		/// <summary>	Less-than-or-equal comparison operator. </summary>
		///
		/// <remarks>	Adrian, 11/09/2015. </remarks>
		///
		/// <param name="comparer">	The comparer. </param>
		///
		/// <returns>	true if the first parameter is less than or equal to the second. </returns>
		///-------------------------------------------------------------------------------------------------

		bool operator < (const VectorTimer& comparer)
		{
			for (int index = 0; index < comparer.vectorTimer.size(); index++)
			{
				if (vectorTimer[index] > comparer.vectorTimer[index])
				{
					return false;
				}
			}
			return true;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Equality operator. </summary>
		///
		/// <remarks>	Adrian, 11/09/2015. </remarks>
		///
		/// <param name="comparer">	The comparer. </param>
		///
		/// <returns>	true if the parameters are considered equivalent. </returns>
		///-------------------------------------------------------------------------------------------------

		bool operator == (const VectorTimer& comparer)
		{
			for (int index = 0, length = comparer.vectorTimer.size(); index < length; index++)
			{
				if (vectorTimer[index] != comparer.vectorTimer[index])
				{
					return false;
				}
			}
			return true;
		}

		static bool VectorsEqual(VectorTimer* p_lhs, VectorTimer* p_rhs)
		{
			for (int index = 0, length = p_lhs->vectorTimer.size(); index < length; index++)
			{
				if (p_lhs->vectorTimer[index] != p_rhs->vectorTimer[index])
				{
					return true;
				}
			}
			return false;
		}

		static bool IsGreater(VectorTimer* p_lhs, VectorTimer* p_rhs)
		{
			for (int index = 0; index < p_lhs->vectorTimer.size(); index++)
			{
				if (p_lhs->vectorTimer[index] < p_rhs->vectorTimer[index])
				{
					return true;
				}
			}

			return false;
		}


		~VectorTimer()
		{}

		vector<int> vectorTimer;

	protected:
		/// <summary>	Zero-based index of the peer. </summary>
		int index;
		
		int m_amountTimers;
	};

	
}
