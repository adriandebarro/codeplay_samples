#ifndef __MESHSCENE__
#define __MESHSCENE__

#include "Scene.h"
#include "..\CoreClasses\GeometricObjects\Objects.h"
#include "..\Material\Matte.h"

namespace Engine
{

	class MeshScene : public Engine::Scene
	{
	public:
		MeshScene()
		{
			BuildScene();
		}

		using Scene::BuildScene;
		void BuildScene(int p_path = 0, string p_pathToObjFile = "")
		{
			float ka = 0.25;
			float kd = 0.75;
			RGBColor yellow(1, 1, 0);
			RGBColor red(0.9,0, 0);
			RGBColor grey(0.25, 0.25, 0.25);

			m_sceneLight.push_back(new PointLight(Point3D(0.0, 20, -10), RGBColor(1, 1, 1), 1, 3));

			int index = 0;
			Matte* bunnyMaterial = new Matte(kd, yellow);
			Matte* secondBunnyMaterial = new Matte(kd, red);
			
			Tris *bunny = new Tris("C:\\Users\\Adrian\\Desktop\\smallerBunny.obj", bunnyMaterial, Point3D(2, 0, -15),true);
			Tris *thirdBunny = new Tris("C:\\Users\\Adrian\\Desktop\\smallerBunny.obj", bunnyMaterial, Point3D(0, -2, -15), true);
			Tris *secondBunny = new Tris("C:\\Users\\Adrian\\Desktop\\smallBunny.obj", secondBunnyMaterial, Point3D(-2, 0, -15), true);
			//Tris* teapot = new Tris("C:\\Users\\Adrian\\Desktop\\smallTeapot.obj", secondBunnyMaterial, Point3D(0, 2, -15), true);
			m_sceneObjects.push_back(bunny);
			m_sceneObjects.push_back(secondBunny);
			m_sceneObjects.push_back(thirdBunny);
			//m_sceneObjects.push_back(teapot);
		}
	private:

	protected:
	};

}
#endif // !__MESHSCENE__
