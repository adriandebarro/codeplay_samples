#ifndef __FEWCICLES__
#define __FEWCICLES__

#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "Scene.h"
#include "..\CoreClasses\GeometricObjects\Objects.h"
#include "..\Material\Matte.h"
#include "..\Light\PointLight.h"
#include "..\Material\Reflective.h"
#include "..\Material\Phong.h"

namespace Engine
{

	class FewCircles : public Engine::Scene
	{
	public:
		FewCircles()
		{
			BuildScene();
		}

		using Scene::BuildScene;
		void BuildScene(int p_path = 0, string p_pathToObjFile = "")
		{
			float ka = 0.25;
			float kd = 0.75;
			float ks = 0.75;

			//light
			m_sceneLight.push_back(new PointLight(Point3D(0.0, 20, -10), RGBColor(1, 1, 1),1,2));
			m_sceneLight.push_back(new PointLight(Point3D(0.0, -20, -10), RGBColor(1, 1, 1), 1, 2));

			//materials

			//Matte * material1 = new	Matte(kd, RGBColor(0.2 0, 0.20, 0.20));
			//Matte * material2 = new	Matte(kd, RGBColor(1.00, 0.32, 0.36));
			//Matte * material3 = new Matte(kd, RGBColor(0.90, 0.76, 0.46));
			//Matte * material4 = new Matte(kd, RGBColor(0.65, 0.77, 0.97));
			//Matte * material5 = new Matte(kd, RGBColor(0.90, 0.90, 0.90));
			//Matte* bunnyMaterial = new Matte(kd, RGBColor(1, 1, 0));
			//Phong * testMaterial = new Phong(0.5, 0.5, RGBColor(0, 1, 0));
			//Reflective* reflectiveMaterial = new Reflective(1.0, RGBColor(1, 1, 0));

			////objects 
			//m_sceneObjects.push_back(new Sphere(Point3D(0.0, -98, -20), 10000, material1));
			//m_sceneObjects.push_back(new Sphere(Point3D(0, -3, -15),4,material2));
			//m_sceneObjects.push_back(new Sphere(Point3D(3.0, -1, -15), 2,material3));
			//m_sceneObjects.push_back(new Sphere(Point3D(5.0, -4, -25), 2, material4));
			//m_sceneObjects.push_back(new Sphere(Point3D(-3, -1, -15), 2,material5));
			//m_sceneObjects.push_back(new Sphere(Point3D(0, -2, -17), 1, testMaterial));
			
			/*Tris *bunny = new Tris("C:\\Users\\Adrian\\Desktop\\smallBunny.obj", bunnyMaterial, Point3D(2, 0, -10), true);
			m_sceneObjects.push_back(bunny);*/

		}
	private:
	};
}
#endif // !__FEWCICLES__
