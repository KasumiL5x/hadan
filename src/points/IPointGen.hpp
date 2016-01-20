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
	 * Sets the fluctuation range as a percentage of the size of the bounding box.
	 * The size of the bounding box is defined as the distance from opposing corner points.
	 * @param flux Fluctuation amount.
	 */
	virtual void setFlux( const float flux )=0;

	/**
	 * Sets the user-provided points that can be optionally used in the point generation.
	 * @param userPoints Vector of positions.
	 */
	virtual void setUserPoints( const std::vector<cc::Vec3f>& userPoints )=0;

	/**
	 * Generates a vector of sample points.
	 * @param[in]  sourceModel Input Model used to generate points.
	 * @param[in]  pointCount  Number of points requested to be generated.
	 * @param[out] outPoints   Output vector of generated points.
	 */
	virtual void generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints )=0;
};

#endif /* __isamplepointgen__ */