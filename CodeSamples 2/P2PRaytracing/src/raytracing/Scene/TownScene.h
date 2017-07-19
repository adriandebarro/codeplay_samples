#pragma once 

#include "Scene.h"
#include "../Light/PointLight.h"
#include "../Light/Directional.h"
#include "../CoreClasses/GeometricObjects/MeshObject.h"
#include "../Light/Ambient.h"





using std::endl;
using std::cout;
namespace Engine
{
	class	TownScene : public Scene
	{
	public:
		TownScene()
		{
			BuildScene();
		}

		TownScene(string p_path)
		{
			BuildScene();
		}

		TownScene(int p_pathToTake)
		{
			PrepAllPaths();
			BuildScene(p_pathToTake);
		}
		
	/*	TownScene(int p_pathNumber, string p_pathConfigFile, float p_deltaFrames, float p_totalTimeForSequence)
		{
			m_deltaFrames = p_deltaFrames;
			m_amountSegements = 0;
			m_totalTimeForSequence = p_totalTimeForSequence;
			BuildScene(p_pathNumber, p_pathConfigFile, "");
		}*/


		TownScene(int p_pathNumber, string p_pathConfigFile, int p_amountSegments, string p_pathObjFile)
		{
			m_deltaFrames = 0;
			m_totalTimeForSequence = 0;
			m_amountSegements = p_amountSegments;
			BuildScene(p_pathNumber, p_pathConfigFile, 35, p_pathObjFile);
			AddSceneLight();
		}

		TownScene(int p_pathNumber, string p_pathConfigFile, int p_amountSegments, string p_pathObjFile, map<string, Light*> p_allLight )
		{
			m_deltaFrames = 0;
			m_totalTimeForSequence = 0;
			m_amountSegements = p_amountSegments;
			BuildScene(p_pathNumber, p_pathConfigFile,10, p_pathObjFile);
			AddSceneLight(p_allLight);
		}

		TownScene(int p_pathNumber, string p_pathConfigFile, int p_amountSegments, int p_fov)
		{
			m_deltaFrames = 0;
			m_totalTimeForSequence = 0;
			m_amountSegements = p_amountSegments;
			BuildScene(p_pathNumber, p_pathConfigFile, p_fov);
		}
		
		using Scene::AddSceneLight;
		virtual void AddSceneLight()
		{}

		void BuildScene(int p_pathNumber, string p_pathConfigFile, int p_fov = 10, string p_pathToObj = 0)
		{
			Camera* currentCamera = nullptr;

			if (m_deltaFrames == 0 && m_totalTimeForSequence == 0)
				currentCamera = new  Engine::Camera(p_fov, 1, 3, p_pathNumber, p_pathConfigFile, m_amountSegements);
			else
				currentCamera = new  Engine::Camera(p_fov, 1, 3, m_deltaFrames, p_pathNumber, p_pathConfigFile,  m_totalTimeForSequence);
			
			_ASSERT(currentCamera != nullptr);

			cout << currentCamera->GetToStringOrthonomal() << endl;
			SetCamera(currentCamera);

			/*Point3D pointlightLocation(2.3, -9, -93);*/

			
			ObjectMesh* cornellObject = new ObjectMesh(p_pathToObj, Point3D(0, 0, 0), true);
			cornellObject->BuildScene(false,1);
			m_sceneObjects.push_back(cornellObject);
		}


		void PathFileParser(string p_filePath)
		{}

