#include "UniformPointGen.hpp"
#include "../PointsUtils.hpp"

UniformPointGen::UniformPointGen()
	: IPointGen() {
}

UniformPointGen::~UniformPointGen() {
}

void UniformPointGen::generateSamplePoints( const Model& sourceModel, const PointGenInfo& info, std::vector<cc::Vec3f>& outPoints ) {
	const BoundingBox bbox = sourceModel.computeBoundingBox();
	for( unsigned int i = 0; i < info.uniformCount; ++i ) {
		outPoints.push_back(PointsUtils::randomPointInBbox(bbox));
	}
}