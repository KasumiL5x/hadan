#include "ClusterPointGen.hpp"
#include <Random.hpp>
#include "../../MTLog.hpp"
#include "../../Random.hpp"

ClusterPointGen::ClusterPointGen()
	: IPointGen() {
}

ClusterPointGen::~ClusterPointGen() {
}

void ClusterPointGen::generateSamplePoints( const BoundingBox& boundingBox, const PointGenInfo& info, std::vector<cc::Vec3f>& outPoints ) {
	if( cc::math::equal<double>(info.flux, 0.0) ) {
		MTLog::instance()->log("Warning: Fluctuation percentate for cluster fracture must be nonzero.  Fracturing may not occur.\n");
	}

	// seeded random
	Random<float, int> rnd(info.seed);

	// initialize to user-provided points
	std::vector<cc::Vec3f> primaryPoints = info.userPoints;
	
	// randomly generate some extra points in addition to user-provided ones
	for( unsigned int i = 0; i < info.primaryCount; ++i ) {
		primaryPoints.push_back(rnd.pointInBBox(boundingBox));
	}

	// push back primary points and generate and add all secondary points
	const float fluxAmount = cc::math::percent<float>(static_cast<float>(boundingBox.getDiagonalDistance()), static_cast<float>(info.flux));
	for( unsigned int i = 0; i < static_cast<unsigned int>(primaryPoints.size()); ++i ) {
		const cc::Vec3f& curr = primaryPoints[i];
		outPoints.push_back(curr);

		for( unsigned int j = 0; j < info.secondaryCount; ++j ) {
			cc::Vec3f second = curr;
			second.x += static_cast<float>(rnd.nextReal(-fluxAmount, fluxAmount)); // cannot be zero or cells can fail to generate
			second.y += static_cast<float>(rnd.nextReal(-fluxAmount, fluxAmount));
			second.z += static_cast<float>(rnd.nextReal(-fluxAmount, fluxAmount));
			outPoints.push_back(second);
		}
	}

	// generate tertiary uniform points to even out the effect
	for( unsigned int i = 0; i < info.uniformCount; ++i ) {
		outPoints.push_back(rnd.pointInBBox(boundingBox));
	}
}