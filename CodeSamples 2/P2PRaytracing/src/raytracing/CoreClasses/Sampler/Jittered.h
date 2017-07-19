#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Jittered.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <vector>

#include "..\Sampler\Sampler.h"
#include "..\UtilClasses\Common.h"
#include "..\Point\Point3D\Point3D.h"

//#define ON_ANGELS
#define AZIMUTH_ANGLE 360
#define ELEV_ANGLE 90
//#define ON_ANGELS

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: Jittered
	//		   Description: Concrete implementation of Abstract Class 
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class Jittered : public Sampler
	{
	public:
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Number of samples, Number of sets
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Jittered(int p_numSamples, int p_numSets, float p_cosineWeighting) : Sampler(p_numSamples, p_numSets, p_cosineWeighting)
		{
			m_needRotation = true;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Int Num of Samples, int Num of Sets of Samples
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Jittered() : Sampler(25, 83)
		{}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GenerateSamples
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using Engine::Sampler::GenerateSamples;
		virtual void GenerateSamples()
		{
		#ifndef ON_ANGELS
			int n = (int)sqrt(numSamples);

			for (int p = 0; p < numSets; p++)
			{
				for (int j = 0; j < n; j++)
				{
					for (int k = 0; k < n; k++)
					{
						Point2D sp((k + twister.Rand()) / n, (j + twister.Rand()) / n);
						samples.push_back(sp);
					}
				}
			}
		#else
			GenerateSamplesOnAngles();
		#endif
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GenerateSamplesOnAngles
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void GenerateSamplesOnAngles()
		{
			int n = (int)sqrt(numSamples);
			float deltaElev = ELEV_ANGLE / n;
			float deltaAzim = AZIMUTH_ANGLE / n;
			float k = 0,j = 0;


			for (int currentSetIndex = 0; currentSetIndex < numSets; currentSetIndex++)
			{
				for (int elevIndex = 0; elevIndex < n; elevIndex++)
				{
					for (int azmiuthIndex = 0; azmiuthIndex < n; azmiuthIndex++)
					{
						k = azmiuthIndex * deltaAzim;
						j = deltaElev * elevIndex;
						Point2D sp(k + GenrateRandomNumber((int)0, (int)deltaAzim) , (j + GenrateRandomNumber((int)0, (int)deltaElev)));
						samples.push_back(sp);
					}
				}
			}
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetRandomizedStratifiedSample
		//		Return Value: Point3D Random Position
		//		  Parameters: int X coordinate, int Y Coordinate
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D GetRandomizedStratifiedSample(int p_xCoor, int p_yCoor)
		{
			//get position in relation to current 
			int relativePoisiton = (p_yCoor * sqrt(numSamples)) + p_xCoor;
			int randomNumber = rand() % numSets;
			relativePoisiton += randomNumber * numSamples;
			return hemisphericalSamples[relativePoisiton];
		}
	};
}
