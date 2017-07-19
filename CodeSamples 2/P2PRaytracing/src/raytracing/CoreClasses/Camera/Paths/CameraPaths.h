///-------------------------------------------------------------------------------------------------
// file:	CoreClasses\Camera\Paths\CameraPaths.h
//
// summary:	Declares the camera paths class
///-------------------------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <string>

#include "..\..\Point\Point3D\Point3D.h"
#include "Checkpoints/Checkpoints.h"
#include "Checkpoints/Crumbles.h"
#include "../../Basis/OrthonormalBasis.h"

using std::vector;
using std::string;

using Engine::Paths::Checkpoints::Crumbles::Crumble;
using Engine::Paths::Checkpoints::Checkpoint;


namespace Engine
{
	///-------------------------------------------------------------------------------------------------
	/// <summary>	Will take care of camera paths. For a given amount of paths. </summary>
	///
	/// <remarks>	Adrian, 20/08/2015. </remarks>
	///-------------------------------------------------------------------------------------------------
	class CameraPaths
	{
	protected:
		/// <summary>	The path points. </summary>
		vector<Checkpoint*> m_pathPoints;
		vector<Checkpoint*> m_checkPoints;
		string m_pathName;
		int m_interpolationAxis;
		
		OrthonomalBasis* m_orthonomalBasis;
		float m_deltaMovment;
		int m_amountSamplesPerSegment;

		//to check
		Point3D m_currentDirection;

		vector<Checkpoint*> m_allPoints;
		vector<string> m_paths;

	public:

		int m_totalAmountFrames;
		float m_totalTimeAnimation;
		float m_timePerFrame;
		int m_currentPositionIndex;

		CameraPaths()
		  : m_currentPositionIndex(0)
		  , m_interpolationAxis(0)
		  , m_orthonomalBasis(nullptr)
		  , m_deltaMovment(1.0f)
		  , m_pathName(string())
		{}


		CameraPaths(string p_pathName, vector<Checkpoint> p_points, OrthonomalBasis* p_orthonomalBasis, float p_deltaMovment)
		  : m_currentPositionIndex(0)
		  , m_interpolationAxis(0)
		  , m_orthonomalBasis(nullptr)
		  , m_deltaMovment(p_deltaMovment)
		  , m_pathName(p_pathName)
    	{}


		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets a point. </summary>
		///
		/// <remarks>	Adrian, 20/08/2015. </remarks>
		///
		/// <param name="p_x">	The x coordinate to get the corresponding 3D point </param>
		///
		/// <returns>	3D Coordinates. </returns>
		///-------------------------------------------------------------------------------------------------
		virtual Point3D GetPoint(float p_x) = 0;

		virtual Point3D GetPoint(Point3D &p_direction)
		{
			return Point3D(0, 0, 0);
		}
		virtual void ComputeCrumbles(vector<Checkpoint*> p_PathCheckpoints) = 0;

		bool CheckMorePoints()
		{
			cout << "ready from from and " << m_currentPositionIndex << "and number of points is equal " << m_pathPoints.size() << " result is equal to " << m_currentPositionIndex << "result: " << (m_currentPositionIndex < m_pathPoints.size()) << "\n";
			return m_currentPositionIndex < m_pathPoints.size();
		}


		virtual Checkpoint* GetNextPosition()
		{
			if (m_currentPositionIndex < m_pathPoints.size() - 1)
			{
				int currentPosition = m_currentPositionIndex;
				m_currentPositionIndex++;
				return m_pathPoints[currentPosition];
			}
			else
			{
				int currentPosition = m_currentPositionIndex;
				m_currentPositionIndex++;
				return m_pathPoints[currentPosition];
			}
				
		}

		virtual Checkpoint* GetNextPosition(int p_nextPosition)
		{
			if (m_currentPositionIndex + p_nextPosition < m_pathPoints.size() - 1)
			{
				m_currentPositionIndex+= p_nextPosition;
				return m_pathPoints[m_currentPositionIndex];
			}
			else
			{
				if (m_currentPositionIndex != m_pathPoints.size() - 1)
				{
					m_currentPositionIndex = m_pathPoints.size() - 1;
					return m_pathPoints[m_currentPositionIndex];
				}
				else
					return m_pathPoints[m_currentPositionIndex];
			}
				
		}

		Checkpoint* GetNextPoint(float p_timeTaken)
		{
			//implement method here 
		}


