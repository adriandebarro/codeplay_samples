#pragma once 

namespace Engine
{
	namespace Networking
	{
		namespace Contexts
		{
			enum DEVICE_TIERS
			{
				HIGH_END_CONNECTION = 1,
				GOOD_CONNECTION = 2,
				NORMAL_CONNECTION = 3,
				LOWER_CONNECTION = 4,
				WORST_CONNECTION = 5
			};

			struct DeviceTier
			{
				DEVICE_TIERS m_deviceTier;
				DeviceTier(DEVICE_TIERS p_deviceTier)
					: m_deviceTier(p_deviceTier)
				{}

				DeviceTier(int p_deviceTier) : m_deviceTier((DEVICE_TIERS)p_deviceTier)
				{}

				DeviceTier()
				{
					m_deviceTier = DEVICE_TIERS::NORMAL_CONNECTION;
				}

				int GetTierInt()
				{
					return (int)m_deviceTier;
				}
			};
		}
	}
}