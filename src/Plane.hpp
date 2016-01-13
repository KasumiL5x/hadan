#ifndef __plane__
#define __plane__

#include <cc/Vec3.hpp>

struct Plane {
	cc::Vec3f normal;
	float constant;

	Plane()
		: normal(cc::Vec3f::up()), constant(0.0f) {
	}

	Plane( const cc::Vec3f& inNormal, const float inConstant )
		: normal(inNormal), constant(inConstant) {
	}
};

#endif /* __plane__ */