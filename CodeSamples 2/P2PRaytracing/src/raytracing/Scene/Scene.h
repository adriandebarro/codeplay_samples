///-------------------------------------------------------------------------------------------------
// file:	Scene\Scene.h
//
// summary:	Declares the scene class
///-------------------------------------------------------------------------------------------------

#pragma once 

#include <vector>
#include <string>

#include "..\CoreClasses\GeometricObjects\GeometricObjects.h"
#include "..\Light\Light.h"
#include "..\Light\Ambient.h"
#include "..\CoreClasses\Camera\Camera.h"
#include "..\CoreClasses\Camera\Paths\CameraPaths.h"
#include "..\CoreClasses\Camera\Paths\Path.h"


using std::string;
using Engine::Paths::Path;

/// <summary>	A macro that defines view plane eye distance. </summary>
#define VIEW_PLANE_EYE_DISTANCE 1

namespace Engine
{
	/// ----------------------------------------------------------------------------------------------------------------
	/// Class Name: ObjectMesh
	/// Description: Represent a TRIS. Concrete implementation of GeometricObjects
	/// ----------------------------------------------------------------------------------------------------------------
	/// Status: REFACTORING
	/// ----------------------------------------------------------------------------------------------------------------
	class Scene
	{
	public:
		/// <summary>	The scene objects. </summary>
		vector<GeometricObjects*>  m_sceneObjects;
		/// <summary>	The scene light. </summary>
		vector<Light*> m_sceneLight;
		/// <summary>	The ambient light. </summary>
		Light * m_ambientLight;
		/// <summary>	The scene camera. </summary>
		Camera *m_sceneCamera;
		/// <summary>	Full pathname of the scene file. </summary>
		CameraPaths* m_ScenePath;
		/// <summary>	Name of the scene. </summary>
		string m_sceneName;
		vector<Path*> m_scenePaths;

		//virtual void AddSceneLight(SceneManager* p_sceneManager)
		virtual void AddSceneLight(map<string, Light*> &p_light)
		{
			for (map<string, Light*>::iterator it = p_light.begin(); it != p_light.end(); it++)
			{
				m_sceneLight.push_back(((Light*)it->second));
			}
		}

		virtual void AddSceneLight() = 0;

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetObjects
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		vector<GeometricObjects*> GetObjects(void)
		{
			return m_sceneObjects;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: AddObjects
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void AddObject(GeometricObjects* p_object)
		{
			m_sceneObjects.push_back(p_object);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetLight
		//		Return Value: vector<Light*>
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		vector<Light*> GetLight(void)
		{
			return m_sceneLight;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: AddLight
		//		Return Value: VOID
		//		  Parameters: Light*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void AddLight(Light* p_currentLight)
		{
			m_sceneLight.push_back(p_currentLight);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetCamera
		//		Return Value: VOID
		//		  Parameters: Camera*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetCamera(Camera *p_camera)
		{
			m_sceneCamera = p_camera;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: GetCamera
		//		Return Value: VOID
		//		  Parameters: Camera*
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		Camera* GetCamera(void)
		{
			return m_sceneCamera;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: BuildScene
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		virtual void BuildScene(int p_path = 0, string p_pathToObjFile = "") = 0;
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: BuildScene
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		string GetSceneName()
		{
			return m_sceneName;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: SetSceneName
		//		Return Value: VOID
		//		  Parameters: String 
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		void SetSceneName(string p_sceneName)
		{
			m_sceneName = p_sceneName;
		}
	private:
		
	};
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------