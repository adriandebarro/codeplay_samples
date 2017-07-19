///-------------------------------------------------------------------------------------------------
// file:	CoreClasses\Camera\Paths\LagrangeInterpolationMethods.h
//
// summary:	Declares the lagrange interpolation method for path creation
#pragma once

#include "CameraPaths.h"
#include "..\..\Point\Point2D\Point2D.h"
#include "..\..\Point\Point3D\Point3D.h"

#include "Checkpoints/Checkpoints.h"


namespace Engine 
{
	namespace Paths
	{
		///-------------------------------------------------------------------------------------------------
		/// <summary>	A lagrange interpolation. </summary>
		///	<summary>  TODO : This method need to be fixed </summary>
		/// <remarks>	Adrian, 20/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------
		class LagrangeInterpolation : public CameraPaths
		{

		public:
			LagrangeInterpolation(string p_pathFileConfig,int p_pathNumber, float p_amountSegments) : CameraPaths()
			{
				_ASSERTE(p_amountSegments > 0 && p_pathNumber > 0 && p_pathFileConfig != "");

				LoadPointsConfig(p_pathFileConfig);
				vector<Checkpoint*> pathPoints = GeneratePath(p_pathNumber);
				this->m_amountSamplesPerSegment = p_amountSegments;

				if (pathPoints.size() % 2 != 0)
					InitLagrange(pathPoints);
				else
					cout << "Error not enough points for path...ERROR ERROR ERROR" << "\n";
			}

			void InitLagrange(vector<Checkpoint*> &p_interpolationPoints)
			{	
				for (int index = 0; index < p_interpolationPoints.size() - 1; index+=2)
				{
					m_currentPathPoints.erase(m_currentPathPoints.begin(), m_currentPathPoints.end());
					m_currentPathPoints.push_back(p_interpolationPoints[index]);
					m_currentPathPoints.push_back(p_interpolationPoints[index+1]);
					m_currentPathPoints.push_back(p_interpolationPoints[index+2]);
					InterpolateLagrange(m_currentPathPoints);
					ProducePoints();
				}

				cout << "path created consiting of: " << m_pathPoints.size() << "\n";
			}

			void InterpolateLagrange(vector<Checkpoint*> p_interpolationPoints)
			{
				float totalDistance = 0;
				m_pivotPoints.erase(m_pivotPoints.begin(), m_pivotPoints.end());
				m_pivotPoints.push_back(0);


				for (int index = 1; index < p_interpolationPoints.size(); index++)
				{
					totalDistance += Point3D::DistanceBetweenPoints(this->m_checkPoints[index]->m_position, this->m_checkPoints[index - 1]->m_position);
					m_pivotPoints.push_back(totalDistance);
				}

				for (int index = 0; index < m_pivotPoints.size(); index++)
				{
					m_pivotPoints[index] = m_pivotPoints[index] / totalDistance;
				}
			}

			void ProducePoints()
			{
				float currentPosition = 0;
				float segmentLength = 0.0f;
				float currentLimit = 0;
				float currentT = 0;

				for (int index = 0; index < 2; index++)
				{
					if (m_pivotPoints.size() == 5)
					{
						if (index == 0)
						{
							segmentLength = (m_pivotPoints[2] - 0) / m_amountSamplesPerSegment;
							currentLimit = m_pivotPoints[2];
							currentT = m_pathPoints.size() == 0? 0 : segmentLength;
						}
						else
						{
							segmentLength = (m_pivotPoints[4] - m_pivotPoints[2]) / m_amountSamplesPerSegment;
							currentLimit = m_pivotPoints[4];
							currentT = m_pivotPoints[2] + segmentLength;
						}
					}
					else
					{
						if (index == 0)
						{
							segmentLength = (m_pivotPoints[1] - 0) / m_amountSamplesPerSegment;
							currentLimit = m_pivotPoints[1];
							currentT = m_pathPoints.size() == 0 ? 0 : segmentLength;
						}
						else
						{
							segmentLength = (m_pivotPoints[2] - m_pivotPoints[1]) / m_amountSamplesPerSegment;
							currentLimit = m_pivotPoints[2];
							currentT = m_pivotPoints[1] + segmentLength;
						}
					}

					for (float tempTime = currentT; tempTime <= currentLimit; tempTime += segmentLength)
					{
						AddNextPathPoint(tempTime);
					}
				}
			}

