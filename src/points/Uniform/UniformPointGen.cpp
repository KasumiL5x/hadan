#include "UniformPointGen.hpp"
#include <random>

UniformPointGen::UniformPointGen()
	: IPointGen() {
}

UniformPointGen::~UniformPointGen() {
}

void UniformPointGen::generateSamplePoints( const Model& sourceModel, const unsigned int pointCount, std::vector<cc::Vec3f>& outPoints ) const {
	const BoundingBox bb = sourceModel.computeBoundingBox();
	std::random_device rnd;
	std::mt19937 gen(rnd());
	std::uniform_real_distribution<float> dx(bb.minX(), bb.maxX());
	std::uniform_real_distribution<float> dy(bb.minY(), bb.maxY());
	std::uniform_real_distribution<float> dz(bb.minZ(), bb.maxZ());
	for( unsigned int i = 0; i < pointCount; ++i ) {
		outPoints.push_back(cc::Vec3f(dx(gen), dy(gen), dz(gen)));
	}
}