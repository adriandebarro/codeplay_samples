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

		enum FrameState
		{
			MOVE,
			TURN
		};

		class FrameStateDescription
		{
		public:
			FrameState m_state;
			Point3D m_direction;

			FrameStateDescription(Point3D p_direction, FrameState p_currentState)
			{
				m_direction = p_direction;
				m_state = p_currentState;
			}

		};


		const int MAX_AMOUNT_CHECKPOINTS = 7;

		class RandomPaths : public CameraPaths
		{
		public:

			vector<string> m_paths;
			Checkpoint* m_currentCheckpoint;
			vector<FrameStateDescription*> m_allFramesDescriptions;


			///-------------------------------------------------------------------------------------------------
			/// <summary>	Generates a random path. </summary>
			///
			/// <remarks>	Adrian, 05/11/2015. </remarks>
			///
			/// <param name="p_IpAddress">	  	The IP address. </param>
			/// <param name="p_variance">	  	The variance. </param>
			/// <param name="p_applyVariance">	true to apply variance. </param>
			///
			/// <returns>	null if it fails, else the random path. </returns>
			///-------------------------------------------------------------------------------------------------

			vector<Checkpoint*> GenerateRandomPath(string p_ipAddress, Point3D p_variance = Point3D(0), bool p_applyVariance = false)
			{
				assert(p_ipAddress != "");

				vector<Checkpoint*> pathCheckPoints;
				int seed = GenerateIntegerFromIp(p_ipAddress);
				SetSRand(seed);

				Checkpoint* temp = nullptr;

				for (int x = 0; x < MAX_AMOUNT_CHECKPOINTS; x++)
				{
					if (x == 0)
					{
						int currentCheckPoint = rand() % m_checkPoints.size();
						temp = m_checkPoints[currentCheckPoint];
						pathCheckPoints.push_back(temp);
					}
					else
					{

						int currentCheckPoint = temp->GetIndex(rand() % temp->GetAMountCheckpoints());
						assert(currentCheckPoint >= 0);
						temp = m_checkPoints[currentCheckPoint != 0 ? currentCheckPoint - 1 : 0];

						if (pathCheckPoints.size() >= 2 && pathCheckPoints[x - 2]->m_position == temp->m_position)
						{
							//cycle break
							pathCheckPoints.push_back(temp);
							break;
						}

						pathCheckPoints.push_back(temp);
					}
				}

				assert(pathCheckPoints.size() > 0);
				return pathCheckPoints;
			}

			RandomPaths() : CameraPaths()
			{}

			RandomPaths(string p_path, string p_ipAddress, Point3D p_amountVariance = Point3D(0), bool p_applyVariance = false) : CameraPaths()
			{
				LoadPointsConfig(p_path);
				vector<Checkpoint*> chosenPoints = GenerateRandomPath(p_ipAddress);
				ComputeCrumbles(chosenPoints);
			}

			RandomPaths(string p_path, int p_pathNumber, float p_deltaMovment, float p_totalTimeForSequence, Point3D p_amountVariance = Point3D(0), bool papplyVariance = false) : CameraPaths()
			{
				//cout << p_pathNumber << endl;
				m_deltaMovment = p_deltaMovment;
				m_totalTimeAnimation = p_totalTimeForSequence;
				LoadMadePathsConfig(p_path);
				vector<Checkpoint*> chosenPoints = GeneratePath(p_pathNumber);
				//PrepAdaptablePath(chosenPoints);
				ComputeAdaptableCrumbles(chosenPoints);

			}

			RandomPaths(string p_path, int p_pathNumber, float p_totalTimeForSequence) : CameraPaths()
			{
				//cout << p_pathNumber << endl;
				//m_deltaMovment = p_deltaMovment;
				m_totalTimeAnimation = p_totalTimeForSequence;
				LoadMadePathsConfig(p_path);
				vector<Checkpoint*> chosenPoints = GeneratePath(p_pathNumber);
				//PrepAdaptablePath(chosenPoints);
				ComputeAdaptableCrumbles(chosenPoints);
			}

			RandomPaths(string p_path, int p_pathNumber, int p_amountSegements) : CameraPaths()
			{
				//cout << p_pathNumber << endl;
				//m_deltaMovment = p_deltaMovment;
				m_totalTimeAnimation = 0;
				LoadMadePathsConfig(p_path);
				vector<Checkpoint*> chosenPoints = GeneratePath(p_pathNumber);
				//PrepAdaptablePath(chosenPoints);
				//ComputeAdaptableCrumbles(chosenPoints);
				AddXAmountCrumblesPerSegment(chosenPoints, p_amountSegements);
			}



			/// Adds the x amount crumbvles per segment.
			/// </summary>
			/// <param name="p_checkpoints">The p_checkpoints.</param>
			/// <param name="p_amountCrumbles">The p_amount crumbles.</param>
			void AddXAmountCrumblesPerSegment(vector<Checkpoint*> p_checkpoints, int p_amountPiecesPerSegment = 4)
			{
				//loop through all the points and compute 
				for (int index = 1; index < p_checkpoints.size(); index++)
				{
					Checkpoint* currentCheckpoint = p_checkpoints[index - 1];
					Checkpoint* nextCheckpoint = p_checkpoints[index];

					Point3D direction = (nextCheckpoint->m_position - currentCheckpoint->m_position);
					direction.normalize();

					currentCheckpoint->m_direction = direction;
					m_pathPoints.push_back(currentCheckpoint);

					int changeAxis = GetChangingAxis(currentCheckpoint->m_position, nextCheckpoint->m_position);

					//change along the x axis 
					if (changeAxis == 0)
					{
						Point3D startingPosition = currentCheckpoint->m_position;
						// total amount of change
						float amountChange = abs(nextCheckpoint->m_position.x) - abs(currentCheckpoint->m_position.x);
						float rateChange = amountChange / p_amountPiecesPerSegment;
						int direction = GetDirection(currentCheckpoint->m_position.x, nextCheckpoint->m_position.x);
						//produce new checkpoints
						for (int x = 0; x < p_amountPiecesPerSegment; x++)
						{
							startingPosition.x = direction == 0 ? startingPosition.x + rateChange : startingPosition.x - rateChange;
							Point3D direction = nextCheckpoint->m_position - startingPosition;
							direction.normalize();
							Checkpoint* checkpointToAdd = new Checkpoint(startingPosition, direction);
							m_pathPoints.push_back(checkpointToAdd);
						}
					}
					//change along the z axis 
					else
					{
						Point3D startingPosition = currentCheckpoint->m_position;
						// total amount of change
						float amountChange = abs(nextCheckpoint->m_position.z) - abs(currentCheckpoint->m_position.z);
						float rateChange = amountChange / p_amountPiecesPerSegment;
						int direction = GetDirection(currentCheckpoint->m_position.z, nextCheckpoint->m_position.z);
						//produce new checkpoints
						for (int z = 0; z < p_amountPiecesPerSegment; z++)
						{
							startingPosition.z = direction == 0 ? startingPosition.z + rateChange : startingPosition.z - rateChange;
							Point3D direction = nextCheckpoint->m_position - startingPosition;
							direction.normalize();
							Checkpoint* checkpointToAdd = new Checkpoint(startingPosition, direction);
							m_pathPoints.push_back(checkpointToAdd);

						}
					}


				}

				m_pathPoints.push_back(p_checkpoints[p_checkpoints.size() - 1]);
			}




			///-------------------------------------------------------------------------------------------------
			/// <summary>	Check change in y coordinate axis. </summary>
			///
			/// <remarks>	Adrian, 14/11/2015. </remarks>
			///
			/// <param name="pCurrentPoint">	The current point. </param>
			/// <param name="pNextPoint">   	The next point. </param>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			///-------------------------------------------------------------------------------------------------

			bool CheckChangeInYAxis(Point3D pCurrentPoint, Point3D pNextPoint)
			{
				return pCurrentPoint.y != pNextPoint.y;
			}

			template <int PRECISION>
			float roundP(float f)
			{
				const int temp = Pow<10, PRECISION>::result;
				return roundf(f*temp) / temp;
			}

			virtual void ComputeCrumbles(vector<Checkpoint*> p_PathCheckpoints)
			{
				assert(p_PathCheckpoints.size() > 1);

				Point3D currentDirection(0);
				Point3D previousDirection(0);

				//loop through the points and produce the curmbles
				for (int x = 0; x < p_PathCheckpoints.size() - 1; x++)
				{
					Checkpoint* currentCheckPoint = p_PathCheckpoints[x];
					Checkpoint* nextCheckpoint = p_PathCheckpoints[x + 1];

					if (x == 0)
					{
						//get the direction
						currentDirection = nextCheckpoint->m_position - currentCheckPoint->m_position;
						//normalize
						currentDirection.normalize();
						assert(!(currentDirection == 0));
						m_currentDirection = currentDirection;
					}
					else
					{
						previousDirection = currentDirection;
						assert(previousDirection != 0);
						currentDirection = nextCheckpoint->m_position - currentCheckPoint->m_position;
						currentDirection.normalize();
						float cosAngle = floor(currentDirection.dot(previousDirection) * 10000 + 0.5) / 10000;
						assert((cosAngle <= 1 && cosAngle >= -1));
						std::setprecision(5);


						if (cosAngle == 1)
						{

						}
						else if (cosAngle == -1)
						{
							//need to rotate by 180 degrees
							RotateAndAdd(180, currentCheckPoint->m_position);
						}
						else if (cosAngle == 0)
						{
							//need to determine between left or right
							//TODO CHECK HERE need t ofigure pout direction if left or right

							int changeAxis = GetChangingAxis(currentCheckPoint->m_position, nextCheckpoint->m_position);
							int direction = -1;

							float currentPos = 0;
							float nextPos = 0;

							if (changeAxis == 0)
							{
								currentPos = currentCheckPoint->m_position.x;
								nextPos = nextCheckpoint->m_position.x;
							}
							else
							{
								currentPos = currentCheckPoint->m_position.z;
								nextPos = nextCheckpoint->m_position.z;
							}

							direction = GetDirection(currentPos, nextPos);

							if (direction == 0)
								RotateAndAdd(90, currentCheckPoint->m_position);
							else
								RotateAndAdd(-90, currentCheckPoint->m_position);
						}
						else
						{
							setprecision(5);
							//need to rotate according to the amount of angle
							float angle = std::acos(cosAngle);
							assert(angle < 90 && angle > -90);
							RotateAndAdd(angle, currentCheckPoint->m_position);
						}

					}

					bool changeYAxis = CheckChangeInYAxis(currentCheckPoint->m_position, nextCheckpoint->m_position);
					float amountChnageYAxis = 0;
					float deltaYAmount = 0;
					int movmentDirectiontY = 0;
					//if there is change we need to find how much and set the delta
					if (changeYAxis)
					{
						amountChnageYAxis = nextCheckpoint->m_position.y - currentCheckPoint->m_position.y;
						deltaYAmount = abs(amountChnageYAxis) / m_deltaMovment;
						movmentDirectiontY = GetDirection(currentCheckPoint->m_position.y, nextCheckpoint->m_position.y);
					}

					if (GetChangingAxis(currentCheckPoint->m_position, nextCheckpoint->m_position) == 0)
					{
						float deltaChange = abs(currentCheckPoint->m_position.x - nextCheckpoint->m_position.x) / m_deltaMovment;

						Checkpoint* toAdd = new Checkpoint(currentCheckPoint->m_position, 0);

						int direction = GetDirection(currentCheckPoint->m_position.x, nextCheckpoint->m_position.x);
						m_pathPoints.push_back(new Checkpoint(toAdd));


						while (CheckStatus(toAdd->m_position.x, nextCheckpoint->m_position.x, direction))
						{
							toAdd->m_position.x = direction == 0 ? toAdd->m_position.x + m_deltaMovment : toAdd->m_position.x - m_deltaMovment;
							if (changeYAxis)
							{
								toAdd->m_position.y = movmentDirectiontY == 0 ? toAdd->m_position.y + deltaYAmount : toAdd->m_position.y - m_deltaMovment;
							}
							m_pathPoints.push_back(new Checkpoint(toAdd->m_position, 0));
						}
					}
					else
					{
						//float deltaChange = abs(nextCheckPoint->m_position.z - currentCheckpoint->m_position.z) / m_deltaMovment;

						Checkpoint* toAdd = new Checkpoint(currentCheckPoint->m_position, 0);

						int direction = GetDirection(currentCheckPoint->m_position.z, nextCheckpoint->m_position.z);
						m_pathPoints.push_back(new Checkpoint(toAdd));


						while (CheckStatus(toAdd->m_position.z, nextCheckpoint->m_position.z, direction))
						{
							toAdd->m_position.z = direction == 0 ? toAdd->m_position.z + m_deltaMovment : toAdd->m_position.z - m_deltaMovment;
							if (changeYAxis)
							{
								toAdd->m_position.y = movmentDirectiontY == 0 ? toAdd->m_position.y + deltaYAmount : toAdd->m_position.y - m_deltaMovment;
							}
							m_pathPoints.push_back(new Checkpoint(Point3D(toAdd->m_position.x, toAdd->m_position.y, toAdd->m_position.z), 0));
						}
					}

					assert(m_pathPoints.size() > 0);
				}
			}


			void ComputeAdaptableCrumbles(vector<Checkpoint*> p_PathCheckpoints)
			{
				assert(p_PathCheckpoints.size() > 1);

				Point3D currentDirection(0);
				Point3D previousDirection(0);

				//loop through the points and produce the curmbles
				for (int x = 0; x < p_PathCheckpoints.size() - 1; x++)
				{
					Checkpoint* currentCheckPoint = p_PathCheckpoints[x];
					Checkpoint* nextCheckpoint = p_PathCheckpoints[x + 1];

					//
					if (x == 0)
					{
						//get the direction
						currentDirection = nextCheckpoint->m_position - currentCheckPoint->m_position;
						//normalize
						currentDirection.normalize();
						assert(!(currentDirection == 0));
						m_currentDirection = currentDirection;
					}
					else
					{

						previousDirection = currentDirection;
						assert(previousDirection != 0);
						currentDirection = nextCheckpoint->m_position - currentCheckPoint->m_position;
						currentDirection.normalize();
						float cosAngle = floor(currentDirection.dot(previousDirection) * 10000 + 0.5) / 10000;
						assert((cosAngle <= 1 && cosAngle >= -1));
						std::setprecision(5);


						if (cosAngle == 1)
						{

						}
						else if (cosAngle == -1)
						{
							//need to rotate by 180 degrees
							RotateAndAdd(180, currentCheckPoint->m_position, currentDirection);
						}
						else if (cosAngle == 0)
						{
							//need to determine between left or right
							//TODO CHECK HERE need t ofigure pout direction if left or right

							int changeAxis = GetChangingAxis(currentCheckPoint->m_position, nextCheckpoint->m_position);
							int direction = -1;

							float currentPos = 0;
							float nextPos = 0;

							if (changeAxis == 0)
							{
								currentPos = currentCheckPoint->m_position.x;
								nextPos = nextCheckpoint->m_position.x;
							}
							else
							{
								currentPos = currentCheckPoint->m_position.z;
								nextPos = nextCheckpoint->m_position.z;
							}

							direction = GetDirection(currentPos, nextPos);

							if (direction == 0)
							{
								RotateAndAdd(90, currentCheckPoint->m_position, currentDirection);
							}
							else
							{
								RotateAndAdd(-90, currentCheckPoint->m_position, currentDirection);
							}
						}
						else
						{
							setprecision(5);
							//need to rotate according to the amount of angle
							float angle = std::acos(cosAngle);
							assert(angle < 90 && angle > -90);
							RotateAndAdd(angle, currentCheckPoint->m_position, currentDirection);
						}
					}

					bool changeYAxis = CheckChangeInYAxis(currentCheckPoint->m_position, nextCheckpoint->m_position);
					float amountChnageYAxis = 0;
					float deltaYAmount = 0;
					int movmentDirectiontY = 0;
					//if there is change we need to find how much and set the delta
					if (changeYAxis)
					{
						amountChnageYAxis = nextCheckpoint->m_position.y - currentCheckPoint->m_position.y;
						deltaYAmount = abs(amountChnageYAxis) / m_deltaMovment;
						movmentDirectiontY = GetDirection(currentCheckPoint->m_position.y, nextCheckpoint->m_position.y);
					}



					if (GetChangingAxis(currentCheckPoint->m_position, nextCheckpoint->m_position) == 0)
					{
						Checkpoint* toAdd = new Checkpoint(currentCheckPoint->m_position, 0);

						int direction = GetDirection(currentCheckPoint->m_position.x, nextCheckpoint->m_position.x);
						m_pathPoints.push_back(new Checkpoint(toAdd));

						while (CheckStatus(toAdd->m_position.x, nextCheckpoint->m_position.x, direction))
						{
							toAdd->m_position.x = direction == 0 ? toAdd->m_position.x + m_deltaMovment : toAdd->m_position.x - m_deltaMovment;
							if (changeYAxis)
							{
								toAdd->m_position.y = movmentDirectiontY == 0 ? toAdd->m_position.y + deltaYAmount : toAdd->m_position.y - m_deltaMovment;
							}
							m_pathPoints.push_back(new Checkpoint(toAdd->m_position, currentDirection));
						}
					}
					else
					{
						Checkpoint* toAdd = new Checkpoint(currentCheckPoint->m_position, 0);

						int direction = GetDirection(currentCheckPoint->m_position.z, nextCheckpoint->m_position.z);
						m_pathPoints.push_back(new Checkpoint(toAdd));

						while (CheckStatus(toAdd->m_position.z, nextCheckpoint->m_position.z, direction))
						{
							toAdd->m_position.z = direction == 0 ? toAdd->m_position.z + m_deltaMovment : toAdd->m_position.z - m_deltaMovment;
							if (changeYAxis)
							{
								toAdd->m_position.y = movmentDirectiontY == 0 ? toAdd->m_position.y + deltaYAmount : toAdd->m_position.y - m_deltaMovment;
							}
							m_pathPoints.push_back(new Checkpoint(Point3D(toAdd->m_position.x, toAdd->m_position.y, toAdd->m_position.z), currentDirection));
						}
					}

					assert(m_pathPoints.size() > 0);

					//time alotted per frame 


				}
				m_timePerFrame = m_totalTimeAnimation / m_pathPoints.size();
			}


			virtual Point3D GetPoint(float p_x)
			{
				return Point3D();
			}

		private:



			///-------------------------------------------------------------------------------------------------
			/// <summary>	Generates an int from address. </summary>
			///
			/// <remarks>	Adrian, 04/11/2015. </remarks>
			///
			/// <param name="p_inputString">The input string. </param>
			///
			/// <returns>	The int from address. </returns>
			///-------------------------------------------------------------------------------------------------

			int GenerateIntegerFromIp(string p_IpAddress)
			{
				int primeNumbemr = 5409413;
				vector<string> pieces;
				Common::Tokenizer::ParseStringSentence(p_IpAddress, ".", pieces);
				int output = 0;
				for (int index = 0; index < pieces.size(); index++)
				{
					if (index == pieces.size() - 1)
					{
						output += (std::stoi(pieces[index]) * primeNumbemr);
					}
					else
						output += std::stoi(pieces[index]);
				}
				return output;
			}

			int GenerateIntegerFromRandomString(string p_RandomString)
			{
				int output = 0;

				for (int index = 0; index < p_RandomString.size(); index++)
				{
					output += (char)p_RandomString[index];
				}


				return output;
			}


			///-------------------------------------------------------------------------------------------------
			/// <summary>	Loads Checkpoints from configuration file. </summary>
			///
			/// <remarks>	Adrian, 04/11/2015. </remarks>
			///
			/// <param name="p_configPath">	Full pathname of the configuration file. </param>
			///-------------------------------------------------------------------------------------------------

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
						else if (firstChar == 'c')
						{
							vector<string> indexes;
							Common::Tokenizer::ParseStringSentence(pieces[1], ":", indexes);

							int index = stoi(indexes[0]) - 1;
							Checkpoint* currentCheckpoint = m_checkPoints[index];

							for (int x = 1; x < indexes.size(); x++)
							{
								int currentIndex = std::stoi(indexes[x]);
								currentCheckpoint->AddIndex(currentIndex);
							}
						}

						cout << currentLine << endl;
					}
				}
				assert(m_checkPoints.size() != 0);
			}

			vector<Checkpoint*> GeneratePath(int p_PathNumber)
			{
				//to return
				vector<Checkpoint*> toReturn;
				//path to process
				cout << p_PathNumber << endl;
				string path = m_paths[p_PathNumber - 1];

				vector<string> allPieces;
				Common::Tokenizer::ParseStringSentence(path, ",", allPieces);


				for (int index = 0; index < allPieces.size(); index++)
				{
					toReturn.push_back(m_checkPoints[std::stoi(allPieces[index]) - 1]);
				}

				return toReturn;
			}

			void LoadMadePathsConfig(string p_configPath)
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
			}

			void SetSRand(int p_seed)
			{
				srand(p_seed);
			}

			vector<Checkpoint*> m_allPoints;
		};
	}
}