		///-------------------------------------------------------------------------------------------------
		/// <summary>	Adds a vector of points to the list of points in the class </summary>
		///
		/// <remarks>	Adrian, 20/08/2015. </remarks>
		///
		/// <param name="p_x">	The x coordinate to get the corresponding 3D point </param>
		///
		/// <returns>	3D Coordinates. </returns>
		///-------------------------------------------------------------------------------------------------
		void AddPoints(vector<Checkpoint*>& p_pointsToAdd)
		{
			for (int pointIndex = 0; pointIndex < p_pointsToAdd.size(); pointIndex++)
			{
				m_pathPoints.push_back(p_pointsToAdd[pointIndex]);
			}
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Adds a point to the list of points. </summary>
		///
		/// <remarks>	Adrian, 20/08/2015. </remarks>
		///
		/// <param name="p_toAdd">	Point to be added to the list of path points</param>
		///
		/// <returns>	3D Coordinates. </returns>
		///-------------------------------------------------------------------------------------------------
		void AddPoint(Checkpoint* p_toAdd)
		{
			m_pathPoints.push_back(p_toAdd);
		}

		void SetDeltaMovement(float p_deltaMovement)
		{
			m_deltaMovment = p_deltaMovement;
		}

		Checkpoint* GetPathPointAt(int p_index)
		{
			if (p_index < m_pathPoints.size() && p_index >= 0)
				return m_pathPoints[p_index];
			else
				return nullptr;
		}

		int AmountCrumbles()
		{
			return ((int)m_pathPoints.size());
		}

		float GetInterpolationPointForZ(float p_x)
		{
			Point3D outputCoor(p_x, 0, 0);

			float x = p_x;
			float z = 0;
			float currentMultiplication = 1;
			for (int summationIndex = 0; summationIndex < m_checkPoints.size(); summationIndex++)
			{

				for (int multiplicationIndex = 0; multiplicationIndex < m_checkPoints.size(); multiplicationIndex++)
				{
					if (multiplicationIndex != summationIndex)
					{
						float xK = m_checkPoints[multiplicationIndex]->m_position.x;
						currentMultiplication *= ((p_x - xK) / (m_checkPoints[summationIndex]->m_position.x - xK));
					}
				}

				currentMultiplication *= m_checkPoints[summationIndex]->m_position.z;
				z += currentMultiplication;
				currentMultiplication = 1;
			}

			return z;
		}



		float GetInterpolationPointForX(float p_z)
		{
			Point3D outputCoor(0, 0, p_z);

			float x = 0;
			float currentMultiplication = 1;
			for (int summationIndex = 0; summationIndex < m_checkPoints.size(); summationIndex++)
			{

				for (int multiplicationIndex = 0; multiplicationIndex < m_checkPoints.size(); multiplicationIndex++)
				{
					if (multiplicationIndex != summationIndex)
					{
						float xK = m_checkPoints[multiplicationIndex]->m_position.z;
						currentMultiplication *= ((p_z - xK) / (m_checkPoints[summationIndex]->m_position.x - xK));
					}
				}

				currentMultiplication *= m_checkPoints[summationIndex]->m_position.z;
				x += currentMultiplication;
				currentMultiplication = 1;
			}

			return x;
		}

		static Point3D RotateCameraByYAxis(Point3D p_vectorToRotate, float p_angle)
		{
			Point3D output(0);
			float pi = 3.1415926;

			float angleRadiance = p_angle * pi / 180;
			output.x = p_vectorToRotate.x * cos(angleRadiance) + p_vectorToRotate.z * sin(angleRadiance);
			output.y = p_vectorToRotate.y;
			output.z = (-1 * p_vectorToRotate.x * sin(angleRadiance)) + p_vectorToRotate.z * cos(angleRadiance);

			return output;
		} 

		void RotateAndAdd(float p_totalAngel, Point3D p_location)
		{
			int direction = GetDirection(0, p_totalAngel);

			float rotationDelta = p_totalAngel / 6;
			float totalTurnAmoutn = 0;

			//_ASSERT(rotationDelta > 1 || rotationDelta < -1);

			while (CheckStatus(totalTurnAmoutn, p_totalAngel, direction))
			{
				m_pathPoints.push_back(new Crumble(p_location, rotationDelta));

				totalTurnAmoutn += rotationDelta;
			}
		}

		void RotateAndAdd(float p_totalAngel, Point3D p_location, Point3D &p_currentDirection)
		{
			_ASSERT(!(p_currentDirection == 0));

			int direction = GetDirection(0, p_totalAngel);

			float rotationDelta = p_totalAngel / 3;
			float totalTurnAmoutn = 0;

			//_ASSERT(rotationDelta > 1 || rotationDelta < -1);

			while (CheckStatus(totalTurnAmoutn, p_totalAngel, direction))
			{
				Point3D viewerDirection = RotateCameraByYAxis(p_currentDirection, rotationDelta);
				viewerDirection.Normalize();
				m_pathPoints.push_back(new Crumble(p_location, viewerDirection));
				p_currentDirection = viewerDirection;
				totalTurnAmoutn += rotationDelta;
			}
		}

		int GetDirection(float p_current, float p_next)
		{
			if (p_next > p_current)
				return 0;
			else
				return 1;
		}

		bool CheckStatus(float p_current, float p_final, int p_direction)
		{
			if (p_direction == 0)
				return(p_current < p_final);
			else
				return (p_current > p_final);
		}

		Point3D GetInitDirection()
		{
			return m_pathPoints[0]->m_direction;
		}

		virtual bool GetNextPosition(Checkpoint &p_nextPosition)
		{
			return false;
		}

	protected:
		int GetChangingAxis(Point3D p_currentPoint, Point3D p_nextPoint)
		{
			if (p_currentPoint.x == p_nextPoint.x)
			{
				return 2;
			}
			else
				return 0;
		}
	};
}