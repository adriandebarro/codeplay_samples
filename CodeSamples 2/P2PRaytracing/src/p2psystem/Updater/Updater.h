#pragma once

#include "../VectorTimer/VectorTimer.h"

namespace Network
{
	class Updater
	{
	public:
		virtual void MergeUpdates(VectorTimer& lhs, VectorTimer& rhs);
		virtual std::string ParseUpdates();
		virtual void SetUpdates(std::string& updates);
		virtual void ConvertUpdate(std::string&);
	};
}

