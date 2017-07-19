#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		Camera.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include "../Point/Point3D/Point3D.h"
#include "../UtilClasses/Common.h"
#include "../Basis/OrthonormalBasis.h"
#include "./Paths/CameraPaths.h"
#include "./Paths/LagrangeInterpolationMethods.h"
#include "./Paths/Path.h"
#include "./Paths/RandomPathGenerator/RandomPathGenerator.h"


namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: PrimitiveBvhRepresentation
	//		   Description: Wraps around geometric objects before sorted out in Acceleration Structure
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class Camera
	{
	public:
		friend class World;
		
		float m_fov;
		float m_aspectRatio;
		float m_viewPlaneDistance;
		Point3D eye;
		int m_totalCreatedFrames;
		CameraPaths* m_path;
		float m_deltaMovment;
		Point3D m_currentDirection;
		bool m_pathExists;
		vector<Engine::Paths::Path*> m_paths;
		float m_timeLimit;
		float m_currentTime;


		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Camera()
		{
			eye = Point3D(0, 0, 1);
			m_orthonomalVectors.w = Point3D(0, 0, -4);
			m_orthonomalVectors.up = Point3D(0, 1, 0);
			//v = w.cr
			//ComputeUvw();
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: MoveCamera
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void MoveCamera()
		{
			m_totalCreatedFrames++;
			Checkpoint* currentCheckPoint = m_path->GetNextPosition();
			eye = currentCheckPoint->m_position;
			RotateComputeUvw(currentCheckPoint->m_angle);
		}

		void MoveCameraWithDirectionDefined()
		{
			m_totalCreatedFrames++;
			Checkpoint currentCheckPoint = Checkpoint(Point3D(0));

			bool foundPath = m_path->GetNextPosition(currentCheckPoint);
			eye = currentCheckPoint.m_position;
			m_currentDirection = currentCheckPoint.m_direction;	
		}


		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: MoveCamera 
		//		Return Value: VOID
		//		  Parameters: Relative to the time taken to render previous frame
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void MoveCamera(float p_timeTaken)
		{
			//add current time to total time 
			m_currentTime += p_timeTaken;

			//if time taken is smaller than imposed time 
			if (p_timeTaken <= m_path->m_timePerFrame)
			{

				//this_thread::sleep_for(chrono::seconds(abs(m_path->m_timePerFrame - p_timeTaken)));
				//get next frame 
				Checkpoint* currentCheckPoint = m_path->GetNextPosition();
				eye = currentCheckPoint->m_position;
				RotateComputeUvw(currentCheckPoint->m_angle);
			}
			//else 
			else
			{
				int nextPos = 0;
				//find the time difference 
				float timeDifference = abs(m_path->m_timePerFrame - p_timeTaken);
				//get the amount of frames that the exceeding amount amounts to
				int roundedAmount = round(timeDifference / m_path->m_timePerFrame);
				//get the extra amount 
				Checkpoint* currentCheckPoint = m_path->GetNextPosition(roundedAmount);
				eye = currentCheckPoint->m_position;
				//check here
				m_currentDirection = currentCheckPoint->m_direction;
			}
		}

		float round(float d)
		{
			return static_cast<float>(floor(d + 0.5));
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Camera(Point3D p_eye, Point3D p_lookat)
		{
			eye = p_eye;
			lookAt = p_lookat;
			ComputeUvw();
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: Eye Postion, Lookat Posiion, Field of View float, aspect Ratio, Distance between viewplane and eye
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Camera(Point3D p_eye, Point3D p_lookat, float p_fov, float p_aspectRatio, float p_viewPlaneDistance)
		{
			m_fov = p_fov;
			m_aspectRatio = p_aspectRatio;
			m_viewPlaneDistance = p_viewPlaneDistance;
			eye = p_eye;
			lookAt = p_lookat;
			ComputeUvw();
		}

		Camera(float p_fov, float p_aspectRatio, float p_viewPlaneDistance, float p_deltaMovement, string p_randomKey, string p_configPath)
		{
			_ASSERT(p_randomKey != "");
			m_fov = p_fov;
			m_aspectRatio = p_aspectRatio;
			m_viewPlaneDistance = p_viewPlaneDistance;
			m_pathExists = true;
			m_deltaMovment = p_deltaMovement;
			
			m_path = new Engine::Paths::RandomPaths(p_configPath, p_randomKey);
			m_currentDirection = m_path->GetInitDirection();
			eye = m_path->GetNextPosition()->m_position;
			RotateComputeUvw(0);
		}

		Camera(float p_fov, float p_aspectRatio, float p_viewPlaneDistance, float p_deltaMovement, int p_pathNumber, string p_configPath, float p_totalTimeForSequence)
		{
			_ASSERT(p_pathNumber != -1);
			m_fov = p_fov;
			m_aspectRatio = p_aspectRatio;
			m_viewPlaneDistance = p_viewPlaneDistance;
			m_pathExists = true;
			m_deltaMovment = p_deltaMovement;

			m_path = new Engine::Paths::RandomPaths(p_configPath, p_pathNumber,p_deltaMovement, p_totalTimeForSequence);
			//m_path = new Engine::Paths::RandomPaths(p_configPath, p_pathNumber, p_totalTimeForSequence);
			m_currentDirection = m_path->GetInitDirection();
			eye = m_path->GetNextPosition()->m_position;
			RotateComputeUvw(0);
		}

		Camera(float p_fov, float p_aspectRatio, float p_viewPlaneDistance, int p_pathNumber, string p_configPath, int p_amountSegements)
		{
			_ASSERT(p_pathNumber != -1);
			m_fov = p_fov;
			m_aspectRatio = p_aspectRatio;
			m_viewPlaneDistance = p_viewPlaneDistance;
			m_pathExists = true;
			m_deltaMovment = 0;

			//m_path = new Engine::Paths::RandomPaths(p_configPath, p_pathNumber,p_deltaMovement, p_totalTimeForSequence);
			//m_path = new Engine::Paths::RandomPaths(p_configPath, p_pathNumber,p_amountSegements);
			m_path = new Engine::Paths::LagrangeInterpolation(p_configPath, p_pathNumber, p_amountSegements);
			Checkpoint currentCheckpoint;
			
			bool foundFirstPosition = m_path->GetNextPosition(currentCheckpoint);
			
			if(foundFirstPosition)
			{
				m_currentDirection = currentCheckpoint.m_direction;
				eye = currentCheckpoint.m_position;
				RotateComputeUvw(0);
			}

			
		}


		Camera(Point3D p_eye, float p_angle, Point3D p_initDirecton, float p_fov, float p_aspectRatio, float p_viewPlaneDistance, vector<Engine::Paths::Path*> p_pathPoints, int p_pathNumber,float p_deltaMovement)
		{
			m_fov = p_fov;
			m_aspectRatio = p_aspectRatio;
			m_viewPlaneDistance = p_viewPlaneDistance;
			eye = p_eye;
			m_pathExists = true;
			m_deltaMovment = p_deltaMovement;
			m_currentDirection = p_initDirecton;
			m_paths = p_pathPoints;
			//m_path = new LagrangeInterpolation(p_pathPoints[p_pathPoints]->GetCheckPoints() , "", p_deltaMovement);
			eye = m_path->AmountCrumbles() > 0 ? m_path->GetNextPosition()->m_position : p_eye;
			RotateComputeUvw(p_angle);
		}


		Camera(Point3D p_eye, float p_angle, Point3D p_initDirection, float p_fov, float p_aspectRatio, float p_viewPlaneDistance)
		{
			m_fov = p_fov;
			m_aspectRatio = p_aspectRatio;
			m_viewPlaneDistance = p_viewPlaneDistance;
			eye = p_eye;
			m_pathExists = false;
			m_currentDirection = p_initDirection;
			RotateComputeUvw(p_angle);
		}


		Camera(Point3D p_eye, Point3D p_lookat, float p_fov, float p_aspectRatio, float p_viewPlaneDistance, vector<Checkpoint*> p_pathPoints, float p_deltaMovement)
		{
			m_fov = p_fov;
			m_aspectRatio = p_aspectRatio;
			m_viewPlaneDistance = p_viewPlaneDistance;
			eye = p_eye;
			lookAt = p_lookat;
			m_pathExists = true;
			m_deltaMovment = p_deltaMovement;
			//m_path = new Paths::LagrangeInterpolation(p_pathPoints,"", p_deltaMovement);
			ComputeUvw();
		}

		Camera(Point3D p_eye, float p_fov, float p_aspectRatio, float p_viewPlaneDistance, Point3D p_up, Point3D p_w, Point3D p_v)
		{
			m_fov = p_fov;
			m_aspectRatio = p_aspectRatio;
			m_viewPlaneDistance = p_viewPlaneDistance;
			eye = p_eye;
			m_pathExists = true;
			ComputeUvw(p_up, p_w, p_v);
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ComputeUVW
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void ComputeUvw()
		{
			Point3D output = lookAt - eye;
			output.Normalize();

			m_orthonomalVectors.ProduceOrthonomalBasis(output);
		}

		void ComputeUvw(Point3D p_up, Point3D p_w, Point3D p_v)
		{
			m_orthonomalVectors.ProduceOrthonomalBasis(p_w, p_v, p_up);
		}


		void RotateComputeUvw(float p_angle)
		{	
			//Point3D directionVector(-0.0185, 0.071234, 0.997288);
			m_currentDirection.Normalize();
			Point3D rotatedVector = RotateCameraByYAxis(m_currentDirection, p_angle);
			m_currentDirection = rotatedVector;
			m_orthonomalVectors.ProduceOrthonomalBasis(rotatedVector);
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetCameraDirection
		//		Return Value: Point3D
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Point3D GetCameraDirection(Point3D p_otherCamera)
		{
			Point3D x = p_otherCamera.x * m_orthonomalVectors.v;
			Point3D y = p_otherCamera.y * m_orthonomalVectors.w;
			Point3D z = p_otherCamera.z * m_orthonomalVectors.up;

			Point3D newOutput = p_otherCamera.x * m_orthonomalVectors.v + ((-1)*p_otherCamera.z) * m_orthonomalVectors.w + p_otherCamera.y * m_orthonomalVectors.up;
			newOutput.normalize();
			return newOutput;
		}

		void SetTotalTimeLimit(float p_timeLimit)
		{
			m_path->m_totalTimeAnimation = p_timeLimit;
			m_timeLimit = p_timeLimit;
		}

		void SetDeltaMovment(float p_deltaMovement)
		{
			m_path->SetDeltaMovement(p_deltaMovement);
			m_deltaMovment = p_deltaMovement;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Rotate camera by y coordinate axis. </summary>
		///
		/// <remarks>	Adrian, 07/10/2015. </remarks>
		///
		/// <param name="p_vectorToRotate">	The vector to rotate. </param>
		/// <param name="p_angle">		   	The angle. </param>
		///
		/// <returns>	A Point3D. </returns>
		///-------------------------------------------------------------------------------------------------

		Point3D RotateCameraByYAxis(Point3D p_vectorToRotate, float p_angle)
		{
			Point3D output(0);
			float pi = 3.1415926;

			float angleRadiance = p_angle * pi / 180;
			output.x = p_vectorToRotate.x * cos(angleRadiance) + p_vectorToRotate.z * sin(angleRadiance);
			output.y = p_vectorToRotate.y;
			output.z = (-1 * p_vectorToRotate.y * sin(angleRadiance)) + p_vectorToRotate.z * cos(angleRadiance);

			return output;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: DESTRUCTOR
		//		Return Value: Point3D
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		~Camera()
		{
			if (m_pathExists)
			{
				delete m_path;
			}
		}

		Point3D GetLookAt()
		{
			return lookAt;
		}

		string GetToStringOrthonomal()
		{
			return m_orthonomalVectors.ToString();
		}

		Point3D GetCameraPosition()
		{
			return eye;
		}

		Point3D GetCameraDirection()
		{
			return m_orthonomalVectors.w;
		}


	protected:
		
		//look at position
		Point3D lookAt;
		//Up, x axis and direction
		OrthonomalBasis m_orthonomalVectors;
		//Delta time
		float exposureTime;
		float viewPlaneDistance;

	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------
