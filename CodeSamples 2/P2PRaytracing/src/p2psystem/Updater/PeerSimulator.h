#pragma once

#include <vector>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "../VectorTimer/VectorTimer.h"
#include "../VectorTimer/FixedVectorTimer.h"
#include "Updater.h"
#include "../UtilityClasses/HelperClass.h"
#include "../UtilityClasses/DetermineUpdate.h"

namespace Network
{
	class PeerSimulator //: public Updater
	{
	private:
		//FixedVectorTimer<std::string>& GetTempVector(std::string& newUpdate)
		//{
		//	int test = 0;
		//	/*FixedVectorTimer<std::string> testTempUpdate(test);
		//	testTempUpdate.FromString(newUpdate);*/
		//	return testTempUpdate;
		//}

	public:
		//FixedVectorTimer<std::string> updater;
		int peerIndex;

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Default constructor. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		PeerSimulator()
		{
			int defaultIndex = 0;
			//updater = FixedVectorTimer<std::string>(defaultIndex);
			InitTimers();
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Constructor. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///
		/// <param name="m_assignedLetter">	[in,out] The assigned letter. </param>
		/// <param name="m_peerIndex">	   	[in,out] Zero-based index of the peer. </param>
		///-------------------------------------------------------------------------------------------------

		PeerSimulator(std::string& m_assignedLetter, int& m_peerIndex)
		{
			//updater = FixedVectorTimer<std::string>(m_peerIndex);
			peerIndex = m_peerIndex;

			//updater.updateRepresented = m_assignedLetter;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Constructor. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///
		/// <param name="m_peerIndex">	[in,out] Zero-based index of the peer. </param>
		/// <param name="data">		  	[in,out] The data. </param>
		///-------------------------------------------------------------------------------------------------

		PeerSimulator(int m_peerIndex, std::string data)
		{
			//updater = FixedVectorTimer<std::string>(m_peerIndex);
			//updater.FromString(data);
			peerIndex = m_peerIndex;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Merge updates. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///
		/// <param name="lhs">	[in,out] The left hand side. </param>
		/// <param name="rhs">	[in,out] The right hand side. </param>
		///
		/// <returns>	An UpdateRelathionship. </returns>
		///-------------------------------------------------------------------------------------------------

		//UpdateRelathionship MergeUpdates(FixedVectorTimer<std::string> lhs, FixedVectorTimer<std::string> rhs)
		//{
		//	/*UpdateRelathionship outcome = DetermineUpdate<std::string>::DetermineUpdatesSequence(lhs, rhs);
		//	return outcome;*/
		//	
		//}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Parse updates. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///
		/// <returns>	A std::string. </returns>
		///-------------------------------------------------------------------------------------------------

		virtual std::string ParseUpdates()
		{
			//return updater.ToString();
			 return "";
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Initialises the timers. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		void InitTimers()
		{
			//no initialization needed
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Merge updates. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///
		/// <param name="recivedUpdate">	[in,out] The recived update. </param>
		///-------------------------------------------------------------------------------------------------

		void MergeUpdates(FixedVectorTimer& recivedUpdate)
		{
			/////////////////////////check here
			//UpdateRelathionship outcome = DetermineUpdate<std::string>::DetermineUpdatesSequence(updater, recivedUpdate);

			//switch (outcome)
			//{
			//case UPDATES_CONCURRENT:

			//	/*for (int index = 0; index < recivedUpdate.vectorTimer.size(); index++)
			//	{
			//		updater.vectorTimer[index] = updater.vectorTimer[index] < recivedUpdate.vectorTimer[index] ? recivedUpdate.vectorTimer[index] : updater.vectorTimer[index];
			//	}

			//	updater.updateRepresented = updater.updateRepresented + recivedUpdate.updateRepresented;
			//	break;*/

			//case RHS_LATEST:
			//	updater = recivedUpdate;
			//	break;

			//case STARING:
			//	//equal updates
			//	break;

			//default:
			//	break;
			//}
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Use recived updates. </summary>
		///
		/// <remarks>	Adrian, 31/08/2015. </remarks>
		///
		/// <param name="update">	[in,out] The update. </param>
		///-------------------------------------------------------------------------------------------------

		void UseRecivedUpdates(std::string& update)
		{
			//MergeUpdates(GetTempVector(update));
		}
	};
}
