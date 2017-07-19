#ifndef __SIBANEK_SCENE_H__
#define __SIBANEL_SCENE_H__

#include "..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\Scene\Scene.h"
#include "..\Light\PointLight.h"
#include "..\Material\Matte.h"
#include "..\CoreClasses\GeometricObjects\Objects.h"
#include "..\CoreClasses\GeometricObjects\MeshObject.h"
#include "..\Scene\SceneHelper.h"

namespace Engine
{
	class SibanekScene : public Scene
	{
	public:
		SibanekScene()
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
			/*Point3D pointlightLocation(0, 2, -4);
			SceneHelper::CreateLineLight(m_sceneLight, pointlightLocation, 5, 2, -2, 1);*/
			SetSceneName("Sibanek");
			vector<Point3D> pathPoints;
			/*pathPoints.push_back(Point3D(-15, -13, -5));
			pathPoints.push_back(Point3D(-13, -13, -2));
			pathPoints.push_back(Point3D(-10, -13, -2));
			pathPoints.push_back(Point3D(-7, -13, -2));
			pathPoints.push_back(Point3D(-5, -13, -4.7));*/
			// Camera* currentCamera = new  Engine::Camera(Point3D(-15, -13, -5), Point3D(5, -10, 0),55, 1, 1, 0.1);
			Camera* currentCamera = new  Engine::Camera(Point3D(-15, -13, -5), Point3D(5, -10, 0), 55, 1, 1);
			//Camera* currentCamera = new  Engine::Camera(Point3D(-15, -13, -5), Point3D(5, -10, 0), 55, 1, 1);
			SetCamera(currentCamera);

			//Point3D(-17,-13,0)
			Point3D pointlightLocation(-15, -13, 0);
			Point3D crucialLightLocation(6.5, 0, -3);
			
			//PointLight* light1 = new PointLight(pointlightLocation, RGBColor(1), 2, 75);
			PointLight* light2 = new PointLight(crucialLightLocation, RGBColor(1), 1, 500);
			//m_sceneLight.push_back(light1);
			m_sceneLight.push_back(light2);

			//Matte * material5 = new Matte(0.75, RGBColor(1, 1, 0));

			//Matte * material5 = new Matte(0.75, RGBColor(1, 1, 0));

			string sibanekPath = "\ObjFiles\\Sibeniknew\\sibenik_a3d.obj";
			ObjectMesh* cornellObject = new ObjectMesh(sibanekPath, Point3D(0,0,-4),true);
			cornellObject->BuildScene(false, 2);
			m_sceneObjects.push_back(cornellObject);
		}


	private:
	};
}

#endif // !__SIBANEK_SCENE_H__
