#include "ClusterPointGen.hpp"
#include <points/PointsUItils.hpp>
#include <Random.hpp>

ClusterPointGen::ClusterPointGen()
	: IPointGen() {
}

ClusterPointGen::~ClusterPointGen() {
}

void ClusterPointGen::setUserPoints( const std::vector<cc::Vec3f>& userPoints ) {
	_primaryPoints = userPoints;
}

void ClusterPointGen::generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) {
	const BoundingBox& bbox = sourceModel.computeBoundingBox();

	if( 0 == _primaryPoints.size() ) {
		for( unsigned int i = 0; i < pointCount; ++i ) {
			_primaryPoints.push_back(PointsUtils::randomPointInBbox(bbox));
		}
	}

	Random<float, int> rnd;

	const float SECONDARY_ITERATIONS = 20;
	const float MIN_FLUX = 0.005f;
	const float MAX_FLUX = 0.1f;
	for( unsigned int i = 0; i < static_cast<unsigned int>(_primaryPoints.size()); ++i ) {
		const cc::Vec3f& curr = _primaryPoints[i];
		outPoints.push_back(curr);

		for( unsigned int j = 0; j < SECONDARY_ITERATIONS; ++j ) {
			cc::Vec3f second = curr;
			second.x += rnd.nextReal(MIN_FLUX, MAX_FLUX);
			second.y += rnd.nextReal(MIN_FLUX, MAX_FLUX);
			second.z += rnd.nextReal(MIN_FLUX, MAX_FLUX);
			outPoints.push_back(second);
		}
	}
}