#pragma once 

#include <Peer/Peer2Peer-Peer/P2pPeer.h>

#include "..\CoreClasses\GeometricObjects\GeometricObjects.h"
#include "..\Scene\SceneFactory.h"
#include "..\Tracer\DepthTracer.h"
#include "..\Light\Light.h"
#include "..\CoreClasses\UtilClasses\Common.h"
#include "..\Material\Reflective.h"
#include "..\Integrators\IrradianceCache.h"
#include "..\GuiHelper\gw.h"
#include "..\CoreClasses\Logger\Logger.h"
#include "..\CoreClasses\Camera\Camera.h"
#include "..\CoreClasses\GeometricObjects\Objects.h"
#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "..\Tracer\Tracer.h"
#include "..\World\View Plane Details\ViewPlane.h"
#include "../Networking/PeerInfo/Update.h"
#include "../Integrators/TwoLayerIrradianceCache.h"

#include "../CoreClasses/AssetsManager/SceneManager.h"

using Engine::ResourceManager::SceneManager;

using namespace Engine::Common;
namespace Engine
{
	class AbstractWorld
	{
	public:
		virtual bool RenderScene(void) = 0;
		virtual RGBColor GetShade(ShadeRec& sr, int depth) = 0;
	
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		AbstractWorld(void) : tracer_ptr(NULL)
		{
			backgroundColor = RGBColor(0, 0, 0);
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: int Scene Type
		//			  Status: DEPECRATED
		//----------------------------------------------------------------------------------------------------------------
		AbstractWorld(int m_sceneType) : sceneType(m_sceneType)
		{
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: int Scene Type, int Width, int Height
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		AbstractWorld(int m_sceneType, int m_width, int m_height) : sceneType(m_sceneType)
		{
			vp.hres = m_width;
			vp.vres = m_height;
			vp.s = 1;
		}
		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: CONSTRUCTOR
		//		Return Value: VOID
		//		  Parameters: int Scene Type, int width, int height, string Image path
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------
		AbstractWorld(int m_sceneType, int m_width, int m_height, string p_ImagePath) : sceneType(m_sceneType)
		{
			vp.hres = m_width;
			vp.vres = m_height;
			vp.s = 1;
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

		RGBColor* GetRgbBuffer()
		{
			return rgbBuffer;
		}

		Scene* GetScene()
		{
			return currentScene;
		}

		void SetCurrentY(int p_currentY)
		{
			m_currentY = p_currentY;
		}

		int GetCurrentY()
		{
			return m_currentY;
		}

		Camera* GetCamera()
		{
			return m_camera;
		}

		void SetCamera(Camera* p_camera)
		{
			m_camera = p_camera;
		}

		

		//----------------------------------------------------------------------------------------------------------------
		//		 Method Name: build
		//		Return Value: VOID
		//		  Parameters: VOID
		//			  Status: READY
		//----------------------------------------------------------------------------------------------------------------

		virtual void Build(string p_configFilePAth, int p_pathNumber, float p_alphaVar, float p_deltaFrames, float p_totalTimeForSequence, int p_numSamples, int p_numSets)
		{
			int numSamples = 25;
			vp.gamma = 1.0;
			vp.s = 1.0;
			//vp.SetSampler(new Jittered(numSamples));
			eye = 500;
			buffer = new Pixel[vp.hres * vp.vres];
			rgbBuffer = new RGBColor[vp.hres * vp.hres];

			int size = vp.hres * vp.vres;
			backgroundColor = RGBColor(255, 0, 0);
			for (int index = 0; index < size; index++)
			{
				buffer[index] = 0;
			}

			GetScene(p_pathNumber, p_configFilePAth, p_alphaVar, p_deltaFrames, p_totalTimeForSequence, p_numSamples, p_numSets);

			//focusing on the yellow ball at the moment
			m_camera = currentScene->GetCamera();
			Logger::LogTrace(m_camera->GetLookAt().toString());
			//progressive rendering
			m_currentPixel = buffer;
			m_currentY = 0;
		}


		virtual void Build(string p_configFilePAth, int p_pathNumber,int p_useTwoLevelIC, float p_alphaVar, float p_globalAlphaVar, int p_numSamples, int p_numSets, int p_amountSegments = 3, int p_amountBounces = 1, int p_sampleRandomizerType = 0, float p_cosineWeighting = 0 )
		{
			int numSamples = 25;
			vp.gamma = 1.0;
			vp.s = 1.0;

			eye = 500;
			buffer = new Pixel[vp.hres * vp.vres];
			rgbBuffer = new RGBColor[vp.hres * vp.hres];

			int size = vp.hres * vp.vres;
			backgroundColor = RGBColor(255, 0, 0);
			for (int index = 0; index < size; index++)
			{
				buffer[index] = 0;
			}

			if (p_useTwoLevelIC == 0)
				GetScene(p_pathNumber, p_configFilePAth, p_alphaVar, p_amountSegments, m_pathToObjFile, p_amountBounces, p_sampleRandomizerType, p_cosineWeighting, p_numSamples, p_numSets);
			else
				GetScene(p_pathNumber, p_configFilePAth, p_alphaVar, p_globalAlphaVar, p_amountSegments, m_pathToObjFile, p_amountBounces, p_sampleRandomizerType, p_cosineWeighting, p_numSamples, p_numSets);

			//focusing on the yellow ball at the moment
			m_camera = currentScene->GetCamera();
			Logger::LogTrace(m_camera->GetLookAt().toString());
			//progressive rendering
			m_currentPixel = buffer;
			m_currentY = 0;
			currentScene->AddSceneLight(m_sceneManager->GetLightManager()->GetLightMap());
			
		}



		void GetScene(int p_pathNumber, string p_pathConfigFile, float p_alphaVar, float p_deltaFrames, float p_totalTimeForSequence, int p_numSamples, int p_numSets)
		{
			currentScene = SceneFactory::GetScene(sceneType, p_pathNumber, p_pathConfigFile,p_deltaFrames, p_totalTimeForSequence);
			tracer_ptr = new DepthTracer(currentScene, true);
			tracer_ptr->UseAccelerationStructure();
			integrator = new IrradianceCache(currentScene, tracer_ptr->GetAccelerationStructure(), tracer_ptr, p_alphaVar,p_numSamples, p_numSets);
		}

		//need to edit this method to get depth
		void GetScene(int p_pathNumber, string p_pathConfigFile, float p_alphaVar, int p_amountSegments, string p_objFilePath, int p_amountBounces, int p_sampleRandomizerType, float p_cosineWeighting, int p_numSamples, int p_numSets)
		{
			currentScene = SceneFactory::GetScene(sceneType, p_pathNumber, p_pathConfigFile, p_amountSegments, p_objFilePath);
			tracer_ptr = new DepthTracer(currentScene, true);
			tracer_ptr->UseAccelerationStructure();
			integrator = new IrradianceCache(currentScene, tracer_ptr->GetAccelerationStructure(), tracer_ptr, p_alphaVar, p_numSamples, p_numSets,p_amountBounces, p_sampleRandomizerType, p_cosineWeighting);
		}

		//need to edit this to add the depth 
		void GetScene(int p_pathNumber, string p_pathConfigFile, float p_alphaVarLocal, float p_alphaVarGlobal ,int p_amountSegments, string p_objFilePath, int p_amountBounces, int p_sampleRandomizerType, float p_cosineWeighting, int p_numSamples, int p_numSets)
		{
			currentScene = SceneFactory::GetScene(sceneType, p_pathNumber, p_pathConfigFile, p_amountSegments, p_objFilePath);
			tracer_ptr = new DepthTracer(currentScene, true);
			tracer_ptr->UseAccelerationStructure();
			integrator = new TwoLayerIrradianceCache(currentScene, tracer_ptr->GetAccelerationStructure(), tracer_ptr, p_alphaVarGlobal,p_alphaVarLocal, p_numSamples, p_numSets, p_amountBounces, p_sampleRandomizerType, p_cosineWeighting);
		}


		///-------------------------------------------------------------------------------------------------
		/// <summary>	Builds this object. </summary>
		///
		/// <remarks>	Adrian, 21/09/2015. </remarks>
		///-------------------------------------------------------------------------------------------------

		virtual void Build(P2pPeer<RadiancePoint, IrradianceCache, Update, LockingOctree>* p_peer, int p_numSamples, int p_numSets)
		{
			int numSamples = 25;
			vp.gamma = 1.0;
			vp.s = 1.0;
			//vp.SetSampler(new Jittered(numSamples));
			eye = 500;
			buffer = new Pixel[vp.hres * vp.vres];
			rgbBuffer = new RGBColor[vp.hres * vp.hres];

			int size = vp.hres * vp.vres;
			backgroundColor = RGBColor(255, 0, 0);
			for (int index = 0; index < size; index++)
			{
				buffer[index] = 0;
			}

			currentScene = SceneFactory::GetScene(sceneType);
			tracer_ptr = new DepthTracer(currentScene, true);
			tracer_ptr->UseAccelerationStructure();
			integrator = new IrradianceCache(currentScene, tracer_ptr, p_peer, p_numSamples, p_numSets);
			//focusing on the yellow ball at the moment
			m_camera = currentScene->GetCamera();
			Logger::LogTrace(m_camera->GetLookAt().toString());
			//progressive rendering
			m_currentPixel = buffer;
			m_currentY = 0;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Builds the given p camera. </summary>
		///
		/// <remarks>	Adrian, 21/09/2015. </remarks>
		///
		/// <param name="p_camera">	[in,out] If non-null, the camera. </param>
		///-------------------------------------------------------------------------------------------------

		virtual void Build(Camera* p_camera)
		{
			int numSamples = 25;
			vp.gamma = 1.0;
			vp.s = 1.0;
			//vp.SetSampler(new Jittered(numSamples));
			eye = 500;
			buffer = new Pixel[vp.hres * vp.vres];
			rgbBuffer = new RGBColor[vp.hres * vp.hres];
			int size = vp.hres * vp.vres;

			for (int index = 0; index < size; index++)
			{
				buffer[index] = 0;
			}
			backgroundColor = RGBColor(0, 0, 0);
			currentScene = SceneFactory::GetScene(sceneType);
			tracer_ptr = new DepthTracer(currentScene, true);
			integrator = new IrradianceCache(currentScene, tracer_ptr);
			tracer_ptr->UseAccelerationStructure();
			m_camera = p_camera;
			m_currentPixel = buffer;
			m_currentY = 0;
		}

		///-------------------------------------------------------------------------------------------------
		/// <summary>	Builds the given p camera. </summary>
		///
		/// <remarks>	Adrian, 21/09/2015. </remarks>
		///
		/// <param name="p_camera">	[in,out] If non-null, the camera. </param>
		///-------------------------------------------------------------------------------------------------

		virtual void Build(Camera* p_camera, P2pPeer<RadiancePoint, IrradianceCache, Update, LockingOctree>* p_peer, int p_numSamples, int p_numSets)
		{
			int numSamples = 25;
			vp.gamma = 1.0;
			vp.s = 1.0;
			//vp.SetSampler(new Jittered(numSamples));
			eye = 500;
			buffer = new Pixel[vp.hres * vp.vres];
			rgbBuffer = new RGBColor[vp.hres * vp.hres];
			int size = vp.hres * vp.vres;

			for (int index = 0; index < size; index++)
			{
				buffer[index] = 0;
			}
			backgroundColor = RGBColor(0, 0, 0);
			currentScene = SceneFactory::GetScene(sceneType);
			tracer_ptr = new DepthTracer(currentScene, true);
			integrator = new IrradianceCache(currentScene, tracer_ptr, p_peer, p_numSamples, p_numSets);
			tracer_ptr->UseAccelerationStructure();
			m_camera = p_camera;
			m_currentPixel = buffer;
			m_currentY = 0;
		}
		///-------------------------------------------------------------------------------------------------
		/// <summary>	Move's camera. </summary>
		///
		/// <remarks>	Adrian, 28/08/2015. </remarks>
		///-------------------------------------------------------------------------------------------------
		void MoveCamera()
		{
			m_camera->MoveCameraWithDirectionDefined();
		}

		void MoveCamera(float p_timeTaken)
		{
			m_camera->MoveCamera(p_timeTaken);
		}

		bool CheckIfMoreFrames()
		{
			return m_camera->m_path->CheckMorePoints();
		}

		bool CheckIfMoreFrames(float p_timeTaken)
		{
			bool toReturn = m_camera->m_path->CheckMorePoints();
			m_camera->MoveCamera(p_timeTaken);
			return toReturn;
		}

		bool MoreFrames()
		{
			return false;
		}

		Integrator* GetIrradainceCache()
		{
			return integrator;
		}

		SceneManager* GetSceneManager()
		{
			return m_sceneManager;
		}

		Tracer* GetTracer()
		{
			return tracer_ptr;
		}

	protected:
		ViewPlane vp;
		RGBColor backgroundColor;
		Tracer* tracer_ptr;
		Pixel *buffer;
		Scene *currentScene;
		int sceneType;
		int eye;
		Integrator* integrator;
		Camera* m_camera;
		int d;
		int path;

		int m_currentY;
		Pixel* m_currentPixel;
		RGBColor *rgbBuffer;
		Logger m_logFile;
		ShadeRec m_currentShadrec;
		string m_pathToObjFile;
		SceneManager* m_sceneManager;

	private:
	};
}