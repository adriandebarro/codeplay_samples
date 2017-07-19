#ifndef __BIGGERSCENE__
#define __BIGGERSCENE__

#include "..\CoreClasses\Point\Point3D\Point3D.h"
#include "..\Scene\Scene.h"
#include "..\Light\PointLight.h"
#include "..\Material\Matte.h"
#include "..\CoreClasses\GeometricObjects\Objects.h"

#define GRID_SIDE_CONSTANT 2
#define STANDARD_DISTANCE_SPACE -100

namespace Engine
{

	
	//using Engine::MatMatte;
	class BiggerScene:public Engine::Scene
	{
	public:

		BiggerScene()
		{
			BuildScene();
		}

		using Scene::BuildScene;
		void BuildScene(int p_path = 0, string p_pathToObjFile = "")
		{
			//RGBColor yellow(1, 1, 0);										// yellow
			//RGBColor brown(0.71, 0.40, 0.16);								// brown
			//RGBColor darkGreen(0.0, 0.41, 0.41);							// darkGreen
			//RGBColor orange(1, 0.75, 0);									// orange
			//RGBColor green(0, 0.6, 0.3);									// green
			//RGBColor lightGreen(0.65, 1, 0.30);								// light green
			//RGBColor darkYellow(0.61, 0.61, 0);								// dark yellow
			//RGBColor lightPurple(0.65, 0.3, 1);								// light purple
			//RGBColor darkPurple(0.5, 0, 1);									// dark purple
			//RGBColor grey(0.25);											// grey


			////lights in the Scene

			//// light

			//PointLight* light_ptr1 = new PointLight();
			///*light_ptr1->SetLocation(100, 100,70);*/
			//light_ptr1->SetLocation(100, 100, -30);
			//light_ptr1->SetRadiance(0.5);
			//AddLight(light_ptr1);


			//// Matte material reflection coefficients - common to all materials

			//float ka = 0.25;
			//float kd = 0.75;
			//

			//int index = 0;

			//// spheres

			//Matte* matte_ptr1 = new Matte();
			//matte_ptr1->SetKa(ka);
			//matte_ptr1->SetKd(kd);
			//matte_ptr1->SetCd(yellow);
			//m_sceneMaterials.push_back(matte_ptr1);

			//Sphere*	sphere_ptr1 = new Sphere(Point3D(5,3,0), 30, green);	
			//m_sceneObjects.push_back(sphere_ptr1);

			//
			//++index;

			//Matte* matte_ptr2 = new Matte();
			//matte_ptr2->SetKa(ka);
			//matte_ptr2->SetKd(kd);
			//matte_ptr2->SetCd(brown);
			//m_sceneMaterials.push_back(matte_ptr2);

			//Sphere*	sphere_ptr2 = new Sphere(Point3D(45, -7, -60), 5, index);
			//m_sceneObjects.push_back(sphere_ptr2);

			//++index;

			//Matte* matte_ptr3 = new Matte();
			//matte_ptr3->SetKa(ka);
			//matte_ptr3->SetKd(kd);
			//matte_ptr3->SetCd(darkGreen);
			//m_sceneMaterials.push_back(matte_ptr3);

			//Sphere*	sphere_ptr3 = new Sphere(Point3D(40, 43, -100), 17, index);				// dark green
			//m_sceneObjects.push_back(sphere_ptr3);

			//++index;

			//Matte* matte_ptr4 = new Matte();
			//matte_ptr4->SetKa(ka);
			//matte_ptr4->SetKd(kd);
			//matte_ptr4->SetCd(orange);

			//m_sceneMaterials.push_back(matte_ptr4);

			//Sphere*	sphere_ptr4 = new Sphere(Point3D(-20, 28, -15), 20, index);
			//m_sceneObjects.push_back(sphere_ptr4);

			//++index;

			//Matte* matte_ptr5 = new Matte();
			//matte_ptr5->SetKa(ka);
			//matte_ptr5->SetKd(kd);
			//matte_ptr5->SetCd(green);
			//m_sceneMaterials.push_back(matte_ptr5);
			//
			//Sphere*	sphere_ptr5 = new Sphere(Point3D(-25, -7, -35), 27, index);						// green
			//m_sceneObjects.push_back(sphere_ptr5);

			//++index;
			///*cyan*/
			//Matte* matte_ptr6 = new Matte(kd, lightGreen);

			//Sphere*	sphere_ptr6 = new Sphere(Point3D(20, -27, -35), 25, matte_ptr6);
			//m_sceneObjects.push_back(sphere_ptr6);


			//++index;

			//Matte* matte_ptr7 = new Matte();
			//matte_ptr7->SetKa(ka);
			//matte_ptr7->SetKd(kd);
			//matte_ptr7->SetCd(green);
			//
			//m_sceneMaterials.push_back(matte_ptr7);
			//
			//Sphere*	sphere_ptr7 = new Sphere(Point3D(35, 18, -35), 22, index);
			//m_sceneObjects.push_back(sphere_ptr7);

			//++index;

			//Matte* greyBase = new Matte(kd,grey);
			//Sphere* base = new Sphere(Point3D(35, 18, -35), 10000,greyBase);
			//m_sceneObjects.push_back(base);

			//Matte* matte_ptr8 = new Matte(kd, brown);
			//Sphere*	sphere_ptr8 = new Sphere(Point3D(-57, -17, -50), 15, matte_ptr8);							// brown
			//m_sceneObjects.push_back(sphere_ptr8);

			//Matte* matte_ptr9 = new Matte(kd, lightGreen);
			//Sphere*	sphere_ptr9 = new Sphere(Point3D(-47, 16, -80), 23, matte_ptr9);
			//// light green
			//m_sceneObjects.push_back(sphere_ptr9);

			//++index;

			//Matte* matte_ptr10 = new Matte(kd, darkGreen);
			//matte_ptr10->SetKa(ka);
			//matte_ptr10->SetKd(kd);
			//matte_ptr10->SetCd(darkGreen);
			//Sphere*	sphere_ptr10 = new Sphere(Point3D(-15, -32, -60), 22, index);
			//
			//m_sceneMaterials.push_back(matte_ptr10);     						// dark green
			//m_sceneObjects.push_back(sphere_ptr10);

			//++index;

			//Matte* matte_ptr11 = new Matte;
			//matte_ptr11->SetKa(ka);
			//matte_ptr11->SetKd(kd);
			//matte_ptr11->SetCd(darkYellow);
			//Sphere*	sphere_ptr11 = new Sphere(Point3D(-35, -37, -80), 22, index);
			//
			//m_sceneMaterials.push_back(matte_ptr11);							// dark yellow
			//m_sceneObjects.push_back(sphere_ptr11);

			//++index;

			//Matte* matte_ptr12 = new Matte;
			//matte_ptr12->SetKa(ka);
			//matte_ptr12->SetKd(kd);
			//matte_ptr12->SetCd(darkYellow);
			//Sphere*	sphere_ptr12 = new Sphere(Point3D(10, 43, -80), 22, index);
			//
			//m_sceneMaterials.push_back(matte_ptr12);							// dark yellow
			//m_sceneObjects.push_back(sphere_ptr12);

			//Matte* matte_ptr13 = new Matte;
			//matte_ptr13->SetKa(ka);
			//matte_ptr13->SetKd(kd);
			//matte_ptr13->SetCd(darkYellow);
			//Sphere*	sphere_ptr13 = new Sphere(Point3D(30, -7, -80), 10);
			//sphere_ptr13->SetMaterial(matte_ptr13);
			//m_sceneObjects.push_back(sphere_ptr13);											// dark yellow (hidden)

			//Matte* matte_ptr14 = new Matte;
			//matte_ptr14->SetKa(ka);
			//matte_ptr14->SetKd(kd);
			//matte_ptr14->SetCd(darkGreen);
			//Sphere*	sphere_ptr14 = new Sphere(Point3D(-40, 48, -110), 18);
			//sphere_ptr14->SetMaterial(matte_ptr14); 							// dark green
			//m_sceneObjects.push_back(sphere_ptr14);

			//Matte* matte_ptr15 = new Matte;
			//matte_ptr15->SetKa(ka);
			//matte_ptr15->SetKd(kd);
			//matte_ptr15->SetCd(brown);
			//Sphere*	sphere_ptr15 = new Sphere(Point3D(-10, 53, -120), 18);
			//sphere_ptr15->SetMaterial(matte_ptr15); 							// brown
			//m_sceneObjects.push_back(sphere_ptr15);

			//Matte* matte_ptr16 = new Matte;
			//matte_ptr16->SetKa(ka);
			//matte_ptr16->SetKd(kd);
			//matte_ptr16->SetCd(lightPurple);
			//Sphere*	sphere_ptr16 = new Sphere(Point3D(-55, -52, -100), 10);
			//sphere_ptr16->SetMaterial(matte_ptr16);							// light purple
			//m_sceneObjects.push_back(sphere_ptr16);

			//Matte* matte_ptr17 = new Matte;
			//matte_ptr17->SetKa(ka);
			//matte_ptr17->SetKd(kd);
			//matte_ptr17->SetCd(brown);
			//Sphere*	sphere_ptr17 = new Sphere(Point3D(5, -52, -100), 15);
			//sphere_ptr17->SetMaterial(matte_ptr17);							// browm
			//m_sceneObjects.push_back(sphere_ptr17);

			//Matte* matte_ptr18 = new Matte;
			//matte_ptr18->SetKa(ka);
			//matte_ptr18->SetKd(kd);
			//matte_ptr18->SetCd(darkPurple);
			//Sphere*	sphere_ptr18 = new Sphere(Point3D(-20, -57, -120), 15);
			//sphere_ptr18->SetMaterial(matte_ptr18);							// dark purple
			//m_sceneObjects.push_back(sphere_ptr18);

			//Matte* matte_ptr19 = new Matte;
			//matte_ptr19->SetKa(ka);
			//matte_ptr19->SetKd(kd);
			//matte_ptr19->SetCd(darkGreen);
			//Sphere*	sphere_ptr19 = new Sphere(Point3D(55, -27, -100), 17);
			//sphere_ptr19->SetMaterial(matte_ptr19);							// dark green
			//m_sceneObjects.push_back(sphere_ptr19);

			//Matte* matte_ptr20 = new Matte;
			//matte_ptr20->SetKa(ka);
			//matte_ptr20->SetKd(kd);
			//matte_ptr20->SetCd(brown);
			//Sphere*	sphere_ptr20 = new Sphere(Point3D(50, -47, -120), 15);
			//sphere_ptr20->SetMaterial(matte_ptr20);							// browm
			//m_sceneObjects.push_back(sphere_ptr20);

			//Matte* matte_ptr21 = new Matte;
			//matte_ptr21->SetKa(ka);
			//matte_ptr21->SetKd(kd);
			//matte_ptr21->SetCd(lightPurple);
			//Sphere*	sphere_ptr21 = new Sphere(Point3D(70, -42, -150), 10);
			//sphere_ptr21->SetMaterial(matte_ptr21);							// light purple
			//m_sceneObjects.push_back(sphere_ptr21);

			//Matte* matte_ptr22 = new Matte;
			//matte_ptr22->SetKa(ka);
			//matte_ptr22->SetKd(kd);
			//matte_ptr22->SetCd(lightPurple);
			//Sphere*	sphere_ptr22 = new Sphere(Point3D(5, 73, -130), 12);
			//sphere_ptr22->SetMaterial(matte_ptr22);							// light purple
			//m_sceneObjects.push_back(sphere_ptr22);

			//Matte* matte_ptr23 = new Matte;
			//matte_ptr23->SetKa(ka);
			//matte_ptr23->SetKd(kd);
			//matte_ptr23->SetCd(darkPurple);
			//Sphere*	sphere_ptr23 = new Sphere(Point3D(66, 21, -130), 13);
			//sphere_ptr23->SetMaterial(matte_ptr23);							// dark purple
			//m_sceneObjects.push_back(sphere_ptr23);

			//Matte* matte_ptr24 = new Matte;
			//matte_ptr24->SetKa(ka);
			//matte_ptr24->SetKd(kd);
			//matte_ptr24->SetCd(lightPurple);
			//Sphere*	sphere_ptr24 = new Sphere(Point3D(72, -12, -140), 12);
			//sphere_ptr24->SetMaterial(matte_ptr24);							// light purple
			//m_sceneObjects.push_back(sphere_ptr24);

			//Matte* matte_ptr25 = new Matte;
			//matte_ptr25->SetKa(ka);
			//matte_ptr25->SetKd(kd);
			//matte_ptr25->SetCd(green);
			//Sphere*	sphere_ptr25 = new Sphere(Point3D(64, 5, -160), 11);
			//sphere_ptr25->SetMaterial(matte_ptr25);					 		// green
			//m_sceneObjects.push_back(sphere_ptr25);

			//Matte* matte_ptr26 = new Matte;
			//matte_ptr26->SetKa(ka);
			//matte_ptr26->SetKd(kd);
			//matte_ptr26->SetCd(lightPurple);
			//Sphere*	sphere_ptr26 = new Sphere(Point3D(55, 38, -160), 12);
			//sphere_ptr26->SetMaterial(matte_ptr26);							// light purple
			//m_sceneObjects.push_back(sphere_ptr26);

			//Matte* matte_ptr27 = new Matte;
			//matte_ptr27->SetKa(ka);
			//matte_ptr27->SetKd(kd);
			//matte_ptr27->SetCd(lightPurple);
			//Sphere*	sphere_ptr27 = new Sphere(Point3D(-73, -2, -160), 12);
			//sphere_ptr27->SetMaterial(matte_ptr27);							// light purple
			//m_sceneObjects.push_back(sphere_ptr27);

			//Matte* matte_ptr28 = new Matte;
			//matte_ptr28->SetKa(ka);
			//matte_ptr28->SetKd(kd);
			//matte_ptr28->SetCd(darkPurple);
			//Sphere*	sphere_ptr28 = new Sphere(Point3D(30, -62, -140), 15);
			//sphere_ptr28->SetMaterial(matte_ptr28); 							// dark purple
			//m_sceneObjects.push_back(sphere_ptr28);

			//Matte* matte_ptr29 = new Matte;
			//matte_ptr29->SetKa(ka);
			//matte_ptr29->SetKd(kd);
			//matte_ptr29->SetCd(darkPurple);
			//Sphere*	sphere_ptr29 = new Sphere(Point3D(25, 63, -140), 15);
			//sphere_ptr29->SetMaterial(matte_ptr29);							// dark purple
			//m_sceneObjects.push_back(sphere_ptr29);

			//Matte* matte_ptr30 = new Matte;
			//matte_ptr30->SetKa(ka);
			//matte_ptr30->SetKd(kd);
			//matte_ptr30->SetCd(darkPurple);
			//Sphere*	sphere_ptr30 = new Sphere(Point3D(-60, 46, -140), 15);
			//sphere_ptr30->SetMaterial(matte_ptr30); 							// dark purple
			//m_sceneObjects.push_back(sphere_ptr30);

			//Matte* matte_ptr31 = new Matte;
			//matte_ptr31->SetKa(ka);
			//matte_ptr31->SetKd(kd);
			//matte_ptr31->SetCd(lightPurple);
			//Sphere*	sphere_ptr31 = new Sphere(Point3D(-30, 68, -130), 12);
			//sphere_ptr31->SetMaterial(matte_ptr31); 							// light purple
			//m_sceneObjects.push_back(sphere_ptr31);

			//Matte* matte_ptr32 = new Matte;
			//matte_ptr32->SetKa(ka);
			//matte_ptr32->SetKd(kd);
			//matte_ptr32->SetCd(green);
			//Sphere*	sphere_ptr32 = new Sphere(Point3D(58, 56, -180), 11);
			//sphere_ptr32->SetMaterial(matte_ptr32);							//  green
			//m_sceneObjects.push_back(sphere_ptr32);

			//Matte* matte_ptr33 = new Matte;
			//matte_ptr33->SetKa(ka);
			//matte_ptr33->SetKd(kd);
			//matte_ptr33->SetCd(green);
			//Sphere*	sphere_ptr33 = new Sphere(Point3D(-63, -39, -180), 11);
			//sphere_ptr33->SetMaterial(matte_ptr33);							// green 
			//m_sceneObjects.push_back(sphere_ptr33);

			//Matte* matte_ptr34 = new Matte;
			//matte_ptr34->SetKa(ka);
			//matte_ptr34->SetKd(kd);
			//matte_ptr34->SetCd(lightPurple);
			//Sphere*	sphere_ptr34 = new Sphere(Point3D(46, 68, -200), 10);
			//sphere_ptr34->SetMaterial(matte_ptr34);							// light purple
			//m_sceneObjects.push_back(sphere_ptr34);

			//Matte* matte_ptr35 = new Matte;
			//matte_ptr35->SetKa(ka);

		}



	private:

		void CreateSphereGrid()
		{
			for (int y = 1; y <= GRID_SIDE_CONSTANT; y++)
			{
				for (int x = 1; x <= GRID_SIDE_CONSTANT; x ++)
				{
					//m_sceneObjects.push_back(new Sphere(Point3D((-5.5 * x) + STANDARD_DISTANCE_SPACE, (-20 * y) + STANDARD_DISTANCE_SPACE, 10), 5, RGBColor(1, 1, 1)));
				}
			}
		}

	};
}
#endif // !__BIGGERSCENE__