			void AddNextPathPoint(float p_currentT)
			{
				m_pathPoints.push_back(GetCheckpoint(p_currentT));
			}

			using CameraPaths::GetPoint;
			virtual Point3D GetPoint(float p_x)
			{
				return Point3D();
			}

			virtual Point3D GetPoint(Point3D &p_direction)
			{
				return Point3D();
			}

			using CameraPaths::GetNextPosition;
			virtual Checkpoint* GetNextPosition()
			{
				if(m_currentPositionIndex < m_pathPoints.size())
				{ 
					Checkpoint* toReturn = m_pathPoints[m_currentPositionIndex];
					m_currentPositionIndex++;
				}
				else
				{
					m_currentPositionIndex++;
					return NULL;
				}
			}

			virtual bool GetNextPosition(Checkpoint &p_nextPosition)
			{
				if (m_currentPositionIndex < m_pathPoints.size())
				{
					p_nextPosition = *m_pathPoints[m_currentPositionIndex];
					m_currentPositionIndex++;
					return true;
				}
				else
					return false;

				return false;
			}

			Checkpoint* GetCheckpoint(float p_t)
			{
				float directionTime = p_t + kEpsilon;

				Point3D output = Interpolate(p_t);
				Point3D directionComponent = Interpolate(directionTime);

				Point3D direction = directionComponent - output;
				return new Checkpoint(output, direction);
			}

			vector<float> m_pivotPoints;

		private:

			Point3D Interpolate(float p_t)
			{
				Point3D output(0,0,0);
				for (int i = 0; i < m_currentPathPoints.size(); i ++)
				{
					float polynomialMult = LagrangePolynomial(p_t, i);
					output = output + m_currentPathPoints[i]->m_position * polynomialMult ;
				}

				return output;
			}

			float LagrangePolynomial(float p_t, float p_currentIteration)
			{
				float currentMultiplication = 1;

				for (int j = 0; j < m_currentPathPoints.size(); j ++)
				{
					if (j == p_currentIteration) continue;

					float numerator = p_t - m_pivotPoints[j];
					float denominator = m_pivotPoints[p_currentIteration] - m_pivotPoints[j];
					currentMultiplication *= (numerator / denominator);
				}

				return currentMultiplication;
			}

			using CameraPaths::ComputeCrumbles;
			virtual void ComputeCrumbles(vector<Checkpoint*> p_CheckPoints)
			{
				
			}

			float m_currentT = 0;

			private:
				vector<Checkpoint*> m_currentPathPoints;
				

				void LoadPointsConfig(string p_configPath)
				{
					assert(p_configPath != "");

					std::ifstream configFile(p_configPath);
					string currentLine = "";

					if (configFile.is_open())
					{
						string currentLine = "";

						while (getline(configFile, currentLine))
						{

							vector<string> pieces;
							Common::Tokenizer::ParseStringSentence(currentLine, " ", pieces);

							char firstChar = pieces[0][0];

							if (firstChar == 'p')
							{
								float xx = std::stof(pieces[1]);
								float yy = std::stof(pieces[2]);
								float zz = std::stof(pieces[3]);

								Point3D currentPosition(xx, yy, zz);
								m_checkPoints.push_back(new Checkpoint(currentPosition));

							}
							else if (firstChar == 'P' && pieces[0][1] == 'A')
							{
								m_paths.push_back(pieces[1]);
							}

#ifdef _DEBUG
							cout << currentLine << endl;
#endif // _DEBUG
						}
					}
					_ASSERT(m_checkPoints.size() != 0);
					_ASSERT(m_paths.size() > 0);
				}

				vector<Checkpoint*> GeneratePath(int p_PathNumber)
				{
					//to return
					vector<Checkpoint*> toReturn;
					//path to process
					cout << p_PathNumber << endl;
					string path = this->m_paths[p_PathNumber - 1];

					vector<string> allPieces;
					Common::Tokenizer::ParseStringSentence(path, ",", allPieces);


					for (int index = 0; index < allPieces.size(); index++)
					{
						toReturn.push_back(m_checkPoints[std::stoi(allPieces[index]) - 1]);
					}

					return toReturn;
				}

		};
	}
}