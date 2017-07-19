///-------------------------------------------------------------------------------------------------
// file:	CoreClasses\Image Representation\ImageSaver\ImageSaver.h
//
// summary:	Takes care of saving curtrent snapshot shown in the viewer
//			For each run it will tak ecare of creating 
///-------------------------------------------------------------------------------------------------

#pragma once 

#include <string>
#include <ostream>
#include <algorithm>
#include <ctime>
#include <fstream>

#include <boost\filesystem.hpp>

#include "..\..\UtilClasses\Common.h"
#include "..\..\RGBObject\RGBColor.h"

using std::string;
using std::min;
using std::max;
using std::ofstream;

namespace Engine
{
	///-------------------------------------------------------------------------------------------------
	/// <summary>	A vid saver. </summary>
	///
	/// <remarks>	Adrian, 26/08/2015. </remarks>
	///-------------------------------------------------------------------------------------------------

	class VidSaver
	{ 
	public:

		RGBColor* m_rgbBuffer;
		string m_path;
		string m_sceneName;
		int m_imageHeight;
		int m_imageWidth;
		int m_currentFrame;
		const string FILE_NAME = "frame";
		string m_saveFilePath = "";
		float GAMMA_CONST = 4.18;

		VidSaver()
		{}
		
		VidSaver(int p_width, int p_height, string p_sceneName, RGBColor* p_rgbBuffer)
		{
			m_currentFrame = 0;
			m_sceneName = p_sceneName;
			CreateDir();
			m_rgbBuffer = p_rgbBuffer;
			m_imageHeight = p_height;
			m_imageWidth = p_width;
		}

		VidSaver(const VidSaver& p_current)
		{
			m_currentFrame = 0;
			m_sceneName = p_current.m_sceneName;
			CreateDir();
			m_rgbBuffer = p_current.m_rgbBuffer;
			m_imageHeight = p_current.m_imageHeight;
			m_imageWidth = p_current.m_imageWidth;
		}
		
		void CreateDir()
		{
			string currentDateTime = GetCurrentDateTime();

			boost::filesystem::path dir("./Backup/");

			if (!(boost::filesystem::exists("./Backup/")))
			{
				boost::filesystem::create_directory("./Backup/");
			}


			string fullPath = "./Backup/" + m_sceneName + "" + currentDateTime;
			m_path = fullPath;
			_mkdir(fullPath.c_str());
		}

		void SaveFrame()
		{
			WriteToFile();
		}

		void WriteToFile()
		{
			m_currentFrame++;
			string fullPath = m_path + "\\" + FILE_NAME + to_string(m_currentFrame) + ".ppm";
			try
			{
				int total = m_imageHeight * m_imageWidth;
				const char * charPath = fullPath.c_str();
				ofstream fp(charPath, ios::out | ios::binary);
				fp << "P6\n" << m_imageWidth << " " << m_imageHeight << "\n255\n";
				for (int index = 0; index < total; index++)
				{
					RGBColor currentColor = m_rgbBuffer[index];

					//writing each pixel ot file 

					fp << (unsigned char)currentColor.r <<
					(unsigned char)currentColor.g <<
					(unsigned char) currentColor.b;



					////[AD] Applying basic tonemapping to be revised with a better formula
					//float toneMappedR = (currentColor.r / (currentColor.r + 1)) * 255;
					//float toneMappedG = (currentColor.g / (currentColor.g + 1)) * 255;
					//float toneMappedB = (currentColor.b/ (currentColor.b + 1)) * 255;

					//fp << (unsigned char) toneMappedR <<
					//(unsigned char)toneMappedG <<
					//(unsigned char)toneMappedB;

					//gamma correction
						
					/*float gammaCorrectedR = powf((currentColor.r / 255), (1 / GAMMA_CONST)) * 255;
					float gammaCorrectedG = powf((currentColor.g / 255), (1 / GAMMA_CONST)) * 255;
					float gammaCorrectedB = powf((currentColor.b / 255), (1 / GAMMA_CONST)) * 255;

					fp << (unsigned char)gammaCorrectedR <<
						(unsigned char)gammaCorrectedG <<
						(unsigned char)gammaCorrectedB;*/

					/*fp << (unsigned char)(min(float(1), currentColor.r) * 255) <<
						(unsigned char)(min(float(1), currentColor.g) * 255) <<
						(unsigned char)(min(float(1), currentColor.b) * 255);*/
				}

				fp.close();
			}
			catch (exception e)
			{
				std::cerr << e.what() << endl;
			}
		}

		void operator = (const VidSaver& p_current)
		{
			m_currentFrame = 0;
			m_sceneName = p_current.m_sceneName;
			CreateDir();
			m_rgbBuffer = p_current.m_rgbBuffer;
			m_imageHeight = p_current.m_imageHeight;
			m_imageWidth = p_current.m_imageWidth;
		}

		
	private:

		string GetCurrentDateTime()
		{
			time_t rawtime;
			struct tm timeinfo;
			char buffer[80];

			time(&rawtime);
			localtime_s(&timeinfo,&rawtime);

			strftime(buffer, 80, "%d%m%Y%I%M%S", &timeinfo);
			std::string str(buffer);

			return str;
		}


		/// <summary>	Buffer for RGB data. </summary>
		
	};
}