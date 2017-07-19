#pragma once 

#include "../Sampler/RandomSampler.h"
#include "./MatlabUtilityDebuger.h"

#include <math.h>

namespace Engine
{
	namespace Debugging
	{
		class SampleDebugger
		{
		public:

			SampleDebugger(float p_sampleRate = 10, float p_radius = 2)
			{
				m_sampleRate = p_sampleRate;
				m_randomSampler = new Engine::Sampling::RandomSampler(12, 24, 0);
				m_randomSampler->GenerateSamples();
				m_randomSampler->MapSamplesToUnitDisk();
				m_randomSampler->MapSamplesToHemisphere(0, false);
				m_sphereRadius = p_radius;
				ProducePoints();
			}

			void TestSamples()
			{
				Twister twisterInstance;
				for (int sphereSamplesIndex = 0; sphereSamplesIndex < m_producedNormals.size(); sphereSamplesIndex++)
				{
					Point3D currentNormal, currentPosition;
					vector<Point3D> selectedSamples;
					vector <Point3D> rotatedSamples;

					currentNormal = m_producedNormals[sphereSamplesIndex];
					currentPosition = m_producedPositions[sphereSamplesIndex];

					string initialPoints = "";
					string resultingPoints = "";

					
					Point3D arbitraryAxisX, arbitraryAxisZ;

					for (int sampleIndex = 0; sampleIndex < m_randomSampler->GetSamplesPerPacket(); sampleIndex ++)
					{
						Point3D currentSample = m_randomSampler->SampleHemispherical(sampleIndex, m_producedNormals[sphereSamplesIndex], m_producedPositions[sphereSamplesIndex], arbitraryAxisX, arbitraryAxisZ, twisterInstance, selectedSamples);

						rotatedSamples.push_back(currentSample);
					}

					//initialPoints = Engine::Debugging::MatlabUtilityDebuger::PlotSamplePoints(Point3D(0, 0, 1), m_producedPositions[sphereSamplesIndex], selectedSamples);
					resultingPoints = Engine::Debugging::MatlabUtilityDebuger::PlotSamplePoints(m_producedNormals[sphereSamplesIndex], m_producedPositions[sphereSamplesIndex], rotatedSamples);

					selectedSamples.clear();
					rotatedSamples.clear();

					//cout << initialPoints << endl;

					cout << resultingPoints << endl;

					cout << "%---------------------------------------------------------------------------------------------------------------" << endl;

					cout << "sphereOrigin = [0 0 0]" << endl;

					cout << "plot(sphereOrigin(1), sphereOrigin(2), sphereOrigin(3),sphereOrigin(1), sphereOrigin(2), sphereOrigin(3), 'x');" << endl;

					//cout << "figure;" << endl;

				}
			}



		private:
			Engine::Sampling::RandomSampler* m_randomSampler;
			
			//producedNormals
			vector<Point3D> m_producedNormals;
			vector<Point3D> m_producedPositions;
			
			float m_sampleRate = 0;
			float m_sphereRadius = 0;


			void ProducePoints()
			{
				//will sample at 4 points on the azimuth axis 0, 90, 180, 270
				for (int azimuth = 0; azimuth <= 270; azimuth += 10)
				{
					//sampling on the az
					for (int zimuth = 0; zimuth <= 180; zimuth += m_sampleRate)
					{
						if(azimuth != 0 && zimuth == 0 || zimuth == 180 && azimuth != 0)
							continue;

						Point3D currentPosition(0);

						float radianZimuth = zimuth * 3.14159265359 / 180;
						float radianAzimuth = azimuth * 3.14159265359 / 180;

						currentPosition.x = m_sphereRadius * std::cos(radianAzimuth) * std::sin(radianZimuth);
						currentPosition.y = m_sphereRadius * std::sin(radianAzimuth) * std::sin(radianZimuth);
						currentPosition.z = m_sphereRadius * std::cos(radianZimuth);


						Point3D producedNormal = currentPosition - Point3D(0);
						producedNormal.Normalize();
						m_producedPositions.push_back(currentPosition);
						m_producedNormals.push_back(producedNormal);
					}
				}
			}

		};

	}
}