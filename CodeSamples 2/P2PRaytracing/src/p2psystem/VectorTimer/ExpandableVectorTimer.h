#pragma once 

#include "VectorTimer.h"

namespace Network
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	An expandable vector timer. Never implemented, fixed vector timer was always used  </summary>
	/// 
	///  <summary> https://www.researchgate.net/publication/221134121_Dynamic_Vector_Clocks_for_Consistent_Ordering_of_Events_in_Dynamic_Distributed_Applications </summary>
	///
	/// <remarks>	Adrian, 07/07/2017. </remarks>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class ExpandableVectorTimer : public VectorTimer
	{
		
	};
}
