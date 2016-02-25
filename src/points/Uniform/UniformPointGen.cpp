#include "UniformPointGen.hpp"
#include <Random.hpp>

UniformPointGen::UniformPointGen()
	: IPointGen() {
}

UniformPointGen::~UniformPointGen() {
}

void UniformPointGen::generateSamplePoints( const BoundingBox& boundingBox, const PointGenInfo& info, std::vector<cc::Vec3f>& outPoints ) {
	Random<float, int> rnd(info.seed);
	for( unsigned int i = 0; i < info.uniformCount; ++i ) {
		outPoints.push_back(rnd.pointInBBox(boundingBox));
	}
}