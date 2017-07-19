#pragma once 

#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <math.h>

#include "../../../UtilClasses/Common.h"
#include "../Checkpoints/Checkpoints.h"
#include "../CameraPaths.h"


using std::vector;
using std::string;

namespace Engine
{

	namespace Paths
	{

		class ConfigFilePath : public CameraPaths
		{
		public:
			ConfigFilePath(string p_filePath, int p_pathNumber)
			{

			}
		};
	}
}