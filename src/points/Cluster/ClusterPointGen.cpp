#include "ClusterPointGen.hpp"

ClusterPointGen::ClusterPointGen()
	: IPointGen() {
}

ClusterPointGen::~ClusterPointGen() {
}

void ClusterPointGen::setUserPoints( const std::vector<cc::Vec3f>& userPoints ) {
	_primaryPoints = userPoints;
}

void ClusterPointGen::generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) const {
	if( 0 == _primaryPoints.size() ) {
		// todo: randomly generate X number of points
	}

	for( unsigned int i = 0; i < static_cast<unsigned int>(_primaryPoints.size()); ++i ) {
		// todo: generate secondary points around primary points w/ random flux
	}
}