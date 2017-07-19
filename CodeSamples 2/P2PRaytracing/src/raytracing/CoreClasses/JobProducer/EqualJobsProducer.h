#pragma once 

#include "JobProducer.h"

namespace Engine
{
	///-------------------------------------------------------------------------------------------------
	/// <summary>	Devides the workload into tiles of equal size </summary>
	///
	/// <remarks>	Adrian, 19/07/2017. </remarks>
	///-------------------------------------------------------------------------------------------------
	class EqualJobProducer : public JobProducer
	{
	public:
		EqualJobProducer(int p_width, int p_height, int p_amountWorkers, int p_sideHeight) : JobProducer(p_width, p_height)
		{
			m_amountWorkers = p_amountWorkers;
			m_sideHeight = p_sideHeight;
			m_amountJobs = 1;
		}

		EqualJobProducer(int p_width, int p_height, int p_amountWorkers, int p_sideHeight, int p_amountJobs) : JobProducer(p_width, p_height)
		{
			m_amountWorkers = p_amountWorkers;
			m_sideHeight = p_sideHeight;
			m_amountJobs = p_amountJobs;
		}

		using JobProducer::CreateJobs;
		virtual void CreateJobs(vector<Tile*>& p_tileList)
		{
			int jobsSide = sqrt(m_amountJobs);
			// ReSharper disable once CppUseAuto
			int widthJob = m_width / jobsSide;
			int heighJob = m_height / jobsSide;
			int currentX = 0; 
			int currentY = 0;
			//split equally between the amount of threads 
			for (int indexY = 0; indexY < jobsSide; indexY++)
			{
				for (int index = 0; index < jobsSide; index++)
				{
					Point2D currentPoint(currentX, currentY);
					Tile* currentTile = new Tile(currentPoint, widthJob, heighJob);
					p_tileList.push_back(currentTile);
					currentX += widthJob;
				}
				currentX = 0;
				currentY += heighJob;
			}
		}

	private:
		int m_sideHeight;
		int m_amountJobs;
	};
}