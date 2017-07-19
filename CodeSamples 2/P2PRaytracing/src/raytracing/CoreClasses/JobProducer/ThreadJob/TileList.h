///-------------------------------------------------------------------------------------------------
// file:	CoreClasses\JobProducer\ThreadJob\TileList.h
//
// summary:	Declares the tile list class
///-------------------------------------------------------------------------------------------------

#pragma once 

#include <vector>
#include <mutex>


#include "Tile.h"
#include "..\..\UtilClasses\Common.h"

using std::vector;

namespace Engine
{
	///-------------------------------------------------------------------------------------------------
	/// <summary>	List of tiles. </summary>
	///
	/// <remarks>	Adrian, 02/09/2015. </remarks>
	///-------------------------------------------------------------------------------------------------

	class TileList
	{
	public:
		/// <summary>	List of to does. </summary>
		vector<Tile*> m_toDoList;
		/// <summary>	The current job. </summary>
		int m_currentJob;
		/// <summary>	The amount jobs. </summary>
		int m_amountJobs;
		
		TileList(vector<Tile*> p_toDoList)
		{
			m_toDoList = p_toDoList;
			m_amountJobs = p_toDoList.size();
			m_currentJob = 0;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets the next item. </summary>
		///
		/// <remarks>	Adrian, 02/09/2015. </remarks>
		///
		/// <returns>	null if it fails, else the next. </returns>
		///-------------------------------------------------------------------------------------------------
		virtual bool GetNext(Tile*&) = 0;

		virtual void RestartCounter() = 0;

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Determines if we can exists more. </summary>
		///
		/// <remarks>	Adrian, 02/09/2015. </remarks>
		///
		/// <returns>	true if it succeeds, false if it fails. </returns>
		///-------------------------------------------------------------------------------------------------

		virtual bool ExistsMore()
		{
			return m_currentJob <= m_toDoList.size() - 1;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Default constructor. </summary>
		///
		/// <remarks>	Adrian, 02/09/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		TileList()
		{}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Destructor. </summary>
		///
		/// <remarks>	Adrian, 02/09/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		~TileList()
		{
			Common::EmptyPointerVector(m_toDoList);
		}

	private:
		
	};
}