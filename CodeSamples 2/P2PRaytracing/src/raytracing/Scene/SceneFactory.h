#pragma once 

#include "SimpleScene.h"
#include "FewCircles.h"
#include "BiggerScene.h"
#include "MeshScene.h"
#include "CornellBox.h"
#include "SibenikScene.h"
#include "TexturedBox.h"
#include "TestTextureScene.h"
#include "TownScene.h"
#include "DamagedTown.h"

#include "../CoreClasses/AssetsManager/SceneManager.h"

using Engine::ResourceManager::SceneManager;
namespace Engine
{
	class SceneFactory
	{
	public:
		static Scene* GetScene(int p_sceneType)
		{
			switch (p_sceneType)
			{
			case 1:
				return new SimpleScene();

			case 5:
				return new CornelBox();

			case 6:
				return new SibanekScene();

			case 7:
				return new TexturedBox();

			case 8:
				return new TestTextureScene();

			case 9:
				return new TownScene();
			case 10:
				return new DamagedTown();

			default:
				break;
			}
		}

		static Scene* GetScene(int p_sceneType, int p_path)
		{
			switch (p_sceneType)
			{
			case 1:
				return new SimpleScene();

			case 5:
				return new CornelBox();

			case 6:
				return new SibanekScene();

			case 7:
				return new TexturedBox();

			case 8:
				return new TestTextureScene();

			case 9:
				return new TownScene(p_path);

			default:
				break;
			}
		}

		static Scene* GetScene(int p_sceneType, string p_seedString, string p_pathConfigFile)
		{
			switch (p_sceneType)
			{
			case 1:
				return new SimpleScene();

			case 5:
				return new CornelBox();

			case 6:
				return new SibanekScene();

			case 7:
				return new TexturedBox();

			case 8:
				return new TestTextureScene();

			case 9:
				return NULL;

			default:
				break;
			}
		}

		static Scene* GetScene(int p_sceneType, int p_pathNumber, string p_pathConfigFile, float p_deltaFrames, float p_totalTimeForSequence)
		{
			switch (p_sceneType)
			{
			case 1:
				return new SimpleScene();

			case 5:
				return new CornelBox();

			case 6:
				return new SibanekScene();

			case 7:
				return new TexturedBox();

			case 8:
				return new TestTextureScene();

			case 9:
				return new TownScene(p_pathNumber, p_pathConfigFile, p_deltaFrames, p_totalTimeForSequence);

			case 10:
				return new DamagedTown(p_pathNumber, p_pathConfigFile, p_deltaFrames, "");

			default:
				break;
			}
		}


		static Scene* GetScene(int p_sceneType, int p_pathNumber, string p_pathConfigFile, int p_amountSegments, string p_objFilePath)
		{
			switch (p_sceneType)
			{
			case 1:
				return new SimpleScene();

			case 5:
				return new CornelBox();

			case 6:
				return new SibanekScene();

			case 7:
				return new TexturedBox();

			case 8:
				return new TestTextureScene();

			case 9:
				return new TownScene(p_pathNumber, p_pathConfigFile, p_amountSegments, p_objFilePath);

			case 10:
				return new DamagedTown(p_pathNumber, p_pathConfigFile, p_amountSegments, p_objFilePath);

			default:
				break;
			}
		}


		static Scene* GetScene(int p_sceneType, int p_pathNumber, string p_pathConfigFile, int p_amountSegments, string p_objFilePath, SceneManager* p_sceneManager)
		{
			switch (p_sceneType)
			{
			case 1:
				return new SimpleScene();

			case 5:
				return new CornelBox();

			case 6:
				return new SibanekScene();

			case 7:
				return new TexturedBox();

			case 8:
				return new TestTextureScene();

			case 9:
				return new TownScene(p_pathNumber, p_pathConfigFile, p_amountSegments, p_objFilePath, p_sceneManager->GetLightManager()->GetLightMap());

			case 10:
				return new DamagedTown(p_pathNumber, p_pathConfigFile, p_amountSegments, p_objFilePath, p_sceneManager->GetLightManager()->GetLightMap());

			default:
				break;
			}
		}




	};

}


