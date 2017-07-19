#ifndef __VIEWPLANE__
#define __VIEWPLANE__

#include "..\..\CoreClasses\Sampler\Sampler.h"
#include "..\..\CoreClasses\Sampler\Jittered.h"

using Engine::Sampler;
namespace Engine
{

	class ViewPlane
	{

	public:
		int hres;
		int vres;
		float s;
		float gamma;
		float invGamma;
		Sampler *sampler;
		int numSamples;
		bool show_out_of_gamut;			// display red if RGBColor out of gamut

		ViewPlane()
		{
			hres = 0;
			vres = 0;
			s = 0;
			gamma = 0;
			invGamma = 0;
			show_out_of_gamut = true;
			numSamples = 0;
		}

		ViewPlane(int _hres, int _vres, int _s, int _gamma) : hres(_hres), vres(_vres), s(_s), gamma(_gamma)
		{
			if (gamma != 0)
			{
				invGamma = 1 / gamma;
			}
		}

		void SetGamma(float _gamma)
		{
			gamma = _gamma;
			invGamma = 1 / gamma;
		}

		float GetGamma()
		{
			return gamma;
		}

		float GetInvGamma()
		{
			return invGamma;
		}

		void SetHres(int _hres)
		{
			hres = _hres;
		}

		int GetHres()
		{
			return hres;
		}

		int GetVres()
		{
			return vres;
		}

		void SetVres(int _vres)
		{
			vres = vres;
		}


		void SetPixelSize(float size)
		{
			s = size;
		}

		void SetSampler(Engine::Sampler *_sampler)
		{
			if (sampler != NULL)
			{
				delete sampler;
				sampler = NULL;
			}

			numSamples = _sampler->GetNumSamples();
			sampler = _sampler;

		}


		void SetSamples(const int n)
		{
			numSamples = n;

			if (sampler != NULL)
			{
				delete sampler;
				sampler = NULL;
			}

			if (numSamples > 1)
			{
				sampler = new Engine::Jittered();
			}
		}

	};
}

#endif // !VIEWPLANE_H
