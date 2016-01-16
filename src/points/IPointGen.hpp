#ifndef __isamplepointgen__
#define __isamplepointgen__

#include <vector>
#include <cc/Vec3.hpp>
#include <Model.hpp>

class IPointGen {
public:
	IPointGen() {
	}
	virtual ~IPointGen() {
	}

	/**
	 * Generates a vector of sample points.
	 * @param[in]  sourceModel Input Model used to generate points.
	 * @param[in]  pointCount  Number of points requested to be generated.
	 * @param[out] outPoints   Output vector of generated points.
	 */
	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) const=0;
};

#endif /* __isamplepointgen__ */