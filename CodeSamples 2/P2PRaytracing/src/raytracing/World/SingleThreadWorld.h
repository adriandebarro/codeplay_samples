#pragma once
//----------------------------------------------------------------------------------------------------------------
//			Filename:		World.h
//			  Author:		Adrian De Barro
//			    Date:		14/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <vector>
#include <iostream>
#include <stdio.h>
#include <random>

#include "..\CoreClasses\GeometricObjects\GeometricObjects.h"
#include "..\Scene\SceneFactory.h"
#include "..\Tracer\DepthTracer.h"
#include "..\Light\Light.h"
#include "..\Material\Reflective.h"
#include "..\Integrators\IrradianceCache.h"
#include "..\GuiHelper\gw.h"
#include "..\CoreClasses\Logger\Logger.h"
#include "..\CoreClasses\Camera\Camera.h"
#include "..\CoreClasses\GeometricObjects\Objects.h"
#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "..\Tracer\Tracer.h"
#include "..\World\View Plane Details\ViewPlane.h"
#include "..\Tracer\DepthTracer.h"
#include "..\Material\Textures\ImageTexture.h"
#include "..\World\AbstractWorld.h"

#define UPDATE_COUNT 2000
#define DEPTH_LIMIT 5
#define INDIRECT_ONLY

//#define max(A,B) ((A > B)? (A ): (B))

