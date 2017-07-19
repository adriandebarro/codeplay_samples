#pragma once 

#include <string>
#include <iostream>
#include <stdio.h>

#include "../JsonParser/document.h"
#include "../JsonParser/filereadstream.h"
// rapidjson's DOM-style API
#include "../JsonParser/prettywriter.h" // for stringify JSON
#include <cstdio>

#include "ResourceManager.h"
#include "../../Light/Light.h"
#include "../../Light/LightFactory.h"
#include "LightManager.h"
#include "../../Material/Skybox.h"
#include "../../System/SystemHelper.h"

using namespace rapidjson;
using std::string;
using std::cout;
using std::cin;
using std::endl;

using Engine::Materials::Skyboxes::Skybox;

namespace Engine
{
	namespace ResourceManager
	{	
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	
		/// 	Manager for scenes, 
		/// 	parses a JSON file and picks scene details . 
		/// </summary>
		///
		/// <remarks>	Adrian, 07/07/2017. </remarks>
		////////////////////////////////////////////////////////////////////////////////////////////////////
		class SceneManager
		{
		private:
			const string LIGHT_ARRAY_JSON = "Light";
			const string ENVIRONMENT_ARRAY_JSON = "Environment";
			
			const string CAMERA_ARRAY_JSON = "Camera";
			const string CAMERA_RANGE_JSON = "CameraRange";
			
			const string SKY_BOX_JSON = "SkyBox";

			const string ENVIRONMENT_LIGHT_NAME_JSON = "Name";
			const string ENVIRONMENT_ENABLED_SKYBOX_JSON = "SkyBoxEnabled";
			
			const string OBJ_FILES_PATHS = "ObjFilePaths";

			Document m_parsedJson;
			string m_jsonPath;

			LightManager<Light>* m_lightManager;
			vector<string> m_objFilePaths;

			Skybox* m_skyBox;
			bool m_skyBoxEnabled = false;

		public:
			SceneManager(string p_jsonPath)
			{
				m_jsonPath = p_jsonPath;
				InitParsing();
			}

			void AddSceneData()
			{
				//Light 
				if(m_parsedJson.HasMember(LIGHT_ARRAY_JSON.c_str()))
				{
					ParseSceneLightDetails();
				}
				//Camera

				//Geometry details

				//skybox
				if (m_parsedJson.HasMember(SKY_BOX_JSON.c_str()))
				{
					ParseSkyBoxDetails();
				}
				else
					m_skyBoxEnabled = false;
			
				//MultipleObjFiles
				if (m_parsedJson.HasMember(OBJ_FILES_PATHS.c_str()))
				{
					GetObjFilesDetails();
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Gets object files details. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			void GetObjFilesDetails()
			{
				const Value& objFilkePathsArray = m_parsedJson[OBJ_FILES_PATHS.c_str()];

				for (int pathIndex = 0; pathIndex < objFilkePathsArray.Size(); pathIndex ++)
				{
					m_objFilePaths.push_back(objFilkePathsArray[pathIndex]["Path"].GetString());
				}	
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Parse sky box details. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			void ParseSkyBoxDetails()
			{
				
				bool skyBoxEnabled = std::stoi(m_parsedJson[ENVIRONMENT_ARRAY_JSON.c_str()][SKY_BOX_JSON.c_str()][ENVIRONMENT_ENABLED_SKYBOX_JSON.c_str()].GetString());

				if (skyBoxEnabled)
				{
					const Value& skyBoxDetails = m_parsedJson[SKY_BOX_JSON.c_str()];
					double radius = skyBoxDetails["Radius"].GetDouble();
					string skyBoxTexture = skyBoxDetails["Path"].GetString();

					m_skyBox = new Skybox(skyBoxTexture, radius);
 					m_skyBoxEnabled = true;
				}
				else
				{
					m_skyBoxEnabled = false;
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Parse scene light details. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			void ParseSceneLightDetails()
			{
				vector<Light*> allowedLights;
				map<string, Light*> allLights;

				const Value& a = m_parsedJson[CAMERA_RANGE_JSON.c_str()];

				string stringRange = a.GetString();
				float range = std::stof(stringRange);


				const Value& lightsArray = m_parsedJson[LIGHT_ARRAY_JSON.c_str()];

				for (int index = 0; index < lightsArray.Size(); index++)
				{
					Point3D position(0);
					Point3D direction(0);
					RGBColor color(0);
					int status = 0;


					string name = lightsArray[index]["Name"].GetString();
					string type = lightsArray[index]["Type"].GetString();
					string stringColor = lightsArray[index]["Color"].GetString();
					

					if (type == "PointLight")
					{
						string stringPosition = lightsArray[index]["Position"].GetString();
						position.FromString(stringPosition);
						status = std::stoi(lightsArray[index]["Status"].GetString());
					}
					else if (type == "DirectionalLight")
					{
						string stringDirection = lightsArray[index]["Direction"].GetString();
						direction.FromString(stringDirection);
					}

					float energy = std::stof(lightsArray[index]["Energy"].GetString());

					color.FromString(stringColor);
					Light* currentLight = LightFactory::CreateLight(type, name, energy, color, position, direction);

					if(type == "PointLight")
					{
						currentLight->SetRange(std::stof(lightsArray[index]["Range"].GetString()));
						currentLight->SetLightStatus(status);
					}

					allLights[currentLight->GetLightName()] = currentLight;
				}

				const Value& approvedLights = m_parsedJson[ENVIRONMENT_ARRAY_JSON.c_str()][LIGHT_ARRAY_JSON.c_str()];

				for (int index = 0; index < approvedLights.Size(); index++)
				{
					string currentName = approvedLights[index][ENVIRONMENT_LIGHT_NAME_JSON.c_str()].GetString();

					if (currentName != "")
					{
						allowedLights.push_back(allLights[currentName]);
					}
				}

				if (allLights.size() > 0)
					m_lightManager = new LightManager<Light>(range, allowedLights);
				else
					cout << "No light was loaded" << endl;
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Parse Json file </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			bool InitParsing()
			{
				std::stringstream stream;
				bool readFile = LoadJson();
				if (readFile)
				{
					AddSceneData();
					return true;
				} else {
					cout << "Error while parsing the scene assets json file" << endl;
					System::SystemUtil::SystemPause();
					return false;
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Loads the JSON. </summary>
			///
			/// <remarks>	Adrian, 07/07/2017. </remarks>
			///
			/// <returns>	true if it succeeds, false if it fails. </returns>
			////////////////////////////////////////////////////////////////////////////////////////////////////
			bool LoadJson()
			{
				#ifdef _WIN32
					std::FILE *file = fopen(m_jsonPath.c_str(),"rb");
				#else
					std::FILE *file = fopen(m_jsonPath.c_str(), "r");
				#endif
				
				if(file != nullptr)
				{
					char buffer[65536];
					FileReadStream is(file, buffer, sizeof(buffer));
					m_parsedJson.ParseStream(is);
					return !m_parsedJson.HasParseError();;
				} else {
					return false;
				}
				return false;
			}

			LightManager<Light>* GetLightManager()
			{
				return m_lightManager;
			}

			Skybox* GetSkyBox()
			{
				return m_skyBox;
			}

			bool IsSkyBoxEnabled()
			{
				return m_skyBoxEnabled;
			}

			vector<string> GetObjPathsVector()
			{
				return m_objFilePaths;
			}

			string GetObjPaths()
			{
				std::stringstream ss;
				for (int index = 0; index < m_objFilePaths.size(); index++)
				{
					ss << m_objFilePaths[index] << ";";
				}

				return ss.str();
			}
		};
	}
}
