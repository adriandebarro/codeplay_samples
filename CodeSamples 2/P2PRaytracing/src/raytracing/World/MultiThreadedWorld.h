#pragma once 

#include <thread>
#include <mutex>
#include <boost/thread/barrier.hpp>

#include "AbstractWorld.h"
#include "..\CoreClasses\JobProducer\JobProducer.h"
#include "..\CoreClasses\JobProducer\EqualJobsProducer.h"
#include "..\CoreClasses\JobProducer\ThreadJob\Tile.h"
#include "..\CoreClasses\JobProducer\ThreadJob\TileList.h"
#include "..\CoreClasses/JobProducer/ThreadJob/FAATileList.h"
#include "..\GuiHelper/gw.h"
                                                                                                                                                                                                                                                                                                                 
#include "../CoreClasses/AssetsManager/SceneManager.h"
#include "./Material/Material.h"
#include "../Material/Skybox.h"
using std::thread;
using Engine::ResourceManager::SceneManager;

using Engine::Materials::Material;

namespace Engine
{
	const enum FrameState
	{
		NO_FRAME,
		UNDER_WAY,
		READY_FRAME
	};

	int ReflectiveDepthLimit = 5;

	///-------------------------------------------------------------------------------------------------
	/// <summary>	A concrete world capable of employing concurent rendering threads </summary>
	///
	/// <remarks>	Adrian, 08/07/2017. </remarks>
	///-------------------------------------------------------------------------------------------------
	class MultiThreadWorld : public AbstractWorld
	{
	public:
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		MultiThreadWorld(void)
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
		MultiThreadWorld(int p_sceneType)
		{
			sceneType = p_sceneType;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: int Scene Type, int Width, int Height
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		MultiThreadWorld(int p_sceneType, int m_width, int m_height)
		{
			sceneType = p_sceneType;
			vp.hres = m_width;
			vp.vres = m_height;
			vp.s = 1;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: int Scene Type, int width, int height, number of threads evaluates automaticaly
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		MultiThreadWorld(int p_sceneType, string p_pathToObjFile, string p_sceneDetailsJson,int m_width, int m_height, int p_numberThreads = 0)
		{
			sceneType = p_sceneType;
			vp.hres = m_width;
			vp.vres = m_height;
			vp.s = 1;
			

			if (p_numberThreads > 0)
				m_amountOfThreads = p_numberThreads;
			else
				m_amountOfThreads = thread::hardware_concurrency();

			m_allThreads = new thread[m_amountOfThreads];
			m_threadReady = new bool[m_amountOfThreads];
			ResetThreadReady(true);
			jobProducer = new EqualJobProducer(m_width, m_height, m_amountOfThreads, 60, 100);
			
			m_firstTime = true;

			m_sceneManager = new SceneManager(p_sceneDetailsJson);

			if(p_pathToObjFile == "@")
			{
				m_pathToObjFile = m_sceneManager->GetObjPaths();
			}
			else
				m_pathToObjFile = p_pathToObjFile;

			if (m_sceneManager->IsSkyBoxEnabled())
			{
				m_skyBox = m_sceneManager->GetSkyBox();
				m_skyBoxEnabled = true;
			}
			else
				m_skyBoxEnabled = false;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Renders the scene. </summary>
		///
		/// <remarks>	Adrian, 28/08/2015. </remarks>
		///
		/// <returns>	true if it succeeds, false if it fails. </returns>
		///-------------------------------------------------------------------------------------------------
		/*using AbstractWorld::RenderScene;
		virtual bool RenderScene(void)
		{
			if (CheckIfFrameReady() && m_frameState == FrameState::NO_FRAME)
			{
				ResetThreadReady(false);
				tracer_ptr->UseAccelerationStructure();
				jobProducer->CreateJobs(m_allJobs);
				double  fractpart, intpart;

				fractpart = modf(float(m_allJobs.size())/ float(jobProducer->GetWorkersAmount()), &intpart);
				if (fractpart > 0)
					intpart++;

				int count = 0;

				cout << jobProducer->GetWorkersAmount() << endl;

				for (int threadIndex = 0; threadIndex < jobProducer->GetWorkersAmount(); threadIndex++)
				{
					int limit = 0;
					TileList *currentList = new TileList();
					while (count < m_allJobs.size() && limit  < intpart)
					{
						currentList->m_toDoList.push_back(m_allJobs[count]);
						count++;
						limit++;
					}
					
					thread currentThread(&MultiThreadWorld::ShortcutPath, this, currentList, vp.hres, vp.vres, threadIndex);
					currentThread.detach();
				}

				m_frameState = FrameState::UNDER_WAY;
				return false;
			}
			else if (!CheckIfFrameReady() && m_frameState == FrameState::UNDER_WAY)
			{
				return false;
			}
			else if (CheckIfFrameReady() && m_frameState == FrameState::UNDER_WAY)
			{
				m_frameState = FrameState::NO_FRAME;
				Common::EmptyPointerVector((m_allJobs));
				return true;
			}
		}*/


		using AbstractWorld::RenderScene;
		virtual bool RenderScene(void)
		{
			if (m_firstTime)
			{
				m_firstBarrier = new boost::barrier(m_amountOfThreads + 1);
				m_secondBarrier = new boost::barrier(m_amountOfThreads + 1);
				jobList = new FaaTileList();
			}

			if (m_firstTime || CheckIfFrameReady() && m_frameState == FrameState::NO_FRAME)
			{
				ResetThreadReady(false);

				if (!m_firstTime)
					m_firstBarrier->wait();

				tracer_ptr->UseAccelerationStructure();
				jobProducer->CreateJobs(jobList->m_jobs);
				jobList->RestartCounter();

				cout << "joblist " << jobList->m_jobs.size() << endl;

				int count = 0;

				cout << jobProducer->GetWorkersAmount() << endl;

				if (m_firstTime)
				{

					for (int threadIndex = 0; threadIndex < jobProducer->GetWorkersAmount(); threadIndex++)
					{
						int limit = 0;

						m_allThreads[threadIndex] = thread(&MultiThreadWorld::ShortcutPath, this, jobList, vp.hres, vp.vres, threadIndex, m_firstBarrier, m_secondBarrier);
						//currentThread.detach();
					}
					m_firstTime = false;
				}
				else
				{
					m_secondBarrier->wait();
				}
				
				m_frameState = FrameState::UNDER_WAY;
				return false;
			}
			else if (!CheckIfFrameReady() && m_frameState == FrameState::UNDER_WAY)
			{
				return false;
			}
			else if (CheckIfFrameReady() && m_frameState == FrameState::UNDER_WAY)
			{
				m_frameState = FrameState::NO_FRAME;
				cout << "Frame is ready" << endl;
				Common::EmptyPointerVector(jobList->m_jobs);
				return true;
			}
		}
		
		static void ShortcutPath(void* userData, FaaTileList* p_toDoList, int p_frameWidth, int p_frameHeight, int p_threadIndex, boost::barrier* p_firstBarrier, boost::barrier* p_secondBarrier)
		{
			MultiThreadWorld* world = static_cast<MultiThreadWorld*>(userData);
			return world->StartJob(p_toDoList, p_frameWidth, p_frameHeight, p_threadIndex, p_firstBarrier,p_secondBarrier);
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Starts a job. </summary>
		///
		/// <remarks>	Adrian, 29/08/2015. </remarks>
		///
		/// <param name="p_toDoList">  	[in,out] If non-null, list of to does. </param>
		/// <param name="p_frameWidth">	Width of the frame. </param>
		/// <param name="p_frameHeigt">	The frame heigt. </param>
		///-------------------------------------------------------------------------------------------------

		void StartJob(TileList* p_toDoList, int p_frameWidth, int p_frameHeigt, int p_threadId)
		{
			float invWidth = 1 / float(vp.hres), invHeight = 1 / float(vp.vres);
			float fov = m_camera->m_fov, aspectratio = m_camera->m_aspectRatio;

			while (1)
			{
				try
				{
					if (p_toDoList->ExistsMore())
					{
						Tile* currentTile = p_toDoList->m_toDoList.front();

						int startingLocation = currentTile->m_startingPoint.y * p_frameWidth + currentTile->m_startingPoint.x;

						//Pixel *currentPixel = m_currentPixel[startingLocation];
						RGBColor *currentPixelColor = rgbBuffer;

						//currentPixel += vp.vres * vp.hres - 1;
						float zw = 10;
						int count = 0;
						clock_t startingTime = clock();


						//float fov = 30, aspectratio = vp.hres / float(vp.vres);
						float eyeViewPlaneDistance = m_camera->eye.x + m_camera->m_viewPlaneDistance;
						float angle = tan(PI_NUM * 0.5 * fov / 180.);
						for (int y = currentTile->m_startingPoint.y; y < currentTile->m_startingPoint.y + currentTile->m_height; ++y)
						{
							for (int x = currentTile->m_startingPoint.x; x < currentTile->m_startingPoint.x + currentTile->m_width; ++x)
							{
								float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
								float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
								Point3D raydir(xx, yy, -1);
								raydir.normalize();
								Point3D tempDir = m_camera->GetCameraDirection(raydir);
								//cout << "first equal operator" << endl;
								RGBColor pixelColor = RGBColor(0);
								ShadeRec rec;
								if (tracer_ptr->TraceRay(Ray(m_camera->eye, tempDir), rec))
								{
									int depth = 0;
									//cout << "second equal operator" << endl;
									float color = 1 / sqrt(rec.tmin);
									pixelColor = RGBColor(color);
									//pixelColor = GetShade(rec, depth);
								}
								//*currentPixel = pixelColor.toInt();
								//cout << currentTile->ToString() << " third equal operator " << y << " " << p_frameWidth << " " << x << " " << (y * p_frameWidth) + x << endl;
								
								pixelColor.Convert();
								rgbBuffer[y * p_frameWidth + x] = pixelColor;
								//cout << "fourth equal operator" << endl;
								SetColor(x, y, pixelColor.r, pixelColor.g, pixelColor.b);
							}
							//cout << "next line"  << this_thread::get_id() << endl;
						}
					}
				}
				catch (exception ex)
				{
					string error = ex.what();
					Logger::LogTrace("Problem with the thread "+ error);
				}

			}
			
			Logger::LogTrace("thread exited");

			//delete p_toDoList;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Starts a job and uses a barrier to wait for the others. </summary>
		///
		/// <remarks>	Adrian, 02/09/2015. </remarks>
		///
		/// <param name="p_toDoList">  	[in,out] If non-null, list of to does. </param>
		/// <param name="p_frameWidth">	Width of the frame. </param>
		/// <param name="p_frameHeigt">	The frame heigt. </param>
		/// <param name="p_threadId">  	Identifier for the thread. </param>
		/// <param name="barrier">	   	[in,out] If non-null, the barrier. </param>
		///-------------------------------------------------------------------------------------------------

		void StartJob(FaaTileList* p_toDoList, int p_frameWidth, int p_frameHeigt, int p_threadId, boost::barrier* p_firstBarrier, boost::barrier* p_secondBarrier)
		{
			float invWidth = 1 / float(vp.hres), invHeight = 1 / float(vp.vres);
			float fov = m_camera->m_fov; //float aspectratio = m_camera->m_aspectRatio;
			float aspectratio = p_frameWidth / p_frameHeigt;

			Tile* currentTile = NULL;

			while (1)
			{
				if (p_toDoList->GetNext(currentTile))
				{
					int startingLocation = currentTile->m_startingPoint.y * p_frameWidth + currentTile->m_startingPoint.x;

					//Pixel *currentPixel = m_currentPixel[startingLocation];
					RGBColor *currentPixelColor = rgbBuffer;

					//currentPixel += vp.vres * vp.hres - 1;
					float zw = 10;
					int count = 0;
					clock_t startingTime = clock();


					//float fov = 30, aspectratio = vp.hres / float(vp.vres);
					float eyeViewPlaneDistance = m_camera->eye.x + m_camera->m_viewPlaneDistance;
					float angle = tan(PI_NUM * 0.5 * fov / 180.);
					for (int y = currentTile->m_startingPoint.y; y < currentTile->m_startingPoint.y + currentTile->m_height; ++y)
					{
						for (int x = currentTile->m_startingPoint.x; x < currentTile->m_startingPoint.x + currentTile->m_width; ++x)
						{
							float xx = (2 * ((x + 0.5) * invWidth) - 1) * angle * aspectratio;
							float yy = (1 - 2 * ((y + 0.5) * invHeight)) * angle;
							Point3D raydir(xx, yy, -1);
							raydir.normalize();
							Point3D tempDir = m_camera->GetCameraDirection(raydir);
							//cout << "first equal operator" << endl;
							RGBColor pixelColor = RGBColor(0);
							ShadeRec rec;
							Ray currentRay(m_camera->eye, tempDir);
							if (tracer_ptr->TraceRay(currentRay, rec))
							{
								int depth = 0;

								//depth image 
								/*float color = 1 / sqrt(rec.tmin);
								pixelColor = RGBColor(color);*/
								//Point3D normalAtPoint = rec.m_shadingNormal;
								//pixelColor = RGBColor(normalAtPoint.x, normalAtPoint.y, normalAtPoint.z);

								pixelColor = GetShade(rec, depth);
				            }
							else
							{
								pixelColor = FillSceneVoid(currentRay);
								//pixelColor = m_skyBox->GetSkyboxColor(currentRay);
							}
							pixelColor.Convert();
							//pixelColor = pixelColor * 255;
							rgbBuffer[y * p_frameWidth + x] = pixelColor;
							SetColor(x, y, pixelColor.r, pixelColor.g, pixelColor.b);
						}
					}
				}
				else
				{
					//maybe more jobs in future?
					//finshed frame
					m_threadReady[p_threadId] = true;
					cout << "thread " << p_threadId<< endl;
					p_firstBarrier->wait();
					//cout << "second barrier awaits thread " << p_threadId << endl;
					//waiting to get more jobs
					p_secondBarrier->wait();
				}
			}

			//delete p_toDoList;
		}


		///-------------------------------------------------------------------------------------------------
		/// <summary>	Gets a shade. </summary>
		///
		/// <remarks>	Adrian, 28/08/2015. </remarks>
		///
		/// <param name="sr">   	[in,out] The sr. </param>
		/// <param name="depth">	The depth. </param>
		///
		/// <returns>	The shade. </returns>
		///-------------------------------------------------------------------------------------------------
		using AbstractWorld::GetShade;
		virtual RGBColor GetShade(ShadeRec& sr, int depth)
		{
			GeometricObjects* object = sr.object;
			Material *materialPtr;

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
			case  Engine::Materials::SURFACE_TYPE::DIFF:
			{
				#define GLOBAL_ILLUMINATION
				#ifdef GLOBAL_ILLUMINATION
 					RGBColor indirectColor = integrator->GetIndirectLightingAtPoint(sr);
					RGBColor albedoColor(0);
					RGBColor directLight = materialPtr->Shade(sr, currentScene->m_sceneLight, tracer_ptr, albedoColor, NULL, true);// +indirectColor*materialPtr->Getcolor();
																																	   
				/*if (indirectColor == RGBColor(1, 0, 0))
						outputColor = outputColor + indirectColor;
					else
						outputColor = outputColor + (albedoColor * directLight) +(albedoColor * indirectColor);*/

					//outputColor = (albedoColor * indirectColor);

					/*if(materialPtr->GetMaterialName() == "floor_material")
					{
						cout << "we need to find this issue" << endl;
					}*/

					//outputColor = directLight;
					//[AD] to debug
					outputColor = outputColor + directLight +(indirectColor * albedoColor);
					//outputColor = outputColor + (directLight + indirectColor);
					//outputColor = outputColor + (albedoColor * indirectColor);

					//outputColor = albedoColor;
					//outputColor = outputColor + (albedoColor  directLight) + (indirectColor);
				#else
					RGBColor albedoColor(0);
					RGBColor directLight = materialPtr->Shade(sr, currentScene->m_sceneLight, tracer_ptr, albedoColor, NULL, false);

					outputColor = outputColor + (albedoColor * directLight);

				#endif

					return outputColor;
			}
			break;

			case Engine::Materials::SURFACE_TYPE::REFL:
			{

				RGBColor albedo(0);
				Matte* diffuseComponent = ((Reflective*)materialPtr)->GetMatteComponent();
				RGBColor diffuseDirectLighting = diffuseComponent->Shade(sr, currentScene->m_sceneLight, tracer_ptr, albedo, NULL, true);
				RGBColor indirectColor = integrator->GetIndirectLightingAtPoint(sr);

				outputColor = diffuseDirectLighting;

				if(depth <  ReflectiveDepthLimit)
				{
					Point3D incomingDirection = sr.localHitPoint - sr.ray.o;
					incomingDirection.Normalize();

					Point3D normalAtPoint = sr.GetNormal();

					//reflection direction
					Point3D reflectionDirection = -(2 * (sr.m_shadingNormal.dot(incomingDirection)) * sr.m_shadingNormal - incomingDirection);
					reflectionDirection.Normalize();
					
					//case specular reflective material 
					ShadeRec shadingDetails;
					Point3D reflectiveRay(0);
					RGBColor albedoColor(0);
					Ray reflectedRay = Ray(sr.GetkEpsillonCollisionPoint(), reflectionDirection);

					if (tracer_ptr->TraceRay(reflectedRay, shadingDetails))
					{
						Material* collisionMaterial = shadingDetails.m_hitTriangle->GetMaterial();
						RGBColor reflectedElement = collisionMaterial->Shade(shadingDetails, currentScene->m_sceneLight, tracer_ptr, albedoColor, NULL, true);// +indirectColor*materialPtr->Getcolor();

						RGBColor ks, kd;

						((Reflective*)materialPtr)->GetComponentRatios(kd, ks);
						outputColor = outputColor + reflectedElement*ks + (diffuseDirectLighting + (indirectColor * albedo))*kd;
					}
					else
					{
						RGBColor ks, kd;
						RGBColor reflectedColor(0);

						((Reflective*)materialPtr)->GetComponentRatios(kd, ks);
						
						if (m_skyBoxEnabled)
							reflectedColor = (m_skyBox->GetSkyboxColor(reflectedRay) * 10);

						outputColor = reflectedColor * ks + diffuseDirectLighting * kd;
					}
				}
				else
				{
					outputColor = diffuseDirectLighting;
				}

				return outputColor;

			}

			case Engine::Materials::SURFACE_TYPE::GLOSSY:
			{
				if (depth <  ReflectiveDepthLimit)
				{
					//RGBColor indirectColor = integrator->GetIndirectLightingAtPoint(sr);
					Point3D incomingDirection = sr.localHitPoint - sr.ray.o;
					incomingDirection.Normalize();

					Point3D normalAtPoint = sr.GetNormal();

					//reflection direction
					Point3D reflectionDirection = -(2 * (sr.m_shadingNormal.dot(incomingDirection)) * sr.m_shadingNormal - incomingDirection);
					reflectionDirection.Normalize();

					//case specular reflective material 
					ShadeRec shadingDetails;
					Point3D reflectiveRay(0);
					RGBColor albedoColor(0);
					RGBColor reflectedComponent(0);
					if (tracer_ptr->TraceRay(Ray(m_camera->eye, reflectionDirection), shadingDetails))
					{
						int nextDepth = depth + 1;
						reflectedComponent  = GetShade(shadingDetails, nextDepth);
					}
					
					RGBColor directLighting = materialPtr->Shade(sr, currentScene->m_sceneLight, tracer_ptr, albedoColor);
					//outputColor = (reflectedComponent * directLighting) +(indirectColor * albedoColor);
					outputColor = (reflectedComponent + directLighting);// +(indirectColor * albedoColor);
				}
				else
					return RGBColor(0);
			}

			default:
				return outputColor;
				break;
			}
		}

		RGBColor FillSceneVoid(Ray& p_currentRay)
		{
			if (m_skyBoxEnabled)
				return (m_skyBox->GetSkyboxColor(p_currentRay) * 10);
			else
				return RGBColor(0);
		}


		///-------------------------------------------------------------------------------------------------
		/// <summary>	Determine if all the threads have finished their tasks. </summary>
		///
		/// <remarks>	Adrian, 28/08/2015. </remarks>
		///
		/// <returns>	true if it succeeds, false if it fails. </returns>
		///-------------------------------------------------------------------------------------------------

		bool CheckIfFrameReady()
		{
			for (int index = 0; index < m_amountOfThreads; index++)
			{
				if (m_threadReady[index] == false)
				{
					return false;
				}
			}
			return true;
		}

		void ResetThreadReady(bool p_boolValue)
		{
			for (int index = 0; index < m_amountOfThreads; index++)
			{
				m_threadReady[index] = p_boolValue;
			}
		}

	private:
		int m_amountOfThreads;
		JobProducer* jobProducer;
		FrameState m_frameState = FrameState::NO_FRAME;
		bool* m_threadReady;
		thread* m_allThreads;
		vector<Tile*> m_allJobs;
		bool m_firstTime;
		boost::barrier* m_firstBarrier;
		boost::barrier* m_secondBarrier;
		FaaTileList* jobList;
		Engine::Materials::Skyboxes::Skybox* m_skyBox;
		bool m_skyBoxEnabled = false;
	};
/// <summary>	......................................... </summary>
}