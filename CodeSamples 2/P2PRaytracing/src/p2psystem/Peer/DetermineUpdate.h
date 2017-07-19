#pragma once

#include "../VectorTimer/VectorTimer.h"
#include "../VectorTimer/FixedVectorTimer.h"

namespace Network
{
	///-------------------------------------------------------------------------------------------------
	/// <summary>	Values that represent the  relathionship of two external events. </summary>
	///
	/// <remarks>	Adrian, 31/08/2015. </remarks>
	///-------------------------------------------------------------------------------------------------

	/*enum UpdateRelathionship
	{
		UPDATES_CONCURRENT,
		LHS_LATEST,
		RHS_LATEST,
		STARING
	};*/
	///-------------------------------------------------------------------------------------------------
	/// <summary>	A determine update. </summary>
	///
	/// <remarks>	Adrian, 31/08/2015. </remarks>
	///
	/// <typeparam name="T">	Generic type parameter. </typeparam>
	///-------------------------------------------------------------------------------------------------
	template <typename T>
	class DetermineUpdate
	{
	public:
		/*static UpdateRelathionship DetermineUpdatesSequence(const FixedVectorTimer<T>& lhs, const FixedVectorTimer<T>& rhs)
		{
			UpdateRelathionship toReturn = STARING;

			for (int index = 0; index < lhs.vectorTimer.size(); index++)
			{
				int lhsTerm = lhs.vectorTimer[index];
				int rhsTerm = rhs.vectorTimer[index];


				if (lhsTerm > rhsTerm)
				{
					if (toReturn == RHS_LATEST)
					{
						return UPDATES_CONCURRENT;
					}
					else
					{
						toReturn = LHS_LATEST;
					}
				}
				else if (lhsTerm < rhsTerm)
				{
					if (toReturn == LHS_LATEST)
					{
						return UPDATES_CONCURRENT;
					}
					else
					{
						toReturn = RHS_LATEST;
					}
				}
			}
			return toReturn;
		}*/
	};
}