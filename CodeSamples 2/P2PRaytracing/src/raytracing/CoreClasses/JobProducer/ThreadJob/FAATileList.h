#pragma once 

#include "Tile.h"
#include "TileList.h"

#include <atomic>

namespace Engine
{
	class FaaTileList : public TileList
	{
	public:
		vector<Tile*> m_jobs;

		FaaTileList()
		{
			
		}

		using TileList::GetNext;
		bool GetNext(Tile*& p_currentTile)
		{
			int previous = std::atomic_fetch_add(&m_counterState, 1);
			if (previous < m_jobs.size())
			{
				
				p_currentTile =  m_jobs[previous];
				return true;
			}
			else
			{
				return false;
			}
		}
		
		void EmptyVector()
		{
			Common::EmptyPointerVector(m_jobs);
		}

		using TileList::RestartCounter;
		virtual void RestartCounter()
		{
			cout << m_counterState << endl;
			m_counterState = 0;
			cout << "after " << m_counterState << endl;
		}

		~FaaTileList()
		{
			Common::EmptyPointerVector(m_jobs);
		}



	private:
		std::atomic<int> m_counterState;
		
	};
}