		using Scene::BuildScene;
		void BuildScene(int p_path = 0, string p_pathToObjFile = "")
		{
			Point3D lookAtVector(1,0,0);

			vector<Checkpoint*> path;

			
			path = m_scenePaths[p_path]->GetCheckpoints();
			
			
			Point3D directionVector(-0.0185, 0.071234, 0.997288);
			//{2.3,-10,-93}
			//Camera* currentCamera = new  Engine::Camera(Point3D(2.3, -5, -91), 0 ,directionVector, 30, 1, 1, checkPoints, 4);
			//Camera* currentCamera = new  Engine::Camera(Point3D(2.3, -5, -33), 0, directionVector, 30, 1, 3, path, 7);
			//cout << currentCamera->GetToStringOrthonomal() << endl;
			//SetCamera(currentCamera);


			cout << "THIS IS DEPECRATED" << "\n";

			Point3D pointlightLocation(2.3, -9, -93);

			Directional* directionalLight = new Directional(Point3D(1,-1,0), RGBColor(1, 1, 1), 1, 15);
			PointLight* secondPointLight = new PointLight(Point3D(-15, -5, -33), RGBColor(1), 1, 2000);
			PointLight* currentLight = new PointLight(Point3D(2.3, -5, -33), RGBColor(1), 1, 2000);
			//Directional* currentLight = new Directional(Point3D(-1, -1, 0));
			//m_sceneLight.push_back(currentLight);
			m_sceneLight.push_back(directionalLight);
			//m_sceneLight.push_back(secondPointLight);

			
			ObjectMesh* cornellObject = new ObjectMesh(p_pathToObjFile, Point3D(0,0,0), true);
			cornellObject->BuildScene(false);
			m_sceneObjects.push_back(cornellObject);
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Prep all paths. 4 paths exist toll; now</summary>
		/// 
		/// <remarks>	Adrian, 31/10/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		void PrepAllPaths()
		{
			vector<Checkpoint*> checkPointsPath1;

			checkPointsPath1.push_back(new Checkpoint(Point3D(2.3, -5, -91), 0));
			checkPointsPath1.push_back(new Checkpoint(Point3D(2.3, -5, -35), 0));
			checkPointsPath1.push_back(new Checkpoint(Point3D(-30, -5, -35), -100));
			checkPointsPath1.push_back(new Checkpoint(Point3D(-30, -5, -93), -160));

			vector<Checkpoint*> checkPointsPath3;

			checkPointsPath3.push_back(new Checkpoint(Point3D(-22, -11, 0.57), 0));
			checkPointsPath3.push_back(new Checkpoint(Point3D(26.496, -11, 0.57), 90));
			checkPointsPath3.push_back(new Checkpoint(Point3D(26.496, -11, 40), -120));
			checkPointsPath3.push_back(new Checkpoint(Point3D(2.303, -11, 45), -90));

			vector<Checkpoint*> checkPointsPath4;

			checkPointsPath4.push_back(new Checkpoint(Point3D(7.0, -8, -91), 0));
			checkPointsPath4.push_back(new Checkpoint(Point3D(7.0, -8, 0.57), 0));
			checkPointsPath4.push_back(new Checkpoint(Point3D(26.496, -8, 0.57), 90));
			checkPointsPath4.push_back(new Checkpoint(Point3D(26.496, -8, 40), -120));
			checkPointsPath4.push_back(new Checkpoint(Point3D(2.303, -8, 45), -90));

			vector<Checkpoint*> checkPointsPath2;

			checkPointsPath2.push_back(new Checkpoint(Point3D(-33, -5, -93), 0));
			checkPointsPath2.push_back(new Checkpoint(Point3D(-33, -5, -35), 0));
			checkPointsPath2.push_back(new Checkpoint(Point3D(2.3, -5, -35), 90));
			checkPointsPath2.push_back(new Checkpoint(Point3D(2.3, -5, -91), 90));
			
			m_scenePaths.push_back(new Path(checkPointsPath1));
			m_scenePaths.push_back(new Path(checkPointsPath2));
			m_scenePaths.push_back(new Path(checkPointsPath3));
			m_scenePaths.push_back(new Path(checkPointsPath4));
		}


	private:
		int m_chosenPath;
		int m_amountSegements;
		float m_deltaFrames; 
		float m_totalTimeForSequence;
	};
}