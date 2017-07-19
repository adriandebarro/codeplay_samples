#pragma once 

#include <math.h>

#include "./Textures/ImageTexture.h"
#include "../CoreClasses/Ray/Ray.h"
#include "../CoreClasses/UtilClasses/Common.h"

namespace Engine
{
	namespace Materials
	{
		namespace Skyboxes
		{
			class Skybox
			{
			private:
				ImageTexture* m_skyboxTexture;
				Point3D m_spherePosition;
				double m_sphereRadius;

			public:
				Skybox(string p_skyboxPath, double p_sphereRadius  = 10000000)
				{
					m_skyboxTexture = new ImageTexture(p_skyboxPath);
					m_spherePosition = Point3D(0);
					m_sphereRadius = p_sphereRadius;
				}

				RGBColor GetSkyboxColor(Ray& p_currentRay)
				{
					//intersect current ray with sphere
					double tmin = 1000000000;
					Point3D l = m_spherePosition - p_currentRay.o;
					float tca = l.dot(p_currentRay.d);
					//if (tca < 0) return false;
					float d2 = l.dot(l) - tca * tca;
					if (d2 > m_sphereRadius) return false;
					float thc = sqrt(m_sphereRadius - d2);
					float t0 = tca - thc;
					float t1 = tca + thc;
					
					if (t0 < t1 && t0 > Engine::Common::kEpsilon)
					{
						tmin = t0;

					}
					else
					{
						tmin = t1;
					}

					Point3D collisionPosition = p_currentRay.o + (p_currentRay.d * tmin);
					Point3D dirToCenter = m_spherePosition - collisionPosition;
					dirToCenter.Normalize();

					float denominator = (2 * 3.14159);
					float fraction = (std::atan2(dirToCenter.z, dirToCenter.x) / denominator );
					float u = 0.5 + fraction;

					float temp = (std::asin(dirToCenter.y) / 3.14159);
					float  v = 0.5 - temp;
	
					return m_skyboxTexture->GetHermite(u, v);
					//return m_skyboxTexture->GetColor(u, v);

					return RGBColor(0);
				}
			};
		}
	}
}