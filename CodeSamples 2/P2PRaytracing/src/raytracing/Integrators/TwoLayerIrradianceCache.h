#pragma once 

#include "../Scene/Scene.h"
#include "IrradianceCache.h"
#include "../CoreClasses/RGBObject/RGBColor.h"
#include "../AccelerationStructures/BVH.h"
#include "../AccelerationStructures/AccelerationStructure.h"


using namespace Engine::AccelerationStructures;

namespace Engine
{
	///-------------------------------------------------------------------------------------------------
	/// <summary>	
	/// 	The Two level irradiance cache offers an improved performance when compared to the irradiance cache 
	/// 	in scenarios where the cache is prone to oversaturation 
	///  </summary>
	///
	/// <remarks>	Adrian, 19/07/2017. </remarks>
	///-------------------------------------------------------------------------------------------------

	class TwoLayerIrradianceCache : public Integrator
	{
	public:
		TwoLayerIrradianceCache(Scene* p_scene, AccelerationStructure* p_bvh, Tracer* p_tracerPtr, float p_AlphaConstantGlobalIC, float p_AlphaConstantLocalIC, int p_numSamples, int p_numSets , int p_amountBounces = 1, int p_sampleRandomizerType = 0, float p_cosineWeigthing = 0)
		{
			m_GlobalIrradianceCache = new IrradianceCache(p_scene, p_bvh, p_tracerPtr, p_AlphaConstantGlobalIC, p_numSamples, p_numSets,p_amountBounces, p_sampleRandomizerType, p_cosineWeigthing);
			m_LocalIrradianceCache = new IrradianceCache(p_scene, p_bvh, p_tracerPtr, p_AlphaConstantLocalIC, p_numSamples, p_numSets, p_amountBounces, p_sampleRandomizerType, p_cosineWeigthing);
		}

		IrradianceCache* GetGlobalIC()
		{
			return m_GlobalIrradianceCache;
		}

		IrradianceCache* GetLocalIC()
		{
			return m_LocalIrradianceCache;
		}

		using Integrator::GetIndirectLightingAtPoint;
		virtual RGBColor GetIndirectLightingAtPoint(ShadeRec& p_shadeRec)
		{
			Point3D normal(p_shadeRec.normal.x, p_shadeRec.normal.y, p_shadeRec.normal.z);
			return  GetIndirectLighting(p_shadeRec, normal);
		}

		RGBColor GetIndirectLighting(ShadeRec& p_shadeRec, Point3D p_normal)
		{
			RGBColor color(0);
			//try get an interpolation
			bool result = m_LocalIrradianceCache->GetInterpolation(color, p_shadeRec, p_normal);
				
			if (result == false)
			{
				
				RadiancePoint* newPoint = m_GlobalIrradianceCache->GetInterpolationOrCompute(p_shadeRec,p_normal);
				#ifndef WAIT_FREE
					ReadLock r_lock(m_GlobalIrradianceCache->GetOctree()->m_myLock);
				#endif
				//-------------------------------------------Readers Lock------------------------------------------
				m_LocalIrradianceCache->AddUpdate(newPoint);
				color = newPoint->radiance;
			}
			else
			{
				return color;
			}
			return color;
		}

		using Integrator::InvalidateCache;
		virtual void InvalidateCache()
		{
			m_GlobalIrradianceCache->InvalidateCache();
			m_LocalIrradianceCache->InvalidateCache();
		}

		using Integrator::PartialInvalidate;
		virtual void PartialInvalidate(BoundingBox* p_boudningBox)
		{
			m_GlobalIrradianceCache->PartialInvalidate(p_boudningBox);
			m_LocalIrradianceCache->PartialInvalidate(p_boudningBox);
		}

	private:
		~TwoLayerIrradianceCache()
		{

		}

		IrradianceCache* m_GlobalIrradianceCache;
		IrradianceCache* m_LocalIrradianceCache;

	};
}
