#pragma once 

#include "..\Scene\Scene.h"
#include "..\Light\Light.h"
#include "..\CoreClasses\GeometricObjects\MeshObject.h"

namespace Engine
{
	class TestTextureScene : public Scene
	{
	public:

		TestTextureScene(void)
		{
			BuildScene();
		}

		using Scene::AddSceneLight;
		virtual void AddSceneLight()
		{
			//no lights to add 
		}

		using Scene::BuildScene;
		void BuildScene(int p_path = 0, string p_pathToObjFile = "")
		{
			Camera* currentCamera = new  Engine::Camera(Point3D(0, 0, VIEW_PLANE_EYE_DISTANCE), Point3D(0, -3, -15), 30, 1, 1);
			SetCamera(currentCamera);

			Point3D pointlightLocation(0, 1.5, -15);
			PointLight* light1 = new PointLight(pointlightLocation, RGBColor(1), 2, 15);
			m_sceneLight.push_back(light1);

			string cornellPath = "\ObjFiles\\CornellBoxobj.obj";
			ObjectMesh* cornellObject = new ObjectMesh(cornellPath, Point3D(0, -3, -15));
			cornellObject->BuildScene();
			m_sceneObjects.push_back(cornellObject);

			string planePath = "\ObjFiles\\TexturePlane\\textureTestobj.obj";
			ObjectMesh* testPlane = new ObjectMesh(planePath, Point3D(0, -1, -17), true);
			testPlane->BuildScene();
			m_sceneObjects.push_back(testPlane);
		}

	private:
	};
}