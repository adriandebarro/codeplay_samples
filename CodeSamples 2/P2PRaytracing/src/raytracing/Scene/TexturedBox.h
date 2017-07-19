#ifndef __TEXTURED_BOX_H__
#define __TEXTURED_BOX_H__

#include "..\Scene\Scene.h"
#include "..\Light\Light.h"
#include "..\CoreClasses\GeometricObjects\MeshObject.h"
#include "..\Light\PointLight.h"

namespace Engine
{
	class TexturedBox : public Scene
	{
	public:

		TexturedBox()
		{
			BuildScene();
		}

		using Scene::AddSceneLight;
		virtual void AddSceneLight()
		{

		}

		using Scene::BuildScene;
		virtual void BuildScene(int p_path = 0, string p_pathToObjFile = "")
		{
			Camera* currentCamera = new  Engine::Camera(Point3D(0, 0, VIEW_PLANE_EYE_DISTANCE), Point3D(0, -3, -15), 30, 1, 1);
			SetCamera(currentCamera);

			Point3D pointlightLocation(0, 1.5, -15);
			PointLight* light1 = new PointLight(pointlightLocation, RGBColor(1), 2, 15);
			m_sceneLight.push_back(light1);
			Matte * material5 = new Matte(0.75, RGBColor(1, 1, 0));

			string cornellPath = "\ObjFiles\\CornellBoxobj.obj";
			ObjectMesh* cornellObject = new ObjectMesh(cornellPath, Point3D(0, -3, -15));
			cornellObject->BuildScene();
			m_sceneObjects.push_back(cornellObject);

			string cubePath = "\ObjFiles\\TexturedBox\\cube.obj";
			ObjectMesh* texturedBoxObject = new ObjectMesh(cubePath, Point3D(0, -1, -17), true);
			texturedBoxObject->BuildScene();
			m_sceneObjects.push_back(texturedBoxObject);
		}

	private:

	};
}

#endif // !__TEXTURED_BOX_H__
