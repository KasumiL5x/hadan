#include "ClusterPointGen.hpp"
#include <points/PointsUtils.hpp>
#include <Random.hpp>
#include <Log.hpp>

ClusterPointGen::ClusterPointGen()
	: IPointGen() {
}

ClusterPointGen::~ClusterPointGen() {
}

void ClusterPointGen::generateSamplePoints( const Model& sourceModel, const PointGenInfo& info, std::vector<cc::Vec3f>& outPoints ) {
	if( cc::math::equal<double>(info.flux, 0.0) ) {
		Log::warning("Warning: Fluctuation percentate for cluster fracture must be nonzero.  Fracturing may not occur.\n");
	}

	const BoundingBox& bbox = sourceModel.computeBoundingBox();

	// initialize to user-provided points
	std::vector<cc::Vec3f> primaryPoints = info.userPoints;
	
	// randomly generate some extra points in addition to user-provided ones
	for( unsigned int i = 0; i < info.primaryCount; ++i ) {
		primaryPoints.push_back(PointsUtils::randomPointInBbox(bbox));
	}

	Random<double, int> rnd;

	const cc::Vec3f cornerDiff = bbox.getCorner(BoundingBox::Corner::BottomLeftBack) - bbox.getCorner(BoundingBox::Corner::TopRightFront);
	const double fluxAmount = cc::math::percent<double>(static_cast<double>(cornerDiff.magnitude()), static_cast<float>(info.flux));
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

	// generate secondary uniform points to even out the effect
	for( unsigned int i = 0; i < info.uniformCount; ++i ) {
		outPoints.push_back(PointsUtils::randomPointInBbox(bbox));
	}
}