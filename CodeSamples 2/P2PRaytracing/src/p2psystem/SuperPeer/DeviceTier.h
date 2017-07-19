#pragma once 

namespace Network
{
	namespace SuperPeers
	{
		namespace Subcribers
		{
			enum CONNECTION_TIERS
			{
				HIGH_END_CONNECTION = 1,
				GOOD_CONNECTION = 2,
				NORMAL_CONNECTION = 3,
				LOWER_CONNECTION = 4,
				WORST_CONNECTION = 5
			};

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	A connection tier. Rates a network connection, 
			/// 				the bigger the number the less will the superpeer will communicate with it</summary>
			///
			/// <remarks>	Adrian. </remarks>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			struct ConnectionTier
			{
				CONNECTION_TIERS m_deviceTier;
				ConnectionTier(CONNECTION_TIERS p_deviceTier)
					: m_deviceTier(p_deviceTier)
				{}

				ConnectionTier(int p_deviceTier) : m_deviceTier((CONNECTION_TIERS)p_deviceTier)
				{}

				ConnectionTier()
				{
					m_deviceTier = CONNECTION_TIERS::NORMAL_CONNECTION;
				}

				int GetTierInt()
				{
					return (int)m_deviceTier;
				}
			};
		}
	}
}