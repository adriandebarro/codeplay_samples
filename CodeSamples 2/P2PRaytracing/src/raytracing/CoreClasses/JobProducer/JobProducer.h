#pragma once 

#include <vector>


#include "ThreadJob\Tile.h"

using std::vector;
namespace Engine
{
	///-------------------------------------------------------------------------------------------------
	/// <summary>	Abstract Jobs Producer </summary>
	///
	/// <remarks>	Adrian, 19/07/2017. </remarks>
	///-------------------------------------------------------------------------------------------------
	class JobProducer
	{
		public:
		virtual ~JobProducer()
		{
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Constructor. </summary>
		///
		/// <remarks>	Adrian, 28/08/2015. </remarks>
		///
		/// <param name="p_width"> 	The width. </param>
		/// <param name="p_height">	The height. </param>
		///-------------------------------------------------------------------------------------------------

		JobProducer(int p_width, int p_height)
		{
			m_width = p_width;
			m_height = p_height;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Creates the jobs. </summary>
		///
		/// <remarks>	Adrian, 28/08/2015. </remarks>
		///
		/// <param name="p_list">	[in,out] The list. </param>
		///-------------------------------------------------------------------------------------------------

		virtual void CreateJobs(vector<Tile*>& p_list) = 0;

		float GetWorkersAmount()
		{
			return ((float)m_amountWorkers);
		}

		protected:
			int m_width;
			int m_height;
			int m_amountWorkers;
	};
}