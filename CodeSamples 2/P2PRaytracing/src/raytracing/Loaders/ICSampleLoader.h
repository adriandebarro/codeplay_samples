#include "../CoreClasses/UtilClasses/Common.h"
#include "../CoreClasses/UtilClasses/ConvertionHelper.h"
#include "../Integrators/IrradianceCache.h"
#include "../Integrators/TwoLayerIrradianceCache.h"

#include <iostream>
#include <fstream>
#include <string>

#include <boost/filesystem/convenience.hpp>


using std::string;
namespace Engine
{
	namespace Loaders
	{

		class ICSampleLoader
		{
		public:
			//-------------------------------------------------------------------------------------------
			// Constructors
			//-------------------------------------------------------------------------------------------

			ICSampleLoader(IrradianceCache* p_IC)
			{
				m_IC = p_IC;
				m_2IC = nullptr;
				m_ICType = 1;
			}
			//-------------------------------------------------------------------------------------------

			//-------------------------------------------------------------------------------------------
			// PopulateICFromBinaryFile: Imports samples from bindary file
			//							 RETURN: bool success
			//-------------------------------------------------------------------------------------------
			virtual bool PopulateICFromBinaryFile(string p_sceneName)
			{
				string path = BINARY_FOLDER_NAME + BINARY_FILE_NAME + p_sceneName + ".raw";

				return ImportSamples(path);
			}
			//-------------------------------------------------------------------------------------------

			//-------------------------------------------------------------------------------------------
			// ExportSamples: Exports the samples stored in the IC as a .raw file
			//				  RETURN: bool success
			//-------------------------------------------------------------------------------------------
			virtual bool ExportSamples(string p_sceneName)
			{
				unsigned char* sampleArray = new unsigned char[40];
				try
				{
					std::ofstream file;
					stringstream tester;
					InitExportFileStream(p_sceneName, file);

					#ifdef WAIT_FREE
						vector<RadiancePoint> icSamples;
						m_IC->GetOctree()->GetRadiancePointsArray(icSamples);
					#else
						vector<RadiancePoint> icSamples = m_IC->GetOctree()->GetRadiancePointsArray();
					#endif
					

					for (int sampleIndex = 0; sampleIndex < icSamples.size(); sampleIndex++)
					{
						vector<unsigned char> serializedSample;
						
						icSamples[sampleIndex].SerializePoint(serializedSample);
						SetCharArray(sampleArray, serializedSample);

						file.write((char*)sampleArray, 40);
					}

					file.flush();

					cout << tester.str() << endl;
				}
				catch(exception ex)
				{
					cout << "Error while exporting current scene samples" << endl;
					return false;
				}

				delete[] sampleArray;
				return true;
			}
			//-------------------------------------------------------------------------------------------

		private:

			//-------------------------------------------------------------------------------------------
			// Setchararray: change vector to char* array
			//-------------------------------------------------------------------------------------------
			void SetCharArray(unsigned char* p_Array, vector<unsigned char>& p_radiancePoint)
			{
				for (int charIndex = 0; charIndex < p_radiancePoint.size(); charIndex++)
				{
					p_Array[charIndex] = p_radiancePoint[charIndex];
				}
			}
			//-------------------------------------------------------------------------------------------

			//-------------------------------------------------------------------------------------------
			// ImportSamples: Imports the samples stored in the .raw file defined
			//				  RETURN: bool success
			//-------------------------------------------------------------------------------------------
			virtual bool ImportSamples(string p_filePath)
			{
				try
				{
					int length = 0;
					std::ifstream file(p_filePath, std::ifstream::binary);
					char* currentSampleDetails = new char[40];
					//string currentSampleDetails = "";
					if (file.is_open())
					{
						int count = 0;
						while(!file.eof())
						{
							RadiancePoint* currentPoint = new RadiancePoint(1);
							//file.read(currentSampleDetails, 40);
							file.read((char *)currentPoint, 40);
							//cout << currentPoint->ToString() << endl;
							m_IC->InsertImportedPoint(currentPoint);
							count++;
						}

						cout << "amount of points loaded " << count << endl;

						//while (length > 0)
						//{
						//	length -= 40;
						//	file.read(currentSampleDetails, 40);
						//	RadiancePoint* currentPoint = new RadiancePoint();
						//	//printf("%s \n", currentSampleDetails);
						//	stringstream ss;

						//	for (int index = 0; index < 40; index++)
						//	{
						//		ss << currentSampleDetails[index];
						//	}

						//	cout << ss.str() << endl;

						//	currentPoint->FromSerialize(currentSampleDetails);
						//	cout << currentPoint->ToString() << endl;
						//	//m_IC->AddToUpdates(currentPoint);
						//}

					/*	cout << length << endl;
						char currentChar = 0;
						int count = 0;
						while(length > 0)
						{
							file.get(currentChar);
							cout << (unsigned char)currentChar << endl;
							length -= 1;
							currentSampleDetails[count] = currentChar;
							count++;

							if(count > 39)
							{
								RadiancePoint* currentPoint = new RadiancePoint();
								currentPoint->FromSerialize(currentSampleDetails);
								count = 0;
							}
						}*/
					}
					else
					{
						cout << "---Couldnt load sample file ----" << endl;
					}
				}
				catch (std::exception ex)
				{
					std::cout << "Error occured during imprt" << "\n";
					return  false;
				}

				return true;
			}
			//-------------------------------------------------------------------------------------------

			//-------------------------------------------------------------------------------------------
			// InitExportFileStream: Initializa directory and create .raw file
			//-------------------------------------------------------------------------------------------
			virtual void InitExportFileStream(string p_sceneName, std::ofstream& p_stream)
			{
				boost::filesystem::path dir();

				if (!(boost::filesystem::exists(BINARY_FILE_NAME)))
				{
					boost::filesystem::create_directory(BINARY_FOLDER_NAME);
				}

				string path = BINARY_FOLDER_NAME + BINARY_FILE_NAME + p_sceneName + ".raw";


				p_stream.open(path, std::ifstream::binary);
			}
			//-------------------------------------------------------------------------------------------

			string BINARY_FILE_NAME = "IcExport_";
			string BINARY_FOLDER_NAME = "./ICExport/";
			int m_ICType;
			IrradianceCache* m_IC;
			TwoLayerIrradianceCache* m_2IC;

		};
	}
}

