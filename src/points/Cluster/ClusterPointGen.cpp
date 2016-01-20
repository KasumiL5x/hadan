#include "ClusterPointGen.hpp"
#include <points/PointsUItils.hpp>
#include <Random.hpp>
#include <Log.hpp>

ClusterPointGen::ClusterPointGen()
	: IPointGen() {
}

ClusterPointGen::~ClusterPointGen() {
}

void ClusterPointGen::setFlux( const float flux ) {
	_flux = flux;
}

void ClusterPointGen::setUserPoints( const std::vector<cc::Vec3f>& userPoints ) {
	_primaryPoints = userPoints;
}

void ClusterPointGen::generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) {
	if( cc::math::equal<float>(_flux, 0.0f) ) {
		Log::warning("Warning: Fluctuation percentate for cluster fracture must be nonzero.  Fracturing may not occur.\n");
	}

	const BoundingBox& bbox = sourceModel.computeBoundingBox();

	if( 0 == _primaryPoints.size() ) {
		for( unsigned int i = 0; i < pointCount; ++i ) {
			_primaryPoints.push_back(PointsUtils::randomPointInBbox(bbox));
		}
	}

	Random<float, int> rnd;

	const cc::Vec3f cornerDiff = bbox.getCorner(BoundingBox::Corner::BottomLeftBack) - bbox.getCorner(BoundingBox::Corner::TopRightFront);
	const float fluxAmount = cc::math::percent<float>(cornerDiff.magnitude(), _flux);
	const float SECONDARY_ITERATIONS = 20;
	for( unsigned int i = 0; i < static_cast<unsigned int>(_primaryPoints.size()); ++i ) {
		const cc::Vec3f& curr = _primaryPoints[i];
		outPoints.push_back(curr);

		for( unsigned int j = 0; j < SECONDARY_ITERATIONS; ++j ) {
			cc::Vec3f second = curr;
			second.x += rnd.nextReal(0.0f, fluxAmount);
			second.y += rnd.nextReal(0.0f, fluxAmount);
			second.z += rnd.nextReal(0.0f, fluxAmount);
			outPoints.push_back(second);
		}
	}
}