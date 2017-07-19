#pragma once 
//----------------------------------------------------------------------------------------------------------------
//			Filename:		MeshObject.h
//			  Author:		Adrian De Barro
//			    Date:		12/08/2015
//----------------------------------------------------------------------------------------------------------------
//			  Status: REFACTORING
//----------------------------------------------------------------------------------------------------------------
#include <string>
#include <vector>

#include "..\CoreClasses\RGBObject\RGBColor.h"
#include "..\Material\Textures\Texture.h"
#include "..\Material\Textures\SolidColor.h"
#include "..\Material\Textures\ImageTexture.h"
#include "..\Light\Light.h"
#include "..\Light\LightFactory.h"
#include "../CoreClasses/NumberGenerators/Generator.h"
#include "../Tracer/Tracer.h"

class ShadeRec;

using Engine::RGBColor;
namespace Engine
{
	namespace Materials
	{
		enum SURFACE_TYPE
		{
			DIFF,
			REFL,
			GLOSSY,
		};

		typedef struct
		{
			int index;
			RGBColor contribtuion;
			float weight;
			Light* ptrLight;

		}LightContribution;

		//----------------------------------------------------------------------------------------------------------------
		//			Class Name: ObjectMesh
		//		   Description: Represent a TRIS. Concrete implementation of GeometricObjects
		//----------------------------------------------------------------------------------------------------------------
		//			    Status: REFACTORING
		//----------------------------------------------------------------------------------------------------------------
		class Material
		{
		public:
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: Surface_Type
			//			  Status: R EADY
			//----------------------------------------------------------------------------------------------------------------
			Material(SURFACE_TYPE p_type)
			{
				m_type = p_type;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			Material(void)
			{
				m_type = DIFF;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: CONSTRUCTOR
			//		Return Value: VOID
			//		  Parameters: Texture*
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			Material(Texture* p_currentTexture)
			{
				m_type = DIFF;
				//m_materialTexture = p_currentTexture;
				m_containsTexture = p_currentTexture->ContainsTexture();
			}

			virtual RGBColor GetBrdf(ShadeRec& p_sr, Point3D p_icomingDirection, Point3D p_outgoingDirection, RGBColor& p_albedo, bool p_co0ntainsTexture = false) = 0;

			virtual RGBColor GetBrdf(ShadeRec& p_sr, Point3D p_icomingDirection, Point3D p_outgoingDirection, bool p_co0ntainsTexture = false) = 0;

			virtual RGBColor GetAlbedo(ShadeRec& p_sr, bool p_containsTexture = false) = 0;

			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: Shade
			//		Return Value: VOID
			//		  Parameters: Collision Detail, vector<Light*>&, Tracer*, Light* ambientLight (Optional)
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			RGBColor Shade(ShadeRec& sr, std::vector<Light*>& sceneLights, Tracer* p_tracerPtr, RGBColor& p_colorAtPoint, Light* p_ambientLight = NULL, bool p_useStochastic = false)
			{
				Point3D w0 = -sr.ray.d;
				//ShadowTester testForShadow(p_tracerPtr, sceneLights);
				Engine::RGBColor outputColor = RGBColor(0);

				Point3D newOrigin = sr.localHitPoint + (kEpsilon * sr.GetNormal());

				if (!p_useStochastic || sceneLights.size() < 5)
				{
					for (int lightIndex = 0; lightIndex < sceneLights.size(); lightIndex++)
					{
						if (sceneLights[lightIndex]->GetStatus() == LIGHT_ON || sceneLights[lightIndex]->GetLightType() != LIGHT_TYPE::POINT_LIGHT)
						{
							ComputeContribution(sceneLights[lightIndex], sr, p_tracerPtr, p_colorAtPoint, w0, outputColor);
							//outputColor = RGBColor(0.5);
						}
					}
				}
				else 
				{
					p_colorAtPoint = GetAlbedo(sr, m_containsTexture);

					vector<Light*> chosenLights;
					GetLightsStochastically(sceneLights, sr, chosenLights);

					for (int index = 0; index < chosenLights.size(); index++)
					{
						ComputeContribution(chosenLights[index], sr, p_tracerPtr, w0, outputColor);
					}
				}
				return outputColor;
			}

			void GetLightsStochastically(vector<Light*> &p_lights, ShadeRec &p_sr, vector<Light*>  &p_selectedContributions_out)
			{
				std::map<int, Light*> chosenLights;
				vector<LightContribution>  lightContributionAtPoint;

				//get all valid light sources and compute the contribtion
				for (int index = 0; index < p_lights.size(); index++)
				{
					//switched on
					//if pointlight add else ass automatically to the final contribution list
					if (p_lights[index]->GetStatus() == LIGHT_STATE::LIGHT_ON && p_lights[index]->GetLightType() == LIGHT_TYPE::POINT_LIGHT)
					{
						Point3D wi = p_lights[index]->GetDirection(p_sr);
						wi.Normalize();
						float n_dot_wi = DOT(p_sr.normal, wi);
						RGBColor sceneLightComponent = p_lights[index]->L(p_sr)* n_dot_wi;

						//check contribution 
						LightContribution current;
						current.contribtuion = sceneLightComponent;
						current.index = index;
						current.ptrLight = p_lights[index];
						lightContributionAtPoint.push_back(current);
					}
					else if (p_lights[index]->GetLightType() != LIGHT_TYPE::POINT_LIGHT)
					{
						p_selectedContributions_out.push_back(p_lights[index]);
					}
				}
				if (lightContributionAtPoint.size() > 0)
				{
					std::sort(lightContributionAtPoint.begin(), lightContributionAtPoint.end(), SortFunction);

					float deltaSize = NumberGenerators::ExponentialDistibution::GetIntervalSize(lightContributionAtPoint.size());
					float currentInterval = NumberGenerators::ExponentialDistibution::MIN_BOUNDS;
					for (int index = 0; index < lightContributionAtPoint.size(); index++)
					{
						lightContributionAtPoint[index].weight = 1;
						//lightContributionAtPoint[index].weight = GetWeightAtInterval(currentInterval);
						currentInterval += deltaSize;
					}

					vector<float> contributionWeight;
					vector<double> intervals;
					//add weights to vector
					for (int index = 0; index < lightContributionAtPoint.size(); index++)
					{
						contributionWeight.push_back(lightContributionAtPoint[index].weight);
						intervals.push_back(index);
					}

					intervals.push_back(lightContributionAtPoint.size());

					//sanity check
					_ASSERT(contributionWeight.size() > 0);

					//build pdf distribution 
					//use pdf dsitribution and apply the generator

					std::piecewise_constant_distribution<> dist = std::piecewise_constant_distribution<double>(intervals.begin(), intervals.end(), contributionWeight.begin());

					while (chosenLights.size() != 5)
					{
						unsigned interval = static_cast<unsigned>(dist(m_generator));

						if (chosenLights.find(interval) == chosenLights.end())
						{
							chosenLights[interval] = lightContributionAtPoint[interval].ptrLight;
							p_selectedContributions_out.push_back(p_lights[lightContributionAtPoint[interval].index]);
						}
					}
				}
			}

			void GetLightsRandomly(vector<Light*> &p_lights, ShadeRec &p_sr, vector<Light*>  &p_selectedContributions_out)
			{
				std::map<int, Light*> chosenLights;
				vector<LightContribution>  lightContributionAtPoint;

				//get all valid light sources and compute the contribtion
				for (int index = 0; index < p_lights.size(); index++)
				{
					//switched on
					//if pointlight add else ass automatically to the final contribution list
					if (p_lights[index]->GetStatus() == LIGHT_STATE::LIGHT_ON && p_lights[index]->GetLightType() == LIGHT_TYPE::POINT_LIGHT)
					{
						Point3D wi = p_lights[index]->GetDirection(p_sr);
						wi.Normalize();
						float n_dot_wi = DOT(p_sr.normal, wi);
						RGBColor sceneLightComponent = p_lights[index]->L(p_sr)* n_dot_wi;

						//check contribution 
						LightContribution current;
						current.contribtuion = sceneLightComponent;
						current.index = index;
						current.ptrLight = p_lights[index];
						lightContributionAtPoint.push_back(current);
					}
					else if (p_lights[index]->GetLightType() != LIGHT_TYPE::POINT_LIGHT)
					{
						p_selectedContributions_out.push_back(p_lights[index]);
					}
				}

				while (chosenLights.size() != 5)
				{
					int chosenLight = rand() % p_lights.size() - 1;

					if (chosenLight > 0)
					{
						chosenLights[chosenLight] = lightContributionAtPoint[chosenLight].ptrLight;
						p_selectedContributions_out.push_back(p_lights[lightContributionAtPoint[chosenLight].index]);
					}
				}
			}

			float ComputeNormalDotWi(ShadeRec &p_sr, Light* p_currentLight, Point3D p_wi)
			{
				float n_dot_wi = 0.0f;
				_ASSERT(p_currentLight);

				if (Common::USE_VERTEX_NORMALS_SHADING)
					n_dot_wi = DOT(p_sr.m_shadingNormal, p_wi);
				else
					n_dot_wi = DOT(p_sr.normal, p_wi);

				return n_dot_wi;
			}

			static bool SortFunction(LightContribution &p_lhs, LightContribution& p_rhs)
			{
				return p_lhs.contribtuion.Summation() > p_rhs.contribtuion.Summation();
			}

			void ComputeContribution(Light* p_currentLight, ShadeRec &p_sr, Tracer* p_tracerPtr, RGBColor &p_colorAtPoint, Point3D p_w0, RGBColor &p_outputColor)
			{
				Point3D wi = p_currentLight->GetDirection(p_sr);
				wi.Normalize();
				float n_dot_wi = ComputeNormalDotWi(p_sr, p_currentLight, wi);

				bool shadowed = true;
				if (p_currentLight->GetLightType() == POINT_LIGHT)
				{
					Point3D directionToLight = (p_currentLight->GetLocation() - p_sr.GetkEpsillonCollisionPoint());
					double distanceToLight = directionToLight.length();
					directionToLight.Normalize();
					Point3D kEpsillonCollisionPoint = p_sr.GetkEpsillonCollisionPoint(directionToLight);

					ShadowRay shadowRay(kEpsillonCollisionPoint, directionToLight, Common::kEpsilon, (distanceToLight - Common::kEpsilon));
					//cout << shadowRay.GetTmax() << endl;
					//how to deal with area ligths
					shadowed = p_tracerPtr->TestShadow(shadowRay);
				}
				else if (p_currentLight->GetLightType() == DIRECTIONAL_LIGHT)
				{
					Point3D directionToLight = (p_currentLight->GetLocation())*-1;
					Point3D lightDirection = directionToLight.normalize();

					Point3D kEpsillonCollisionPoint = p_sr.GetkEpsillonCollisionPoint(lightDirection);

					ShadowRay shadowRay(kEpsillonCollisionPoint, directionToLight, Common::kEpsilon, 1000000);
					shadowed = p_tracerPtr->TestShadow(shadowRay);
					//cout << shadowed << endl;
				}

				

				if (n_dot_wi > 0 && !shadowed)
				{
					//ShadowRay shadowRay(newOrigin, wi);
					ShadeRec tempDetails;
					/*bool intersection = testForShadow.TestShadowRay(shadowRay, sceneLights[lightIndex]);
					if (!intersection)
					{*/
					if (!this->m_containsTexture)
					{
						RGBColor brdfComponent = GetBrdf(p_sr, wi, p_w0, p_colorAtPoint, false);
						//[AD] debugging
						//RGBColor brdfComponent = RGBColor(1,0,0);
						RGBColor sceneLightComponent = p_currentLight->L(p_sr)* n_dot_wi;

						RGBColor diffuseLightAddition = brdfComponent * sceneLightComponent;
						p_outputColor = p_outputColor + diffuseLightAddition;
					}
					else
					{
						RGBColor brdfComponent = GetBrdf(p_sr, wi, p_w0, p_colorAtPoint, true);
						//[AD] debugging
						//RGBColor brdfComponent = RGBColor(1, 0, 0);
						RGBColor sceneLightComponent = p_currentLight->L(p_sr)* n_dot_wi;
						RGBColor diffuseLightAddition = brdfComponent * sceneLightComponent;
						//[AD] to debug
						//cout << brdfComponent.ToString() << " " << sceneLightComponent.ToString() << " " << diffuseLightAddition.ToString() << " " << endl;
						p_outputColor = p_outputColor + diffuseLightAddition;
					}
				}
				else
				{
					//[AD] to debug
					//cout << "shadowed :" << endl;
					p_colorAtPoint = GetAlbedo(p_sr, this->m_containsTexture);
				}
			}


			void ComputeContribution(Light* p_currentLight, ShadeRec &p_sr, Tracer* p_tracerPtr, Point3D p_w0, RGBColor &p_outputColor)
			{
				Point3D wi = p_currentLight->GetDirection(p_sr);
				float n_dot_wi = ComputeNormalDotWi(p_sr, p_currentLight, wi);
				bool shadowed = true;
				if (p_currentLight->GetLightType() == POINT_LIGHT)
				{
					Point3D directionToLight = (p_currentLight->GetLocation() - p_sr.localHitPoint);
					double distanceToLight = directionToLight.length();
					Point3D lightDirection = directionToLight.normalize();
					Point3D kEpsillonCollisionPoint = p_sr.GetkEpsillonCollisionPoint(lightDirection);

					ShadowRay shadowRay(kEpsillonCollisionPoint, lightDirection, Common::kEpsilon, (distanceToLight - Common::kEpsilon));
					//cout << shadowRay.GetTmax() << endl;
					//how to deal with area ligths
					shadowed = p_tracerPtr->TestShadow(shadowRay);
				}
				else if (p_currentLight->GetLightType() == DIRECTIONAL_LIGHT)
				{
					Point3D directionToLight = (p_currentLight->GetLocation())*-1;
					Point3D lightDirection = directionToLight.normalize();

					Point3D kEpsillonCollisionPoint = p_sr.GetkEpsillonCollisionPoint(lightDirection);

					ShadowRay shadowRay(kEpsillonCollisionPoint, directionToLight, Common::kEpsilon, 1000000);
					shadowed = p_tracerPtr->TestShadow(shadowRay);
					//cout << shadowed << endl;
				}


				if (n_dot_wi > 0 && !shadowed)
				{
					//ShadowRay shadowRay(newOrigin, wi);
					ShadeRec tempDetails;
					/*bool intersection = testForShadow.TestShadowRay(shadowRay, sceneLights[lightIndex]);
					if (!intersection)
					{*/
					if (!this->m_containsTexture)
					{
						RGBColor diffuseBrdfComponent = GetBrdf(p_sr, wi, p_w0);
						RGBColor sceneLightComponent = p_currentLight->L(p_sr)* n_dot_wi;

						RGBColor diffuseLightAddition = diffuseBrdfComponent * sceneLightComponent;
						p_outputColor = p_outputColor + diffuseLightAddition;
					}
					else
					{
						RGBColor diffuseBrdfComponent = GetBrdf(p_sr, wi, p_w0);
						RGBColor sceneLightComponent = p_currentLight->L(p_sr)* n_dot_wi;
						RGBColor diffuseLightAddition = diffuseBrdfComponent * sceneLightComponent;
						p_outputColor = p_outputColor + diffuseLightAddition;
					}
				}
				else
				{
					p_outputColor = RGBColor(0);
				}
			}

			////----------------------------------------------------------------------------------------------------------------
			////		 Method Name: GetColor
			////		Return Value: VOID
			////		  Parameters: RGBColor
			////			  Status: READY
			////----------------------------------------------------------------------------------------------------------------
			//RGBColor Getcolor()
			//{
			//	float x = 0, y = 0;
			//	return m_materialTexture->GetColor(x, y);
			//}
			////----------------------------------------------------------------------------------------------------------------
			////		 Method Name: GetColor
			////		Return Value: RGBColor
			////		  Parameters: float x, float y
			////			  Status: READY
			////----------------------------------------------------------------------------------------------------------------
			//RGBColor GetColor(float p_x, float p_y)
			//{
			//	return m_materialTexture->GetColor(p_x, p_y);
			//}
			////----------------------------------------------------------------------------------------------------------------
			////		 Method Name: SetColor
			////		Return Value: VOID
			////		  Parameters: RGBColor 
			////			  Status: READY
			////----------------------------------------------------------------------------------------------------------------
			//void SetColor(RGBColor p_cD)
			//{
			//	m_materialTexture = new SolidColor(p_cD);
			//}
			////----------------------------------------------------------------------------------------------------------------
			////		 Method Name: SetTexture
			////		Return Value: VOID
			////		  Parameters: RGBColor
			////			  Status: READY
			////----------------------------------------------------------------------------------------------------------------
			//void SetTexture(RGBColor p_color)
			//{
			//	m_materialTexture = new SolidColor(p_color);
			//}
			////----------------------------------------------------------------------------------------------------------------
			////		 Method Name: SetTexture
			////		Return Value: VOID
			////		  Parameters: String path
			////			  Status: READY
			////----------------------------------------------------------------------------------------------------------------
			//void SetTexture(string p_path)
			//{
			//	m_materialTexture = new ImageTexture(p_path);
			//}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: GetType
			//		Return Value: SURFACE_TYPE
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			SURFACE_TYPE GetType()
			{
				return m_type;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: SetType
			//		Return Value: VOID
			//		  Parameters: SURFACE_TYPE
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			void SetType(SURFACE_TYPE p_type)
			{
				m_type = p_type;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: GetReflectionConstant
			//		Return Value: float
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			float GetReflectionConstant()
			{
				return m_Kr;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: SetReflectionConstant
			//		Return Value: VOID
			//		  Parameters: float 
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			void SetReflectionConstant(float p_Kr)
			{
				m_Kr = p_Kr;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: GetMaterialName
			//		Return Value: string
			//		  Parameters: VOID
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			std::string GetMaterialName()
			{
				return m_materialName;
			}
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: SetTexture
			//		Return Value: VOID
			//		  Parameters: Texture*
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			/*void SetTexture(Texture* p_texture)
			{
				m_materialTexture = p_texture;
			}*/
			//----------------------------------------------------------------------------------------------------------------
			//		 Method Name: SetMaterialName
			//		Return Value: VOID
			//		  Parameters: string 
			//			  Status: READY
			//----------------------------------------------------------------------------------------------------------------
			void SetMaterialName(std::string p_materialName)
			{
				m_materialName = p_materialName;
			}

			bool IsTextured()
			{
				return m_containsTexture;
			}

		protected:
			//Texture* m_materialTexture;
			bool m_containsTexture;
			SURFACE_TYPE m_type;
			float m_Kr;
			std::string m_materialName;
			std::default_random_engine m_generator;
		};
	}
	//----------------------------------------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------------------------------------
