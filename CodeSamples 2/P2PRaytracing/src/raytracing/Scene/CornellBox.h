#ifndef __CORNELLBOX__
#define __CORNELLBOX__

#include "..\Scene\Scene.h"
#include "..\Material\Material.h"
#include "..\Material\Matte.h"
#include "..\Light\PointLight.h"
#include "..\CoreClasses\GeometricObjects\MeshObject.h"


namespace Engine
{
	class CornelBox :public Engine::Scene
	{
	public:
		CornelBox()
		{
			BuildScene();
		}

		using Scene::AddSceneLight;
		virtual void AddSceneLight()
		{

		}

		using Scene::BuildScene;
		void BuildScene(int p_path = 0, string p_pathToObjFile = "")
		{
			SetSceneName("Cornell Box");
			Camera* currentCamera = new  Engine::Camera(Point3D(0, 0, 3), Point3D(0, 0, -15), 30, 1, 1);
			SetCamera(currentCamera);

			Point3D pointlightLocation(0,1.5,-15);
			PointLight* light1 = new PointLight(pointlightLocation,RGBColor(1),2,15);
			m_sceneLight.push_back(light1);
			Matte * material5 = new Matte(0.75, RGBColor(1,1,0));

			string cornellPath = "\ObjFiles\\CornellBoxobj.obj";
			ObjectMesh* cornellObject = new ObjectMesh(cornellPath,Point3D(0,-3,-15));
			cornellObject->BuildScene();
			m_sceneObjects.push_back(cornellObject);
			m_sceneObjects.push_back(new Sphere(Point3D(0, -1, -15), 3, material5));
		}

		//Sphere spheres[] = {//Scene: radius, position, emission, color, material
		//	Sphere(1e5, Vec(1e5 + 1, 40.8, 81.6), Vec(), Vec(.75, .25, .25), DIFF),//Left
		//	Sphere(1e5, Vec(-1e5 + 99, 40.8, 81.6), Vec(), Vec(.25, .25, .75), DIFF),//Rght
		//	Sphere(1e5, Vec(50, 40.8, 1e5), Vec(), Vec(.75, .75, .75), DIFF),//Back
		//	Sphere(1e5, Vec(50, 40.8, -1e5 + 170), Vec(), Vec(), DIFF),//Frnt
		//	Sphere(1e5, Vec(50, 1e5, 81.6), Vec(), Vec(.75, .75, .75), DIFF),//Botm
		//	Sphere(1e5, Vec(50, -1e5 + 81.6, 81.6), Vec(), Vec(.75, .75, .75), DIFF),//Top
		//	Sphere(16.5, Vec(27, 16.5, 47), Vec(), Vec(1, 1, 1)*.999, SPEC),//Mirr
		//	Sphere(16.5, Vec(73, 16.5, 78), Vec(), Vec(1, 1, 1)*.999, REFR),//Glas
		//	Sphere(600, Vec(50, 681.6 - .27, 81.6), Vec(12, 12, 12), Vec(), DIFF) //Lite
		//};
	};
}
#endif // !__CORNELLBOX__