namespace Engine
{
	//----------------------------------------------------------------------------------------------------------------
	//			Class Name: World
	//		   Description: Ties together the Scene, its building, tracing and shading according to each material
	//----------------------------------------------------------------------------------------------------------------
	//			    Status: REFACTORING
	//----------------------------------------------------------------------------------------------------------------
	class SingleThreadWorld : public AbstractWorld
	{
	public:
		
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		SingleThreadWorld(void) 
		{
			tracer_ptr = NULL;
			backgroundColor = RGBColor(0, 0, 0);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: int Scene Type
		//			  Status: DEPECRATED
		//----------------------------------------------------------------------------------------------------------------
		SingleThreadWorld(int p_sceneType)
		{
			sceneType = p_sceneType;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: int Scene Type, int Width, int Height
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		SingleThreadWorld(int p_sceneType, int m_width, int m_height)
		{
			sceneType = p_sceneType;
			vp.hres = m_width;
			vp.vres = m_height;
			vp.s = 1;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetShade
		//		Return Value: VOID
		//		  Parameters: CollisionDetails, int
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using AbstractWorld::GetShade;
		RGBColor GetShade(ShadeRec& sr, int depth)
		{
			GeometricObjects* object = sr.object;
			Materials::Material *materialPtr;

			if (sr.m_hitTriangle == NULL)
			{
				materialPtr = sr.object->GetMaterial();
			}
			else
			{
				materialPtr = sr.m_hitTriangle->GetMaterial();
			}

			RGBColor outputColor = RGBColor(0);

			switch (materialPtr->GetType())
			{
				case Materials::GLOSSY:
				case Materials::DIFF:
				{
				#ifdef INDIRECT_ONLY
					//RGBColor indirectColor = integrator->GetIndirectLightingAtPoint(sr);
					RGBColor albedoColor(0);
					RGBColor directLight = materialPtr->Shade(sr, currentScene->m_sceneLight, tracer_ptr, albedoColor);// +indirectColor*materialPtr->Getcolor();
					outputColor = outputColor + (directLight);// +(albedoColor * indirectColor);
					//outputColor = outputColor + materialPtr->Shade(sr, currentScene->m_sceneLight, tracer_ptr) +indirectColor*materialPtr->Getcolor();
					//outputColor = outputColor + indirectColor*materialPtr->Getcolor();
					return outputColor;
				#else
					//check shadow ray 
					RGBColor indirectColor = integrator->GetIndirectLightingAtPoint(sr);
					outputColor = outputColor + materialPtr->Shade(sr, currentScene->m_sceneLight, tracer_ptr) + indirectColor*materialPtr->Getcolor();
					//outputColor = indirectColor * materialPtr->Getcolor();
					//m_logFile.WriteToFile(outputColor.ToString());
					return outputColor;
				#endif

				}
					break;

				case Materials::REFL:
				{
					ShadeRec refDetails;
					Point3D N = sr.object->GetNormal(sr.localHitPoint);
					Point3D r = sr.ray.d - 2 * sr.ray.d.dot(N) * N;
					float bias = 1e-4;
					Point3D newOrigin = (sr.localHitPoint + bias * r);
					Ray newRay(newOrigin, r);
					newRay.d.normalize();
					//solve this problem
					if (++depth < DEPTH_LIMIT)
					{
						tracer_ptr->TraceRay(newRay, refDetails);
						GeometricObjects *currentObject = refDetails.object;
						if (currentObject != NULL)
						{
							//outputColor += materialPtr->Shade(sr, currentScene->m_sceneLight) * GetShade(refDetails, depth);
							RGBColor rcol = GetShade(refDetails, depth);
							//outputColor = outputColor + materialPtr->Getcolor() * rcol * materialPtr->GetReflectionConstant();
						}
						else
						{
							RGBColor colorAtPoint;
							outputColor += materialPtr->Shade(sr, currentScene->m_sceneLight, tracer_ptr, colorAtPoint);
						}
					}
					return outputColor;
				}
				break;

				default:
					return outputColor;

				break;
			}
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: render_scene
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		using AbstractWorld::RenderScene;
		bool RenderScene(void)
		{
			RGBColor pixelColor;
			Ray ray;
			//open_window();
			tracer_ptr->UseAccelerationStructure();
			ray.d = Point3D(0, 0, -4);
			Pixel *currentPixel = m_currentPixel;
			RGBColor *currentPixelColor = rgbBuffer;
			//currentPixel += vp.vres * vp.hres - 1;
			float zw = 10;
			int count = 0;
			clock_t startingTime = clock();

			float invWidth = 1 / float(vp.hres), invHeight = 1 / float(vp.vres);
			float fov = m_camera->m_fov, aspectratio = m_camera->m_aspectRatio;
			//float fov = 30, aspectratio = vp.hres / float(vp.vres);
			float eyeViewPlaneDistance = m_camera->eye.x + m_camera->m_viewPlaneDistance;
			float angle = tan(PI_NUM * 0.5 * fov / 180.);
			for (unsigned y = m_currentY; y < vp.vres; ++y) {
				for (unsigned x = 0; x < vp.hres; ++x) {
					float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
					float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
					Point3D raydir(xx, yy, -1);
					raydir.normalize();

					Point3D tempDir = m_camera->GetCameraDirection(raydir);

					pixelColor = RGBColor(0);
					ShadeRec rec;
					if (tracer_ptr->TraceRay(Ray(m_camera->eye, tempDir), rec))
					{
						int depth = 0;
						pixelColor = GetShade(rec, depth);
					}
					//*currentPixel = pixelColor.toInt();
					rgbBuffer[y * vp.hres + x] = pixelColor;
					pixelColor.Convert();
					
					SetColor(x, y, pixelColor.r, pixelColor.g, pixelColor.b);
					++currentPixel;
					++count;
					m_currentShadrec = rec;
				}

				if (count >= UPDATE_COUNT)
				{
					m_currentY = ++y;
					m_currentPixel = currentPixel;
					return false;
				}
			}

			clock_t endTimer = clock();


			double timeInSeconds = ((double)endTimer - startingTime) / (double)CLOCKS_PER_SEC;
			return true;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: RenderSceneBagOfTasks
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool render_scene_BOT(int )
		{
			RGBColor pixelColor;
			Ray ray;
			//open_window();
			tracer_ptr->UseAccelerationStructure();
			ray.d = Point3D(0, 0, -4);
			Pixel *currentPixel = m_currentPixel;
			//currentPixel += vp.vres * vp.hres - 1;
			float zw = 10;
			int count = 0;
			clock_t startingTime = clock();

			float invWidth = 1 / float(vp.hres), invHeight = 1 / float(vp.vres);
			float fov = m_camera->m_fov, aspectratio = m_camera->m_aspectRatio;
			//float fov = 30, aspectratio = vp.hres / float(vp.vres);
			float eyeViewPlaneDistance = m_camera->eye.x + m_camera->m_viewPlaneDistance;
			float angle = tan(PI_NUM * 0.5 * fov / 180.);
			for (unsigned y = m_currentY; y < vp.vres; ++y) {
				for (unsigned x = 0; x < vp.hres; ++x) {
					float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
					float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
					Point3D raydir(xx, yy, -1);
					raydir.normalize();

					Point3D tempDir = m_camera->GetCameraDirection(raydir);

					pixelColor = RGBColor(0);
					ShadeRec rec;
					if (tracer_ptr->TraceRay(Ray(m_camera->eye, tempDir), rec))
					{
						int depth = 0;
						pixelColor = GetShade(rec, depth);
					}
					//*currentPixel = pixelColor.toInt();
					pixelColor.Convert();
					SetColor(x, y, pixelColor.r, pixelColor.g, pixelColor.b);
					++currentPixel;
					++count;
					m_currentShadrec = rec;
				}

				if (count >= UPDATE_COUNT)
				{
					m_currentY = ++y;
					m_currentPixel = currentPixel;
					return false;
				}
			}

			clock_t endTimer = clock();


			double timeInSeconds = ((double)endTimer - startingTime) / (double)CLOCKS_PER_SEC;
			return true;
		}

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: render_scene_zbuffer
		//		Return Value: bool
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		bool render_scene_zbuffer(void)
		{
			RGBColor pixelColor;
			Ray ray;
			//open_window();
			tracer_ptr->UseAccelerationStructure();
			ray.d = Point3D(0, 0, -4);
			Pixel *currentPixel = m_currentPixel;
			//currentPixel += vp.vres * vp.hres - 1;
			float zw = 10;
			int count = 0;
			clock_t startingTime = clock();

			float invWidth = 1 / float(vp.hres), invHeight = 1 / float(vp.vres);
			float fov = 55, aspectratio = 1;
			//float fov = 30, aspectratio = vp.hres / float(vp.vres);

			float angle = tan(PI_NUM * 0.5 * fov / 180.);
			for (unsigned y = m_currentY; y < vp.vres; ++y) {
				for (unsigned x = 0; x < vp.hres; ++x) {

					float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
					float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
					Point3D raydir(xx, yy, -1);
					raydir.normalize();

					Point3D tempDir = m_camera->GetCameraDirection(raydir);

					pixelColor = RGBColor(0);
					ShadeRec rec;
					if (tracer_ptr->TraceRay(Ray(m_camera->eye, tempDir), rec))
					{
						int depth = 0;
						//pixelColor = GetShade(rec, depth);
						float tempTmin = min(rec.tmin * 0.1, 1);
						pixelColor = RGBColor(tempTmin);
					}
					//*currentPixel = pixelColor.toInt();
					pixelColor.Convert();
					SetColor(x, y, pixelColor.r, pixelColor.g, pixelColor.b);
					++currentPixel;
					++count;
					m_currentShadrec = rec;
				}

				if (count >= UPDATE_COUNT)
				{
					m_currentY = ++y;
					m_currentPixel = currentPixel;
					return false;
				}
			}

			clock_t endTimer = clock();


			double timeInSeconds = ((double)endTimer - startingTime) / (double)CLOCKS_PER_SEC;
			return true;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: ClampToColor
		//		Return Value: RGBColor
		//		  Parameters: RGBColor
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor ClampToColor(RGBColor rawColor) const
		{
			RGBColor c(rawColor);

			if (rawColor.r > 1.0 || rawColor.g > 1.0 || rawColor.b > 1.0) {
				c.r = 1.0; c.g = 0.0; c.b = 0.0;
			}

			return (c);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: RGBColor
		//		  Parameters: RGBColor
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		RGBColor MaxToOne(RGBColor c) const
		{
			float max_value = max(c.r, max(c.g, c.b));

			if (max_value > 1.0)
				return (c / max_value);
			else
				return (c);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetBuffer
		//		Return Value: Pixel*
		//		  Parameters: VOID
		//			  Status: DEPCRATED
		//----------------------------------------------------------------------------------------------------------------
		Pixel* GetBuffer()
		{
			return buffer;
		}

	private:
		ShadeRec m_currentShadrec;
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------