#include "Scene.h"
#include "..\CoreClasses\GeometricObjects\Objects.h"

namespace Engine
{
	using Engine::Scene;
	class SimpleScene : public Scene
	{

	public:
		SimpleScene()
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
			RGBColor darkGreen(0,100,0);
			/*m_sceneObjects.push_back(new Sphere(Point3D(0.0,-25,0), 80, darkGreen));
			m_sceneObjects.push_back(new Sphere(Point3D(0,30,0),60,RGBColor(0,1,1)));*/
		}
	};
}