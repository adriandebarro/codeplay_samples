#ifndef __PLABESCENE__
#define __PLANESCENE__

#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "..\Scene\Scene.h"
#include "..\CoreClasses\GeometricObjects\Objects.h"

namespace Engine
{

	class PlaneScene : public Engine::Scene
	{
	public:
		PlaneScene()
		{
			BuildScene();
		}

		using Scene::BuildScene;
		void BuildScene(int p_path = 0, string p_pathToObjFile = "")
		{
			
			//sceneObjects.push_back(new Sphere(Point3D(500, 500, 10), 80, RGBColor(1, 0, 0)));
			//sceneObjects.push_back(new Plane(Point3D(-0, -0, 10),Point3D(0,1,0), RGBColor(0,0,1)));
		}
	private:
	};
}
#endif // !__FEWCICLES__
