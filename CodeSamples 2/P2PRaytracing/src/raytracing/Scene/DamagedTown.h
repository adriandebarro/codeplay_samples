#pragma once 

#include "Scene.h"
#include "../Light/PointLight.h"
#include "../Light/Directional.h"
#include "../CoreClasses/GeometricObjects/MeshObject.h"
#include "../Light/Ambient.h"

namespace Engine
{
		
	class DamagedTown : public Scene
	{
	private:
		int m_chosenPath;
		int m_amountSegements;
		float m_deltaFrames;
		float m_totalTimeForSequence;

	public:
		DamagedTown(int p_pathNumber, string p_pathConfigFile, int p_amountSegments, string p_objFileDetails)
		{
			m_deltaFrames = 0;
			m_totalTimeForSequence = 0;
			BuildScene(p_pathNumber, p_pathConfigFile, p_objFileDetails);
			AddSceneLight();
		}

		DamagedTown(int p_pathNumber, string p_pathConfigFile, int p_amountSegments, string p_objFileDetails,map<string, Light*> p_lightMap)
		{
			m_deltaFrames = 0;
			m_totalTimeForSequence = 0;
			BuildScene(p_pathNumber, p_pathConfigFile, p_objFileDetails);
			AddSceneLight(p_lightMap);
		}

		DamagedTown(int p_pathNumber = 1)
		{}

		//still image test
		using Scene::BuildScene;
		void BuildScene(int p_path = 0, string p_pathToObjFile = "")
		{}

		using Scene::AddSceneLight;
		virtual void AddSceneLight()
		{
			//the sun 
			Directional* directionalLight = new Directional(Point3D(1, -1, 0), RGBColor(1, 1, 1), 1, 15);
			m_sceneLight.push_back(directionalLight);
		}

		void BuildScene(int p_pathNumber,string p_pathConfigFile, string p_pathToObjFile, int p_fov = 40)
		{
			Camera* currentCamera = nullptr;

			if (m_deltaFrames == 0 && m_totalTimeForSequence == 0)
				currentCamera = new  Engine::Camera(p_fov, 1, 3, p_pathNumber, p_pathConfigFile, m_amountSegements);
			else
				currentCamera = new  Engine::Camera(p_fov, 1, 3, m_deltaFrames, p_pathNumber, p_pathConfigFile, m_totalTimeForSequence);

			_ASSERT(currentCamera != nullptr);
			AddSceneLight();
			SetCamera(currentCamera);
			string damagedTownFilePath = "\ObjFiles\\DamagedDowntown\\DownTown_Damage_0.obj";
			ObjectMesh* damagedTownObject = new ObjectMesh(p_pathToObjFile, Point3D(0, 0, 0), true);
			damagedTownObject->BuildScene(false, 0);
			m_sceneObjects.push_back(damagedTownObject);
		}
	};
}