#include "UniformPointGen.hpp"
#include <Random.hpp>

UniformPointGen::UniformPointGen()
	: IPointGen() {
}

UniformPointGen::~UniformPointGen() {
}

void UniformPointGen::generateSamplePoints( const Model& sourceModel, const PointGenInfo& info, std::vector<cc::Vec3f>& outPoints ) {
	const BoundingBox bbox = sourceModel.computeBoundingBox();
	Random<float, int> rnd(info.seed);
	for( unsigned int i = 0; i < info.uniformCount; ++i ) {
		outPoints.push_back(rnd.pointInBBox(bbox));
	}
}