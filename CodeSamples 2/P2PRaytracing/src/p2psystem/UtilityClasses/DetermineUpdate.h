#pragma once

#include "../VectorTimer/VectorTimer.h"
namespace Network
{
	/*enum UpdateRelathionship
	{
		UPDATES_CONCURRENT,
		LHS_LATEST,
		RHS_LATEST,
		STARING
	};

	template <typename T>
	class DetermineUpdate
	{
	public:
		static UpdateRelathionship DetermineUpdatesSequence(const FixedVectorTimer<T>& lhs, const FixedVectorTimer<T>& rhs)
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
		}
*/
	//};
}

