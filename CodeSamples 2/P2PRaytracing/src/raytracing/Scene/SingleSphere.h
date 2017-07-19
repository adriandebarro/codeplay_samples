#ifndef SINGLE_SPHERE_H
#define SINGLE_SPHERE_H

#include "..\Tracer\Tracer.h"
#include "..\CoreClasses\GeometricObjects\Objects.h"

class SingleShere : public Engine::Tracer
{
public:
	Sphere sphere;

	SingleShere(void)
	{

	}

	SingleShere(const Sphere& _sphere)
	{
		sphere = _sphere;
	}

	using Tracer::TraceRay;
	virtual bool TraceRay(const Point3D& o, const Point3D& d, bool p_shadowTest = false) const
	{
		////not used for now
		//ShadeRec &sr = ShadeRec();
		//double t;

		//if (sphere.hit(o,d, t, sr))
		//{
		//	return true;
		//}
		//else
		//{
		//	return false;
		//}
		return true;
	}
};

#endif // !SINGLE_SPHERE_H
