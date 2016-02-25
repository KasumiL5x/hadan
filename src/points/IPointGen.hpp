#ifndef __isamplepointgen__
#define __isamplepointgen__

#include <vector>
#include <cc/Vec3.hpp>
#include "../BoundingBox.hpp"
#include "PointGenInfo.hpp"

class IPointGen {
public:
	IPointGen() {
	}
	virtual ~IPointGen() {
	}

	/**
	 * Generates a vector of sample points.
	 * @param[in]  boundingBox Input geometry's bounding box.
	 * @param[in]  info        Information to assist generation.
	 * @param[out] outPoints   Output vector of generated points.
	 */
	virtual void generateSamplePoints( const BoundingBox& boundingBox, const PointGenInfo& info, std::vector<cc::Vec3f>& outPoints )=0;
};

#endif /* __isamplepointgen__ */